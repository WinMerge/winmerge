/////////////////////////////////////////////////////////////////////////////
//    Dark mode for WinMerge
//    Copyright (C) 2025  ozone10
//    SPDX-License-Identifier: MPL-2.0
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  editlibdarkmode.h
 *
 * @brief Include file for Dark mode for editlib project used in WinMerge
 *
 */

#pragma once

#include "DarkModeSubclass.h"

#if !defined(_DARKMODELIB_NOT_USED) \
	&& (defined(__x86_64__) || defined(_M_X64) \
	|| defined(__arm64__) || defined(__arm64) || defined(_M_ARM64))
#define USE_DARKMODELIB
#endif
