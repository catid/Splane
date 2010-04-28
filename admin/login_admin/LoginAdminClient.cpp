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

#include "LoginAdminClient.moc"
#include "LoginAdminForm.hpp"
#include "BanIdLabyrinth.hpp"
#include "BanIdentifiers.hpp"
using namespace std;
using namespace cat;
using namespace sphynx;

LoginAdminClient::LoginAdminClient(LoginAdminForm *form)
{
	connect(this, SIGNAL(DisplayMessageBox(QString)), form, SLOT(DisplayMessageBox(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(on_connect()), form, SLOT(on_connect()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_disconnect()), form, SLOT(on_disconnect()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_unknown_username()), form, SLOT(on_unknown_username()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_unknown_alias()), form, SLOT(on_unknown_alias()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_login_failure()), form, SLOT(on_login_failure()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_login_success()), form, SLOT(on_login_success()), Qt::QueuedConnection);
	connect(this, SIGNAL(on_add_player(QString)), form, SLOT(on_add_player(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(on_kill_player(QString)), form, SLOT(on_kill_player(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(on_chat_message(QString,QString)), form, SLOT(on_chat_message(QString,QString)), Qt::QueuedConnection);

	CAT_OBJCLR(_handlers);
	CAT_OBJCLR(_public_key);

	_handlers[LC_TAMPERING_DETECTED] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnTampering);
}

void LoginAdminClient::OnClose()
{
	WARN("AdminClient") << "-- SOCKET CLOSED";
	on_disconnect();
}

void LoginAdminClient::OnConnectFail()
{
	WARN("AdminClient") << "-- CONNECT FAIL";
	on_disconnect();
}

void LoginAdminClient::OnConnect(ThreadPoolLocalStorage *tls)
{
	WARN("AdminClient") << "-- CONNECTED";
	on_connect();

	_handlers[LC_LOGIN_BAD_NAME] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginBadName);
	_handlers[LC_LOGIN_BAD_ALIAS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginBadAlias);

	_auth_enc.GenerateProof(_proof, 32);
	WriteReliable(STREAM_1, LS_KEY_PROOF, _proof, sizeof(_proof));

	PostLogin();

	_handlers[LC_LOGIN_ERROR] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginError);
	_handlers[LC_LOGIN_BUSY] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginBusy);
	_handlers[LC_LOGIN_BANNED] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginBanned);
	_handlers[LC_LOGIN_UNKNOWN_NAME] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginUnknownName);
	_handlers[LC_LOGIN_UNKNOWN_ALIAS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginUnknownAlias);
	_handlers[LC_ALIAS_WRONG_ACCOUNT] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnAliasWrongAccount);
	_handlers[LC_LOGIN_KEYFILE_REQUIRED] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginKeyFileRequired);
	_handlers[LC_LOGIN_WRONG_KEYFILE] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginWrongKeyFile);
	_handlers[LC_LOGIN_CHALLENGE_NO_KEYFILE] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginChallengeNoKeyFile);
	_handlers[LC_LOGIN_CHALLENGE_HAS_KEYFILE] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginChallengeHasKeyFile);
	_handlers[LC_LOGIN_WRONG_PASSWORD] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginWrongPassword);
	_handlers[LC_LOGIN_BAD_SIGNATURE] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginBadSignature);
	_handlers[LC_LOGIN_SUCCESS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnLoginSuccess);
	_handlers[LC_CREATE_SUCCESS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnCreateSuccess);
	_handlers[LC_ACCOUNT_EXISTS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnAccountExists);
	_handlers[LC_ALIAS_EXISTS] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnAliasExists);
}

void LoginAdminClient::PostLogin()
{
	u8 login_request[256];
	BufferStream bs(login_request);

	bs.write(_public_key, sizeof(_public_key));
	bs.write(_username, 32);
	bs.write(_alias, 32);

	WriteReliable(STREAM_1, LS_LOGIN_REQUEST, login_request, bs.GetOffset(login_request));

	u8 opcode, banid[8+2];
	BanIdLabyrinth labyrinth;
	u32 len;

	ChaChaKey banid_key;
	banid_key.Set(_proof, 32);

	len = labyrinth.Cdrom(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.MachGuid(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.RegId1(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.UserName(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Canary1(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.VSN(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.FileId1(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.SystemUUID(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Canary2(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Harddrive1(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.MACAddr(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Motherboard(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.FileId2(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Harddrive2(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Canary3(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.RegId2(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.CompName(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.ProdId(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
	len = labyrinth.Processor(banid_key, banid, opcode); WriteReliable(STREAM_1, opcode, banid, len);
}

void LoginAdminClient::PostCreateAccount()
{
	FortunaOutput *csprng = FortunaFactory::ii->Create();
	if (!csprng)
	{
		WARN("AdminClient") << "Out of memory: Unable to create CSPRNG";
		on_disconnect();
		return;
	}

	u8 create_request[8+32];
	csprng->Generate(create_request, 8);

	GeneratePasswordHash(_username, create_request, 8, _password.c_str(), _password_hash, 32);
	memcpy(create_request + 8, _password_hash, 32);

	WriteReliable(STREAM_1, LS_CREATE_ACCOUNT, create_request, sizeof(create_request));

	delete csprng;
}

void LoginAdminClient::PostCreateAlias()
{
	WriteReliable(STREAM_1, LS_CREATE_ALIAS);
}

void LoginAdminClient::PostChat(const char *message)
{
	WriteReliable(STREAM_1, LS_CHAT, message, (u32)strlen(message));
}

void LoginAdminClient::SetLoginNoKey(const char *username, const char *alias, const char *password)
{
	CAT_STRNCPY(_username, username, sizeof(_username));
	CAT_STRNCPY(_alias, alias, sizeof(_alias));
	_password = password;

	CAT_OBJCLR(_public_key);
	CAT_OBJCLR(_private_key);
}
void LoginAdminClient::SetLoginHasKey(const char *username, const char *alias, const char *password, const u8 *public_key, const u8 *private_key)
{
	CAT_STRNCPY(_username, username, sizeof(_username));
	CAT_STRNCPY(_alias, alias, sizeof(_alias));
	_password = password;

	memcpy(_public_key, public_key, 64);
	memcpy(_private_key, private_key, 32);
}

void LoginAdminClient::OnDisconnect()
{
	WARN("AdminClient") << "-- DISCONNECTED";
	on_disconnect();
}

void LoginAdminClient::OnTimestampDeltaUpdate(u32 rtt, s32 delta)
{
	WARN("AdminClient") << "Got timestamp delta update rtt=" << rtt << " delta=" << delta;
}

void LoginAdminClient::OnMessage(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes > 0)
	{
		u8 opcode = msg[0];

		WARN("AdminClient") << "Got message type " << (u32)opcode;

		if (_handlers[opcode])
		{
			_handlers[opcode](tls, msg, bytes);
		}
		else
		{
			WARN("AdminClient") << "Ignored unexpected message type " << (u32)opcode;
		}
	}
}

void LoginAdminClient::OnTick(ThreadPoolLocalStorage *tls, u32 now)
{
}

void LoginAdminClient::OnTampering(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	DisplayMessageBox("Tampering Detected. Normally this would kill the client");
}

void LoginAdminClient::OnLoginError(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Database Error");
}
void LoginAdminClient::OnLoginBadName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Bad Name");
}
void LoginAdminClient::OnLoginBadAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Bad Alias");
}
void LoginAdminClient::OnLoginBusy(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Server is busy. Try again soon");
}
void LoginAdminClient::OnLoginBanned(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Banned");
}
void LoginAdminClient::OnLoginUnknownName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_unknown_username();
}
void LoginAdminClient::OnLoginUnknownAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_unknown_alias();
}
void LoginAdminClient::OnAliasWrongAccount(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Alias is taken by another user");
}
void LoginAdminClient::OnLoginKeyFileRequired(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Key File required to log in");
}
void LoginAdminClient::OnLoginWrongKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Wrong Key File provided");
}

void LoginAdminClient::OnLoginChallengeNoKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("AdminClient") << "No key file challenge";

	if (bytes != 1 + 8 + 32)
	{
		WARN("AdminClient") << "Corrupted packet";
		on_disconnect();
		return;
	}

	// type(1) salt(8) server_nonce(32)
	GeneratePasswordHash(_username, msg + 1, 8, _password.c_str(), _password_hash, 32);

	// type(1) client_nonce(32) MAC_password_proof(32) { server_nonce || client_nonce }
	u8 answer[32+32];
	tls->csprng->Generate(answer, 32);
	memcpy(_client_password_nonce, answer, 32);
	GeneratePasswordProof(_password_hash, 32, answer, 32, msg + 1 + 8, 32, answer+32, 32);

	WriteReliable(STREAM_1, LS_LOGIN_ANSWER_NO_KEYFILE, answer, sizeof(answer));
}
void LoginAdminClient::OnLoginChallengeHasKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("AdminClient") << "Has key file challenge";

	if (bytes != 1 + 8 + 32 + 32)
	{
		WARN("AdminClient") << "Corrupted packet";
		on_disconnect();
		return;
	}

	// type(1) salt(8) server_nonce(32) server_nonce'(32)
	u8 *salt = msg + 1;
	u8 *server_nonce = salt + 8;
	u8 *server_nonce_sig = server_nonce + 32;

	GeneratePasswordHash(_username, salt, 8, _password.c_str(), _password_hash, 32);

	// client_nonce(32) MAC_password_proof(32) { server_nonce || client_nonce } client_nonce'(32) Signature(64) { server_nonce' || client_nonce' }
	u8 answer[32+32+32+64];
	u8 *client_nonce = answer;
	u8 *mac_password_proof = client_nonce + 32;
	u8 *client_nonce_sig = mac_password_proof + 32;
	u8 *signature = client_nonce_sig + 32;

	tls->csprng->Generate(client_nonce, 32);
	memcpy(_client_password_nonce, client_nonce, 32);

	GeneratePasswordProof(_password_hash, 32, client_nonce, 32, server_nonce, 32, mac_password_proof, 32);

	tls->csprng->Generate(client_nonce_sig, 32);

	GenerateKeyProof(tls, _public_key, sizeof(_public_key), _private_key, sizeof(_private_key), client_nonce_sig, 32, server_nonce_sig, 32, signature, 64);

	WriteReliable(STREAM_1, LS_LOGIN_ANSWER_HAS_KEYFILE, answer, sizeof(answer));
}

void LoginAdminClient::OnLoginWrongPassword(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Wrong password");
}
void LoginAdminClient::OnLoginBadSignature(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Bad signature");
}

void LoginAdminClient::OnLoginSuccess(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes != 1 + 32 + 8 + 8 + 32)
	{
		WARN("AdminClient") << "Corrupted login success packet";
		on_disconnect();
		return;
	}

	if (!ValidServerPasswordProof(_password_hash, sizeof(_password_hash), _client_password_nonce, sizeof(_client_password_nonce), msg+1, 32))
	{
		WARN("AdminClient") << "Bad server password proof!  Please do not trust this server.";
		on_disconnect();
		DisplayMessageBox("Server failed to prove knowledge of your password.  Please do not trust this server.");
		return;
	}

	msg += 1 + 32;
	msg >> _account_id >> _alias_id >> _session_key;

	WARN("AdminClient") << "Logged in!";

	OnLogin();
}

void LoginAdminClient::OnCreateSuccess(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes != 1 + 8 + 8 + 32)
	{
		WARN("AdminClient") << "Corrupted create success packet";
		on_disconnect();
		return;
	}

	++msg >> _account_id >> _alias_id >> _session_key;

	WARN("AdminClient") << "Account create success.  Logged in!";

	OnLogin();
}

void LoginAdminClient::OnAccountExists(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Account name already exists. Choose a new Account name");
}

void LoginAdminClient::OnAliasExists(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	on_login_failure();
	DisplayMessageBox("Alias name already exists. Choose a new Alias name");
}

void LoginAdminClient::OnLogin()
{
	on_login_success();

	_handlers[LC_ADD_PLAYER] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnAddPlayer);
	_handlers[LC_KILL_PLAYER] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnKillPlayer);
	_handlers[LC_CHAT] = fastdelegate::MakeDelegate(this, &LoginAdminClient::OnChat);
}

void LoginAdminClient::OnAddPlayer(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes < 1 + 2 + 1)
	{
		WARN("AdminClient") << "Corrupted add player packet";
		on_disconnect();
		return;
	}

	u16 id;
	++msg >> id;

	Player &player = _player_list[id];

	SetFixedStr(player.name, USER_NAME_MAXLEN+1, msg.c_str(), bytes - 3);

	WARN("AdminClient") << "Player joined: " << player.name;

	on_add_player(player.name);
}

void LoginAdminClient::OnKillPlayer(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes != 1 + 2)
	{
		WARN("AdminClient") << "Corrupted kill player packet";
		on_disconnect();
		return;
	}

	u16 id;
	++msg >> id;

	Player &player = _player_list[id];

	WARN("AdminClient") << "Player left: " << player.name;

	on_kill_player(player.name);

	_player_list.erase(id);
}

void LoginAdminClient::OnChat(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	if (bytes < 1 + 2)
	{
		WARN("AdminClient") << "Corrupted chat packet";
		on_disconnect();
		return;
	}

	u32 msglen = bytes - 3;

	u16 id;
	++msg >> id;

	QString chatline = QString::fromAscii(msg.c_str(), msglen);

	Player &player = _player_list[id];

	WARN("AdminClient") << "<" << player.name << "> " << chatline.toAscii().data();

	on_chat_message(player.name, chatline);
}
