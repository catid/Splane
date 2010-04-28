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


//// LoginDatabaseConnexion

LoginDatabaseConnexion::LoginDatabaseConnexion(LoginDatabaseServer *server)
{
	_state = SS_LOGIN;
	_server = server;
}

LoginDatabaseConnexion::~LoginDatabaseConnexion()
{

}

void LoginDatabaseConnexion::OnConnect(ThreadPoolLocalStorage *tls)
{
	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Connected.";
}

void LoginDatabaseConnexion::OnDestroy()
{
	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Destroyed.";
}

void LoginDatabaseConnexion::OnDisconnect()
{
	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Disconnected.";
	Destroy();
}

void LoginDatabaseConnexion::OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes <= 0) return;

	u8 opcode = msg[0];

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got message " << (u32)opcode << HexDumpString(msg, bytes);

	switch (_state)
	{
	case SS_LOGIN:
		if (opcode == LDS_LOGIN && bytes == 1+32+64 && OnLogin(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_CHALLENGE:
		if (opcode == LDS_ANSWER && bytes == 1+32+64 && OnAnswer(tls, msg, bytes))
		{
			return;
		}
		break;

	case SS_READY:
		if (opcode == LDS_TRANSACTION_QUERY && bytes >= 1+4+1 && OnQuery(tls, msg, bytes))
		{
			return;
		}
		break;
	}

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Message tampering detected type " << (u32)opcode;

	WriteUnreliable(LDC_TAMPERING_DETECTED);
	FlushWrite();
	Destroy();
}

bool LoginDatabaseConnexion::OnLogin(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes)
{
	if (!_auth_enc.ValidateProof(msg+1, 32))
	{
		WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Invalid proof of key";
		return false;
	}

	if (!SecureEqual(_server->_access_public_key, msg+1+32, 64))
	{
		WriteReliable(STREAM_1, LDC_WRONG_KEY);

		WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Login public key mismatch";
		return true;
	}

	u8 challenge[32];
	tls->csprng->Generate(_server_nonce, 32);
	memcpy(challenge, _server_nonce, 32);
	WriteReliable(STREAM_1, LDC_CHALLENGE, challenge, sizeof(challenge));

	_state = SS_CHALLENGE;

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got login";

	return true;
}

bool LoginDatabaseConnexion::OnAnswer(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes)
{
	u8 buffer[32+32];

	// Copy client nonce and server nonce into buffer
	memcpy(buffer, msg+1, 32);
	memcpy(buffer+32, _server_nonce, 32);

	if (!_server->_signature_check.Verify(tls->math, buffer, sizeof(buffer), msg+1+32, 64))
	{
		WriteReliable(STREAM_1, LDC_BAD_SIGNATURE);

		WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Bad login attempt";
		return true;
	}

	_state = SS_READY;

	WriteReliable(STREAM_1, LDC_SUCCESS);

	WARN("DatabaseConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got answer.  Login server is connected";

	return true;
}

void LoginDatabaseConnexion::OnTick(ThreadPoolLocalStorage *tls, u32 now)
{

}


//// LoginDatabaseServer

LoginDatabaseServer::LoginDatabaseServer()
	: _shutdown_wait(REFOBJ_PRIO_0 + 20) // Lowest priority
{
	_account_table = 0;
	_alias_table = 0;
	_ban_table = 0;
	_zone_table = 0;

	_account_name_index = 0;
}

LoginDatabaseServer::~LoginDatabaseServer()
{
	Cleanup();
}

void LoginDatabaseServer::Cleanup()
{
	ThreadRefObject::SafeRelease(_account_table);
	ThreadRefObject::SafeRelease(_alias_table);
	ThreadRefObject::SafeRelease(_ban_table);
	ThreadRefObject::SafeRelease(_zone_table);
}

bool LoginDatabaseServer::Initialize()
{
	AddRef();

#define FAILCHK(statement) if (!(statement)) return false;

	// Create table objects
	static const int MIN_CACHE_SIZE = 1000000;

	FAILCHK(_account_table = new bombay::Table("s_account.db", sizeof(AccountSchema),
		BoundMin(MIN_CACHE_SIZE, Settings::ii->getInt("Table.Account.CacheSize", 4000000)), _shutdown_wait.GetObserver()));
	FAILCHK(_account_name_index = _account_table->MakeIndex<AccountSchemaNameHash>("u_account_name.idx", true));

	FAILCHK(_alias_table = new bombay::Table("u_alias.db", sizeof(AliasSchema),
		BoundMin(MIN_CACHE_SIZE, Settings::ii->getInt("Table.Alias.CacheSize", 2000000)), _shutdown_wait.GetObserver()));
	FAILCHK(_alias_name_index = _alias_table->MakeIndex<AliasSchemaNameHash>("u_alias_name.idx", true));

	FAILCHK(_ban_table = new bombay::Table("u_ban.db", sizeof(BanSchema),
		BoundMin(MIN_CACHE_SIZE, Settings::ii->getInt("Table.Ban.CacheSize", 1000000)), _shutdown_wait.GetObserver()));
	FAILCHK(_ban_cdrom_index = _ban_table->MakeIndex<BanSchemaCdRomHash>("u_ban_id0.idx", false));
	FAILCHK(_ban_mach_guid_index = _ban_table->MakeIndex<BanSchemaMachGuidHash>("u_ban_id1.idx", false));
	FAILCHK(_ban_user_name_index = _ban_table->MakeIndex<BanSchemaUserNameHash>("u_ban_id2.idx", false));
	FAILCHK(_ban_vol_serial_index = _ban_table->MakeIndex<BanSchemaVolSerialHash>("u_ban_id3.idx", false));
	FAILCHK(_ban_system_uuid_index = _ban_table->MakeIndex<BanSchemaSystemUUIDHash>("u_ban_id4.idx", false));
	FAILCHK(_ban_hdd0_index = _ban_table->MakeIndex<BanSchemaHDD0Hash>("u_ban_id5.idx", false));
	FAILCHK(_ban_hdd1_index = _ban_table->MakeIndex<BanSchemaHDD1Hash>("u_ban_id6.idx", false));
	FAILCHK(_ban_reg0_index = _ban_table->MakeIndex<BanSchemaRegId0Hash>("u_ban_id7.idx", false));
	FAILCHK(_ban_reg1_index = _ban_table->MakeIndex<BanSchemaRegId1Hash>("u_ban_id8.idx", false));
	FAILCHK(_ban_file0_index = _ban_table->MakeIndex<BanSchemaFileId0Hash>("u_ban_id9.idx", false));
	FAILCHK(_ban_file1_index = _ban_table->MakeIndex<BanSchemaFileId1Hash>("u_ban_id10.idx", false));
	FAILCHK(_ban_mac_addr_index = _ban_table->MakeIndex<BanSchemaMacAddrHash>("u_ban_id11.idx", false));
	FAILCHK(_ban_motherboard_index = _ban_table->MakeIndex<BanSchemaMotherboardHash>("u_ban_id12.idx", false));
	FAILCHK(_ban_comp_name_index = _ban_table->MakeIndex<BanSchemaCompNameHash>("u_ban_id13.idx", false));
	FAILCHK(_ban_prod_id_index = _ban_table->MakeIndex<BanSchemaProdIdHash>("u_ban_id14.idx", false));
	FAILCHK(_ban_processor_index = _ban_table->MakeIndex<BanSchemaProcessorHash>("u_ban_id15.idx", false));

	FAILCHK(_zone_table = new bombay::Table("s_zone.db", sizeof(ZoneSchema),
		BoundMin(MIN_CACHE_SIZE, Settings::ii->getInt("Table.Zone.CacheSize", 1000000)), _shutdown_wait.GetObserver()));
	FAILCHK(_zone_name_index = _zone_table->MakeIndex<ZoneSchemaNameHash>("u_zone_name.idx", true));

	// Initialize the tables, kicking off indexing
	bool success = true;
	success &= _account_table->Initialize();
	success &= _alias_table->Initialize();
	success &= _ban_table->Initialize();
	success &= _zone_table->Initialize();

	if (!success)
	{
		WARN("LoginDatabase") << "Unable to initialize one of the database tables";
		return false;
	}

	Port port = (Port)Settings::ii->getInt("Login.Database.Port", 23000);
	if (!port)
	{
		WARN("LoginDatabase") << "Invalid port specified in settings file";
		return false;
	}

	ThreadPoolLocalStorage tls;
	u8 public_key[PUBLIC_KEY_BYTES];
	u8 private_key[PRIVATE_KEY_BYTES];
	const char *SessionKey = "LoginDB";

	if (!GenerateKeyPair(&tls, "LoginDatabasePublic.txt", "LoginDatabasePrivate.key", public_key, sizeof(public_key), private_key, sizeof(private_key)))
	{
		WARN("LoginDatabase") << "Unable to get key pair";
		return false;
	}

	if (!StartServer(&tls, port, public_key, sizeof(public_key), private_key, sizeof(private_key), SessionKey))
	{
		WARN("LoginDatabase") << "Unable to start server";
		return false;
	}

	const char *access_key_base64 = Settings::ref()->getStr("Database.Access.PublicKey", "Put key here");

	if (PUBLIC_KEY_BYTES != ReadBase64(access_key_base64, (int)strlen(access_key_base64), _access_public_key, sizeof(_access_public_key)))
	{
		WARN("LoginDatabase") << "Database Access Key uninitialized.  Unable to accept connections";
		return false;
	}

	if (!_signature_check.Initialize(tls.math, _access_public_key, sizeof(_access_public_key)))
	{
		WARN("LoginDatabase") << "Unable to initialize signature checking";
		return false;
	}

	return true;
}

void LoginDatabaseServer::Shutdown()
{
	// Kill server
	Close();

	// Kill tables
	Cleanup();

	static const u32 SHUTDOWN_WAIT_MAX = 30000; // 30 seconds

	// Wait for shutdown
	if (!_shutdown_wait.WaitForShutdown(SHUTDOWN_WAIT_MAX))
	{
		WARN("LoginDatabase") << "Wait failed for table cleanup!  Data may have been lost";
	}

	// And make sure it gets deleted
	ReleaseRef();
}

void LoginDatabaseServer::Pause()
{

}

void LoginDatabaseServer::Continue()
{

}

sphynx::Connexion *LoginDatabaseServer::NewConnexion()
{
	return new LoginDatabaseConnexion(this);
}

bool LoginDatabaseServer::AcceptNewConnexion(const NetAddr &src)
{
	return true; // allow all
}
