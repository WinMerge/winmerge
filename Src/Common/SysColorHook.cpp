/**
 * @file  SysColorHook.h
 *
 * @brief Provides functionality to hook the GetSysColor() API and return custom colors.
 */
// SPDX-License-Identifier: MIT
#include "stdafx.h"
#include "IatHook.h"
#include "SysColorHook.h"

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

DWORD MyGetSysColor(int nIndex)
{
	if (nIndex < 0 || nIndex >= std::size(g_syscolor))
		return 0;
	return g_syscolor[nIndex].color;
}

HBRUSH MyGetSysColorBrush(int nIndex)
{
	if (nIndex < 0 || nIndex >= std::size(g_syscolor))
		return 0;
	return g_syscolor[nIndex].brush;
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
	for (int i = 0; i < std::size(g_syscolor); ++i)
	{
		if (!g_syscolor[i].isCustom)
		{
			g_syscolor[i].color = g_orgGetSysColor(i);
			g_syscolor[i].brush = g_orgGetSysColorBrush(i);
		}
	}
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
}

void SetSysColor(int nIndex, unsigned color)
{
	if (!g_orgGetSysColor)
		return;
	if (nIndex < 0 || nIndex >= std::size(g_syscolor))
		return;
	if (g_syscolor[nIndex].color == color)
		return;
	if (g_syscolor[nIndex].isCustom)
		DeleteObject(g_syscolor[nIndex].brush);
	g_syscolor[nIndex].brush = CreateSolidBrush(color);
	g_syscolor[nIndex].color = color;
	g_syscolor[nIndex].isCustom = true;
}

}
