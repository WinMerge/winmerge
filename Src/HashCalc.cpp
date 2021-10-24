/** 
 * @file  HashCalc.cpp
 *
 * @brief Implementation file for HashCalc
 */
#include "pch.h"
#include "HashCalc.h"

#ifdef _WIN64

#pragma comment(lib, "bcrypt.lib")
 
static NTSTATUS CalculateHashValue(HANDLE hFile, BCRYPT_HASH_HANDLE hHash, ULONG hashSize, std::vector<uint8_t>& hash)
{
	hash.resize(hashSize);
	std::vector<uint8_t> buffer(8196);
	NTSTATUS status = 0;
	while (status == 0)
	{
		DWORD dwRead = 0;
		if (!ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()), &dwRead, nullptr))
		{
			status = 1; // STATUS_UNSUCCESSFUL
			hash.clear();
			break;
		}
		status = BCryptHashData(hHash, buffer.data(), dwRead, 0);
		if (buffer.size() != dwRead)
			break;
	}
	if (status == 0)
	{
		status = BCryptFinishHash(hHash, hash.data(), static_cast<ULONG>(hash.size()), 0);
	}
	return status;
}

NTSTATUS CalculateHashValue(HANDLE hFile, const wchar_t *pAlgoId, std::vector<uint8_t>& hash)
{
	hash.clear();
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, pAlgoId, nullptr, 0);
	if (status == 0)
	{
		ULONG bytesWritten = 0;
		ULONG objectSize = 0;
		status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectSize), sizeof(DWORD), &bytesWritten, 0);
		if (status == 0)
		{
			std::vector<uint8_t> hashObject(objectSize);
			BCRYPT_HASH_HANDLE hHash = nullptr;
			status = BCryptCreateHash(hAlg, &hHash, hashObject.data(), static_cast<ULONG>(hashObject.size()), nullptr, 0, 0);
			if (status == 0)
			{
				ULONG hashSize = 0;
				status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hashSize), sizeof(DWORD), &bytesWritten, 0);
				if (status == 0)
				{
					status = CalculateHashValue(hFile, hHash, hashSize, hash);
				}
				BCryptDestroyHash(hHash);
			}
		}
		BCryptCloseAlgorithmProvider(hAlg, 0);
	}
	return status;
}

#endif