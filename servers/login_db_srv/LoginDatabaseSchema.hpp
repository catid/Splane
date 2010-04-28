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

#ifndef CAT_LOGIN_DATABASE_SCHEMA_HPP
#define CAT_LOGIN_DATABASE_SCHEMA_HPP

#include <cat/AllFramework.hpp>
#include "LoginServerProtocol.hpp"

namespace cat {


/*
	Tables for Login Database:

	Account : User account with perhaps many aliases
		(Implicit Account ID #)
		(Implicit Deleted?)
		Name
		Password
		Public Key (or zero)
		Access level
		Last Ban IDs
			IP address
			15? ban ids
		Banned?

	Alias : Alias of a user
		(Implicit Alias ID #)
		(Implicit Deleted?)
		Account ID #
		Name

	Ban : Ban against a user
		(Implicit Ban ID #)
		(Implicit Deleted?)
		Account ID #
		Account name
		Creator name
		Creation date
		Ban IDs
			IP address
			IP address mask
			15? ban ids
		Expiration date

	Zone : Zone server account
		(Implicit Zone ID #)
		(Implicit Deleted?)
		Name
		Public Key
*/

DECL_BOMBAY_SCHEMA_VAR_FIELD_HASH(AccountSchemaNameHash);

DECL_BOMBAY_SCHEMA_VAR_FIELD_HASH(AliasSchemaNameHash);

DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaCdRomHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaMachGuidHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaUserNameHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaVolSerialHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaSystemUUIDHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaHDD0Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaHDD1Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaRegId0Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaRegId1Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaFileId0Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaFileId1Hash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaMacAddrHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaMotherboardHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaCompNameHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaProdIdHash);
DECL_BOMBAY_SCHEMA_FIXED_FIELD_HASH(BanSchemaProcessorHash);

DECL_BOMBAY_SCHEMA_VAR_FIELD_HASH(ZoneSchemaNameHash);

struct AccountSchema
{
	char name[USER_NAME_MAXLEN];
	u8 password[40];
	u8 public_key[64];
	DatabaseBanIds last_ids;
	u32 last_ip;
	u32 type;
};

struct AliasSchema
{
	char name[USER_NAME_MAXLEN];
	u64 account_id;
};

struct BanSchema
{
	char account_name[USER_NAME_MAXLEN];
	char creator_name[USER_NAME_MAXLEN];
	u64 creation_datetime; // UNIX timestamp
	u64 expiration_datetime; // UNIX timestamp
	DatabaseBanIds ids;
	u32 ip;
	u64 account_id;
};

struct ZoneSchema
{
	char name[ZONE_NAME_MAXLEN];
	u8 password[40];
	u8 public_key[64];
};


} // namespace cat

#endif // CAT_LOGIN_DATABASE_SCHEMA_HPP
