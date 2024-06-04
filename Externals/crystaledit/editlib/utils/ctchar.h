// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdarg>

#if defined(_WIN32)
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define wcscasecmp _wcsicmp
#define wcsncasecmp _wcsnicmp
#define strcasecoll _stricoll
#define wcscasecoll _wcsicoll
#define strdup _strdup
extern "C" __declspec(dllimport) int __stdcall StrCmpLogicalW(const wchar_t* psz1, const wchar_t* psz2);
extern "C" __declspec(dllimport) char* __stdcall CharNextA(const char* psz);
extern "C" __declspec(dllimport) wchar_t* __stdcall CharNextW(const wchar_t* psz);
extern "C" __declspec(dllimport) char* __stdcall CharPrevA(const char* start, const char* psz);
extern "C" __declspec(dllimport) wchar_t* __stdcall CharPrevW(const wchar_t* start, const wchar_t* psz);
#define strcmplogical StrCmpLogicalA // TODO:
#define wcscmplogical StrCmpLogicalW
#define charnext CharNextA 
#define wcharnext CharNextW
#define charprev CharPrevA 
#define wcharprev CharPrevW
#define _strlcpy StringCchCopyA
#define _wcslcpy StringCchCopyW
#define _strlcat StringCchCatA
#define _wcslcat StringCchCatW
#else
#define _TRUNCATE ((size_t)-1) 
#define strcasecoll strcasecmp // TODO:
#define wcscasecoll wcscasecmp // TODO:
#define strcmplogical strcasecmp // TODO:
#define wcscmplogical wcscasecmp // TODO:
#define _vsnprintf_s vsnprintf // TODO:
// TODO:
inline void _strlcpy(char* dest, size_t cchdest, const char* src) { snprintf(dest, cchdest, "%s", src); }
inline void _wcslcpy(wchar_t* dest, size_t cchdest, const wchar_t* src) { swprintf(dest, cchdest, L"%s", src); }
inline void _strlcat(char* dest, size_t cchdest, const char* src) { snprintf(dest, cchdest, "%s%s", dest, src); }
inline void _wcslcat(wchar_t* dest, size_t cchdest, const wchar_t* src) { swprintf(dest, cchdest, L"%s%s", dest, src); }
inline int _vsnprintf_s(char* _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList) { return ::vsnprintf(_Buffer, _BufferCount, _Format, _ArgList); }
inline int _vsnwprintf_s(wchar_t* _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList) { return ::vswprintf(_Buffer,  _BufferCount, _Format, _ArgList); }
inline char* charnext(const char* psz) { return const_cast<char*>(psz + 1); }
inline wchar_t* wcharnext(const wchar_t* psz) { return const_cast<wchar_t*>(psz + 1); }
inline char* charprev(const char* start, const char* psz) { return const_cast<char*>((psz > start) ? psz - 1 : psz); }
inline wchar_t* wcharprev(const wchar_t* start, const wchar_t* psz) { return const_cast<wchar_t*>((psz > start) ? psz - 1 : psz); }
#endif

#ifdef _UNICODE
#include <wctype.h>
typedef wchar_t tchar_t;
#ifndef _T
#define _T(x) L##x
#endif
#else
#include <ctype.h>
typedef char tchar_t;
#ifndef _T
#define _T(x) x
#endif
#endif // _UNICODE

namespace tc
{
#ifdef _UNICODE
	constexpr auto istupper = ::iswupper;
	constexpr auto istlower = ::iswlower;
	constexpr auto istdigit = ::iswdigit;
	constexpr auto istspace = ::iswspace;
	constexpr auto istalnum = ::iswalnum;
	constexpr auto istalpha = ::iswalpha;
	constexpr auto totlower = ::towlower;
	constexpr auto totupper = ::towupper;
	constexpr auto tcscmp = ::wcscmp;
	constexpr auto tcsncmp = ::wcsncmp;
	constexpr auto tcsicmp = ::wcscasecmp;
	constexpr auto tcsnicmp = ::wcsncasecmp;
	constexpr auto tcscoll = ::wcscoll;
	constexpr auto tcsicoll = ::wcscasecoll;
	constexpr auto tcscmplogical = ::wcscmplogical;
	constexpr auto tcsspn = ::wcsspn;
	constexpr auto tcsdup = ::wcsdup;
	inline tchar_t* tcschr(const tchar_t* str, tchar_t ch) { return const_cast<tchar_t*>(::wcschr(str, ch)); }
	inline tchar_t* tcsrchr(const tchar_t* str, tchar_t ch) { return const_cast<tchar_t*>(::wcsrchr(str, ch)); }
	inline tchar_t* tcsstr(const tchar_t* str, const tchar_t* substr) { return const_cast<tchar_t*>(::wcsstr(str, substr)); }
	inline tchar_t* tcspbrk(const tchar_t* str, const tchar_t* charset) { return const_cast<tchar_t*>(::wcspbrk(str, charset)); }
	constexpr auto tcslen = ::wcslen;
	inline int ttoi(const tchar_t* str) { return static_cast<int>(wcstol(str, 0, 10)); }
	inline long ttol(const tchar_t* str) { return wcstol(str, 0, 10); }
	constexpr auto tcstol = wcstol;
	constexpr auto tcstoll = wcstoll;
	constexpr auto tcstod = wcstod;
	inline int vsntprintf_s(wchar_t* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList) { return ::_vsnwprintf_s(_Buffer, _BufferCount, _MaxCount, _Format, _ArgList); }
	constexpr auto tcharnext = ::wcharnext;
	constexpr auto tcharprev = ::wcharprev;
	inline void tcslcpy(wchar_t* dest, size_t cchdest, const wchar_t* src) { _wcslcpy(dest, cchdest, src); }
	inline void tcslcat(wchar_t* dest, size_t cchdest, const wchar_t* src) { _wcslcat(dest, cchdest, src); }
	inline tchar_t* tcsinc(const tchar_t* str) { return const_cast<tchar_t*>(str + 1); }
#else
	constexpr auto istupper = ::isupper;
	constexpr auto istlower = ::islower;
	constexpr auto istdigit = ::isdigit;
	constexpr auto istspace = ::isspace;
	constexpr auto istalnum = ::isalnum;
	constexpr auto istalpha = ::isalpha;
	constexpr auto totlower = ::tolower;
	constexpr auto totupper = ::toupper;
	constexpr auto tcscmp = ::strcmp;
	constexpr auto tcsncmp = ::strncmp;
	constexpr auto tcsicmp = ::strcasecmp;
	constexpr auto tcsnicmp = ::strncasecmp;
	constexpr auto tcscoll = ::strcoll;
	constexpr auto tcsicoll = ::strcasecoll;
	constexpr auto tcscmplogical = ::strcmplogical;
	constexpr auto tcsspn = ::strspn;
	constexpr auto tcsdup = ::strdup;
	inline tchar_t* tcschr(const tchar_t* str, tchar_t ch) { return const_cast<tchar_t*>(::strchr(str, ch)); }
	inline tchar_t* tcsrchr(const tchar_t* str, tchar_t ch) { return const_cast<tchar_t*>(::strrchr(str, ch)); }
	inline tchar_t* tcsstr(const tchar_t* str, const tchar_t* substr) { return const_cast<tchar_t*>(::strstr(str, substr)); }
	inline tchar_t* tcspbrk(const tchar_t* str, const tchar_t* charset) { return const_cast<tchar_t*>(::strpbrk(str, charset)); }
	constexpr auto tcslen = ::strlen;
	constexpr auto ttoi = ::atoi;
	constexpr auto ttol = ::atol;
	constexpr auto tcstol = strtol;
	constexpr auto tcstoll = strtoll;
	constexpr auto tcstod = strtod;
	inline int vsntprintf_s(char* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList) { return ::_vsnprintf_s(_Buffer, _BufferCount, _MaxCount, _Format, _ArgList); }
	constexpr auto tcharnext = ::charnext;
	constexpr auto tcharprev = ::charprev;
	inline void tcslcpy(char* dest, size_t cchdest, const char* src) { _strlcpy(dest, cchdest, src); }
	inline void tcslcat(char* dest, size_t cchdest, const char* src) { _strlcat(dest, cchdest, src); }
	inline tchar_t* tcsinc(const tchar_t* str) { return const_cast<tchar_t*>(str + 1); } // TODO:
#endif
	template <typename T, size_t N>
	void tcslcpy(T(&dst)[N], const T* src) { tcslcpy(reinterpret_cast<T*>(&dst), N, src); }
	template <typename T, size_t N>
	void tcslcat(T(&dst)[N], const T* src) { tcslcat(reinterpret_cast<T*>(&dst), N, src); }
}
