// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <ctime>

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
inline void _wcslcpy(wchar_t* dest, size_t cchdest, const wchar_t* src) { swprintf(dest, cchdest, L"%ls", src); }
inline void _strlcat(char* dest, size_t cchdest, const char* src)
{
	size_t len = strlen(dest);
	if (len < cchdest)
		snprintf(dest + len, cchdest - len, "%s", src);
}
inline void _wcslcat(wchar_t* dest, size_t cchdest, const wchar_t* src)
{
	size_t len = wcslen(dest);
	if (len < cchdest)
		swprintf(dest + len, cchdest - len, L"%ls", src);
}
inline int _vsnprintf_s(char* _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList) { return ::vsnprintf(_Buffer, _BufferCount, _Format, _ArgList); }
inline int _vsnwprintf_s(wchar_t* _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList) { return ::vswprintf(_Buffer, _BufferCount, _Format, _ArgList); }
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
	inline int    istupper(wint_t c)    noexcept { return ::iswupper(c); }
	inline int    istlower(wint_t c)    noexcept { return ::iswlower(c); }
	inline int    istdigit(wint_t c)    noexcept { return ::iswdigit(c); }
	inline int    istspace(wint_t c)    noexcept { return ::iswspace(c); }
	inline int    istalnum(wint_t c)    noexcept { return ::iswalnum(c); }
	inline int    istalpha(wint_t c)    noexcept { return ::iswalpha(c); }
	inline wint_t totlower(wint_t c)    noexcept { return ::towlower(c); }
	inline wint_t totupper(wint_t c)    noexcept { return ::towupper(c); }
	inline int    tcscmp(const wchar_t* a, const wchar_t* b)                          noexcept { return ::wcscmp(a, b); }
	inline int    tcsncmp(const wchar_t* a, const wchar_t* b, size_t n)               noexcept { return ::wcsncmp(a, b, n); }
	inline int    tcsicmp(const wchar_t* a, const wchar_t* b)                         noexcept { return ::wcscasecmp(a, b); }
	inline int    tcsnicmp(const wchar_t* a, const wchar_t* b, size_t n)              noexcept { return ::wcsncasecmp(a, b, n); }
	inline int    tcscoll(const wchar_t* a, const wchar_t* b)                         noexcept { return ::wcscoll(a, b); }
	inline int    tcsicoll(const wchar_t* a, const wchar_t* b)                        noexcept { return ::wcscasecoll(a, b); }
	inline int    tcscmplogical(const wchar_t* a, const wchar_t* b)                   noexcept { return ::wcscmplogical(a, b); }
	inline size_t tcsspn(const wchar_t* str, const wchar_t* accept)                   noexcept { return ::wcsspn(str, accept); }
	inline wchar_t* tcsdup(const wchar_t* str)
	{
#if defined(_WIN32)
		return ::_wcsdup(str);
#else
		return ::wcsdup(str);
#endif
	}
	inline size_t tcsftime(wchar_t* s, size_t max, const wchar_t* fmt, const struct tm* tm) noexcept { return ::wcsftime(s, max, fmt, tm); }
	inline tchar_t* tcschr(const tchar_t* str, tchar_t ch)    noexcept { return const_cast<tchar_t*>(::wcschr(str, ch)); }
	inline tchar_t* tcsrchr(const tchar_t* str, tchar_t ch)   noexcept { return const_cast<tchar_t*>(::wcsrchr(str, ch)); }
	inline tchar_t* tcsstr(const tchar_t* str, const tchar_t* substr)   noexcept { return const_cast<tchar_t*>(::wcsstr(str, substr)); }
	inline tchar_t* tcspbrk(const tchar_t* str, const tchar_t* charset) noexcept { return const_cast<tchar_t*>(::wcspbrk(str, charset)); }
	inline size_t tcslen(const wchar_t* str)                             noexcept { return ::wcslen(str); }
	inline int    ttoi(const tchar_t* str)  noexcept { return static_cast<int>(::wcstol(str, nullptr, 10)); }
	inline long   ttol(const tchar_t* str)  noexcept { return ::wcstol(str, nullptr, 10); }
	inline long        tcstol(const wchar_t* str, wchar_t** end, int base)  noexcept { return ::wcstol(str, end, base); }
	inline long long   tcstoll(const wchar_t* str, wchar_t** end, int base) noexcept { return ::wcstoll(str, end, base); }
	inline double      tcstod(const wchar_t* str, wchar_t** end)            noexcept { return ::wcstod(str, end); }
	inline int vsntprintf_s(wchar_t* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList) noexcept { return ::_vsnwprintf_s(_Buffer, _BufferCount, _MaxCount, _Format, _ArgList); }
	inline tchar_t* tcharnext(const tchar_t* psz)                          noexcept { return ::wcharnext(psz); }
	inline tchar_t* tcharprev(const tchar_t* start, const tchar_t* psz)   noexcept { return ::wcharprev(start, psz); }
	inline void tcslcpy(wchar_t* dest, size_t cchdest, const wchar_t* src) noexcept { _wcslcpy(dest, cchdest, src); }
	inline void tcslcat(wchar_t* dest, size_t cchdest, const wchar_t* src) noexcept { _wcslcat(dest, cchdest, src); }
	inline tchar_t* tcsinc(const tchar_t* str)                             noexcept { return const_cast<tchar_t*>(str + 1); }
#else
	inline int    istupper(int c) noexcept { return ::isupper(static_cast<unsigned char>(c)); }
	inline int    istlower(int c) noexcept { return ::islower(static_cast<unsigned char>(c)); }
	inline int    istdigit(int c) noexcept { return ::isdigit(static_cast<unsigned char>(c)); }
	inline int    istspace(int c) noexcept { return ::isspace(static_cast<unsigned char>(c)); }
	inline int    istalnum(int c) noexcept { return ::isalnum(static_cast<unsigned char>(c)); }
	inline int    istalpha(int c) noexcept { return ::isalpha(static_cast<unsigned char>(c)); }
	inline int    totlower(int c) noexcept { return ::tolower(static_cast<unsigned char>(c)); }
	inline int    totupper(int c) noexcept { return ::toupper(static_cast<unsigned char>(c)); }
	inline int    tcscmp(const char* a, const char* b)               noexcept { return ::strcmp(a, b); }
	inline int    tcsncmp(const char* a, const char* b, size_t n)    noexcept { return ::strncmp(a, b, n); }
	inline int    tcsicmp(const char* a, const char* b)              noexcept { return ::strcasecmp(a, b); }
	inline int    tcsnicmp(const char* a, const char* b, size_t n)   noexcept { return ::strncasecmp(a, b, n); }
	inline int    tcscoll(const char* a, const char* b)              noexcept { return ::strcoll(a, b); }
	inline int    tcsicoll(const char* a, const char* b)             noexcept { return ::strcasecoll(a, b); }
	inline int    tcscmplogical(const char* a, const char* b)        noexcept { return ::strcmplogical(a, b); }
	inline size_t tcsspn(const char* str, const char* accept)        noexcept { return ::strspn(str, accept); }
	inline char* tcsdup(const char* str) { return ::strdup(str); }
	inline size_t tcsftime(char* s, size_t max, const char* fmt, const struct tm* tm) noexcept { return ::strftime(s, max, fmt, tm); }
	inline tchar_t* tcschr(const tchar_t* str, tchar_t ch)    noexcept { return const_cast<tchar_t*>(::strchr(str, ch)); }
	inline tchar_t* tcsrchr(const tchar_t* str, tchar_t ch)   noexcept { return const_cast<tchar_t*>(::strrchr(str, ch)); }
	inline tchar_t* tcsstr(const tchar_t* str, const tchar_t* substr)   noexcept { return const_cast<tchar_t*>(::strstr(str, substr)); }
	inline tchar_t* tcspbrk(const tchar_t* str, const tchar_t* charset) noexcept { return const_cast<tchar_t*>(::strpbrk(str, charset)); }
	inline size_t tcslen(const char* str)                               noexcept { return ::strlen(str); }
	inline int    ttoi(const tchar_t* str) noexcept { return ::atoi(str); }
	inline long   ttol(const tchar_t* str) noexcept { return ::atol(str); }
	inline long        tcstol(const char* str, char** end, int base)  noexcept { return ::strtol(str, end, base); }
	inline long long   tcstoll(const char* str, char** end, int base) noexcept { return ::strtoll(str, end, base); }
	inline double      tcstod(const char* str, char** end)            noexcept { return ::strtod(str, end); }
	inline int vsntprintf_s(char* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList) noexcept { return ::_vsnprintf_s(_Buffer, _BufferCount, _MaxCount, _Format, _ArgList); }
	inline tchar_t* tcharnext(const tchar_t* psz)                        noexcept { return ::charnext(psz); }
	inline tchar_t* tcharprev(const tchar_t* start, const tchar_t* psz) noexcept { return ::charprev(start, psz); }
	inline void tcslcpy(char* dest, size_t cchdest, const char* src) noexcept { _strlcpy(dest, cchdest, src); }
	inline void tcslcat(char* dest, size_t cchdest, const char* src) noexcept { _strlcat(dest, cchdest, src); }
	inline tchar_t* tcsinc(const tchar_t* str) noexcept { return const_cast<tchar_t*>(str + 1); } // TODO:
#endif
	template <typename T, size_t N>
	void tcslcpy(T(&dst)[N], const T* src) { tcslcpy(reinterpret_cast<T*>(&dst), N, src); }
	template <typename T, size_t N>
	void tcslcat(T(&dst)[N], const T* src) { tcslcat(reinterpret_cast<T*>(&dst), N, src); }
}