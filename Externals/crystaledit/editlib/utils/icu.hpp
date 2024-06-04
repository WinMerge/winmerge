// Copyright (c) 2019 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
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
#define U_MAX_VERSION_LENGTH 4

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
typedef uint8_t UVersionInfo[U_MAX_VERSION_LENGTH];

class ICULoaderImpl;
class ICUBreakIterator;

template<int N>
extern thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator;
extern thread_local std::unique_ptr<ICUBreakIterator> m_pWordBreakIterator;

typedef void (*u_getVersion_type)(UVersionInfo versionArray);
typedef UBreakIterator* (*ubrk_open_type)(UBreakIteratorType type, const char* locale, const UChar* text, int32_t textLength, UErrorCode* status);
typedef UBreakIterator* (*ubrk_openRules_type)(const UChar *rules, int32_t rulesLength, const UChar *text, int32_t textLength, UParseError *parseErr, UErrorCode *status);
typedef UBreakIterator* (*ubrk_openBinaryRules_type)(const uint8_t *binaryRules, int32_t rulesLength, const UChar *  text, int32_t textLength, UErrorCode *status);
typedef void (*ubrk_setText_type)(UBreakIterator *bi, const UChar* text, int32_t textLength, UErrorCode* status);
typedef void (*ubrk_close_type)(UBreakIterator* bi);
typedef int32_t (*ubrk_first_type)(UBreakIterator* bi);
typedef int32_t (*ubrk_previous_type)(UBreakIterator* bi);
typedef int32_t (*ubrk_next_type)(UBreakIterator* bi);
typedef int32_t (*ubrk_preceding_type)(UBreakIterator* bi, int32_t offset);
typedef int32_t (*ubrk_following_type)(UBreakIterator* bi, int32_t offset);

ICU_EXTERN u_getVersion_type u_getVersion;
ICU_EXTERN ubrk_open_type ubrk_open;
ICU_EXTERN ubrk_openRules_type ubrk_openRules;
ICU_EXTERN ubrk_openBinaryRules_type ubrk_openBinaryRules;
ICU_EXTERN ubrk_setText_type ubrk_setText;
ICU_EXTERN ubrk_close_type ubrk_close;
ICU_EXTERN ubrk_first_type ubrk_first;
ICU_EXTERN ubrk_previous_type ubrk_previous;
ICU_EXTERN ubrk_next_type ubrk_next;
ICU_EXTERN ubrk_preceding_type ubrk_preceding;
ICU_EXTERN ubrk_following_type ubrk_following;

namespace ICULoader
{
	bool IsLoaded();
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
				UVersionInfo ver;
				u_getVersion(ver);
				if (ver[0] == 64)
					m_iter = ubrk_openBinaryRules(kCustomBinaryRules, kCustomBinaryRules_size, text, textLength, &status);
				else
					m_iter = ubrk_openRules(kCustomRules, static_cast<int32_t>(wcslen(reinterpret_cast<const wchar_t *>(kCustomRules))), text, textLength, &parseError, &status);
			}
			else if (type == UBRK_WORD)
			{
				UParseError parseError;
				UVersionInfo ver;
				u_getVersion(ver);
				if (ver[0] == 64)
					m_iter = ubrk_openBinaryRules(kCustomWordBreakBinaryRules, kCustomWordBreakBinaryRules_size, text, textLength, &status);
				else
					m_iter = ubrk_openRules(kCustomWordBreakRules, static_cast<int32_t>(wcslen(reinterpret_cast<const wchar_t *>(kCustomWordBreakRules))), text, textLength, &parseError, &status);
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

	static ICUBreakIterator *getCharacterBreakIterator(const wchar_t * text, int32_t textLength)
	{
		return getCharacterBreakIterator<1>(reinterpret_cast<const UChar *>(text), textLength);
	}

	template<int N>
	static ICUBreakIterator *getCharacterBreakIterator(const UChar * text, int32_t textLength)
	{
		if (!m_pCharacterBreakIterator<N>)
			m_pCharacterBreakIterator<N>.reset(new ICUBreakIterator(UBRK_CHARACTER, "en", text, textLength));
		else
			m_pCharacterBreakIterator<N>->setText(text, textLength);
		return m_pCharacterBreakIterator<N>.get();
	}

	static ICUBreakIterator *getWordBreakIterator(const UChar * text, int32_t textLength)
	{
		if (!m_pWordBreakIterator)
			m_pWordBreakIterator.reset(new ICUBreakIterator(UBRK_WORD, "en", text, textLength));
		else
			m_pWordBreakIterator->setText(text, textLength);
		return m_pWordBreakIterator.get();
	}

#define GLEN(i) ((m_text[(i)] != '\r') ? \
	(U16_IS_SURROGATE(m_text[(i)]) ? 2 : 1) : \
	(((i) < m_textLength - 1 && m_text[(i) + 1] == '\n') ? 2 : 1))
#define GLENP(i) ((m_text[(i)] != '\n') ? \
	(U16_IS_SURROGATE(m_text[(i)]) ? 2 : 1) : \
	(((i) > 0 && m_text[(i) - 1] == '\r') ? 2 : 1))

private:
	int mynext()
	{
		if (m_type == UBRK_CHARACTER)
		{
			m_i += GLEN(m_i);
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
				m_i = offset - GLENP(offset - 1);
		}
		else if (m_type == UBRK_WORD)
		{
			int nPos = offset;
			int nPrevPos;
			while (nPos > 0 && xisspace(m_text[nPrevPos = nPos - GLENP(nPos - 1)]))
				nPos = nPrevPos;
			if (nPos > 0)
			{
				nPrevPos = nPos - GLENP(nPos - 1);
				nPos = nPrevPos;
				if (xisalnum(m_text[nPos]))
				{
					while (nPos > 0 && xisalnum(m_text[nPrevPos = nPos - GLENP(nPos - 1)]))
						nPos = nPrevPos;
				}
				else
				{
					while (nPos > 0 && !xisalnum(m_text[nPrevPos = nPos - GLENP(nPos - 1)])
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
			m_i = offset + GLEN(offset);
		}
		else if (m_type == UBRK_WORD)
		{
			int nPos = offset;
			if (xisalnum(m_text[nPos]))
			{
				while (nPos < m_textLength && xisalnum(m_text[nPos]))
					nPos += GLEN(nPos);
			}
			else
			{
				while (nPos < m_textLength && !xisalnum(m_text[nPos])
					&& !iswspace(m_text[nPos]))
					nPos += GLEN(nPos);
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
	static const UChar *kCustomWordBreakRules;
	static const uint8_t kCustomBinaryRules[];
	static const uint8_t kCustomWordBreakBinaryRules[];
	static const uint32_t kCustomBinaryRules_size;
	static const uint32_t kCustomWordBreakBinaryRules_size;
};

