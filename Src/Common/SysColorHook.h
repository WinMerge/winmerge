/**
 * @file  SysColorHook.h
 *
 * @brief Provides functionality to hook the GetSysColor() API and return custom colors.
 */
#pragma once

namespace SysColorHook
{
bool Hook(void* moduleBase);
void Unhook(void* moduleBase);
void SetSysColor(int nIndex, unsigned color);
bool IsCustomSysColor(int nIndex);
size_t GetSysColorCount();
}
