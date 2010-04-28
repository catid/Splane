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

#ifndef CAT_BAN_ID_LABYRINTH_HPP
#define CAT_BAN_ID_LABYRINTH_HPP

#include <cat/AllFramework.hpp>
#include "BanIdentifiers.hpp"

/*
	Checksums are based on an algorithm from Thomas Wang
	http://www.cris.com/~Ttwang/tech/inthash.htm
*/

namespace cat {


#if defined(CAT_CLIENT_CODE)

/*
	At the center of the labyrinth, the
	minotaur waits for another hero

	Obfuscated checksum generation to
	guard integrity of the ban ids
*/
class BanIdLabyrinth : public BanIdentifiers
{
	HardwareSerial64 _system_uuid, _motherboard_serial, _processor_serial;
	HardwareSerial64 _hdd0, _hdd1;

public:
	// OS:
	u32 MachGuid(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 ProdId(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 UserName(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 CompName(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 VSN(const ChaChaKey &key, u8 *msg, u8 &opcode);

	// Hardware:
	u32 MACAddr(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 SystemUUID(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Motherboard(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Processor(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Cdrom(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Harddrive1(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Harddrive2(const ChaChaKey &key, u8 *msg, u8 &opcode);

	// Generated IDs:
	u32 FileId1(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 FileId2(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 RegId1(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 RegId2(const ChaChaKey &key, u8 *msg, u8 &opcode);

	// Canaries:
	u32 Canary1(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Canary2(const ChaChaKey &key, u8 *msg, u8 &opcode);
	u32 Canary3(const ChaChaKey &key, u8 *msg, u8 &opcode);
};

#else // Server code:

/*
	Theseus finds his way to the heart of
	the labyrinth and slays the minotaur

	Simple trapdoor versions of obfuscated
	client checksums for use on the server
*/
class BanIdTheseus
{
public:
	// OS:
	static bool MachGuid(u64 &id, u16 checksum);
	static bool ProdId(u64 &id, u16 checksum);
	static bool UserName(u32 &id, u16 checksum);
	static bool CompName(u32 &id, u16 checksum);
	static bool VSN(u32 &id, u16 checksum);

	// Hardware:
	static bool MACAddr(u64 &id, u16 checksum);
	static bool SystemUUID(u64 &id, u16 checksum);
	static bool Motherboard(u64 &id, u16 checksum);
	static bool Processor(u64 &id, u16 checksum);
	static bool Cdrom(u64 &id, u16 checksum);
	static bool Harddrive1(u64 &id, u16 checksum);
	static bool Harddrive2(u64 &id, u16 checksum);

	// Generated IDs:
	static bool FileId1(u64 &id, u16 checksum);
	static bool FileId2(u64 &id, u16 checksum);
	static bool RegId1(u64 &id, u16 checksum);
	static bool RegId2(u64 &id, u16 checksum);

	// Canaries:
	static bool Canary1(u64 &id, u16 checksum);
	static bool Canary2(u32 &id, u16 checksum);
	static bool Canary3(u64 &id, u16 checksum);
};

#endif // CAT_CLIENT_CODE


} // namespace cat

#endif // CAT_BAN_ID_LABYRINTH_HPP
