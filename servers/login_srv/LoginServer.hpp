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

#ifndef CAT_LOGIN_SERVER_HPP
#define CAT_LOGIN_SERVER_HPP

#include <cat/AllFramework.hpp>
#include "LoginServerProtocol.hpp"
#include "LoginDatabaseClient.hpp"

namespace cat {


//// LoginConnexion

class LoginServer;

class LoginConnexion : public sphynx::Connexion
{
	LoginServer *_server;

	enum SessionState
	{
		SS_KEY_PROOF,
		SS_LOGIN_REQUEST,
		SS_LOGIN_BANID_CDROM,
		SS_LOGIN_BANID_MACHINE_GUID,
		SS_LOGIN_BANID_REG_ID1,
		SS_LOGIN_BANID_USER_NAME,
		SS_LOGIN_BANID_CANARY1,
		SS_LOGIN_BANID_VSN,
		SS_LOGIN_BANID_FILE_ID1,
		SS_LOGIN_BANID_SYSTEM_UUID,
		SS_LOGIN_BANID_CANARY2,
		SS_LOGIN_BANID_HARDDRIVE1,
		SS_LOGIN_BANID_MAC_ADDR,
		SS_LOGIN_BANID_MOTHERBOARD,
		SS_LOGIN_BANID_FILE_ID2,
		SS_LOGIN_BANID_HARDDRIVE2,
		SS_LOGIN_BANID_CANARY3,
		SS_LOGIN_BANID_REG_ID2,
		SS_LOGIN_BANID_COMP_NAME,
		SS_LOGIN_BANID_PROD_ID,
		SS_LOGIN_BANID_PROCESSOR,
		SS_DBWAIT_LOGIN,
		SS_CREATE_ACCOUNT,
		SS_DBWAIT_CREATE_ACCOUNT,
		SS_CREATE_ALIAS,
		SS_DBWAIT_CREATE_ALIAS,
		SS_CHALLENGE_ANSWER_NO_KEY,
		SS_CHALLENGE_ANSWER_HAS_KEY,
		SS_LOGIN_SUCCESS
	} _state;

	enum SecondaryState
	{
		S2_GROOVY,
		S2_TAMPERING
	} _state2;

	ChaChaKey _banid_key;

	LoginTransaction _login_query;

protected:
	u8 _public_key[sphynx::PUBLIC_KEY_BYTES];
	char _account_name[USER_NAME_MAXLEN];
	char _alias_name[USER_NAME_MAXLEN];
	DatabaseBanIds _ban_ids;
	u8 _account_password_salt[PASSWORD_SALT_BYTES];
	u8 _account_password_hash[PASSWORD_HASH_BYTES];
	u32 _account_type;
	u8 _server_nonce_pass[32], _server_nonce_key[32];

	u64 _account_id;
	u64 _alias_id;
	u8 _session_key[32];

public:
	LoginConnexion(LoginServer *server);
	virtual ~LoginConnexion();

protected:
	virtual void OnConnect(ThreadPoolLocalStorage *tls);
	virtual void OnDestroy();
	virtual void OnDisconnect();
	virtual void OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	virtual void OnTick(ThreadPoolLocalStorage *tls, u32 now);

protected:
	virtual void OnLoginComplete();

protected:
	// Unlocked by a valid key proof:
	bool OnLoginRequest(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdCdRom(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdMachineGuid(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdRegId1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdUserName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdCanary1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdVSN(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdFileId1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdSystemUUID(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdCanary2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdHardDrive1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdMACAddr(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdMotherboard(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdFileId2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdHardDrive2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdCanary3(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdRegId2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdCompName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdProdId(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginBanIdProcessor(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	// Database server responses to LDQ_ACCOUNT_LOGIN:
	void OnDBLoginAnswer(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes);
	void OnDBLoginData(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes);
	void OnDBUnknownAlias(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes);

	void PostLoginChallenge(ThreadPoolLocalStorage *tls);

	// Database server responses to LDQ_ACCOUNT_CREATE:
	void OnDBCreateAnswer(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes);
	void OnDBCreateData(ThreadPoolLocalStorage *tls, BufferStream data, u32 bytes);

	// Database server responses to LDQ_ALIAS_CREATE:
	void OnDBCreateAlias(ThreadPoolLocalStorage *tls, bool success, BufferStream data, u32 bytes);

	// C_LOGIN_UNKNOWN_NAME response:
	bool OnCreateAccount(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	// C_LOGIN_UNKNOWN_ALIAS response:
	bool OnCreateAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	// C_LOGIN_CHALLENGE* responses:
	bool OnLoginAnswerNoKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnLoginAnswerHasKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	// Unlocked by a successful login:
	bool OnChat(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	bool OnZoneEnter(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
};


//// LoginServer

class LoginServer : protected sphynx::Server
{
	friend class LoginDatabaseClient;

	LoginDatabaseClient *_database_uplink;
	Mutex _uplink_lock;

	u8 _database_access_public_key[sphynx::PUBLIC_KEY_BYTES];
	KeyAgreementResponder _database_access_signer;

	bool GenerateNewDatabaseKeyPair(ThreadPoolLocalStorage *tls, u8 *private_key, int private_bytes);

public:
	sphynx::Collexion<LoginConnexion> _chatters;

public:
	LoginServer();
	virtual ~LoginServer();

protected:
	virtual sphynx::Connexion *NewConnexion();
	virtual bool AcceptNewConnexion(const NetAddr &src);

public:
	bool Initialize(ThreadPoolLocalStorage *tls);
	void Shutdown();
	void Pause();
	void Continue();

protected:
	LoginDatabaseClient *GetUplink(ThreadPoolLocalStorage *tls);

public:
	bool Query(ThreadPoolLocalStorage *tls, LoginTransaction &transaction,
			   u8 *data, int bytes, const TransactionCallback &callback);
};


} // namespace cat

#endif // CAT_LOGIN_SERVER_HPP
