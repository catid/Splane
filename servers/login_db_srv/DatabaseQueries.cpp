/*
	Copyright (c) 2009-2010 Christopher A. Taylor.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of LibCat nor the names of its contributors may be used
	  to endorse or promote products derived from this software without
	  specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "LoginDatabaseServer.hpp"
#include "LoginServerProtocol.hpp"
using namespace cat;
using namespace sphynx;

bool LoginDatabaseConnexion::OnQuery(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes)
{
	u32 query_id;
	u8 query_type;

	BufferStream bs(msg);
	++bs >> query_id >> query_type;

	u32 query_bytes = bytes - bs.GetOffset(msg);

	switch (query_type)
	{
	case LDQ_ACCOUNT_LOGIN: 	return OnQueryLogin(tls, query_id, bs, query_bytes);
	case LDQ_ACCOUNT_CREATE: 	return OnQueryCreateAccount(tls, query_id, bs, query_bytes);
	case LDQ_ALIAS_CREATE:		return OnQueryAliasCreate(tls, query_id, bs, query_bytes);
	}

	return false;
}

struct QueryLoginTag : public bombay::AsyncQueryRead
{
	u64 account_offset, alias_offset;
	u32 query_id;
	DatabaseBanIds ban_ids;
	u32 ip;
	char name[USER_NAME_MAXLEN];
	char alias[USER_NAME_MAXLEN];
};

struct QueryAliasTag : public bombay::AsyncQueryRead
{
	AsyncBuffer *login_data;
};

bool LoginDatabaseConnexion::OnQueryLogin(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream query, u32 query_bytes)
{
	if (query_bytes != sizeof(DatabaseBanIds) + 4 + 2*USER_NAME_MAXLEN)
		return false;

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got query: Login";

	u8 resp[16];
	BufferStream bs_resp(resp);
	bs_resp << query_id;

	char *name = query.c_str() + sizeof(DatabaseBanIds) + 4;
	int namelen = GetFixedStrLen(name, USER_NAME_MAXLEN);

	char *alias = name + USER_NAME_MAXLEN;
	int aliaslen = GetFixedStrLen(alias, USER_NAME_MAXLEN);

	if (!ValidLoginServerUserName(name, namelen))
		bs_resp << (u8)LDA_BAD_NAME;
	else if (!ValidLoginServerUserName(alias, aliaslen))
		bs_resp << (u8)LDA_BAD_ALIAS;
	else
	{
		DesimilarizeFixedString(name, namelen);
		DesimilarizeFixedString(alias, aliaslen);

		u64 account_offset = _server->_account_name_index->LookupVarField(name, namelen);
		u64 alias_offset = _server->_alias_name_index->LookupVarField(alias, aliaslen);

		if (account_offset == bombay::INVALID_RECORD_OFFSET)
			bs_resp << (u8)LDA_UNKNOWN_NAME;
		else
		{
			AsyncBuffer *buffer;

			if (AsyncBuffer::Acquire(buffer, sizeof(AccountSchema), sizeof(QueryLoginTag)))
			{
				QueryLoginTag *tag = buffer->GetTag<QueryLoginTag>();

				query >> tag->ban_ids >> tag->ip;
				query.write(tag->name, USER_NAME_MAXLEN);
				query.write(tag->alias, USER_NAME_MAXLEN);
				tag->account_offset = account_offset;
				tag->alias_offset = alias_offset;
				tag->query_id = query_id;
				tag->SetCallback(fastdelegate::MakeDelegate(this, &LoginDatabaseConnexion::OnReadLoginAccount), this);

				if (_server->_account_table->Query(account_offset, buffer))
					return true;
			}

			bs_resp << (u8)LDA_ERROR;
		}
	}

	WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_ANSWER, resp, bs_resp.GetOffset(resp));

	return true;
}

bool LoginDatabaseConnexion::OnReadLoginAccount(ThreadPoolLocalStorage *tls, int error, AsyncBuffer *buffer, u32 bytes)
{
	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Query read login completion";

	AccountSchema *account = buffer->GetData<AccountSchema>();
	QueryLoginTag *tag = buffer->GetTag<QueryLoginTag>();

	u8 resp[256];
	BufferStream bs_resp(resp);
	bs_resp << tag->query_id;

	if (error || bytes != sizeof(AccountSchema))
		bs_resp << (u8)LDA_ERROR;
	else
	{
		switch (account->type)
		{
		case ACCTTYPE_DELETED:
			bs_resp << (u8)LDA_UNKNOWN_NAME;
			break;

		case ACCTTYPE_BANNED:
			bs_resp << (u8)LDA_BANNED;
			break;

		default:
			if (bombay::INVALID_RECORD_OFFSET == tag->alias_offset)
			{
				bs_resp << (u8)LDA_UNKNOWN_ALIAS << tag->account_offset << account->type;
				bs_resp.write(account->password, 40);
				bs_resp.write(account->public_key, 64);
			}
			else
			{
				AsyncBuffer *alias_buffer;

				if (AsyncBuffer::Acquire(alias_buffer, sizeof(AliasSchema), sizeof(QueryAliasTag)))
				{
					QueryAliasTag *alias_tag = alias_buffer->GetTag<QueryAliasTag>();

					alias_tag->login_data = buffer;
					alias_tag->SetCallback(fastdelegate::MakeDelegate(this, &LoginDatabaseConnexion::OnReadLoginAlias), this);

					if (_server->_alias_table->Query(tag->alias_offset, alias_buffer))
						return false; // Keep account buffer until completion of alias read
				}

				bs_resp << (u8)LDA_ERROR;
			}
			break;
		}
	}

	WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_ANSWER, resp, bs_resp.GetOffset(resp));

	return true;
}

bool LoginDatabaseConnexion::OnReadLoginAlias(ThreadPoolLocalStorage *tls, int error, AsyncBuffer *buffer, u32 bytes)
{
	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Query read alias completion";

	AliasSchema *alias = buffer->GetData<AliasSchema>();
	QueryAliasTag *alias_tag = buffer->GetTag<QueryAliasTag>();
	QueryLoginTag *account_tag = alias_tag->login_data->GetTag<QueryLoginTag>();
	AccountSchema *account = alias_tag->login_data->GetData<AccountSchema>();

	u8 resp[256];
	BufferStream bs_resp(resp);
	bs_resp << account_tag->query_id;

	if (error || bytes != sizeof(AliasSchema))
		bs_resp << (u8)LDA_ERROR;
	else if (alias->account_id != account_tag->account_offset)
	{
		// If alias belongs to a different account,
		bs_resp << (u8)LDA_ALIAS_WRONG_ACCOUNT;
	}
	else
	{
		bs_resp << (u8)LDA_ACCOUNT_DATA << account_tag->account_offset << account_tag->alias_offset << account->type;
		bs_resp.write(account->password, 40);
		bs_resp.write(account->public_key, 64);
	}

	WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_ANSWER, resp, bs_resp.GetOffset(resp));

	if (alias_tag->login_data)
		alias_tag->login_data->Release();

	return true;
}


bool LoginDatabaseConnexion::OnQueryCreateAccount(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream query, u32 query_bytes)
{
	if (query_bytes != sizeof(DatabaseBanIds) + 4 + 40 + 64 + 2*USER_NAME_MAXLEN)
		return false;

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got query: Create Account";

	u8 resp[256];
	BufferStream bs_resp(resp);
	bs_resp << query_id;

	char *name = query.c_str() + sizeof(DatabaseBanIds) + 4 + 40 + 64;
	int namelen = GetFixedStrLen(name, USER_NAME_MAXLEN);

	char *alias = name + USER_NAME_MAXLEN;
	int aliaslen = GetFixedStrLen(alias, USER_NAME_MAXLEN);

	if (!ValidLoginServerUserName(name, namelen))
		bs_resp << (u8)LDA_BAD_NAME;
	else if (!ValidLoginServerUserName(alias, aliaslen))
		bs_resp << (u8)LDA_BAD_ALIAS;
	else
	{
		DesimilarizeFixedString(name, namelen);

		AccountSchema *account_record;

		if (!AsyncBuffer::Acquire(account_record))
			bs_resp << (u8)LDA_ERROR;
		else
		{
			account_record->type = ACCTTYPE_USER;
			query >> account_record->last_ids >> account_record->last_ip;
			query.read(account_record->password, 40);
			query.read(account_record->public_key, 64);
			SetFixedStr(account_record->name, sizeof(account_record->name), name, namelen);

			u64 account_offset = _server->_account_table->Insert(account_record);

			if (bombay::INVALID_RECORD_OFFSET == account_offset)
				bs_resp << (u8)LDA_ACCOUNT_EXISTS;
			else
			{
				DesimilarizeFixedString(alias, aliaslen);

				AliasSchema *alias_record;

				if (!AsyncBuffer::Acquire(alias_record))
					bs_resp << (u8)LDA_ERROR;
				else
				{
					alias_record->account_id = account_offset;
					SetFixedStr(alias_record->name, USER_NAME_MAXLEN, alias, aliaslen);

					u64 alias_offset = _server->_alias_table->Insert(alias_record);

					if (bombay::INVALID_RECORD_OFFSET == alias_offset)
						bs_resp << (u8)LDA_ALIAS_EXISTS;
					else
					{
						bs_resp << (u8)LDA_ACCOUNT_CREATED << account_offset << alias_offset << ACCTTYPE_USER;
					}
				}
			}
		}
	}

	WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_ANSWER, resp, bs_resp.GetOffset(resp));

	return true;
}


bool LoginDatabaseConnexion::OnQueryAliasCreate(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream query, u32 query_bytes)
{
	if (query_bytes != 8 + USER_NAME_MAXLEN)
		return false;

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got query: Create Alias";

	u8 resp[256];
	BufferStream bs_resp(resp);
	bs_resp << query_id;

	char *alias = query.c_str() + 8;
	int aliaslen = GetFixedStrLen(alias, USER_NAME_MAXLEN);

	if (!ValidLoginServerUserName(alias, aliaslen))
		bs_resp << (u8)LDA_BAD_ALIAS;
	else
	{
		DesimilarizeFixedString(alias, aliaslen);

		AliasSchema *alias_record;

		if (!AsyncBuffer::Acquire(alias_record))
			bs_resp << (u8)LDA_ERROR;
		else
		{
			query >> alias_record->account_id;
			SetFixedStr(alias_record->name, USER_NAME_MAXLEN, alias, aliaslen);

			u64 alias_offset = _server->_alias_table->Insert(alias_record);

			if (bombay::INVALID_RECORD_OFFSET == alias_offset)
				bs_resp << (u8)LDA_ALIAS_EXISTS;
			else
			{
				bs_resp << (u8)LDA_ALIAS_CREATED << alias_offset;
			}
		}
	}

	WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_ANSWER, resp, bs_resp.GetOffset(resp));

	return true;
}
