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

#include "LoginServer.hpp"
using namespace cat;
using namespace cat::sphynx;

void LoginConnexion::OnDBLoginAnswer(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes)
{
	if (_state != SS_DBWAIT_LOGIN)
	{
		WARN("LoginConnexion") << "DB: Ignored late login from earlier state";
		return;
	}

	if (success)
	{
		if (bytes < 1) return;

		WARN("LoginConnexion") << "DB: Got login answer";

		switch (data[0])
		{
		case LDA_ERROR:
			WriteReliable(STREAM_1, LC_LOGIN_ERROR);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BAD_NAME:
			WriteReliable(STREAM_1, LC_LOGIN_BAD_NAME);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BAD_ALIAS:
			WriteReliable(STREAM_1, LC_LOGIN_BAD_ALIAS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ALIAS_WRONG_ACCOUNT:
			WriteReliable(STREAM_1, LC_ALIAS_WRONG_ACCOUNT);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BANNED:
			WriteReliable(STREAM_1, LC_LOGIN_BANNED);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_UNKNOWN_NAME:
			WriteReliable(STREAM_1, LC_LOGIN_UNKNOWN_NAME);
			_state = SS_CREATE_ACCOUNT;
			break;

		case LDA_ACCOUNT_DATA:
			if (bytes == 1+8+8+4+8+32+64)
				OnDBLoginData(tls, data, bytes);
			break;

		case LDA_UNKNOWN_ALIAS:
			if (bytes == 1+8+4+8+32+64)
				OnDBUnknownAlias(tls, data, bytes);
			break;
		}
	}
	else // Request was canceled
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);

		WARN("LoginConnexion") << "DB: Login request was canceled";
	}
}

void LoginConnexion::OnDBLoginData(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes)
{
	WARN("LoginConnexion") << "DB: Login data received";

	++data >> _account_id >> _alias_id >> _account_type >> _account_password_salt >> _account_password_hash;

	if (!SecureEqual(_public_key, data, 64))
	{
		WriteReliable(STREAM_1, LC_LOGIN_WRONG_KEYFILE);
		_state = SS_LOGIN_REQUEST;
		return;
	}

	switch (_account_type)
	{
	case ACCTTYPE_USER:
	case ACCTTYPE_MOD:
	case ACCTTYPE_SMOD:
	case ACCTTYPE_SYSOP:
	case ACCTTYPE_NETOP:
		PostLoginChallenge(tls);
		break;

	default:
		WriteReliable(STREAM_1, LC_LOGIN_ERROR);
		_state = SS_LOGIN_REQUEST;
		return;
	}
}

void LoginConnexion::OnDBUnknownAlias(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes)
{
	WARN("LoginConnexion") << "DB: Unknown alias received";

	++data >> _account_id >> _account_type >> _account_password_salt >> _account_password_hash;

	if (!SecureEqual(_public_key, data, 64))
	{
		WriteReliable(STREAM_1, LC_LOGIN_WRONG_KEYFILE);
		_state = SS_LOGIN_REQUEST;
		return;
	}

	switch (_account_type)
	{
	case ACCTTYPE_USER:
	case ACCTTYPE_MOD:
	case ACCTTYPE_SMOD:
	case ACCTTYPE_SYSOP:
	case ACCTTYPE_NETOP:
		WriteReliable(STREAM_1, LC_LOGIN_UNKNOWN_ALIAS);
		_state = SS_CREATE_ALIAS;
		break;

	default:
		WriteReliable(STREAM_1, LC_LOGIN_ERROR);
		_state = SS_LOGIN_REQUEST;
		return;
	}
}


void LoginConnexion::OnDBCreateAnswer(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes)
{
	if (_state != SS_DBWAIT_CREATE_ACCOUNT)
	{
		WARN("LoginConnexion") << "DB: Ignored late create account from earlier state";
		return;
	}

	if (success)
	{
		if (bytes < 1) return;

		WARN("LoginConnexion") << "DB: Got create answer";

		switch (data[0])
		{
		case LDA_ERROR:
			WriteReliable(STREAM_1, LC_LOGIN_ERROR);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BAD_NAME:
			WriteReliable(STREAM_1, LC_LOGIN_BAD_NAME);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BAD_ALIAS:
			WriteReliable(STREAM_1, LC_LOGIN_BAD_ALIAS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BANNED:
			WriteReliable(STREAM_1, LC_LOGIN_BANNED);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ACCOUNT_EXISTS:
			WriteReliable(STREAM_1, LC_ACCOUNT_EXISTS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ALIAS_EXISTS:
			WriteReliable(STREAM_1, LC_ALIAS_EXISTS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ACCOUNT_CREATED:
			if (bytes == 1+8+8+4)
				OnDBCreateData(tls, data, bytes);
			break;
		}
	}
	else // Request was canceled
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);

		WARN("LoginConnexion") << "DB: Login request was canceled";
	}
}

void LoginConnexion::OnDBCreateData(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got DB Create Data";

	++data >> _account_id >> _alias_id >> _account_type;

	tls->csprng->Generate(_session_key, sizeof(_session_key));

	u8 pkt[256];
	BufferStream success(pkt);
	success << _account_id << _alias_id << _session_key;

	WriteReliable(STREAM_1, LC_CREATE_SUCCESS, pkt, success.GetOffset(pkt));

	OnLoginComplete();
}


void LoginConnexion::OnDBCreateAlias(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes)
{
	if (_state != SS_DBWAIT_CREATE_ALIAS)
	{
		WARN("LoginConnexion") << "DB: Ignored late create alias from earlier state";
		return;
	}

	if (success)
	{
		if (bytes < 1) return;

		WARN("LoginConnexion") << "DB: Got create answer";

		switch (data[0])
		{
		case LDA_ERROR:
			WriteReliable(STREAM_1, LC_LOGIN_ERROR);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_BAD_ALIAS:
			WriteReliable(STREAM_1, LC_LOGIN_BAD_ALIAS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ALIAS_EXISTS:
			WriteReliable(STREAM_1, LC_ALIAS_EXISTS);
			_state = SS_LOGIN_REQUEST;
			break;

		case LDA_ALIAS_CREATED:
			if (bytes == 1+8)
			{
				WARN("LoginConnexion") << "DB: Got alias created";

				++data >> _alias_id;

				PostLoginChallenge(tls);
			}
			break;
		}
	}
	else // Request was canceled
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);

		WARN("LoginConnexion") << "DB: Login request was canceled";
	}
}
