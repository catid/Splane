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

#include "LoginDatabaseSchema.hpp"
using namespace cat;


//// Account Schema:

static const u64 ACCOUNT_NAME_HASH_SALT = 0xbadDECAFdeadBEEF;

static CAT_INLINE u64 Hash64(const void *data, u32 bytes, u64 salt)
{
	u64 hash = MurmurHash64(data, bytes, ACCOUNT_NAME_HASH_SALT);
	if (hash == 0) hash = ~(u64)0;
	return hash;
}

u64 AccountSchemaNameHash::HashVarField(const void *just_field, u32 bytes)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, bytes, ACCOUNT_NAME_HASH_SALT);
}
u64 AccountSchemaNameHash::HashField(const void *just_field)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, GetFixedStrLen(name, USER_NAME_MAXLEN), ACCOUNT_NAME_HASH_SALT);
}
u64 AccountSchemaNameHash::HashComplete(const void *complete_record)
{
	const AccountSchema *schema = reinterpret_cast<const AccountSchema*>( complete_record );
	const char *name = schema->name;
	if (!*name) return 0;
	return Hash64(name, GetFixedStrLen(name, USER_NAME_MAXLEN), ACCOUNT_NAME_HASH_SALT);
}


//// Alias Schema:

static const u64 ALIAS_NAME_HASH_SALT = 0x0123456789abcdef;

u64 AliasSchemaNameHash::HashVarField(const void *just_field, u32 bytes)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, bytes, ALIAS_NAME_HASH_SALT);
}
u64 AliasSchemaNameHash::HashField(const void *just_field)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, GetFixedStrLen(name, USER_NAME_MAXLEN), ALIAS_NAME_HASH_SALT);
}
u64 AliasSchemaNameHash::HashComplete(const void *complete_record)
{
	const AliasSchema *schema = reinterpret_cast<const AliasSchema*>( complete_record );
	const char *name = schema->name;
	if (!*name) return 0;
	return Hash64(name, GetFixedStrLen(name, USER_NAME_MAXLEN), ALIAS_NAME_HASH_SALT);
}


//// Ban Schema:

#define DEFINE_BAN_SCHEMA_HASH(T, SZ, MEM) \
u64 T::HashField(const void *just_field) \
{ \
	const SZ *id = reinterpret_cast<const SZ*>( just_field ); \
	u64 hash = *id >> 1; \
	if (!hash) hash = 1; \
	return hash; \
} \
u64 T::HashComplete(const void *complete_record) \
{ \
	const BanSchema *schema = reinterpret_cast<const BanSchema*>( complete_record ); \
	u64 hash = schema->ids.MEM >> 1; \
	if (!hash) hash = 1; \
	return hash; \
}

DEFINE_BAN_SCHEMA_HASH(BanSchemaCdRomHash, u64, cdrom);
DEFINE_BAN_SCHEMA_HASH(BanSchemaMachGuidHash, u64, mach_guid);
DEFINE_BAN_SCHEMA_HASH(BanSchemaUserNameHash, u32, user_name);
DEFINE_BAN_SCHEMA_HASH(BanSchemaVolSerialHash, u32, vol_serial);
DEFINE_BAN_SCHEMA_HASH(BanSchemaSystemUUIDHash, u64, system_uuid);
DEFINE_BAN_SCHEMA_HASH(BanSchemaHDD0Hash, u64, harddrive[0]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaHDD1Hash, u64, harddrive[1]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaRegId0Hash, u64, reg_id[0]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaRegId1Hash, u64, reg_id[1]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaFileId0Hash, u64, file_id[0]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaFileId1Hash, u64, file_id[1]);
DEFINE_BAN_SCHEMA_HASH(BanSchemaMacAddrHash, u64, mac_addr);
DEFINE_BAN_SCHEMA_HASH(BanSchemaMotherboardHash, u64, motherboard);
DEFINE_BAN_SCHEMA_HASH(BanSchemaCompNameHash, u32, comp_name);
DEFINE_BAN_SCHEMA_HASH(BanSchemaProdIdHash, u64, prod_id);
DEFINE_BAN_SCHEMA_HASH(BanSchemaProcessorHash, u64, processor);


//// Zone Schema:

static const u64 ZONE_NAME_HASH_SALT = 0x0123456789abcdef;

u64 ZoneSchemaNameHash::HashVarField(const void *just_field, u32 bytes)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, bytes, ZONE_NAME_HASH_SALT);
}
u64 ZoneSchemaNameHash::HashField(const void *just_field)
{
	const char *name = reinterpret_cast<const char*>( just_field );
	if (!*name) return 0;
	return Hash64(name, GetFixedStrLen(name, ZONE_NAME_MAXLEN), ZONE_NAME_HASH_SALT);
}
u64 ZoneSchemaNameHash::HashComplete(const void *complete_record)
{
	const ZoneSchema *schema = reinterpret_cast<const ZoneSchema*>( complete_record );
	const char *name = schema->name;
	if (!*name) return 0;
	return Hash64(schema->name, GetFixedStrLen(schema->name, ZONE_NAME_MAXLEN), ZONE_NAME_HASH_SALT);
}
