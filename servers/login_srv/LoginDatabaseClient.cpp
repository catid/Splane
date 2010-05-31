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

#include "LoginDatabaseClient.hpp"
#include "LoginServerProtocol.hpp"
#include "LoginServer.hpp"
using namespace cat;
using namespace sphynx;


//// Login Database Transaction

LoginTransaction::LoginTransaction()
{
	_client = 0;
	_next = 0;
	_prev = 0;
}

LoginTransaction::~LoginTransaction()
{
	Release();
}

void LoginTransaction::Release()
{
	if (_client)
	{
		_client->ForgetQuery(this);
		_client = 0;
	}
}


//// Login Database Client

LoginDatabaseClient::LoginDatabaseClient(LoginServer *owner)
{
	_owner = owner;

	_transaction_tail = 0;
	_transaction_head = 0;
	_next_transaction_id = 0;

	_state = CS_CHALLENGE;
}

LoginDatabaseClient::~LoginDatabaseClient()
{
	AutoMutex lock(_transaction_lock);

	for (LoginTransaction *transaction = _transaction_head; transaction; transaction = transaction->_next)
	{
		// Fail all of the callbacks
		transaction->_callback(0, false, BufferStream(0), 0);
	}
}

bool LoginDatabaseClient::Query(LoginTransaction *transaction, u8 *request, int bytes, const TransactionCallback &callback)
{
	if (!transaction || bytes <= DB_QUERY_OVERHEAD || !callback) return false;

	// If transaction is already busy, abort
	if (transaction->_client) return false;

	transaction->_client = this;
	transaction->_callback = callback;

	AutoMutex lock(_transaction_lock);

		transaction->_next = 0;
		transaction->_prev = _transaction_tail;
		if (_transaction_tail) _transaction_tail->_next = transaction;
		else _transaction_head = transaction;
		_transaction_tail = transaction;

		u32 id = _next_transaction_id++;

		transaction->_id = id;

	lock.Release();

	*reinterpret_cast<u32*>( request ) = getLE(id);

	WARN("DatabaseClient") << "Query sent " << id;

	// Add a reference to this client
	AddRef();

	bool success = WriteReliable(STREAM_UNORDERED, LDS_TRANSACTION_QUERY, request, bytes);
	if (!success) ForgetQuery(transaction);

	return success;
}

void LoginDatabaseClient::ForgetQuery(LoginTransaction *transaction)
{
	if (transaction && transaction->_client)
	{
		WARN("DatabaseClient") << "Query forgot " << transaction->_id;

		AutoMutex lock(_transaction_lock);

		if (transaction->_client)
		{
			transaction->_client = 0;

			LoginTransaction *next = transaction->_next;
			LoginTransaction *prev = transaction->_prev;

			if (next) next->_prev = prev;
			else _transaction_tail = prev;
			if (prev) prev->_next = next;
			else _transaction_head = next;

			ReleaseRef();
		}
	}
}

void LoginDatabaseClient::AnswerQuery(ThreadPoolLocalStorage *tls, u32 id, BufferStream answer, int bytes)
{
	WARN("DatabaseClient") << "Query answer " << id;

	AutoMutex lock(_transaction_lock);

	for (LoginTransaction *transaction = _transaction_head; transaction; transaction = transaction->_next)
	{
		if (transaction->_id == id)
		{
			transaction->_client = 0;

			LoginTransaction *next = transaction->_next;
			LoginTransaction *prev = transaction->_prev;

			if (next) next->_prev = prev;
			else _transaction_tail = prev;
			if (prev) prev->_next = next;
			else _transaction_head = next;

			// Cache callback to avoid potential race conditions
			TransactionCallback callback = transaction->_callback;

			lock.Release();

			// First parameter true to indicate success
			callback(tls, true, answer, bytes);

			ReleaseRef();
			return;
		}
	}
}

void LoginDatabaseClient::ConnectToDatabase(ThreadPoolLocalStorage *tls)
{
	const char *hostname = Settings::ref()->getStr("Database.Address", "127.0.0.1");
	const char *port_str = Settings::ref()->getStr("Database.Port", "23000");
	const char *public_key_base64 = Settings::ref()->getStr("Database.PublicKey", "Put key here");

	u8 public_key[PUBLIC_KEY_BYTES];

	if (sizeof(public_key) != ReadBase64(public_key_base64, (int)strlen(public_key_base64), public_key, sizeof(public_key)))
	{
		WARN("DatabaseClient") << "Unable to log into Database Server.  Database.PublicKey in settings is not set or invalid";
		Disconnect();
		return;
	}

	if (!SetServerKey(tls, public_key, sizeof(public_key), "LoginDB"))
	{
		WARN("DatabaseClient") << "Unable to log into Database Server.  Database.PublicKey in settings is invalid";
		Disconnect();
		return;
	}

	Connect(hostname, atoi(port_str));
}

void LoginDatabaseClient::OnClose()
{
	WARN("DatabaseClient") << "-- SOCKET CLOSED";
}

void LoginDatabaseClient::OnConnectFail(sphynx::HandshakeError err)
{
	WARN("DatabaseClient") << "-- CONNECT FAIL: " << GetHandshakeErrorString(err);
}

void LoginDatabaseClient::OnConnect(ThreadPoolLocalStorage *tls)
{
	WARN("DatabaseClient") << "-- CONNECTED";

	u8 req[32+64];
	_auth_enc.GenerateProof(req, 32);
	memcpy(req+32, _owner->_database_access_public_key, 64);

	if (!WriteReliable(STREAM_1, LDS_LOGIN, req, sizeof(req)))
	{
		FATAL("DatabaseClient") << "Unable to post login packet to database server";
		Disconnect();
		return;
	}
}

void LoginDatabaseClient::OnDisconnect()
{
	WARN("DatabaseClient") << "-- DISCONNECTED";
}

void LoginDatabaseClient::OnTimestampDeltaUpdate(u32 rtt, s32 delta)
{
	WARN("DatabaseClient") << "Got timestamp delta update rtt=" << rtt << " delta=" << delta;
}

void LoginDatabaseClient::OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes < 1) return;

	u8 opcode = msg[0];

	if (opcode == LDC_TAMPERING_DETECTED)
	{
		FATAL("DatabaseClient") << "Database Server closed connexion for packet tampering";
		Disconnect();
		return;
	}

	switch (_state)
	{
	case CS_CHALLENGE:
		switch (opcode)
		{
		case LDC_WRONG_KEY:
			if (bytes == 1)
			{
				FATAL("DatabaseClient") << "Database Server rejected our access key!  Make sure the correct key is set in settings.txt under Database.Key";
				Disconnect();
				return;
			}
			break;

		case LDC_CHALLENGE:
			if (bytes == 1+32 && OnChallenge(tls, msg, bytes))
			{
				return;
			}
			break;
		}
		break;

	case CS_LOGIN_WAIT:
		switch (opcode)
		{
		case LDC_BAD_SIGNATURE:
			if (bytes == 1)
			{
				FATAL("DatabaseClient") << "Database Server rejected our access key(2)!  Make sure the correct key is set in settings.txt under Database.Key";
				Disconnect();
				return;
			}
			break;

		case LDC_SUCCESS:
			if (bytes == 1)
			{
				INFO("DatabaseClient") << "Successfully logged into the database server";
				_state = CS_READY;
				return;
			}
			break;
		}
		break;

	case CS_READY:
		if (opcode == LDS_TRANSACTION_ANSWER && bytes >= 1+4+1)
		{
			u32 id = getLE(*reinterpret_cast<u32*>( msg+1 ));
			AnswerQuery(tls, id, BufferStream(msg+1+4), bytes-1-4);
			return;
		}
		break;
	}

	WARN("DatabaseClient") << "Disconnecting database server for malformed packet " << (u32)opcode;
	Disconnect();
}

void LoginDatabaseClient::OnTick(ThreadPoolLocalStorage *tls, u32 now)
{
}

bool LoginDatabaseClient::OnChallenge(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes)
{
	WARN("DatabaseClient") << "Got access key challenge from database server";

	u8 answer[32+64];
	tls->csprng->Generate(answer, 32);

	u8 signed_message[32+32];
	memcpy(signed_message, answer, 32);
	memcpy(signed_message+32, msg+1, 32);

	if (!_owner->_database_access_signer.Sign(tls->math, tls->csprng, signed_message, 32+32, answer+32, 64))
	{
		WARN("DatabaseClient") << "Unable to log into Database Server.  Signature creation failed";
		Disconnect();
		return true;
	}

	if (!WriteReliable(STREAM_1, LDS_ANSWER, answer, sizeof(answer)))
	{
		WARN("DatabaseClient") << "Unable to log into Database Server.  Write failure";
		Disconnect();
		return true;
	}

	_state = CS_LOGIN_WAIT;

	return true;
}
