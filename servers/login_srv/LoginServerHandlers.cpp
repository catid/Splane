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
#include "BanIdLabyrinth.hpp"
using namespace cat;
using namespace cat::sphynx;

// Unlocked by a valid key proof:

bool LoginConnexion::OnLoginRequest(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginRequest";

	// Validate name
	char *name = msg.c_str() + 1 + 64;
	int name_len = GetFixedStrLen(name, USER_NAME_MAXLEN);

	if (!ValidLoginServerUserName(name, name_len))
	{
		WriteReliable(STREAM_1, LC_LOGIN_BAD_NAME);
		return true;
	}

	// Validate alias
	char *alias = name + USER_NAME_MAXLEN;
	int alias_len = GetFixedStrLen(alias, USER_NAME_MAXLEN);

	if (!ValidLoginServerUserName(alias, alias_len))
	{
		WriteReliable(STREAM_1, LC_LOGIN_BAD_ALIAS);
		return true;
	}

	// Copy message data into connexion object
	SetFixedStr(_account_name, sizeof(_account_name), name, name_len);
	SetFixedStr(_alias_name, sizeof(_alias_name), alias, alias_len);
	memcpy(_public_key, msg+1, sizeof(_public_key));

	// Expect ban ids next
	_state = SS_LOGIN_BANID_CDROM;
	return true;
}
bool LoginConnexion::OnLoginBanIdCdRom(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdCdRom";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::Cdrom(id, crc))
			_ban_ids.cdrom = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_MACHINE_GUID;
	return true;
}
bool LoginConnexion::OnLoginBanIdMachineGuid(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdMachineGuid";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::MachGuid(id, crc))
			_ban_ids.mach_guid = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_REG_ID1;
	return true;
}
bool LoginConnexion::OnLoginBanIdRegId1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdRegId1";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u16 crc;
		u64 id;
		++msg >> crc >> id;

		if (BanIdTheseus::RegId1(id, crc))
			_ban_ids.reg_id[0] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_USER_NAME;
	return true;
}
bool LoginConnexion::OnLoginBanIdUserName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdUserName";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u32 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::UserName(id, crc))
			_ban_ids.user_name = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_CANARY1;
	return true;
}
bool LoginConnexion::OnLoginBanIdCanary1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdCanary1";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (!BanIdTheseus::Canary1(id, crc))
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_VSN;
	return true;
}
bool LoginConnexion::OnLoginBanIdVSN(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdVSN";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u32 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::VSN(id, crc))
			_ban_ids.vol_serial = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_FILE_ID1;
	return true;
}
bool LoginConnexion::OnLoginBanIdFileId1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdFileId1";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u16 crc;
		u64 id;
		++msg >> crc >> id;

		if (BanIdTheseus::FileId1(id, crc))
			_ban_ids.file_id[0] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_SYSTEM_UUID;
	return true;
}
bool LoginConnexion::OnLoginBanIdSystemUUID(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdSystemUUID";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::SystemUUID(id, crc))
			_ban_ids.system_uuid = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_CANARY2;
	return true;
}
bool LoginConnexion::OnLoginBanIdCanary2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdCanary2";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u32 id;
		u16 crc;
		++msg >> id >> crc;

		if (!BanIdTheseus::Canary2(id, crc))
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_HARDDRIVE1;
	return true;
}
bool LoginConnexion::OnLoginBanIdHardDrive1(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdHardDrive1";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::Harddrive1(id, crc))
			_ban_ids.harddrive[0] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_MAC_ADDR;
	return true;
}
bool LoginConnexion::OnLoginBanIdMACAddr(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdMACAddr";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::MACAddr(id, crc))
			_ban_ids.mac_addr = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_MOTHERBOARD;
	return true;
}
bool LoginConnexion::OnLoginBanIdMotherboard(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdMotherboard";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u16 crc;
		u64 id;
		++msg >> crc >> id;

		if (BanIdTheseus::Motherboard(id, crc))
			_ban_ids.motherboard = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_FILE_ID2;
	return true;
}
bool LoginConnexion::OnLoginBanIdFileId2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdFileId2";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::FileId2(id, crc))
			_ban_ids.file_id[1] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_HARDDRIVE2;
	return true;
}
bool LoginConnexion::OnLoginBanIdHardDrive2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdHardDrive2";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::Harddrive2(id, crc))
			_ban_ids.harddrive[1] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_CANARY3;
	return true;
}
bool LoginConnexion::OnLoginBanIdCanary3(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdCanary3";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u16 crc;
		u64 id;
		++msg >> crc >> id;

		if (!BanIdTheseus::Canary3(id, crc))
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_REG_ID2;
	return true;
}
bool LoginConnexion::OnLoginBanIdRegId2(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdRegId2";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::RegId2(id, crc))
			_ban_ids.reg_id[1] = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_COMP_NAME;
	return true;
}
bool LoginConnexion::OnLoginBanIdCompName(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdCompName";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u32 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::CompName(id, crc))
			_ban_ids.comp_name = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_PROD_ID;
	return true;
}
bool LoginConnexion::OnLoginBanIdProdId(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdProdId";

	if (_state2 == S2_GROOVY)
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u16 crc;
		u64 id;
		++msg >> crc >> id;

		if (BanIdTheseus::ProdId(id, crc))
			_ban_ids.prod_id = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_LOGIN_BANID_PROCESSOR;
	return true;
}
bool LoginConnexion::OnLoginBanIdProcessor(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	//WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginBanIdProcessor";

	if (_state2 != S2_GROOVY)
		return false;
	else
	{
		ChaChaOutput cipher(_banid_key, msg[0]);
		cipher.Crypt(msg+1, msg+1, bytes-1);

		u64 id;
		u16 crc;
		++msg >> id >> crc;

		if (BanIdTheseus::Processor(id, crc))
			_ban_ids.processor = id;
		else
			_state2 = S2_TAMPERING;
	}

	_state = SS_DBWAIT_LOGIN;
	_state2 = S2_GROOVY;

	// Construct query
	u8 resp[DB_QUERY_OVERHEAD + 512];
	BufferStream bs_resp(resp + DB_QUERY_OVERHEAD);

	bs_resp << (u8)LDQ_ACCOUNT_LOGIN;
	bs_resp.write(&_ban_ids, sizeof(DatabaseBanIds));
	bs_resp << _client_addr.GetIP4();
	bs_resp.write(_account_name, sizeof(_account_name));
	bs_resp.write(_alias_name, sizeof(_alias_name));

	// Post query
	if (!_server->Query(tls, _login_query, resp, bs_resp.GetOffset(resp),
		fastdelegate::MakeDelegate(this, &LoginConnexion::OnDBLoginAnswer)))
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);
		_state = SS_LOGIN_REQUEST;
	}

	return true;
}

// C_LOGIN_UNKNOWN_NAME response:

bool LoginConnexion::OnCreateAccount(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got CreateAccount";

	_state = SS_DBWAIT_CREATE_ACCOUNT;
	_state2 = S2_GROOVY;

	++msg >> _account_password_salt >> _account_password_hash;

	// Construct query
	u8 resp[DB_QUERY_OVERHEAD + 512];
	BufferStream bs_resp(resp + DB_QUERY_OVERHEAD);

	bs_resp << (u8)LDQ_ACCOUNT_CREATE;
	bs_resp.write(&_ban_ids, sizeof(DatabaseBanIds));
	bs_resp << _client_addr.GetIP4();
	bs_resp.write(_account_password_salt, PASSWORD_SALT_BYTES);
	bs_resp.write(_account_password_hash, PASSWORD_HASH_BYTES);
	bs_resp.write(_public_key, PUBLIC_KEY_BYTES);
	bs_resp.write(_account_name, sizeof(_account_name));
	bs_resp.write(_alias_name, sizeof(_alias_name));

	// Post query
	if (!_server->Query(tls, _login_query, resp, bs_resp.GetOffset(resp),
		fastdelegate::MakeDelegate(this, &LoginConnexion::OnDBCreateAnswer)))
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);
		_state = SS_LOGIN_REQUEST;
	}

	return true;
}

// C_LOGIN_UNKNOWN_ALIAS response:

bool LoginConnexion::OnCreateAlias(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got CreateAlias";

	_state = SS_DBWAIT_CREATE_ALIAS;
	_state2 = S2_GROOVY;

	// Construct query
	u8 resp[DB_QUERY_OVERHEAD + 256];
	BufferStream bs_resp(resp + DB_QUERY_OVERHEAD);

	bs_resp << (u8)LDQ_ALIAS_CREATE << _account_id;
	bs_resp.write(_alias_name, sizeof(_alias_name));

	// Post query
	if (!_server->Query(tls, _login_query, resp, bs_resp.GetOffset(resp),
		fastdelegate::MakeDelegate(this, &LoginConnexion::OnDBCreateAlias)))
	{
		WriteReliable(STREAM_1, LC_LOGIN_BUSY);
		_state = SS_LOGIN_REQUEST;
	}

	return true;
}

// C_LOGIN_CHALLENGE* responses:

bool LoginConnexion::OnLoginAnswerNoKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginAnswerNoKeyFile";

	if (!ValidPasswordProof(_account_password_hash, sizeof(_account_password_hash), msg+1, 32,
							_server_nonce_pass, sizeof(_server_nonce_pass), msg+1+32, 32))
	{
		_state = SS_LOGIN_REQUEST;

		WriteReliable(STREAM_1, LC_LOGIN_WRONG_PASSWORD);
	}
	else
	{
		_state = SS_LOGIN_SUCCESS;

		tls->csprng->Generate(_session_key, sizeof(_session_key));

		u8 pkt[256];
		BufferStream success(pkt);

		GenerateServerPasswordProof(_account_password_hash, sizeof(_account_password_hash), msg+1, 32, success, 32);
		success += 32;
		success << _account_id << _alias_id << _session_key;

		WriteReliable(STREAM_1, LC_LOGIN_SUCCESS, pkt, success.GetOffset(pkt));

		OnLoginComplete();
	}

	return true;
}
bool LoginConnexion::OnLoginAnswerHasKeyFile(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got LoginAnswerHasKeyFile";

	// client_nonce(32) MAC_password_proof(32) { server_nonce || client_nonce } client_nonce'(32) Signature(64) { server_nonce' || client_nonce' }
	u8 *client_nonce = msg + 1;
	u8 *mac_password_proof = client_nonce + 32;
	u8 *client_nonce_sig = mac_password_proof + 32;
	u8 *signature = client_nonce_sig + 32;

	_state = SS_LOGIN_REQUEST;

	if (!ValidPasswordProof(_account_password_hash, sizeof(_account_password_hash), client_nonce, 32,
							_server_nonce_pass, sizeof(_server_nonce_pass), mac_password_proof, 32))
	{
		WriteReliable(STREAM_1, LC_LOGIN_WRONG_PASSWORD);
	}
	else
	{
		if (!ValidateKeyProof(tls, _public_key, sizeof(_public_key), client_nonce_sig, 32,
						   	  _server_nonce_key, sizeof(_server_nonce_key), signature, 64))
		{
			WriteReliable(STREAM_1, LC_LOGIN_BAD_SIGNATURE);
		}
		else
		{
			_state = SS_LOGIN_SUCCESS;

			tls->csprng->Generate(_session_key, sizeof(_session_key));

			u8 pkt[256];
			BufferStream success(pkt);

			GenerateServerPasswordProof(_account_password_hash, sizeof(_account_password_hash), client_nonce, 32, success, 32);
			success += 32;
			success << _account_id << _alias_id << _session_key;

			WriteReliable(STREAM_1, LC_LOGIN_SUCCESS, pkt, success.GetOffset(pkt));

			OnLoginComplete();
		}
	}

	return true;
}

// Unlocked by a successful login:

void LoginConnexion::OnLoginComplete()
{
	_state = SS_LOGIN_SUCCESS;

	// Insert player into list
	_server->_chatters.Insert(this);

	// Construct message for notifying other players of the new one
	u8 new_msg[2 + USER_NAME_MAXLEN];
	BufferStream new_bs(new_msg);

	new_bs << (u16)GetKey();
	u32 new_namelen = SetFixedStr((char*)new_msg + 2, USER_NAME_MAXLEN, _alias_name, USER_NAME_MAXLEN);

	// Notify user and others that he has connected
	for (sphynx::CollexionIterator<LoginConnexion> ii = _server->_chatters; ii; ++ii)
	{
		// construct message for notifying player about other players
		u8 msg[2 + USER_NAME_MAXLEN];
		BufferStream bs(msg);

		bs << (u16)ii->GetKey();
		u32 namelen = SetFixedStr((char*)msg + 2, USER_NAME_MAXLEN, ii->_alias_name, USER_NAME_MAXLEN);

		WriteReliable(STREAM_2, LC_ADD_PLAYER, msg, 2 + namelen);
		if (ii != this) ii->WriteReliable(STREAM_2, LC_ADD_PLAYER, new_msg, 2 + new_namelen);
	}
}

bool LoginConnexion::OnChat(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got Chat";

	u32 msglen = bytes - 1;
	if (msglen > CHAT_MAX_CHARS)
		msglen = CHAT_MAX_CHARS;

	u8 broadcast[2 + CHAT_MAX_CHARS];
	BufferStream bs(broadcast);

	bs << (u16)GetKey();
	bs.write(msg + 1, msglen);

	for (sphynx::CollexionIterator<LoginConnexion> ii = _server->_chatters; ii; ++ii)
		ii->WriteReliable(STREAM_2, LC_CHAT, broadcast, bs.GetOffset(broadcast));

	return true;
}

bool LoginConnexion::OnZoneEnter(ThreadPoolLocalStorage *tls, BufferStream msg, u32 bytes)
{
	WARN("LoginConnexion") << _client_addr.IPToString() << ':' << _client_addr.GetPort() << " : Got ZoneEnter";

	// TODO
	return true;
}
