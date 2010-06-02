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

#ifndef CAT_LOGIN_SERVER_PROTOCOL_HPP
#define CAT_LOGIN_SERVER_PROTOCOL_HPP

#include <cat/AllFramework.hpp>

namespace cat {


// Returns true if user name is valid for login
bool ValidLoginServerUserName(const void *name, int bytes);

// Password hash utility functions
void GeneratePasswordHash(const char *username, const void *salt, int salt_bytes, const char *password, void *password_hash, int password_hash_bytes);

void GeneratePasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes,
						   const void *server_nonce, int server_nonce_bytes, void *proof_hash, int proof_hash_bytes);
bool ValidPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes,
						const void *server_nonce, int server_nonce_bytes, const void *proof_hash, int proof_hash_bytes);

// Server password hash utilities
void GenerateServerPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes, void *proof_hash, int proof_hash_bytes);
bool ValidServerPasswordProof(const void *password_hash, int password_hash_bytes, const void *client_nonce, int client_nonce_bytes, const void *proof_hash, int proof_hash_bytes);

// Key proof utilities
bool GenerateKeyProof(ThreadPoolLocalStorage *tls, const u8 *public_key, int public_key_bytes, const u8 *private_key, int private_key_bytes,
					  const void *client_nonce, int client_nonce_bytes, const void *server_nonce, int server_nonce_bytes, u8 *signature, int signature_bytes);
bool ValidateKeyProof(ThreadPoolLocalStorage *tls, const u8 *public_key, int public_key_bytes, const void *client_nonce, int client_nonce_bytes,
				   const void *server_nonce, int server_nonce_bytes, const u8 *signature, int signature_bytes);


// Login name limits
static const u32 USER_NAME_MINLEN = 1;
static const u32 USER_NAME_MAXLEN = 32;

static const u32 ZONE_NAME_MAXLEN = 64;

static const int PASSWORD_STRENGTHENING_ROUNDS = 1000;

static const u32 PASSWORD_SALT_BYTES = 8;
static const u32 PASSWORD_HASH_BYTES = 32;

static const u32 CHAT_MAX_CHARS = 256;

#pragma pack(push)
#pragma pack(4)

struct DatabaseBanIds
{
	// Low bit of each ban id in this structure indicates whether or
	// not it is a strong id.  1= Strong.  0= Weak.
	u64 cdrom;
	u64 mach_guid;
	u32 user_name;
	u32 vol_serial;
	u64 system_uuid;
	u64 harddrive[2];
	u64 reg_id[2];
	u64 file_id[2];
	u64 mac_addr;
	u64 motherboard;
	u32 comp_name;
	u64 prod_id;
	u64 processor;
};

#pragma pack(pop)

enum AccountType
{
	ACCTTYPE_DELETED,
	ACCTTYPE_BANNED,
	ACCTTYPE_USER,
	ACCTTYPE_MOD,
	ACCTTYPE_SMOD,
	ACCTTYPE_SYSOP,
	ACCTTYPE_NETOP
};


enum LoginDatabaseQueries
{
	LDQ_ACCOUNT_LOGIN = 0x35,			// [S0] ban_ids(sizeof DatabaseBanIds) IP(4) name(32) alias(32)
	LDQ_ACCOUNT_CREATE = 0x7d,			// [S0] ban_ids(sizeof DatabaseBanIds) IP(4) salt(8) password_hash(32) public_key(64) name(32) alias(32)
	LDQ_ALIAS_CREATE = 0x2f				// [S0] account_id(8) alias(32)
};

enum LoginDatabaseAnswers
{
	// LDQ_ACCOUNT_LOGIN, LDQ_ACCOUNT_CREATE, LDQ_ALIAS_CREATE response:
	LDA_ERROR = 0x6b,					// [S0] -
	LDA_BAD_ALIAS = 0xf1,				// [S0] -

	// LDQ_ACCOUNT_LOGIN response:
	LDA_UNKNOWN_NAME = 0xa3,			// [S0] -
	LDA_ALIAS_WRONG_ACCOUNT = 0x64,		// [S0] -
	LDA_ACCOUNT_DATA = 0x29,			// [S0] account_id(8) alias_id(8) access(4) salt(8) password_hash(32) public_key(64)
	LDA_UNKNOWN_ALIAS = 0x9e,			// [S0] account_id(8) access(4) salt(8) password_hash(32) public_key(64)

	// LDQ_ACCOUNT_LOGIN, LDQ_ACCOUNT_CREATE response:
	LDA_BAD_NAME = 0x83,				// [S0] -
	LDA_BANNED = 0x12,					// [S0] - 

	// LDQ_ACCOUNT_CREATE response:
	LDA_ACCOUNT_EXISTS = 0xcf,			// [S0] -
	LDA_ACCOUNT_CREATED = 0x32,			// [S0] account_id(8) alias_id(8) access(4)

	// LDQ_ACCOUNT_CREATE, LDQ_ALIAS_CREATE response:
	LDA_ALIAS_EXISTS = 0x58,			// [S0] -

	// LDQ_ALIAS_CREATE response:
	LDA_ALIAS_CREATED = 0x4c			// [S0] alias_id(8)
};

static const int DB_QUERY_OVERHEAD = 4; // Includes request_id(4)

enum LoginDatabaseServerOpCodes
{
	// LoginServer proof of key and public key confirmation (always first message):
	LDS_LOGIN = 0xcc,					// [S1] MAC_key_proof(32) public_key(64)

	// LDC_CHALLENGE response:
	LDS_ANSWER = 0xde,					// [S1] client_nonce(32) Signature(64) { client_nonce || server_nonce }

	// Unlocked by LDC_SUCCESS:
	LDS_TRANSACTION_QUERY = 0xad		// [S0] request_id(4) LDQ_*(1) data(x)
};

enum LoginDatabaseClientOpCodes
{
	LDC_WRONG_KEY = 0xc0,				// [S1] -
	LDC_CHALLENGE = 0x1c,				// [S1] server_nonce(32)

	// LDS_LOGIN_ANSWER response:
	LDC_BAD_SIGNATURE = 0xf2,			// [S1] -
	LDC_SUCCESS = 0xe7,					// [S1] -

	// Unlocked by LDC_SUCCESS:
	LDS_TRANSACTION_ANSWER = 0x21		// [S0] request_id(4) LDA_*(1) data(x)
};

enum LoginServerOpCodes
{
	// Client proof of key (always first message):
	LS_KEY_PROOF = 0xd6,				// [S1] MAC_key_proof(32)

	// Client login protocol:
	LS_LOGIN_REQUEST = 0x6f,			// [S1] public_key(64) name(32) alias(32)
	LS_LOGIN_BANID_CDROM = 0x78,		// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_MACH_GUID = 0xa1,	// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_REG_ID1 = 0x09,		// [S1] crc(2) hash(8)
	LS_LOGIN_BANID_USER_NAME = 0x83,	// [S1] hash(4) crc(2)
	LS_LOGIN_BANID_CANARY1 = 0x86,		// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_VSN = 0xdd,			// [S1] hash(4) crc(2)
	LS_LOGIN_BANID_FILE_ID1 = 0x43,		// [S1] crc(2) hash(8)
	LS_LOGIN_BANID_SYSTEM_UUID = 0x1e,	// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_CANARY2 = 0x1d,		// [S1] hash(4) crc(2)
	LS_LOGIN_BANID_HARDDRIVE1 = 0xc7,	// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_MAC_ADDR = 0x2a,		// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_MOTHERBOARD = 0x32,	// [S1] crc(2) hash(8)
	LS_LOGIN_BANID_FILE_ID2 = 0xe6,		// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_HARDDRIVE2 = 0xd0,	// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_CANARY3 = 0x05,		// [S1] crc(2) hash(8)
	LS_LOGIN_BANID_REG_ID2 = 0x3b,		// [S1] hash(8) crc(2)
	LS_LOGIN_BANID_COMP_NAME = 0xb4,	// [S1] hash(4) crc(2)
	LS_LOGIN_BANID_PROD_ID = 0xf0,		// [S1] crc(2) hash(8)
	LS_LOGIN_BANID_PROCESSOR = 0x28,	// [S1] hash(8) crc(2)

	// C_LOGIN_UNKNOWN_NAME response:
	LS_CREATE_ACCOUNT = 0x87,			// [S1] salt(8) password_hash(32)

	// C_LOGIN_UNKNOWN_ALIAS response:
	LS_CREATE_ALIAS = 0xb9,				// [S1] -

	// C_LOGIN_CHALLENGE* responses:
	LS_LOGIN_ANSWER_NO_KEYFILE = 0x40,	// [S1] client_nonce(32) MAC_password_proof(32) { server_nonce || client_nonce }
	LS_LOGIN_ANSWER_HAS_KEYFILE = 0x7d,	// [S1] client_nonce(32) MAC_password_proof(32) { server_nonce || client_nonce } client_nonce'(32) Signature(64) { server_nonce' || client_nonce' }

	// Unlocked by a successful login:
	LS_CHAT = 0x07,						// [S2] text(x)
	LS_ZONE_ENTER = 0xc2,				// [S0] TODO

	// Zone server login protocol:
	LS_ZONE_LOGIN_REQUEST = 0x95,		// [S1] public_key(64) name(x)
	LS_ZONE_LOGIN_ANSWER = 0xf2			// [S1] client_nonce(32) MAC_password_proof(32) { client_nonce || server_nonce } client_nonce'(32) Signature(64) { client_nonce' || server_nonce' }
};

enum LoginClientOpCodes
{
	// LS_LOGIN_REQUEST responses:
	LC_LOGIN_BAD_NAME = 0xfc,			// [S1] -
	LC_LOGIN_BAD_ALIAS = 0x2a,			// [S1] -

	// LS_LOGIN_BANID_PROCESSOR responses:
	LC_LOGIN_ERROR = 0xa1,				// [S1] -
	LC_LOGIN_BUSY = 0x63,				// [S1] -
	LC_LOGIN_BANNED = 0x1d,				// [S1] -
	LC_LOGIN_UNKNOWN_NAME = 0x7a,		// [S1] -
	LC_LOGIN_UNKNOWN_ALIAS = 0x25,		// [S1] -
	LC_LOGIN_KEYFILE_REQUIRED = 0x32,	// [S1] -
	LC_LOGIN_WRONG_KEYFILE = 0x05,		// [S1] -
	LC_LOGIN_CHALLENGE_NO_KEYFILE = 0x5e,	// [S1] salt(8) server_nonce(32)
	LC_LOGIN_CHALLENGE_HAS_KEYFILE = 0xb4,	// [S1] salt(8) server_nonce(32) server_nonce'(32)
	LC_ALIAS_WRONG_ACCOUNT = 0x39,		// [S1] -

	// LS_LOGIN_ANSWER* responses:
	LC_LOGIN_WRONG_PASSWORD = 0xb1,		// [S1] -
	LC_LOGIN_BAD_SIGNATURE = 0xcd,		// [S1] -
	LC_LOGIN_SUCCESS = 0x9a,			// [S1] MAC_server_password_proof(32) { client_nonce } account_id(8) alias_id(8) session_key(32)

	// LS_CREATE_ACCOUNT responses:
	LC_CREATE_SUCCESS = 0xe2,			// [S1] account_id(8) alias_id(8) session_key(32)
	LC_ACCOUNT_EXISTS = 0x0d,			// [S1] -
	LC_ALIAS_EXISTS = 0x50,				// [S1] -

	// Unlocked by a successful login:
	LC_ADD_PLAYER = 0x47,				// [S2] player_id(2) alias(x)
	LC_KILL_PLAYER = 0x20,				// [S2] player_id(2)
	LC_CHAT = 0xce,						// [S2] player_id(2) message(x)
	LC_ZONE_LIST = 0x4b,				// [S2] TODO
	LC_ZONE_UPDATE = 0x56				// [S2] TODO
};


} // namespace cat

#endif // CAT_LOGIN_SERVER_PROTOCOL_HPP
