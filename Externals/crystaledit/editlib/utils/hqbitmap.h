// SPDX-License-Identifier: BSL-1.0
// Copyright (c) 2020 Takashi Sawanaka
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <atlimage.h>

HBITMAP LoadBitmapAndConvertTo32bit(HINSTANCE hInstance, int nIDResource, int nNewWidth, int nNewHeight, bool bGrayscale, COLORREF clrMask);
