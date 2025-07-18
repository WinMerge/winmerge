/**
 * @file  SysColorHook.cpp
 *
 * @brief Provides functionality to hook the GetSysColor() API and return custom colors.
 */
// SPDX-License-Identifier: MIT
#include "stdafx.h"
#include "SysColorHook.h"
#include "MergeDarkMode.h"

#if defined(_DARKMODELIB_EXTERNAL_IATHOOK) || !defined(USE_DARKMODELIB)
#include "IatHook.h"
#else
extern PIMAGE_THUNK_DATA FindAddressByName(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, const char* funcName);
extern PIMAGE_THUNK_DATA FindAddressByOrdinal(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, uint16_t ordinal);
extern PIMAGE_THUNK_DATA FindIatThunkInModule(void* moduleBase, const char* dllName, const char* funcName);
extern PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, const char* funcName);
extern PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, uint16_t ordinal);
#endif

using fnGetSysColor = DWORD(WINAPI*)(int nIndex);
using fnGetSysColorBrush = HBRUSH(WINAPI*)(int nIndex);

struct SysColor
{
	DWORD color;
	HBRUSH brush;
	bool isCustom;
} g_syscolor[COLOR_MENUBAR + 1];

static fnGetSysColor g_orgGetSysColor;
static fnGetSysColorBrush g_orgGetSysColorBrush;

namespace SysColorHook
{

DWORD __stdcall MyGetSysColor(int nIndex)
{
	if (nIndex < 0 || nIndex >= static_cast<int>(std::size(g_syscolor)))
		return 0;
	return g_syscolor[nIndex].color;
}

HBRUSH __stdcall MyGetSysColorBrush(int nIndex)
{
	if (nIndex < 0 || nIndex >= static_cast<int>(std::size(g_syscolor)))
		return 0;
	return g_syscolor[nIndex].brush;
}

bool IsCustomSysColor(int nIndex)
{
	if (nIndex < 0 || nIndex >= static_cast<int>(std::size(g_syscolor)))
		return false;
	return g_syscolor[nIndex].isCustom;
}

size_t GetSysColorCount()
{
	return std::size(g_syscolor);
}

unsigned GetOrgSysColor(int nIndex)
{
	if (!g_orgGetSysColor)
		return GetSysColor(nIndex);
	return g_orgGetSysColor(nIndex);
}

uintptr_t ReplaceFunction(IMAGE_THUNK_DATA* addr, uintptr_t newFunction)
{
	DWORD oldProtect;
	if (!VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), PAGE_READWRITE, &oldProtect))
		return 0;
	uintptr_t oldFunction = addr->u1.Function;
	addr->u1.Function = newFunction;
	VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), oldProtect, &oldProtect);
	return oldFunction;
}

void Init()
{
	auto getSysColor = (g_orgGetSysColor ? g_orgGetSysColor : GetSysColor);
	auto getSysColorBrush = (g_orgGetSysColorBrush ? g_orgGetSysColorBrush : GetSysColorBrush);
	for (int i = 0; i < static_cast<int>(std::size(g_syscolor)); ++i)
	{
		if (g_syscolor[i].isCustom)
			DeleteObject(g_syscolor[i].brush);
		g_syscolor[i].color = getSysColor(i);
		g_syscolor[i].brush = getSysColorBrush(i);
		g_syscolor[i].isCustom = false;
	}
}

bool Hook(void* moduleBase)
{
	auto addrGetSysColor = FindIatThunkInModule(moduleBase, "user32.dll", "GetSysColor");
	auto addrGetSysColorBrush = FindIatThunkInModule(moduleBase, "user32.dll", "GetSysColorBrush");
	if (!addrGetSysColor || !addrGetSysColorBrush)
		return false;
	auto orgGetSysColor = reinterpret_cast<fnGetSysColor>(ReplaceFunction(addrGetSysColor, reinterpret_cast<uintptr_t>(static_cast<fnGetSysColor>(MyGetSysColor))));
	if (!g_orgGetSysColor)
		g_orgGetSysColor = orgGetSysColor;
	auto orgGetSysColorBrush = reinterpret_cast<fnGetSysColorBrush>(ReplaceFunction(addrGetSysColorBrush, reinterpret_cast<uintptr_t>(static_cast<fnGetSysColorBrush>(MyGetSysColorBrush))));
	if (!g_orgGetSysColorBrush)
		g_orgGetSysColorBrush = orgGetSysColorBrush;
	return true;
}

void Unhook(void* moduleBase)
{
	if (!g_orgGetSysColor)
		return;
	auto addrGetSysColor = FindIatThunkInModule(moduleBase, "user32.dll", "GetSysColor");
	auto addrGetSysColorBrush = FindIatThunkInModule(moduleBase, "user32.dll", "GetSysColorBrush");
	if (!addrGetSysColor || !addrGetSysColorBrush)
		return;
	ReplaceFunction(addrGetSysColor, reinterpret_cast<uintptr_t>(static_cast<fnGetSysColor>(g_orgGetSysColor)));
	ReplaceFunction(addrGetSysColorBrush, reinterpret_cast<uintptr_t>(static_cast<fnGetSysColorBrush>(g_orgGetSysColorBrush)));
	g_orgGetSysColor = nullptr;
	g_orgGetSysColorBrush = nullptr;
}

void SetSysColor(int nIndex, unsigned color)
{
	if (nIndex < 0 || nIndex >= static_cast<int>(std::size(g_syscolor)))
		return;
	if (g_syscolor[nIndex].color == color)
		return;
	if (g_syscolor[nIndex].isCustom)
		DeleteObject(g_syscolor[nIndex].brush);
	g_syscolor[nIndex].brush = CreateSolidBrush(color);
	g_syscolor[nIndex].color = color;
	g_syscolor[nIndex].isCustom = true;
}

void Deserialize(const String& colorsStr, COLORREF* colors)
{
	auto sysColorMapping = strutils::split(colorsStr, ',');
	for (auto&& sysColorEntry : sysColorMapping)
	{
		auto pair = strutils::split(sysColorEntry, ':');
		if (pair.size() == 2)
		{
			const int index = tc::ttoi(String(pair[0].data(), pair[0].length()).c_str());
			tchar_t* endptr = nullptr;
			const String colorStr = String(pair[1].data(), pair[1].length());
			unsigned color = static_cast<unsigned>(tc::tcstoll(colorStr.c_str(), &endptr,
				(colorStr.length() >= 2 && colorStr[1] == 'x') ? 16 : 10));
			if (colors)
			{
				if (index >= 0 && index < static_cast<int>(std::size(g_syscolor)))
					colors[index] = color;
			}
			else
				SysColorHook::SetSysColor(index, color);
		}
	}
}

String Serialize()
{
	std::vector<String> sysColorMapping;
	const int count = static_cast<int>(SysColorHook::GetSysColorCount());
	for (int i = 0; i < count; ++i)
	{
		if (g_syscolor[i].isCustom)
			sysColorMapping.push_back(strutils::format(_T("%d:0x%08x"), i, g_syscolor[i].color));
	}
	return strutils::join(sysColorMapping.begin(), sysColorMapping.end(), _T(","));
}

}


