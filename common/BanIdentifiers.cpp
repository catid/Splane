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

#include "BanIdentifiers.hpp"
#include <comdef.h>		// COM
#include <Wbemidl.h>	// WMI for System Management BIOS dump
#include <Lmcons.h>		// LAN-MAN constants for "UNLEN" username max length
#include <Iphlpapi.h>	// GetAdaptersInfo()
#include <winioctl.h>	// DeviceIoControl()
using namespace cat;


struct BanIdentifiers::WMIPimpl
{
	bool _com_initialized;
	IWbemLocator *_wmi_loc; // Wbem locator
	IWbemServices *_wmi_svc; // Wbem services object
	IEnumWbemClassObject *_wmi_enum; // Wbem enumerator
};


//// Ctors

void BanIdentifiers::UninitializeCOM()
{
	if (_wmi->_com_initialized)
	{
		_wmi->_com_initialized = false;
		CoUninitialize();
	}
}

bool BanIdentifiers::InitializePRNG()
{
	if (_prng_initialized) return true;

	HardwareSerial64 system_uuid, motherboard_serial, processor_serial;
	if (!GetBIOSInfo(system_uuid, motherboard_serial, processor_serial))
		return false;

	if (!_prng.BeginKey(256))
		return false;

	// Hash all of the BIOS info to form a seed for the PRNG
	_prng.Crunch(&system_uuid.id, sizeof(system_uuid.id));
	_prng.Crunch(&motherboard_serial.id, sizeof(motherboard_serial.id));
	_prng.Crunch(&processor_serial.id, sizeof(processor_serial.id));

	_prng.End();

	// Generate all of the random IDs now
	_prng.Generate(_random_ids, sizeof(_random_ids));

	_prng_initialized = true;
	return true;
}

BanIdentifiers::BanIdentifiers()
{
	_prng_initialized = false;

	// Attempt to randomize the address of the WMI Pimpl each runtime
	u8 *randomize_offset = new u8[GetTickCount() & 0xffff];
	_wmi = new WMIPimpl;
	if (randomize_offset) delete []randomize_offset;
	if (!_wmi) return;

	_wmi->_com_initialized = false;
	_wmi->_wmi_svc = 0;
	_wmi->_wmi_loc = 0;
	_wmi->_wmi_enum = 0;

	// Initialize COM
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
		return;

	_wmi->_com_initialized = true;

	// Obtain Wbem locator instance
	if (FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
								IID_IWbemLocator, (void**)&_wmi->_wmi_loc)))
		return;

	// Connect to the root\cimv2 namespace with the current user and obtain IWbemServices instance
	if (FAILED(_wmi->_wmi_loc->ConnectServer(L"ROOT\\WMI", 0, 0, 0, 0, 0, 0, &_wmi->_wmi_svc)))
		return;

	// Set the IWbemServices proxy so that impersonation of the user (client) occurs
	if (FAILED(CoSetProxyBlanket(_wmi->_wmi_svc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, 0,
								 RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, 0, EOAC_NONE)))
		return;

	// Create enumerator instance
	if (FAILED(_wmi->_wmi_svc->CreateInstanceEnum(L"MSSMBios_RawSMBiosTables", 0, 0, &_wmi->_wmi_enum)))
		return;
}

BanIdentifiers::~BanIdentifiers()
{
	if (_wmi)
	{
		if (_wmi->_wmi_loc) _wmi->_wmi_loc->Release();
		if (_wmi->_wmi_svc) _wmi->_wmi_svc->Release();
		UninitializeCOM();
	}
}


//// Entropy Estimation

bool BanIdentifiers::IsStrongSerial(const u8 *serial, int buffer_bytes)
{
	/*
		Serials contain upper-case letters and numbers and dashes.
		Strings like "OEM" "O.E.M." "Serial" "Number" "001" "002" "Version", spaces, and periods do not help.
	*/

	u32 numbers_seen = 0;
	u8 ch, last = 0, last_accepted = 0;
	u32 score = 0;
	while (buffer_bytes-- > 0)
	{
		ch = *serial++;
		if (!ch) break;

		// Ignore runs of the same character
		if (ch == last) continue;

		switch ((char)ch)
		{
		case '.':
		case '\t':
		case ' ':
			// Do not count periods and whitespace.
			break;

		case '1':
		case '2':
		case '3':
			if (last == '0')
			{
				// If the previous character was 0, ignore 1-3.
				break;
			}
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			++score;
			last_accepted = ch;
			++numbers_seen;
			break;

		// If the previous accepted character was 'I', ignore 'A' (Serial)
		// If the previous accepted character was 'I', ignore 'O' (Version)
		// If the previous accepted character was 'I', ignore 'D' (ID)
		case 'A':
		case 'a':
		case 'O':
		case 'o':
		case 'D':
		case 'd':
			if (last_accepted != 'I' && last_accepted != 'i')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'M', ignore 'B' (Number)
		case 'B':
		case 'b':
			if (last_accepted != 'M' && last_accepted != 'm')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'B', ignore 'E' (Number)
		// If the previous accepted character was 'O', ignore 'E' (OEM)
		// If the previous accepted character was 'S', ignore 'E' (Serial)
		// If the previous accepted character was 'V', ignore 'E' (Version)
		case 'E':
		case 'e':
			if (last_accepted != 'B' && last_accepted != 'b' &&
				last_accepted != 'O' && last_accepted != 'o' &&
				last_accepted != 'S' && last_accepted != 's' &&
				last_accepted != 'V' && last_accepted != 'v')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'R', ignore 'I' (Serial)
		// If the previous accepted character was 'S', ignore 'I' (Version)
		case 'I':
		case 'i':
			if (last_accepted != 'R' && last_accepted != 'r' &&
				last_accepted != 'S' && last_accepted != 's')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'A', ignore 'L' (Serial)
		case 'L':
		case 'l':
			if (last_accepted != 'A' && last_accepted != 'a')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'E', ignore 'M' (OEM)
		// If the previous accepted character was 'U', ignore 'M' (Number)
		case 'M':
		case 'm':
			if (last_accepted != 'E' && last_accepted != 'e' &&
				last_accepted != 'U' && last_accepted != 'u')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'O', ignore 'N' (Version)
		case 'N':
		case 'n':
			if (last_accepted != 'O' && last_accepted != 'o')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'E', ignore 'R' (Serial)
		// If the previous accepted character was 'E', ignore 'R' (Version)
		// If the previous accepted character was 'E', ignore 'R' (Number)
		case 'R':
		case 'r':
			if (last_accepted != 'E' && last_accepted != 'e')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'R', ignore 'S' (Version)
		case 'S':
		case 's':
			if (last_accepted != 'R' && last_accepted != 'r')
			{
				++score;
				last_accepted = ch;
			}
			break;

		// If the previous accepted character was 'N', ignore 'U' (Number)
		case 'U':
		case 'u':
			if (last_accepted != 'N' && last_accepted != 'n')
			{
				++score;
				last_accepted = ch;
			}
			break;

		default:
			++score;
			last_accepted = ch;
			break;
		}

		last = ch;
	}

	// If at least 4 characters score towards a good serial number, then it is strong enough to ban on.
	// There need to be at least 2 numbers in the serial or it might just be a string like "Needs to be set by O.E.M."
	return score >= 4 && numbers_seen >= 2;
}


//// Weak OS Identifiers

bool BanIdentifiers::GetUserNameHash(HardwareSerial32 &serial)
{
	const u32 HASH_SALT = 0x12123434;

	TCHAR user_name[UNLEN + 1];
	DWORD user_len = sizeof(user_name) / sizeof(TCHAR);

	if (GetUserName(user_name, &user_len))
	{
		serial.strong = (user_len >= 8);
		serial.id = MurmurHash32(user_name, user_len * sizeof(TCHAR), HASH_SALT);
		return true;
	}
	else
	{
		serial.strong = false;
		serial.id = 0;
		return false;
	}
}

bool BanIdentifiers::GetComputerNameHash(HardwareSerial32 &serial)
{
	const u32 HASH_SALT = 0x53234123;

	TCHAR computer_name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD name_len = sizeof(computer_name) / sizeof(TCHAR);

	if (GetComputerName(computer_name, &name_len))
	{
		serial.strong = (name_len >= 12);
		serial.id = MurmurHash32(computer_name, name_len * sizeof(TCHAR), HASH_SALT);
		return true;
	}
	else
	{
		serial.strong = false;
		serial.id = 0;
		return false;
	}
}

bool BanIdentifiers::GetVSN(HardwareSerial32 &serial)
{
	serial.id = 0;
	serial.strong = false;

	DWORD needed, buffer_chars = 256;
	TCHAR *buffer = new TCHAR[buffer_chars+1];
	if (!buffer) return false;

	for (;;)
	{
		needed = GetLogicalDriveStrings(buffer_chars, buffer);
		if (needed <= buffer_chars) break;

		// Grow buffer to indicated size
		delete []buffer;
		buffer_chars = needed;
		buffer = new TCHAR[buffer_chars+1];
		if (!buffer) return false;
	}

	buffer[needed] = 0; // Insure the buffer is null-terminated
	TCHAR *drive = buffer;

	// While more drives to parse,
	while (*drive)
	{
		// If drive is fixed, (ignores floppy drives, CDROM, and network shares)
		if (GetDriveType(drive) == DRIVE_FIXED)
		{
			TCHAR volumeName[MAX_PATH + 1] = { 0 };
			TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
			DWORD serialNumber;
			DWORD maxComponentLen = 0;
			DWORD fileSystemFlags = 0;

			// Grab volume serial number for first accessible drive
			if (GetVolumeInformation(
				drive,
				volumeName,
				ARRAYSIZE(volumeName),
				&serialNumber,
				&maxComponentLen,
				&fileSystemFlags,
				fileSystemName,
				ARRAYSIZE(fileSystemName)))
			{
				serial.strong = true;
				serial.id = serialNumber;
				break;
			}
		}

		// Seek to next drive
		while(*drive++);
	}

	delete []buffer;
	return serial.strong;
}


//// Strong OS Identifiers

bool BanIdentifiers::GetMACAddress(HardwareSerial64 &serial)
{
	const u64 HASH_SALT = 0xcb123456789abcdf;

	serial.strong = false;
	serial.id = 0;

	DWORD buffer_bytes = 16 * sizeof(IP_ADAPTER_INFO);
	u8 *buffer = new u8[buffer_bytes];
	if (!buffer) return false;

	for (;;)
	{
		DWORD status = GetAdaptersInfo((PIP_ADAPTER_INFO)buffer, &buffer_bytes);

		if (status == ERROR_SUCCESS)
		{
			PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)buffer;

			do {
				if (pAdapterInfo->AddressLength >= 6)
				{
					serial.strong = true;
					serial.id = MurmurHash64(pAdapterInfo->Address, pAdapterInfo->AddressLength, HASH_SALT);
					break;
				}

				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);

			break;
		}

		if (status != ERROR_BUFFER_OVERFLOW) break;

		// Grow buffer to indicated size
		delete []buffer;
		buffer = new u8[buffer_bytes];
		if (!buffer) return false;
	}

	delete []buffer;
	return serial.strong;
}

bool BanIdentifiers::GetWindowsProductIDHash(HardwareSerial64 &serial)
{
	serial.id = 0;
	serial.strong = false;

	HKEY currentVersion;

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &currentVersion))
		return false;

	DWORD type;
	u8 data[256];
	DWORD len = sizeof(data);

	if (ERROR_SUCCESS == RegQueryValueEx(currentVersion, L"ProductId", 0, &type, data, &len))
	{
		if (type == REG_SZ)
		{
			Skein s;
			if (s.BeginKey(256))
			{
				s.Crunch(data, len);
				serial.strong = true;

				// ape's ideas:
				// Search for "OEM" in the ProductId.  If it is present, then many users will
				// have this same ProductId so mark it as a weak identifier.
				for (u32 ii = 2; ii < len; ++ii)
				{
					if (tolower(data[ii]) == 'm' &&
						tolower(data[ii-1]) == 'e' &&
						tolower(data[ii-2]) == 'o')
					{
						serial.strong = false;

						// Also mix in the InstallDate to help strengthen the identifier in this case.
						len = sizeof(data);
						if (ERROR_SUCCESS == RegQueryValueEx(currentVersion, L"InstallDate", 0, &type, data, &len))
						{
							if (type == REG_DWORD && len == 4)
							{
								s.Crunch(data, len);
							}
						}

						break;
					}
				}

				s.End();

				s.Generate(&serial.id, sizeof(serial.id));
			}
		}
	}

	RegCloseKey(currentVersion);

	return serial.strong;
}

bool BanIdentifiers::GetCryptographyMachineGuidHash(HardwareSerial64 &serial)
{
	serial.id = 0;
	serial.strong = false;

	HKEY cryptoKey;

	// Must specify KEY_WOW64_64KEY since on 64-bit OS it is not normally accessible by 32-bit applications.
	// Specifying this key does not break on a 32-bit OS.
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Cryptography", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &cryptoKey))
		return false;

	DWORD type;
	u8 data[512];
	DWORD len = sizeof(data);

	if (ERROR_SUCCESS == RegQueryValueEx(cryptoKey, L"MachineGuid", 0, &type, data, &len))
	{
		if (type == REG_SZ)
		{
			Skein s;
			if (s.BeginKey(256))
			{
				s.Crunch(data, len);
				s.End();

				s.Generate(&serial.id, sizeof(serial.id));
				serial.strong = true;
			}
		}
	}

	RegCloseKey(cryptoKey);

	return serial.strong;
}


//// Generated Identifiers

bool BanIdentifiers::GetHiddenRegID1(HardwareSerial64 &serial)
{
	serial.strong = false;
	serial.id = 0;

	HKEY key;

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Splane", 0, 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_QUERY_VALUE, 0, &key, 0))
		return false;

	DWORD type;
	DWORD len = sizeof(serial.id);

	LSTATUS status = RegQueryValueEx(key, L"PrefMask", 0, &type, (u8*)&serial.id, &len);

	if (ERROR_FILE_NOT_FOUND == status || len != sizeof(serial.id) || type != REG_BINARY)
	{
		if (InitializePRNG())
		{
			serial.id = _random_ids[0];
			serial.strong = true;

			RegSetValueEx(key, L"PrefMask", 0, REG_BINARY, (u8*)&serial.id, sizeof(serial.id));
		}
	}
	else if (ERROR_SUCCESS == status)
	{
		serial.strong = true;
	}

	RegCloseKey(key);

	return serial.strong;
}

bool BanIdentifiers::GetHiddenRegID2(HardwareSerial64 &serial)
{
	serial.strong = false;
	serial.id = 0;

	HKEY key;

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\Windows Assurance", 0, 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_QUERY_VALUE, 0, &key, 0))
		return 0;

	DWORD type;
	DWORD len = sizeof(serial.id);

	LSTATUS status = RegQueryValueEx(key, L"LastTicket", 0, &type, (u8*)&serial.id, &len);

	if (ERROR_FILE_NOT_FOUND == status || len != sizeof(serial.id) || type != REG_BINARY)
	{
		if (InitializePRNG())
		{
			serial.id = _random_ids[1];
			serial.strong = true;

			RegSetValueEx(key, L"LastTicket", 0, REG_BINARY, (u8*)&serial.id, sizeof(serial.id));
		}
	}
	else if (ERROR_SUCCESS == status)
	{
		serial.strong = true;
	}

	RegCloseKey(key);

	return serial.strong;
}

bool BanIdentifiers::GetHiddenFileID1(HardwareSerial64 &serial)
{
	serial.strong = false;
	serial.id = 0;

	DWORD bytes;
	HANDLE file = CreateFile(L"Banner.tiff", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		file = CreateFile(L"Banner.tiff", GENERIC_READ, 0, 0, OPEN_ALWAYS,
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, 0);
	}

	if ((file != INVALID_HANDLE_VALUE) &&
		(GetLastError() != ERROR_ALREADY_EXISTS ||
		!ReadFile(file, (u8*)&serial.id, sizeof(serial.id), &bytes, 0) ||
		bytes != sizeof(serial.id)))
	{
		if (InitializePRNG())
		{
			serial.id = _random_ids[2];
			serial.strong = true;

			WriteFile(file, (u8*)&serial.id, sizeof(serial.id), &bytes, 0);
		}
	}
	else if (serial.id != 0)
	{
		serial.strong = true;
	}

	CloseHandle(file);

	return serial.strong;
}

bool BanIdentifiers::GetHiddenFileID2(HardwareSerial64 &serial)
{
	serial.strong = false;
	serial.id = 0;

	DWORD needed, buffer_chars = 256;
	TCHAR *buffer = new TCHAR[buffer_chars+32+1];
	if (!buffer) return 0;

	for (;;)
	{
		needed = GetTempPath(buffer_chars, buffer);
		if (needed <= buffer_chars) break;

		// Grow buffer to indicated size
		delete []buffer;
		buffer_chars = needed;
		buffer = new TCHAR[buffer_chars+32+1];
		if (!buffer) return 0;
	}

	memcpy(buffer + needed, L"\\MT_B301.tmp", 13 * sizeof(TCHAR));

	DWORD bytes;
	HANDLE file = CreateFile(buffer, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_ALWAYS,
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, 0);
	}

	delete []buffer;

	if ((file != INVALID_HANDLE_VALUE) &&
		(GetLastError() != ERROR_ALREADY_EXISTS ||
		!ReadFile(file, (u8*)&serial.id, sizeof(serial.id), &bytes, 0) ||
		bytes != sizeof(serial.id)))
	{
		if (InitializePRNG())
		{
			serial.id = _random_ids[3];
			serial.strong = true;

			WriteFile(file, (u8*)&serial.id, sizeof(serial.id), &bytes, 0);
		}
	}
	else if (serial.id != 0)
	{
		serial.strong = true;
	}

	CloseHandle(file);

	return serial.strong;
}


//// Hardware Identifiers

bool BanIdentifiers::GetHarddiskSerials(std::vector<HardwareSerial64> &serials)
{
	const u64 HASH_SALT = 0xde64ab3201234567;
	const int MAX_DRIVES = 16;

	for (int ii = 0; ii < MAX_DRIVES; ++ii)
	{
		char device_name[256];
		sprintf(device_name, "\\\\.\\PhysicalDrive%d", ii);

		// Open device
		HANDLE device = CreateFileA(device_name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (device != INVALID_HANDLE_VALUE)
		{
			STORAGE_PROPERTY_QUERY query;
			CAT_OBJCLR(query);
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			u8 buffer[4096];
			CAT_OBJCLR(buffer);
			DWORD bytes;

			// Read storage device descriptor
			if (DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buffer, sizeof(buffer), &bytes, 0))
			{
				STORAGE_DEVICE_DESCRIPTOR *descrip = (STORAGE_DEVICE_DESCRIPTOR *)buffer;

				const u32 DESCRIP_BYTES = offsetof(STORAGE_DEVICE_DESCRIPTOR, RawDeviceProperties);

				if (bytes >= DESCRIP_BYTES && descrip->Size <= bytes)
				{
					u8 gbuffer[4096];
					CAT_OBJCLR(gbuffer);
					DWORD gbytes;

					// Read geometry
					if (DeviceIoControl(device, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, 0, 0, gbuffer, sizeof(gbuffer), &gbytes, 0))
					{
						DISK_GEOMETRY_EX *geometry = (DISK_GEOMETRY_EX*)gbuffer;
						DISK_PARTITION_INFO *parts = DiskGeometryGetPartition(geometry);

						const u32 GEOMETRY_BYTES = offsetof(DISK_GEOMETRY_EX, Data);

						// Verify that drive is fixed
						if (geometry->Geometry.MediaType == FixedMedia)
						{
							IncrementalMurmurHash64 hash;
							hash.Begin(HASH_SALT); // Don't mix in drive index, so swapping drive order doesn't mess it up
							hash.Add(descrip->RawDeviceProperties, descrip->Size - DESCRIP_BYTES);
							if (gbytes >= GEOMETRY_BYTES) hash.Add(gbuffer, GEOMETRY_BYTES);

							HardwareSerial64 serial;
							serial.id = hash.End();

							// If the ProductId is too short to be "raid" or isn't "raid", then verify entropy:
							// This includes the check if SerialNumberOffset = (u32)-1
							u32 pid_offset = descrip->ProductIdOffset;
							if (descrip->SerialNumberOffset > 0 &&
								descrip->SerialNumberOffset < descrip->Size &&
								(pid_offset == 0 ||
								 pid_offset + 4 > bytes ||
								tolower(buffer[pid_offset])   != 'r' ||
								tolower(buffer[pid_offset+1]) != 'a' ||
								tolower(buffer[pid_offset+2]) != 'i' ||
								tolower(buffer[pid_offset+3]) != 'd'))
							{
								serial.strong = IsStrongSerial(&buffer[descrip->SerialNumberOffset], descrip->Size - descrip->SerialNumberOffset);
							}
							else
							{
								serial.strong = false;
							}

							serials.push_back(serial);
						}
					}
				}
			}

			CloseHandle(device);
		}
	}

	return serials.size() > 0;
}

bool BanIdentifiers::GetCdRomSerial(HardwareSerial64 &serial)
{
	const u64 HASH_SALT = 0xd67ab0123e64543f;

	serial.id = 0;
	serial.strong = false;
	bool success = false;

	// Open device
	HANDLE device = CreateFileW(L"\\\\.\\CdRom0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (device != INVALID_HANDLE_VALUE)
	{
		STORAGE_PROPERTY_QUERY query;
		CAT_OBJCLR(query);
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyExistsQuery;

		u8 buffer[4096];
		CAT_OBJCLR(buffer);
		DWORD bytes = sizeof(buffer);

		// Read storage device descriptor
		if (DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buffer, sizeof(buffer), &bytes, 0))
		{
			STORAGE_DEVICE_DESCRIPTOR *descrip = (STORAGE_DEVICE_DESCRIPTOR *)buffer;

			const u32 DESCRIP_BYTES = offsetof(STORAGE_DEVICE_DESCRIPTOR, RawDeviceProperties);

			// Don't query the geometry for CD-ROM drives since it only exists if a disk is in the drive
			if (bytes >= DESCRIP_BYTES && descrip->Size <= bytes)
			{
				serial.id = MurmurHash64(descrip->RawDeviceProperties, descrip->Size - DESCRIP_BYTES, HASH_SALT);

				// If it does not have a serial number field,
				// This includes the case where SerialNumberOffset = (u32)-1
				if (descrip->SerialNumberOffset == 0 ||
					descrip->SerialNumberOffset >= descrip->Size)
				{
					serial.strong = false;
				}
				else
				{
					serial.strong = IsStrongSerial(&buffer[descrip->SerialNumberOffset], descrip->Size - descrip->SerialNumberOffset);
				}

				success = true;
			}
		}

		CloseHandle(device);
	}

	return success;
}

void BanIdentifiers::ParseSystemUUID(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes)
{
	u8 *uuid = formatted_data + 4;
	static const u32 UUID_BYTES = 16;

	static const u8 UNDEFINED_UUID[UUID_BYTES] = { 0 };
	static const u8 UNSET_UUID[UUID_BYTES] = { 0xFF };

	Skein s;
	if (s.BeginKey(256))
	{
		// If the UUID is not available,
		if (formatted_bytes < 20 || // Spec version 2.0 didn't include UUID but did have a serial #
			0 == memcmp(uuid, UNSET_UUID, UUID_BYTES) ||
			0 == memcmp(uuid, UNDEFINED_UUID, UUID_BYTES))
		{
			// Make do with the system info strings (SKU, family, other product info)
			s.Crunch(unformatted_data, unformatted_bytes);
			serial.strong = false; // But mark it weak
		}
		else
		{
			// Crunch the 128-bit UUID into a 64-bit hash for privacy
			s.Crunch(uuid, UUID_BYTES);
			serial.strong = true;
		}

		s.End();
		s.Generate(&serial.id, sizeof(serial.id));
	}
}

void BanIdentifiers::ParseMotherboard(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes)
{
	u64 salt = 0xd67c70123e615437;

	// Mix identifying information from the formatted section, if available:
	if (formatted_bytes >= 10) salt += formatted_data[9]; // Motherboard type

	// Determine if serial number is present
	serial.strong = false;
	if (formatted_bytes >= 3)
	{
		u32 str_index = formatted_data[3];
		u8 *str = unformatted_data;
		u32 remaining = unformatted_bytes;

		// While there is still data to process and we haven't found the string yet,
		while (remaining > 0 && str_index > 1)
		{
			// Grab next character and shrink available space
			u8 ch = *str++;
			remaining--;

			// Subtract the string index for each string we pass
			if (ch == '\0') --str_index;
		}

		// If we found the string, check if it's strong
		if (str_index == 1) serial.strong = IsStrongSerial(str, remaining);
	}

	serial.id = MurmurHash64(unformatted_data, unformatted_bytes, salt);
}

void BanIdentifiers::ParseProcessor(HardwareSerial64 &serial, u8 *formatted_data, u32 formatted_bytes, u8 *unformatted_data, u32 unformatted_bytes)
{
	u64 salt = 0xa67ab0113764cc35;

	// Mix identifying information from the formatted section, if available:
	if (formatted_bytes >= 2) salt += formatted_data[1]; // Processor Type
	if (formatted_bytes >= 3) salt += (u64)formatted_data[2] << 8; // Processor Family
	if (formatted_bytes >= 8) salt += (u64)*(u32*)&formatted_data[4] << 16; // Processor ID
	if (formatted_bytes >= 18) salt += (u64)*(u16*)&formatted_data[16] << 48; // Max Speed

	// Determine if serial number is present
	serial.strong = false;
	if (formatted_bytes >= 29)
	{
		u32 str_index = formatted_data[28];
		u8 *str = unformatted_data;
		u32 remaining = unformatted_bytes;

		// While there is still data to process and we haven't found the string yet,
		while (remaining > 0 && str_index > 1)
		{
			// Grab next character and shrink available space
			u8 ch = *str++;
			remaining--;

			// Subtract the string index for each string we pass
			if (ch == '\0') --str_index;
		}

		// If we found the string, check if it's strong
		if (str_index == 1) serial.strong = IsStrongSerial(str, remaining);
	}

	serial.id = MurmurHash64(unformatted_data, unformatted_bytes, salt);
}

bool BanIdentifiers::ParseSMBIOS(u8 *data, u32 bytes, HardwareSerial64 &system_uuid,
								 HardwareSerial64 &motherboard_serial, HardwareSerial64 &processor_serial)
{
	/*
		DMTF standard for System Management BIOS:
		http://www.dmtf.org/standards/published_documents/DSP0134_2.6.1.pdf

		SMBIOS format: Type(1) Length(1) Handle(2) FormattedData(x) UnformattedData(y) 00 00

			Type: Type of section
			Length: Bytes in section including header and formatted data parts
			Handle: Section number in the file, used for references between sections
			Formatted Data: Section data
			Unformatted Data: Variable-length section data
			00 00: Two-byte terminator for section
	*/

	bool success = false;

	while (bytes >= 4)
	{
		u8 type = data[0];
		u32 len = (u32)data[1] - 4;
		if (len < 0) break;
		u16 handle = *reinterpret_cast<u16*>(&data[2]);

		data += 4;
		bytes -= 4;

		if (bytes < len) break;

		u8 *formatted_data = data;
		u32 formatted_data_len = len;

		data += formatted_data_len;
		bytes -= formatted_data_len;

		u8 *unformatted_data = data;
		u32 unformatted_data_len = 0;

		// Find unformatted data length
		u8 last = 1;
		while (bytes > 0)
		{
			u8 current = *data++;
			--bytes;

			if (!current && !last)
			{
				// Unformatted data len does not include trailing 00 00
				unformatted_data_len = (u32)((data - unformatted_data) - 2);
				break;
			}

			last = current;
		}

		// Handle section
		// NOTE: Not guaranteed that unformatted_data is a valid pointer or null-terminated.
		switch (type)
		{
		case 1: // System UUID
			ParseSystemUUID(system_uuid, formatted_data, formatted_data_len, unformatted_data, unformatted_data_len);
			success = true;
			break;

		case 2: // Motherboard Serial
			ParseMotherboard(motherboard_serial, formatted_data, formatted_data_len, unformatted_data, unformatted_data_len);
			success = true;
			break;

		case 4: // Processor Serial
			ParseProcessor(processor_serial, formatted_data, formatted_data_len, unformatted_data, unformatted_data_len);
			success = true;
			break;
		}
	}

	return success;
}

bool BanIdentifiers::GetBIOSInfo(HardwareSerial64 &system_uuid, HardwareSerial64 &motherboard_serial, HardwareSerial64 &processor_serial)
{
	system_uuid.id = 0;
	system_uuid.strong = false;
	motherboard_serial.id = 0;
	motherboard_serial.strong = false;
	processor_serial.id = 0;
	processor_serial.strong = false;
	bool success = false;

	if (!_wmi || !_wmi->_wmi_enum) return false;

	// Reset enumerator to the start
	_wmi->_wmi_enum->Reset();

	HRESULT hr;

	do 
	{
		IWbemClassObject *obj = 0;
		ULONG count = 0;

		hr = _wmi->_wmi_enum->Next(WBEM_INFINITE, 1, &obj, &count);

		if (SUCCEEDED(hr))
		{
			VARIANT varBIOSData;
			VariantInit(&varBIOSData);
			CIMTYPE type;

			hr = obj->Get(L"SMBiosData", 0, &varBIOSData, &type, 0);

			if (SUCCEEDED(hr))
			{
				if (varBIOSData.vt == (VT_UI1 | VT_ARRAY))
				{
					SAFEARRAY *sdata = 0;
					sdata = V_ARRAY(&varBIOSData);
					BYTE *data = (BYTE*)sdata->pvData;
					DWORD bytes = sdata->rgsabound[0].cElements;

					success = ParseSMBIOS(data, bytes, system_uuid, motherboard_serial, processor_serial);
				}
			}

			VariantClear(&varBIOSData);
			break;
		}
	} while (hr == WBEM_S_NO_ERROR);

	return success;
}

bool BanIdentifiers::GetCanary1(HardwareSerial64 &serial)
{
	const u64 salt = 0xc67ab07134cc3576;

	serial.id = 0;
	serial.strong = false;

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	serial.id = MurmurHash64(&si, sizeof(si), salt);
	serial.strong = false;

	serial.id -= serial.id % 431;

	return true;
}

bool BanIdentifiers::GetCanary2(HardwareSerial32 &serial)
{
	const u32 salt = 0xc67a3576;

	serial.id = 0;
	serial.strong = false;

	TIME_ZONE_INFORMATION tzi;
	if (TIME_ZONE_ID_INVALID == GetTimeZoneInformation(&tzi))
		return false;

	serial.id = MurmurHash32(&tzi, sizeof(tzi), salt);
	serial.strong = false;

	serial.id -= serial.id % 487;

	return true;
}

bool BanIdentifiers::GetCanary3(HardwareSerial64 &serial)
{
	const u64 salt = 0xc67ab07134cc3576;

	serial.id = 0;
	serial.strong = false;

	HW_PROFILE_INFO hwp;
	if (!GetCurrentHwProfile(&hwp))
		return false;

	serial.id = MurmurHash64(&hwp, sizeof(hwp), salt);
	serial.strong = true;

	serial.id -= serial.id % 521;

	return true;
}
