#pragma once

#include <cstdint>
#include <memory>
#include "string_util.h"

#ifndef ICU_EXTERN
#define ICU_EXTERN extern
#endif

#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((UChar32)(lead)<<10UL)+(UChar32)(trail)-U16_SURROGATE_OFFSET)
#define U16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)
#define U_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SURROGATE(c) U_IS_SURROGATE(c)
#define UBRK_DONE ((int32_t) -1)

typedef enum UBreakIteratorType { UBRK_CHARACTER = 0, UBRK_WORD = 1, UBRK_LINE = 2, UBRK_SENTENCE = 3 } UBreakIteratorType;
typedef int32_t UChar32;
typedef char16_t UChar;
typedef struct UBreakIterator UBreakIterator;
typedef enum UErrorCode { U_ZERO_ERROR = 0 } UErrorCode;
enum { U_PARSE_CONTEXT_LEN = 16 };
typedef struct UParseError {
	int32_t line;
	int32_t offset;
	UChar   preContext[U_PARSE_CONTEXT_LEN];
	UChar   postContext[U_PARSE_CONTEXT_LEN];
} UParseError;

class ICUBreakIterator;

template<int N>
extern thread_local std::unique_ptr<ICUBreakIterator> m_pCharaterBreakIterator;

typedef UBreakIterator* (*ubrk_open_type)(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status);
ICU_EXTERN UBreakIterator* (*g_pubrk_open)(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status);
inline UBreakIterator* ubrk_open(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status)
{
	return g_pubrk_open(type, locale, text, textLength, status);
}

typedef UBreakIterator* (*ubrk_openRules_type)(const UChar *rules, int32_t rulesLength, const UChar *text, int32_t textLength, UParseError *parseErr, UErrorCode *status);
ICU_EXTERN UBreakIterator* (*g_pubrk_openRules)(const UChar *rules, int32_t rulesLength, const UChar *text, int32_t textLength, UParseError *parseErr, UErrorCode *status);
inline UBreakIterator* ubrk_openRules(const UChar *rules, int32_t rulesLength, const UChar *text, int32_t textLength, UParseError *parseErr, UErrorCode *status)
{
	return g_pubrk_openRules(rules, rulesLength, text, textLength, parseErr, status);
}

typedef void (*ubrk_setText_type)(UBreakIterator *bi, const UChar* text, int32_t textLength, UErrorCode* status);
ICU_EXTERN void (*g_pubrk_setText)(UBreakIterator *bi, const UChar* text, int32_t textLength, UErrorCode* status);
inline void ubrk_setText(UBreakIterator *bi, const UChar* text, int32_t textLength, UErrorCode* status)
{
	g_pubrk_setText(bi, text, textLength, status);
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

typedef int32_t (*ubrk_previous_type)(UBreakIterator* bi);
ICU_EXTERN int32_t (*g_pubrk_previous)(UBreakIterator* bi);
inline int32_t ubrk_previous(UBreakIterator* bi)
{
	return g_pubrk_previous(bi);
}

typedef int32_t (*ubrk_next_type)(UBreakIterator* bi);
ICU_EXTERN int32_t (*g_pubrk_next)(UBreakIterator* bi);
inline int32_t ubrk_next(UBreakIterator* bi)
{
	return g_pubrk_next(bi);
}

typedef int32_t (*ubrk_preceding_type)(UBreakIterator* bi, int32_t offset);
ICU_EXTERN int32_t (*g_pubrk_preceding)(UBreakIterator* bi, int32_t offset);
inline int32_t ubrk_preceding(UBreakIterator* bi, int32_t offset)
{
	return g_pubrk_preceding(bi, offset);
}

typedef int32_t (*ubrk_following_type)(UBreakIterator* bi, int32_t offset);
ICU_EXTERN int32_t (*g_pubrk_following)(UBreakIterator* bi, int32_t offset);
inline int32_t ubrk_following(UBreakIterator* bi, int32_t offset)
{
	return g_pubrk_following(bi, offset);
}

class ICULoader {
public:
	ICULoader()
	{
		m_hLibrary = LoadLibraryW(L"icu.dll");
		if (!m_hLibrary)
			return;
		g_pubrk_open = reinterpret_cast<ubrk_open_type>(GetProcAddress(m_hLibrary, "ubrk_open"));
		g_pubrk_openRules = reinterpret_cast<ubrk_openRules_type>(GetProcAddress(m_hLibrary, "ubrk_openRules"));
		g_pubrk_setText = reinterpret_cast<ubrk_setText_type>(GetProcAddress(m_hLibrary, "ubrk_setText"));
		g_pubrk_close = reinterpret_cast<ubrk_close_type>(GetProcAddress(m_hLibrary, "ubrk_close"));
		g_pubrk_first = reinterpret_cast<ubrk_first_type>(GetProcAddress(m_hLibrary, "ubrk_first"));
		g_pubrk_previous = reinterpret_cast<ubrk_previous_type>(GetProcAddress(m_hLibrary, "ubrk_previous"));
		g_pubrk_next = reinterpret_cast<ubrk_next_type>(GetProcAddress(m_hLibrary, "ubrk_next"));
		g_pubrk_preceding = reinterpret_cast<ubrk_preceding_type>(GetProcAddress(m_hLibrary, "ubrk_preceding"));
		g_pubrk_following = reinterpret_cast<ubrk_following_type>(GetProcAddress(m_hLibrary, "ubrk_following"));
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
		: m_iter(nullptr), m_type(type), m_text(text), m_i(0), m_textLength(textLength)
	{
		if (ICULoader::IsLoaded())
		{
			UErrorCode status = U_ZERO_ERROR;
			if (type == UBRK_CHARACTER)
			{
				UParseError parseError;
				m_iter = ubrk_openRules(kCustomRules, static_cast<int32_t>(wcslen(reinterpret_cast<const wchar_t *>(kCustomRules))), text, textLength, &parseError, &status);
			}
			else
			{
				m_iter = ubrk_open(type, locale, reinterpret_cast<const UChar *>(text), textLength, &status);
			}
			if (m_iter)
				ubrk_first(m_iter);
		}
	}

	~ICUBreakIterator()
	{
		if (m_iter)
			ubrk_close(m_iter);
	}

	UErrorCode setText(const UChar* text, int32_t textLength)
	{
		m_text = text;
		m_textLength = textLength;
		m_i = 0;
		UErrorCode status = U_ZERO_ERROR;
		if (m_iter)
			ubrk_setText(m_iter, text, textLength, &status);
		return status;
	}

	int first()
	{
		if (m_iter)
			return ubrk_first(m_iter);
		m_i = 0;
		return m_i;
	}

	int next()
	{
		if (m_iter)
		{
			int pos = ubrk_next(m_iter);
			return (pos == UBRK_DONE) ? m_textLength : pos;
		}
		return mynext();
	}

	int previous()
	{
		if (m_iter)
			return ubrk_previous(m_iter);
		return myprevious();
	}

	int preceding(int32_t offset)
	{
		if (m_iter)
		{
			int pos = ubrk_preceding(m_iter, offset);
			return (pos == UBRK_DONE) ? 0: pos;
		}
		return mypreceding(offset);
	}

	int following(int32_t offset)
	{
		if (m_iter)
		{
			int pos = ubrk_following(m_iter, offset);
			return (pos == UBRK_DONE) ? m_textLength : pos;
		}
		return myfollowing(offset);
	}

	static ICUBreakIterator *getCharacterBreakIterator(const UChar * text, int32_t textLength)
	{
		return getCharacterBreakIterator<1>(text, textLength);
	}

	template<int N>
	static ICUBreakIterator *getCharacterBreakIterator(const UChar * text, int32_t textLength)
	{
		if (!m_pCharaterBreakIterator<N>)
			m_pCharaterBreakIterator<N>.reset(new ICUBreakIterator(UBRK_CHARACTER, "en", text, textLength));
		else
			m_pCharaterBreakIterator<N>->setText(text, textLength);
		return m_pCharaterBreakIterator<N>.get();
	}

private:
	int mynext()
	{
		if (m_type == UBRK_CHARACTER)
		{
			m_i += U16_IS_SURROGATE(m_text[m_i]) ? 2 : 1;
		}
		else if (m_type == UBRK_WORD)
		{
			m_i = following(m_i);
		}
		return m_i;
	}

	int myprevious()
	{
		m_i = mypreceding(m_i);
		return m_i;
	}

	int mypreceding(int32_t offset)
	{
		if (m_type == UBRK_CHARACTER)
		{
			if (offset <= 1)
			{
				m_i = offset - 1;
				if (m_i < 0)
					m_i = UBRK_DONE;
			}
			else
				m_i = offset - (U16_IS_SURROGATE(m_text[offset - 1]) ? 2 : 1);
		}
		else if (m_type == UBRK_WORD)
		{
			int nPos = offset;
			int nPrevPos;
			while (nPos > 0 && xisspace(m_text[nPrevPos = nPos - (U16_IS_SURROGATE(m_text[nPos - 1]) ? 2 : 1)]))
				nPos = nPrevPos;
			if (nPos > 0)
			{
				nPrevPos = nPos - (U16_IS_SURROGATE(m_text[nPos - 1]) ? 2 : 1);
				nPos = nPrevPos;
				if (xisalnum(m_text[nPos]))
				{
					while (nPos > 0 && xisalnum(m_text[nPrevPos = nPos - (U16_IS_SURROGATE(m_text[nPos - 1]) ? 2 : 1)]))
						nPos = nPrevPos;
				}
				else
				{
					while (nPos > 0 && !xisalnum(m_text[nPrevPos = nPos - (U16_IS_SURROGATE(m_text[nPos - 1]) ? 2 : 1)])
						&& !xisspace(m_text[nPrevPos]))
						nPos = nPrevPos;
				}
			}
			m_i = nPos;
		}
		return m_i;
	}

	int myfollowing(int32_t offset)
	{
		if (m_type == UBRK_CHARACTER)
		{
			m_i = offset + (U16_IS_SURROGATE(m_text[offset]) ? 2 : 1);
		}
		else if (m_type == UBRK_WORD)
		{
			int nPos = offset;
			if (xisalnum(m_text[nPos]))
			{
				while (nPos < m_textLength && xisalnum(m_text[nPos]))
					nPos += (U16_IS_SURROGATE(m_text[nPos]) ? 2 : 1);
			}
			else
			{
				while (nPos < m_textLength && !xisalnum(m_text[nPos])
					&& !iswspace(m_text[nPos]))
					nPos += (U16_IS_SURROGATE(m_text[nPos]) ? 2 : 1);
			}
			m_i = nPos;
		}
		return m_i;
	}
	UBreakIterator* m_iter;
	UBreakIteratorType m_type;
	const UChar *m_text;
	int m_i;
	int m_textLength;
	static const UChar *kCustomRules;
};

