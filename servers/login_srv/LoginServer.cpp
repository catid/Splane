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
using namespace sphynx;


//// LoginConnexion

LoginConnexion::LoginConnexion(LoginServer *server)
{
	_server = server;

	_state = SS_KEY_PROOF;
	_state2 = S2_GROOVY;
}

LoginConnexion::~LoginConnexion()
{
}

void LoginConnexion::OnConnect(ThreadPoolLocalStorage *tls)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Connected.";
}

void LoginConnexion::OnDestroy()
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Destroyed connexion.";

	if (_server->_chatters.Remove(this))
	{
		// Notify others that he has disconnected
		for (sphynx::CollexionIterator<LoginConnexion> ii = _server->_chatters; ii; ++ii)
		{
			// construct message for notifying player about other players
			u8 msg[2];
			BufferStream bs(msg);

			bs << (u16)GetKey();

			ii->WriteReliable(STREAM_2, LC_KILL_PLAYER, msg, bs.GetOffset(msg));
		}
	}
}

void LoginConnexion::OnDisconnect()
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Disconnected from server.";
	Destroy();
}

void LoginConnexion::OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes <= 0) return;

	u8 opcode = msg[0];

	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got message " << (u32)opcode << HexDumpString(msg, bytes);

	switch (_state)
	{
	case SS_KEY_PROOF:
		if (opcode == LS_KEY_PROOF &&
			bytes == 1+32 &&
			_auth_enc.ValidateProof(msg+1, 32))
		{
			_banid_key.Set(msg+1, 32);

			_state = SS_LOGIN_REQUEST;
			return;
		}
		break;

	case SS_LOGIN_REQUEST:
		switch (opcode)
		{
		case LS_LOGIN_REQUEST:
			if (bytes == 1+64+32+32 && OnLoginRequest(tls, msg, bytes))
				return;
			break;

		default:
			return; // Ignore all other opcodes without error
		}
		break;

	case SS_LOGIN_BANID_CDROM:
		if (opcode == LS_LOGIN_BANID_CDROM &&
			bytes == 1+8+2 &&
			OnLoginBanIdCdRom(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_MACHINE_GUID:
		if (opcode == LS_LOGIN_BANID_MACH_GUID &&
			bytes == 1+8+2 &&
			OnLoginBanIdMachineGuid(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_REG_ID1:
		if (opcode == LS_LOGIN_BANID_REG_ID1 &&
			bytes == 1+2+8 &&
			OnLoginBanIdRegId1(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_USER_NAME:
		if (opcode == LS_LOGIN_BANID_USER_NAME &&
			bytes == 1+4+2 &&
			OnLoginBanIdUserName(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_CANARY1:
		if (opcode == LS_LOGIN_BANID_CANARY1 &&
			bytes == 1+8+2 &&
			OnLoginBanIdCanary1(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_VSN:
		if (opcode == LS_LOGIN_BANID_VSN &&
			bytes == 1+4+2 &&
			OnLoginBanIdVSN(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_FILE_ID1:
		if (opcode == LS_LOGIN_BANID_FILE_ID1 &&
			bytes == 1+2+8 &&
			OnLoginBanIdFileId1(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_SYSTEM_UUID:
		if (opcode == LS_LOGIN_BANID_SYSTEM_UUID &&
			bytes == 1+8+2 &&
			OnLoginBanIdSystemUUID(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_CANARY2:
		if (opcode == LS_LOGIN_BANID_CANARY2 &&
			bytes == 1+4+2 &&
			OnLoginBanIdCanary2(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_HARDDRIVE1:
		if (opcode == LS_LOGIN_BANID_HARDDRIVE1 &&
			bytes == 1+8+2 &&
			OnLoginBanIdHardDrive1(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_MAC_ADDR:
		if (opcode == LS_LOGIN_BANID_MAC_ADDR &&
			bytes == 1+8+2 &&
			OnLoginBanIdMACAddr(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_MOTHERBOARD:
		if (opcode == LS_LOGIN_BANID_MOTHERBOARD &&
			bytes == 1+2+8 &&
			OnLoginBanIdMotherboard(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_FILE_ID2:
		if (opcode == LS_LOGIN_BANID_FILE_ID2 &&
			bytes == 1+8+2 &&
			OnLoginBanIdFileId2(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_HARDDRIVE2:
		if (opcode == LS_LOGIN_BANID_HARDDRIVE2 &&
			bytes == 1+8+2 &&
			OnLoginBanIdHardDrive2(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_CANARY3:
		if (opcode == LS_LOGIN_BANID_CANARY3 &&
			bytes == 1+2+8 &&
			OnLoginBanIdCanary3(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_REG_ID2:
		if (opcode == LS_LOGIN_BANID_REG_ID2 &&
			bytes == 1+8+2 &&
			OnLoginBanIdRegId2(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_COMP_NAME:
		if (opcode == LS_LOGIN_BANID_COMP_NAME &&
			bytes == 1+4+2 &&
			OnLoginBanIdCompName(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_PROD_ID:
		if (opcode == LS_LOGIN_BANID_PROD_ID &&
			bytes == 1+2+8 &&
			OnLoginBanIdProdId(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_BANID_PROCESSOR:
		if (opcode == LS_LOGIN_BANID_PROCESSOR &&
			bytes == 1+8+2 &&
			OnLoginBanIdProcessor(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_DBWAIT_LOGIN:
		break;

	case SS_CREATE_ACCOUNT:
		if (opcode == LS_CREATE_ACCOUNT &&
			bytes == 1+8+32 &&
			OnCreateAccount(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_DBWAIT_CREATE_ACCOUNT:
		break;

	case SS_CREATE_ALIAS:
		if (opcode == LS_CREATE_ALIAS &&
			bytes == 1 &&
			OnCreateAlias(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_DBWAIT_CREATE_ALIAS:
		break;

	case SS_CHALLENGE_ANSWER_NO_KEY:
		if (opcode == LS_LOGIN_ANSWER_NO_KEYFILE &&
			bytes == 1+32+32 &&
			OnLoginAnswerNoKeyFile(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_CHALLENGE_ANSWER_HAS_KEY:
		if (opcode == LS_LOGIN_ANSWER_HAS_KEYFILE &&
			bytes == 1+32+32+32+64 &&
			OnLoginAnswerHasKeyFile(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_LOGIN_SUCCESS:
		if (opcode == LS_CHAT &&
			OnChat(tls, msg, bytes))
		{
			return;
		}
		break;
	}

	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Message tampering detected type " << (u32)opcode;

	WriteUnreliable(LC_TAMPERING_DETECTED);
	FlushWrite();
	Destroy();
}

void LoginConnexion::OnTick(ThreadPoolLocalStorage *tls, u32 now)
{

}

void LoginConnexion::PostLoginChallenge(ThreadPoolLocalStorage *tls)
{
	u8 opcode, pkt[8 + 32 + 32];
	BufferStream challenge(pkt);

	if (IsZeroFixedBuffer(_public_key, sizeof(_public_key)))
	{
		tls->csprng->Generate(_server_nonce_pass, sizeof(_server_nonce_pass));

		opcode = LC_LOGIN_CHALLENGE_NO_KEYFILE;
		challenge << _account_password_salt << _server_nonce_pass;

		_state = SS_CHALLENGE_ANSWER_NO_KEY;
	}
	else
	{
		tls->csprng->Generate(_server_nonce_pass, sizeof(_server_nonce_pass));
		tls->csprng->Generate(_server_nonce_key, sizeof(_server_nonce_key));

		opcode = LC_LOGIN_CHALLENGE_HAS_KEYFILE;
		challenge << _account_password_salt << _server_nonce_pass << _server_nonce_key;

		_state = SS_CHALLENGE_ANSWER_HAS_KEY;
	}

	WriteReliable(STREAM_1, opcode, pkt, challenge.GetOffset(pkt));
}


//// LoginServer

LoginServer::LoginServer()
{
	_database_uplink = 0;
}

LoginServer::~LoginServer()
{
	ThreadRefObject::SafeRelease(_database_uplink);
}

bool LoginServer::Initialize(ThreadPoolLocalStorage *tls)
{
	AddRef();

	// Database access key

	const char *access_public_key_base64 = Settings::ref()->getStr("Database.Access.Public");
	const char *access_private_key_base64 = Settings::ref()->getStr("Database.Access.Private");

	u8 access_private_key[PRIVATE_KEY_BYTES];

	if (sizeof(_database_access_public_key) != ReadBase64(access_public_key_base64, (int)strlen(access_public_key_base64), _database_access_public_key, sizeof(_database_access_public_key)) ||
		sizeof(access_private_key) != ReadBase64(access_private_key_base64, (int)strlen(access_private_key_base64), access_private_key, sizeof(access_private_key)))
	{
		if (!GenerateNewDatabaseKeyPair(tls, access_private_key, sizeof(access_private_key)))
		{
			WARN("LoginServer") << "Unable to generate database key pair";
			return false;
		}
		else
		{
			INFO("LoginServer") << "Generated a new database access key pair.  It is in the settings file";
		}
	}

	if (!_database_access_signer.Initialize(tls->math, tls->csprng, _database_access_public_key, sizeof(_database_access_public_key), access_private_key, sizeof(access_private_key)))
	{
		WARN("LoginServer") << "Database access key is invalid.  Delete Database.Access.* from the settings file and restart the server to create a new key";
		return false;
	}

	// Start server

	Port port = (Port)Settings::ii->getInt("Login.Server.Port", 22000);
	if (!port)
	{
		WARN("LoginServer") << "Invalid port specified in settings file";
		return false;
	}

	u8 public_key[sphynx::PUBLIC_KEY_BYTES];
	u8 private_key[sphynx::PRIVATE_KEY_BYTES];
	const char *SessionKey = "Login";

	if (!GenerateKeyPair(tls, "LoginServerPublic.txt", "LoginServerPrivate.key", public_key, sizeof(public_key), private_key, sizeof(private_key)))
	{
		WARN("LoginServer") << "Unable to get key pair";
		return false;
	}

	if (!StartServer(tls, port, public_key, sizeof(public_key), private_key, sizeof(private_key), SessionKey))
	{
		WARN("LoginServer") << "Unable to start server";
		return false;
	}

	// Try to start database client before first user connects
	LoginDatabaseClient *uplink = GetUplink(tls);
	if (uplink) uplink->ReleaseRef();

	return true;
}

void LoginServer::Shutdown()
{
	// Kill server
	Close();

	// And make sure it gets deleted
	ReleaseRef();
}

void LoginServer::Pause()
{

}

void LoginServer::Continue()
{

}

sphynx::Connexion *LoginServer::NewConnexion()
{

	WARN("TEST") << sizeof(LoginConnexion);
	return new LoginConnexion(this);
}

bool LoginServer::AcceptNewConnexion(const NetAddr &src)
{
	return true; // allow all
}

LoginDatabaseClient *LoginServer::GetUplink(ThreadPoolLocalStorage *tls)
{
	AutoMutex lock(_uplink_lock);

	// If database uplink died somehow,
	if (_database_uplink && _database_uplink->IsClosed())
	{
		ThreadRefObject::SafeRelease(_database_uplink);
	}

	// If uplink does not exist,
	if (!_database_uplink)
	{
		// Create a new one
		_database_uplink = new LoginDatabaseClient(this);
		if (!_database_uplink) return 0;

		// Add a reference to keep it in scope
		_database_uplink->AddRef();

		// Start connecting
		_database_uplink->ConnectToDatabase(tls);
	}

	// Add reference for this request: Caller must ->ReleaseRef() when done
	_database_uplink->AddRef();
	return _database_uplink;
}

bool LoginServer::Query(ThreadPoolLocalStorage *tls, LoginTransaction &transaction, u8 *data, int bytes, const TransactionCallback &callback)
{
	bool success = false;

	LoginDatabaseClient *ldc = GetUplink(tls);

	if (ldc)
	{
		if (ldc->IsReady())
		{
			success = ldc->Query(&transaction, data, bytes, callback);
		}

		ldc->ReleaseRef();
	}

	return success;
}

bool LoginServer::GenerateNewDatabaseKeyPair(ThreadPoolLocalStorage *tls, u8 *private_key, int private_bytes)
{
	KeyMaker bob;
	if (!bob.GenerateKeyPair(tls->math, tls->csprng, _database_access_public_key, sizeof(_database_access_public_key), private_key, private_bytes))
	{
		FATAL("LoginServer") << "Unable to generate a database key pair for some reason!";
		return false;
	}

	char public_key_base64[256], private_key_base64[256];

	int public_bytes64 = WriteBase64Str(_database_access_public_key, sizeof(_database_access_public_key), public_key_base64, sizeof(public_key_base64));
	int private_bytes64 = WriteBase64Str(private_key, private_bytes, private_key_base64, sizeof(private_key_base64));

	if (!public_bytes64 || !private_bytes64)
	{
		FATAL("LoginServer") << "Unable to encode database keys in Base64";
		return false;
	}

	Settings::ref()->setStr("Database.Access.Public", public_key_base64);
	Settings::ref()->setStr("Database.Access.Private", private_key_base64);

	// Flush new keys to disk right away so that the Login Database
	// server can be updated without restarting the Login server
	Settings::ref()->write();

	return true;
}
