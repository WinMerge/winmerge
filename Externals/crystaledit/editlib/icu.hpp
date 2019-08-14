#include <cstdint>
#include "icu.h"

#ifndef ICU_EXTERN
#define ICU_EXTERN extern
#endif

typedef UBreakIterator* (*ubrk_open_type)(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status);
ICU_EXTERN UBreakIterator* (*g_pubrk_open)(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status);
inline UBreakIterator* ubrk_open(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status)
{
	return g_pubrk_open(type, locale, text, textLength, status);
}

typedef void (*ubrk_close_type)(UBreakIterator* bi);
ICU_EXTERN void (*g_pubrk_close)(UBreakIterator* bi);
inline void ubrk_close(UBreakIterator* bi)
{
	g_pubrk_close(bi);
}

typedef int32_t (*ubrk_first_type)(UBreakIterator* bi);
ICU_EXTERN int32_t (*g_pubrk_first)(UBreakIterator* bi);
inline int32_t ubrk_first(UBreakIterator* bi)
{
	return g_pubrk_first(bi);
}

typedef int32_t (*ubrk_next_type)(UBreakIterator* bi);
ICU_EXTERN int32_t (*g_pubrk_next)(UBreakIterator* bi);
inline int32_t ubrk_next(UBreakIterator* bi)
{
	return g_pubrk_next(bi);
}

class ICULoader {
public:
	ICULoader()
	{
		m_hLibrary = LoadLibraryW(L"icu.dll");
		if (!m_hLibrary)
			return;
		g_pubrk_open = reinterpret_cast<ubrk_open_type>(GetProcAddress(m_hLibrary, "ubrk_open"));
		g_pubrk_close = reinterpret_cast<ubrk_close_type>(GetProcAddress(m_hLibrary, "ubrk_close"));
		g_pubrk_first = reinterpret_cast<ubrk_first_type>(GetProcAddress(m_hLibrary, "ubrk_first"));
		g_pubrk_next = reinterpret_cast<ubrk_next_type>(GetProcAddress(m_hLibrary, "ubrk_next"));

	}
	~ICULoader()
	{
		if (m_hLibrary)
			FreeLibrary(m_hLibrary);
	}
	static bool IsLoaded() { return m_hLibrary != nullptr && g_pubrk_open != nullptr; }
private:
	static HMODULE m_hLibrary;
};

class ICUBreakIterator
{
public:
	ICUBreakIterator(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength)
		: m_iter(nullptr), m_i(0), m_textLength(textLength)
	{
		if (ICULoader::IsLoaded())
		{
			UErrorCode status = U_ZERO_ERROR;
			m_iter = ubrk_open(type, locale, reinterpret_cast<const UChar *>(text), textLength, &status);
			if (m_iter)
				ubrk_first(m_iter);
		}
	}
	~ICUBreakIterator()
	{
		if (m_iter)
			ubrk_close(m_iter);
	}

	int next()
	{
		if (m_iter)
		{
			int pos = ubrk_next(m_iter);
			return (pos == UBRK_DONE) ? m_textLength : pos;
		}
		else
		{
			m_i++;
			return m_i;
		}
	}
private:
	UBreakIterator* m_iter;
	int m_i;
	int m_textLength;
};

