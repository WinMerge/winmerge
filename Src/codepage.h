/**
 * @file  codepage.h
 */
#pragma once

#ifndef CP_UTF8
#define CP_UTF8 65001
#define CP_ACP 0
#define CP_THREAD_ACP 3
#endif

#ifndef CP_UCS2LE
#define CP_UCS2LE 1200
#define CP_UCS2BE 1201
#endif

bool isCodepageInstalled(int codepage);
