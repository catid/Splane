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

#ifndef CAT_BAN_IDENTIFIERS_HPP
#define CAT_BAN_IDENTIFIERS_HPP

#include <cat/AllFramework.hpp>
#include <vector>

namespace cat {

/*
	GetHarddiskSerials() and GetCdRomSerial() are based on research done
	by the PunkBuster reverse-engineering community, particularly evilbert.

	GetBIOSInfo() is based on open-source software distributed under the
	Code Project Open License, written by W. J. Francis.
	http://www.codeproject.com/KB/system/SMBIOS_Peek.aspx

	All other functionality is my original work.
*/


struct HardwareSerial32
{
	bool strong;
	u32 id;
};

struct HardwareSerial64
{
	bool strong;
	u64 id;
};

// Class is not thread-safe
class BanIdentifiers
{
	// Entropy estimation:
	bool IsStrongSerial(const u8 *serial, int buffer_bytes);

protected:
	bool _prng_initialized;
	Skein _prng; // Stuck it here to obfuscate the COM objects a bit more
	u64 _random_ids[4];

	bool InitializePRNG();

protected:
	struct WMIPimpl;
	WMIPimpl *_wmi; // Also helps to stick the object at a different memory location each runtime

	void UninitializeCOM();
	CAT_INLINE bool ParseSMBIOS(u8 *data, u32 bytes, HardwareSerial64 &system_uuid, HardwareSerial64 &motherboard_serial, HardwareSerial64 &processor_serial);
	CAT_INLINE void ParseSystemUUID(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes);
	CAT_INLINE void ParseMotherboard(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes);
	CAT_INLINE void ParseProcessor(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes);

public:
	// Ctors:
	BanIdentifiers();
	~BanIdentifiers();

public:
	// Weak OS identifiers:
	bool GetUserNameHash(HardwareSerial32 &serial);
	bool GetComputerNameHash(HardwareSerial32 &serial);
	bool GetVSN(HardwareSerial32 &serial);

public:
	// Strong OS identifiers:
	bool GetMACAddress(HardwareSerial64 &serial);
	bool GetWindowsProductIDHash(HardwareSerial64 &serial);
	bool GetCryptographyMachineGuidHash(HardwareSerial64 &serial);

public:
	// Generated identifiers:
	bool GetHiddenRegID1(HardwareSerial64 &serial);
	bool GetHiddenRegID2(HardwareSerial64 &serial);
	bool GetHiddenFileID1(HardwareSerial64 &serial);
	bool GetHiddenFileID2(HardwareSerial64 &serial);

public:
	// Hardware identifiers:
	bool GetCdRomSerial(HardwareSerial64 &serial);
	bool GetHarddiskSerials(std::vector<HardwareSerial64> &serials);
	bool GetBIOSInfo(HardwareSerial64 &system_uuid, HardwareSerial64 &motherboard_serial, HardwareSerial64 &processor_serial);

public:
	bool GetCanary1(HardwareSerial64 &serial);
	bool GetCanary2(HardwareSerial32 &serial);
	bool GetCanary3(HardwareSerial64 &serial);
};


} // namespace cat

#endif // CAT_BAN_IDENTIFIERS_HPP
