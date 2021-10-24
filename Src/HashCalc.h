/** 
 * @file  HashCalc.h
 *
 * @brief Declaration file for HashCalc
 */
#pragma once

#include <Windows.h>
#include <vector>

NTSTATUS CalculateHashValue(HANDLE hFile, const wchar_t* pAlgoId, std::vector<uint8_t>& hash);
