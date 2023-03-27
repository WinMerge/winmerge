/** 
 * @file  locality.h
 *
 * @brief Declaration of helper functions involving locale
 */
#pragma once

#include "UnicodeString.h"

namespace locality {

String NumToLocaleStr(int n);
String NumToLocaleStr(int64_t n);
String GetLocaleStr(const tchar_t *str, int decimalDigits = 0);
String TimeString(const int64_t * tim);

};
