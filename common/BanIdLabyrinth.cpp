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

#include "BanIdLabyrinth.hpp"
#include "LoginServerProtocol.hpp"
using namespace std;
using namespace cat;

// TODO: Generate labyrinths based on these parameterized checksums

static u16 GetChecksum64(u64 id)
{
	const u64 CS_SALT_0 = 0xbaddecafbaddecaf;
	const u64 CS_SALT_1 = 0x0123456789abcdef;
	const u64 CS_SALT_2 = 0xdeadbeefdeadbeef;
	const u64 CS_SALT_3 = 0xbaddecafbaddecaf;
	const u64 CS_SALT_4 = 0x0123456789abcdef;
	const int SHIFT_0 = 18;
	const int SHIFT_1 = 31;
	const int SHIFT_2 = 11;
	const int SHIFT_3 = 6;
	const int SHIFT_4 = 22;
	const u64 M = 21;

	u64 cs = ~id + (id << SHIFT_0);
	cs -= CS_SALT_0;
	cs = cs ^ (cs >> SHIFT_1);
	cs ^= CS_SALT_1;
	cs = cs * M;
	cs += CS_SALT_2;
	cs = cs ^ (cs >> SHIFT_2);
	cs ^= CS_SALT_3;
	cs = cs + (cs << SHIFT_3);
	cs -= CS_SALT_4;
	cs = cs ^ (cs >> SHIFT_4);

	u16 chk = (u16)cs;
	chk += (u16)(cs >> 16);
	chk ^= (u16)(cs >> 32);
	chk += (u16)(cs >> 48);

	return chk;
}

static u16 GetChecksum32(u32 id)
{
	const u32 CS_SALT_0 = 0xbaddecaf;
	const u32 CS_SALT_1 = 0x01234567;
	const u32 CS_SALT_2 = 0xdeadbeef;
	const u32 CS_SALT_3 = 0xbaddecaf;
	const int SHIFT_0 = 16;
	const int SHIFT_1 = 3;
	const int SHIFT_2 = 4;
	const int SHIFT_3 = 15;
	const u32 M = 0x27d4eb2d; // a prime

	u32 cs = (id ^ 61) ^ (id >> SHIFT_0);
	cs ^= CS_SALT_0;
	cs = cs + (cs << SHIFT_1);
	cs -= CS_SALT_1;
	cs = cs ^ (cs >> SHIFT_2);
	cs ^= CS_SALT_2;
	cs = cs * M;
	cs += CS_SALT_3;
	cs = cs ^ (cs >> SHIFT_3);

	u16 chk = (u16)cs;
	chk += (u16)(cs >> 16);

	return chk;
}





#if defined(CAT_CLIENT_CODE)






static u16 Generate64(u64 id, bool strong)
{
	const int SBIT_0 = 11; // replaced bit
	const int SBIT_1 = 3;
	const int SBIT_2 = 7;
	const int SBIT_3 = 10;
	const int CBIT_0 = 12; // replaced bit
	const int CBIT_1 = 4;
	const int CBIT_2 = 13;
	const int CBIT_3 = 1;
	const u16 BIAS = 1;

	u16 chk = GetChecksum64(id);

	u16 bias = strong ? BIAS : (BIAS ^ 1);

	u16 strong0 = (chk >> SBIT_0) ^ (chk >> SBIT_1) ^ (chk >> SBIT_2) ^ (chk >> SBIT_3) ^ bias;
	u16 strong1 = (chk >> CBIT_0) ^ (chk >> CBIT_1) ^ (chk >> CBIT_2) ^ (chk >> CBIT_3) ^ bias;

	chk ^= (strong0 & 1) << SBIT_0;
	chk ^= (strong1 & 1) << CBIT_0;

	return chk;
}

static u16 Generate32(u32 id, bool strong)
{
	const int SBIT_0 = 0; // replaced bit
	const int SBIT_1 = 3;
	const int SBIT_2 = 7;
	const int SBIT_3 = 10;
	const int CBIT_0 = 15; // replaced bit
	const int CBIT_1 = 4;
	const int CBIT_2 = 13;
	const int CBIT_3 = 1;
	const u16 BIAS = 0;

	u16 chk = GetChecksum32(id);

	u16 bias = strong ? BIAS : (BIAS ^ 1);

	u16 strong0 = (chk >> SBIT_0) ^ (chk >> SBIT_1) ^ (chk >> SBIT_2) ^ (chk >> SBIT_3) ^ bias;
	u16 strong1 = (chk >> CBIT_0) ^ (chk >> CBIT_1) ^ (chk >> CBIT_2) ^ (chk >> CBIT_3) ^ bias;

	chk ^= (strong0 & 1) << SBIT_0;
	chk ^= (strong1 & 1) << CBIT_0;

	return chk;
}

u32 BanIdLabyrinth::Cdrom(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_CDROM;

	HardwareSerial64 serial64;
	GetCdRomSerial(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::MachGuid(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_MACH_GUID;

	HardwareSerial64 serial64;
	GetCryptographyMachineGuidHash(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::RegId1(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_REG_ID1;

	HardwareSerial64 serial64;
	GetHiddenRegID1(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u16*>( msg ) = getLE( chksum );
	*reinterpret_cast<u64*>( msg+2 ) = getLE( serial64.id );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 2+8);

	return 2+8;
}

u32 BanIdLabyrinth::UserName(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_USER_NAME;

	HardwareSerial32 serial32;
	GetUserNameHash(serial32);
	u16 chksum = Generate32(serial32.id, serial32.strong);

	*reinterpret_cast<u32*>( msg ) = getLE( serial32.id );
	*reinterpret_cast<u16*>( msg+4 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 4+2);

	return 4+2;
}

u32 BanIdLabyrinth::Canary1(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_CANARY1;

	HardwareSerial64 serial64;
	GetCanary1(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::VSN(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_VSN;

	HardwareSerial32 serial32;
	GetVSN(serial32);
	u16 chksum = Generate32(serial32.id, serial32.strong);

	*reinterpret_cast<u32*>( msg ) = getLE( serial32.id );
	*reinterpret_cast<u16*>( msg+4 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 4+2);

	return 4+2;
}

u32 BanIdLabyrinth::FileId1(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_FILE_ID1;

	HardwareSerial64 serial64;
	GetHiddenFileID1(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u16*>( msg ) = getLE( chksum );
	*reinterpret_cast<u64*>( msg+2 ) = getLE( serial64.id );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 2+8);

	return 2+8;
}

u32 BanIdLabyrinth::SystemUUID(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_SYSTEM_UUID;

	GetBIOSInfo(_system_uuid, _motherboard_serial, _processor_serial);
	u16 chksum = Generate64(_system_uuid.id, _system_uuid.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( _system_uuid.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::Canary2(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_CANARY2;

	HardwareSerial32 serial32;
	GetCanary2(serial32);
	u16 chksum = Generate32(serial32.id, serial32.strong);

	*reinterpret_cast<u32*>( msg ) = getLE( serial32.id );
	*reinterpret_cast<u16*>( msg+4 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 4+2);

	return 4+2;
}

u32 BanIdLabyrinth::Harddrive1(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_HARDDRIVE1;

	_hdd0.id = 0;
	_hdd0.strong = 0;
	_hdd1.id = 0;
	_hdd1.strong = 0;

	vector<HardwareSerial64> hdds;
	GetHarddiskSerials(hdds);
	if (hdds.size() >= 1) _hdd0 = hdds[0];
	if (hdds.size() >= 2) _hdd1 = hdds[1];
	u16 chksum = Generate64(_hdd0.id, _hdd0.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( _hdd0.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::MACAddr(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_MAC_ADDR;

	HardwareSerial64 serial64;
	GetMACAddress(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::Motherboard(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_MOTHERBOARD;

	u16 chksum = Generate64(_motherboard_serial.id, _motherboard_serial.strong);

	*reinterpret_cast<u16*>( msg ) = getLE( chksum );
	*reinterpret_cast<u64*>( msg+2 ) = getLE( _motherboard_serial.id );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 2+8);

	return 2+8;
}

u32 BanIdLabyrinth::FileId2(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_FILE_ID2;

	HardwareSerial64 serial64;
	GetHiddenFileID2(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::Harddrive2(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_HARDDRIVE2;

	u16 chksum = Generate64(_hdd1.id, _hdd1.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( _hdd1.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::Canary3(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_CANARY3;

	HardwareSerial64 serial64;
	GetCanary3(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u16*>( msg ) = getLE( chksum );
	*reinterpret_cast<u64*>( msg+2 ) = getLE( serial64.id );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 2+8);

	return 2+8;
}

u32 BanIdLabyrinth::RegId2(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_REG_ID2;

	HardwareSerial64 serial64;
	GetHiddenRegID2(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( serial64.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}

u32 BanIdLabyrinth::CompName(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_COMP_NAME;

	HardwareSerial32 serial32;
	GetComputerNameHash(serial32);
	u16 chksum = Generate32(serial32.id, serial32.strong);

	*reinterpret_cast<u32*>( msg ) = getLE( serial32.id );
	*reinterpret_cast<u16*>( msg+4 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 4+2);

	return 4+2;
}

u32 BanIdLabyrinth::ProdId(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_PROD_ID;

	HardwareSerial64 serial64;
	GetWindowsProductIDHash(serial64);
	u16 chksum = Generate64(serial64.id, serial64.strong);

	*reinterpret_cast<u16*>( msg ) = getLE( chksum );
	*reinterpret_cast<u64*>( msg+2 ) = getLE( serial64.id );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 2+8);

	return 2+8;
}

u32 BanIdLabyrinth::Processor(const ChaChaKey &key, u8 *msg, u8 &opcode)
{
	opcode = LS_LOGIN_BANID_PROCESSOR;

	u16 chksum = Generate64(_processor_serial.id, _processor_serial.strong);

	*reinterpret_cast<u64*>( msg ) = getLE( _processor_serial.id );
	*reinterpret_cast<u16*>( msg+8 ) = getLE( chksum );

	ChaChaOutput cipher(key, opcode);
	cipher.Crypt(msg, msg, 8+2);

	return 8+2;
}




#else // Server code:





static bool Validate64(u64 &id, u16 checksum)
{
	// Must match generator
	const int SBIT_0 = 11; // replaced bit
	const int SBIT_1 = 3;
	const int SBIT_2 = 7;
	const int SBIT_3 = 10;
	const int CBIT_0 = 12; // replaced bit
	const int CBIT_1 = 4;
	const int CBIT_2 = 13;
	const int CBIT_3 = 1;
	const u16 BIAS = 1;

	u16 strong0 = (checksum >> SBIT_0) ^ (checksum >> SBIT_1) ^ (checksum >> SBIT_2) ^ (checksum >> SBIT_3);
	u16 strong1 = (checksum >> CBIT_0) ^ (checksum >> CBIT_1) ^ (checksum >> CBIT_2) ^ (checksum >> CBIT_3);

	if (((strong0 ^ strong1) & 1) != 0) return false;

	u16 chk = GetChecksum64(id);

	if (((chk ^ checksum) & ~(((u16)1 << SBIT_0) | ((u16)1 << CBIT_0))) != 0) return false;

	id = (id << 1) | ((strong0 ^ BIAS ^ 1) & 1);

	return true;
}

static bool Validate32(u32 &id, u16 checksum)
{
	// Must match generator
	const int SBIT_0 = 0; // replaced bit
	const int SBIT_1 = 3;
	const int SBIT_2 = 7;
	const int SBIT_3 = 10;
	const int CBIT_0 = 15; // replaced bit
	const int CBIT_1 = 4;
	const int CBIT_2 = 13;
	const int CBIT_3 = 1;
	const u16 BIAS = 0;

	u16 strong0 = (checksum >> SBIT_0) ^ (checksum >> SBIT_1) ^ (checksum >> SBIT_2) ^ (checksum >> SBIT_3);
	u16 strong1 = (checksum >> CBIT_0) ^ (checksum >> CBIT_1) ^ (checksum >> CBIT_2) ^ (checksum >> CBIT_3);

	if (((strong0 ^ strong1) & 1) != 0) return false;

	u16 chk = GetChecksum32(id);

	if (((chk ^ checksum) & ~(((u16)1 << SBIT_0) | ((u16)1 << CBIT_0))) != 0) return false;

	id = (id << 1) | ((strong0 ^ BIAS ^ 1) & 1);

	return true;
}

// OS:
bool BanIdTheseus::MachGuid(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::ProdId(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::UserName(u32 &id, u16 checksum)
{
	return Validate32(id, checksum);
}
bool BanIdTheseus::CompName(u32 &id, u16 checksum)
{
	return Validate32(id, checksum);
}
bool BanIdTheseus::VSN(u32 &id, u16 checksum)
{
	return Validate32(id, checksum);
}

// Hardware:
bool BanIdTheseus::MACAddr(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::SystemUUID(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Motherboard(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Processor(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Cdrom(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Harddrive1(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Harddrive2(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}

// Generated IDs:
bool BanIdTheseus::FileId1(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::FileId2(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::RegId1(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::RegId2(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}

// Canaries:
bool BanIdTheseus::Canary1(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}
bool BanIdTheseus::Canary2(u32 &id, u16 checksum)
{
	return Validate32(id, checksum);
}
bool BanIdTheseus::Canary3(u64 &id, u16 checksum)
{
	return Validate64(id, checksum);
}


#endif // CAT_CLIENT_CODE
