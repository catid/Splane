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

#ifndef CAT_LOGIN_DATABASE_SERVER_HPP
#define CAT_LOGIN_DATABASE_SERVER_HPP

#include <cat/AllFramework.hpp>
#include "LoginServerProtocol.hpp"
#include "LoginDatabaseSchema.hpp"

namespace cat {


class LoginDatabaseServer;

class LoginDatabaseConnexion : public sphynx::Connexion
{
	LoginDatabaseServer *_server;

	u8 _server_nonce[32];

	enum SessionState
	{
		SS_LOGIN,
		SS_CHALLENGE,
		SS_READY
	} _state;

public:
	LoginDatabaseConnexion(LoginDatabaseServer *server);
	virtual ~LoginDatabaseConnexion();

protected:
	virtual void OnConnect(ThreadPoolLocalStorage *tls);
	virtual void OnDestroy();
	virtual void OnDisconnect();
	virtual void OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	virtual void OnTick(ThreadPoolLocalStorage *tls, u32 now);

protected:
	virtual bool OnLogin(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes);
	virtual bool OnAnswer(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes);
	virtual bool OnQuery(ThreadPoolLocalStorage *tls, u8 *msg, u32 bytes);

protected:
	virtual bool OnQueryLogin(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream bs, u32 bytes);
	bool OnReadLoginAccount(ThreadPoolLocalStorage *tls, int error, AsyncBuffer *buffer, u32 bytes);
	bool OnReadLoginAlias(ThreadPoolLocalStorage *tls, int error, AsyncBuffer *buffer, u32 bytes);
	virtual bool OnQueryCreateAccount(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream bs, u32 bytes);
	virtual bool OnQueryAliasCreate(ThreadPoolLocalStorage *tls, u32 query_id, BufferStream bs, u32 bytes);
};

class LoginDatabaseServer : protected sphynx::Server
{
	friend class LoginDatabaseConnexion;

private:
	u8 _access_public_key[sphynx::PUBLIC_KEY_BYTES];
	KeyAgreementInitiator _signature_check;

	ShutdownWait _shutdown_wait;

private:
	bombay::Table *_account_table, *_alias_table, *_ban_table, *_zone_table;

	bombay::TableIndex *_account_name_index, *_alias_name_index, *_zone_name_index;
	bombay::TableIndex *_ban_cdrom_index, *_ban_mach_guid_index, *_ban_user_name_index;
	bombay::TableIndex *_ban_vol_serial_index, *_ban_system_uuid_index;
	bombay::TableIndex *_ban_hdd0_index, *_ban_hdd1_index;
	bombay::TableIndex *_ban_reg0_index, *_ban_reg1_index;
	bombay::TableIndex *_ban_file0_index, *_ban_file1_index;
	bombay::TableIndex *_ban_mac_addr_index, *_ban_motherboard_index, *_ban_comp_name_index;
	bombay::TableIndex *_ban_prod_id_index, *_ban_processor_index;

public:
	LoginDatabaseServer();
	virtual ~LoginDatabaseServer();

	void Cleanup();

protected:
	virtual sphynx::Connexion *NewConnexion();
	virtual bool AcceptNewConnexion(const NetAddr &src);

public:
	bool Initialize();
	void Shutdown();
	void Pause();
	void Continue();
};


} // namespace cat

#endif // CAT_LOGIN_DATABASE_SERVER_HPP
