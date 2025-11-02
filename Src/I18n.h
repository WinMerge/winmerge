/**
 * @file I18n.h
 * @brief Internationalization (I18n) utility functions for string translation.
 */
#pragma once

#include "UnicodeString.h"
#include <string>

#define _(x) I18n::tr(x)
#define N_(x) (x)
#define NC_(c, x) (x)

namespace I18n
{
	String tr(const std::string &str);
	String tr(const std::wstring &str);
	String tr(const char *msgctxt, const std::string &str);
}
