// SPDX-License-Identifier: BSL-1.0
// Copyright (c) 2019 Takashi Sawanaka
#include "StdAfx.h"
#define ICU_EXTERN
#include "icu.hpp"
#include "icurules.h"
#include "icucharbrkbinrules.h"
#include "icuwordbrkbinrules.h"

template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<1>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<2>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<3>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<4>;
thread_local std::unique_ptr<ICUBreakIterator> m_pWordBreakIterator;

class ICULoaderImpl {
#ifdef _WIN32
public:
	ICULoaderImpl()
	{
		m_hLibrary = LoadLibraryW(L"icu.dll");
		if (!m_hLibrary)
			return;
		u_getVersion = reinterpret_cast<u_getVersion_type>(GetProcAddress(m_hLibrary, "u_getVersion"));
		ubrk_open = reinterpret_cast<ubrk_open_type>(GetProcAddress(m_hLibrary, "ubrk_open"));
		ubrk_openRules = reinterpret_cast<ubrk_openRules_type>(GetProcAddress(m_hLibrary, "ubrk_openRules"));
		ubrk_openBinaryRules = reinterpret_cast<ubrk_openBinaryRules_type>(GetProcAddress(m_hLibrary, "ubrk_openBinaryRules"));
		ubrk_setText = reinterpret_cast<ubrk_setText_type>(GetProcAddress(m_hLibrary, "ubrk_setText"));
		ubrk_close = reinterpret_cast<ubrk_close_type>(GetProcAddress(m_hLibrary, "ubrk_close"));
		ubrk_first = reinterpret_cast<ubrk_first_type>(GetProcAddress(m_hLibrary, "ubrk_first"));
		ubrk_previous = reinterpret_cast<ubrk_previous_type>(GetProcAddress(m_hLibrary, "ubrk_previous"));
		ubrk_next = reinterpret_cast<ubrk_next_type>(GetProcAddress(m_hLibrary, "ubrk_next"));
		ubrk_preceding = reinterpret_cast<ubrk_preceding_type>(GetProcAddress(m_hLibrary, "ubrk_preceding"));
		ubrk_following = reinterpret_cast<ubrk_following_type>(GetProcAddress(m_hLibrary, "ubrk_following"));
		ubrk_next = reinterpret_cast<ubrk_next_type>(GetProcAddress(m_hLibrary, "ubrk_next"));

	}
	~ICULoaderImpl()
	{
		if (m_hLibrary)
			FreeLibrary(m_hLibrary);
	}
	ICULoaderImpl(const ICULoaderImpl &) = delete;
	ICULoaderImpl & operator=(const ICULoaderImpl &) = delete;

	bool IsLoaded() { return m_hLibrary != nullptr && ubrk_open != nullptr; }
private:
	HMODULE m_hLibrary = nullptr;
#else
public:
	ICULoaderImpl() { }
	bool IsLoaded() { return false; }
#endif
};

static std::unique_ptr<ICULoaderImpl> m_loaderImpl{ new ICULoaderImpl() };

bool ICULoader::IsLoaded()
{
	return m_loaderImpl->IsLoaded();
}

