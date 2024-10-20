/**
 * @file  SysColorHook.h
 *
 * @brief Provides functionality to hook the GetSysColor() API and return custom colors.
 */
#pragma once

namespace SysColorHook
{
void Init();
bool Hook(void* moduleBase);
void Unhook(void* moduleBase);
void SetSysColor(int nIndex, unsigned color);
unsigned GetOrgSysColor(int nIndex);
bool IsCustomSysColor(int nIndex);
size_t GetSysColorCount();
void Deserialize(const String& colorStr, COLORREF* colors = nullptr);
String Serialize();

}
