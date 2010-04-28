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

#ifndef CAT_LABYRINTH_HPP
#define CAT_LABYRINTH_HPP

#include "../ban_ids/BanIdentifiers.hpp"

namespace cat {


/*
	Obfuscation ideas:

		Exceptions
		Function pointers
		Data-dependent execution flow
		Recursive implementation
			To obfuscate code, generate constants with more code
				Then obfuscate the code used to generate the constants
					Repeat!
		Garbage opcodes to confuse static analysis
		Dual opcodes that are both valid but start one byte earlier
		Decrypt jump and call addresses
		Modify return address
*/

typedef bool (*BanIdGenerator32)(HardwareSerial32 &);
typedef bool (*BanIdGenerator64)(HardwareSerial64 &);

#if defined(CAT_WORD_64)
#define CAT_MINOTAUR(minotaur, encryption_key) (*(u64*)(&minotaur) ^ encryption_key)
#define Theseus
#else // 32-bit version:
#define CAT_MINOTAUR(minotaur, encryption_key) (*(u32*)(&minotaur) ^ encryption_key)
#endif

class Labyrinth
{
protected:
	// Cached data so that a maze can be wrapped around each one separately
	HardwareSerial64 _harddrive_serial2;
	HardwareSerial64 _processor_serial;
	HardwareSerial64 _motherboard_serial;

public:
	// These functions must be called in order from top to bottom.
	void CdRomSerial(u8 *buffer, BanIdGenerator64);
	void MachineGuid(u8 *buffer, BanIdGenerator64);
	void RegistryID1(u8 *buffer, BanIdGenerator64);
	void UserName(u8 *buffer, BanIdGenerator32);
	void Canary1(u8 *buffer, BanIdGenerator64);
	void VSN(u8 *buffer, BanIdGenerator32);
	void FileID1(u8 *buffer, BanIdGenerator64);
	void SystemUUID(u8 *buffer, BanIdGenerator64);
	void Canary2(u8 *buffer, BanIdGenerator32);
	void Harddrive1(u8 *buffer, BanIdGenerator64);
	void MACAddress(u8 *buffer, BanIdGenerator64);
	void Motherboard(u8 *buffer, BanIdGenerator64);
	void FileID2(u8 *buffer, BanIdGenerator64);
	void Harddrive2(u8 *buffer, BanIdGenerator64);
	void Canary3(u8 *buffer, BanIdGenerator64);
	void RegistryID2(u8 *buffer, BanIdGenerator64);
	void ComputerName(u8 *buffer, BanIdGenerator32);
	void ProductId(u8 *buffer, BanIdGenerator64);
	void Processor(u8 *buffer, BanIdGenerator64);
};


} // namespace cat

#endif // CAT_LABYRINTH_HPP
