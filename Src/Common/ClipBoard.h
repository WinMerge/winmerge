/**
 * @file  ClipBoard.h
 *
 * @brief ClipBoard helper functions definitions.
 */
#pragma once

#include <windows.h>
#include "UnicodeString.h"

bool PutToClipboard(const String & text, HWND currentWindowHandle);
bool GetFromClipboard(String & text, HWND currentWindowHandle);
