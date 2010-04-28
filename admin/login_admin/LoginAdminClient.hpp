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

#ifndef CAT_LOGIN_ADMIN_CLIENT_HPP
#define CAT_LOGIN_ADMIN_CLIENT_HPP

#include <QtCore/QVariant>
#include <cat/AllFramework.hpp>
#include "LoginServerProtocol.hpp"
#include <map>

namespace cat {


class LoginAdminForm;

typedef fastdelegate::FastDelegate3<ThreadPoolLocalStorage *, BufferStream, u32> ClientPacketHandler;

class LoginAdminClient : public QObject, public sphynx::Client
{
	Q_OBJECT

signals:
	void DisplayMessageBox(QString str);

	void on_connect();
	void on_disconnect();
	void on_unknown_username();
	void on_unknown_alias();
	void on_login_failure();
	void on_login_success();
	void on_add_player(QString str);
	void on_kill_player(QString str);
	void on_chat_message(QString name, QString message);

private:
	ClientPacketHandler _handlers[256];
	char _username[USER_NAME_MAXLEN+1];
	char _alias[USER_NAME_MAXLEN+1];
	std::string _password;
	u8 _password_hash[32];
	u8 _client_password_nonce[32];
	u8 _public_key[64], _private_key[32];
	u64 _account_id, _alias_id;
	u8 _session_key[32];
	u8 _proof[32];

	struct Player
	{
		char name[USER_NAME_MAXLEN+1];

		Player()
		{
			CAT_OBJCLR(name);
		}
	};
	std::map<u16, Player> _player_list;

	void OnLogin();

public:
	LoginAdminClient(LoginAdminForm *form);

	void SetLoginNoKey(const char *username, const char *alias, const char *password);
	void SetLoginHasKey(const char *username, const char *alias, const char *password, const u8 *public_key, const u8 *private_key);

protected:
	void OnClose();
	void OnConnectFail();
	void OnConnect(ThreadPoolLocalStorage *tls);
	void OnDisconnect();
	void OnTimestampDeltaUpdate(u32 rtt, s32 delta);
	void OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnTick(ThreadPoolLocalStorage *tls, u32 now);

protected:
	void OnTampering(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	void OnLoginBadName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginBadAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	void OnLoginError(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginBusy(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginBanned(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginUnknownName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginUnknownAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnAliasWrongAccount(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginKeyFileRequired(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginWrongKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginChallengeNoKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginChallengeHasKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	void OnLoginWrongPassword(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginBadSignature(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnLoginSuccess(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	void OnCreateSuccess(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnAccountExists(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnAliasExists(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

	void OnAddPlayer(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnKillPlayer(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);
	void OnChat(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes);

public:
	void PostLogin();
	void PostCreateAccount();
	void PostCreateAlias();
	void PostChat(const char *message);
};


} // namespace cat

#endif // CAT_LOGIN_ADMIN_CLIENT_HPP
