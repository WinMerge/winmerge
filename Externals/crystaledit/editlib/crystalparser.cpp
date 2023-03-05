////////////////////////////////////////////////////////////////////////////
//	File:		CrystalParser.cpp
//
//	Author:		Sven Wiegand
//	E-mail:		sven.wiegand@gmx.de
//
//	Implementation of the CCrystalParser class, a part of Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use or modify this code to the following restrictions:
//	- Acknowledge me somewhere in your about box, simple "Parts of code by.."
//	will be enough. If you can't (or don't want to), contact me personally.
//	- LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "crystalparser.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "utils/icu.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CCrystalParser, CObject )

CCrystalParser::CCrystalParser()
// : m_iterChar(UBRK_CHARACTER, "en", nullptr, 0)
{
	m_pTextView = nullptr;
}


CCrystalParser::~CCrystalParser()
{
}


unsigned CCrystalParser::ParseLine(unsigned /*dwCookie*/, 
								int /*nLineIndex*/, 
								CCrystalTextBlock * /*pBlock*/ )
{
	return 0;
}

void CCrystalParser::WrapLine( int nLineIndex, int nMaxLineWidth, std::vector<int>* anBreaks, int& nBreaks )
{
	// The parser must be attached to a view!
	ASSERT( m_pTextView != nullptr );

	int			nLineLength = m_pTextView->GetLineLength( nLineIndex );
	int			nTabWidth = m_pTextView->GetTabSize();
	int			nLineCharCount = 0;
	int			nCharCount = 0;
	const tchar_t*	szLine = m_pTextView->GetLineChars( nLineIndex );
	int			nLastBreakPos = 0;
	int			nLastCharBreakPos = 0;
	bool		bBreakable = false;
	WORD		wCharType;

	if (anBreaks)
		anBreaks->resize(0);

	if (m_pTextView->m_pTextBuffer && m_pTextView->m_pTextBuffer->GetTableEditing ())
	{
		nMaxLineWidth = m_pTextView->m_pTextBuffer->GetColumnWidth (0);
		int nMaxBreaks = 0;
		int nColumn = 0;
		bool bInQuote = false;
		const int sep = m_pTextView->m_pTextBuffer->GetFieldDelimiter ();
		const int quote = m_pTextView->m_pTextBuffer->GetFieldEnclosure ();
		for (int i = 0; i < nLineLength; )
		{
			tchar_t ch = szLine[i];
			int previ = i;
			int nCharCountPrev = nCharCount;

			// remember position of whitespace for wrap
			if (bBreakable)
			{
				nLastBreakPos = i;
				nLastCharBreakPos = nCharCount;
				bBreakable = false;
			}

			if (ch == quote)
			{
				bInQuote = !bInQuote;
				nLineCharCount++;
				nCharCount++;
			}
			// increment char counter (evtl. expand tab)
			else if (ch == _T('\t'))
			{
				nLineCharCount ++;
				nCharCount ++;
				if (sep != ch || bInQuote)
					// remember whitespace
					bBreakable = true;
			}
			else if (ch >= _T('\x00') && ch <= _T('\x1F'))
			{
				nLineCharCount+= 3;
				nCharCount+= 3;
				if (sep != ch || bInQuote)
					bBreakable = true;
			}
			else
			{
#ifndef _UNICODE
				if( IsDBCSLeadByte((BYTE)ch) )
				{
					nLineCharCount += 2;
					nCharCount += 2;
					GetStringTypeA(LOCALE_USER_DEFAULT,CT_CTYPE3, &szLine[i], 2, &wCharType);
					// remember whitespace
					if( (wCharType & (C3_IDEOGRAPH | C3_HIRAGANA | C3_KATAKANA)))
						bBreakable = true;
				}
				else
				{
					nLineCharCount ++;
					nCharCount ++;
					// remember whitespace
					if( ch == _T(' ') )
						bBreakable = true;
				}
#else
				if (ch & 0xff80)
				{
					int n = m_pTextView->GetCharCellCountFromChar(szLine + i);
					nLineCharCount += n;
					nCharCount += n;
					GetStringTypeW(CT_CTYPE3, &ch, 1, &wCharType);
					// remember whitespace
					if( wCharType & (C3_IDEOGRAPH | C3_HIRAGANA | C3_KATAKANA) )
						bBreakable = true;
				}
				else
				{
					nLineCharCount ++;
					nCharCount ++;
					// remember whitespace
					if( ch == _T(' ') )
						bBreakable = true;
				}
#endif
			}

#ifndef _UNICODE
			i++;
			if (IsDBCSLeadByte((BYTE)ch))
				i++;
#else
			i += U16_IS_SURROGATE(szLine[i]) ? 2 : 1;
#endif
			if (ch == '\r' && i < nLineLength && szLine[i] == '\n')
				i++;

			int nLastBreakPosSaved = -1;

			if (!bInQuote && ch == sep)
			{
				nLastCharBreakPos = nCharCount;
				
				if (anBreaks)
					anBreaks->push_back(-i);
				nLastBreakPosSaved = -i;

				if (nBreaks > nMaxBreaks)
					nMaxBreaks = nBreaks;
				nLineCharCount = 0;
				nLastBreakPos = 0;
				nBreaks = 0;
				nMaxLineWidth = m_pTextView->m_pTextBuffer->GetColumnWidth(++nColumn);
			}
			// wrap line
			else if( nLineCharCount >= nMaxLineWidth )
			{
				// if no wrap position found, but line is to wide, 
				// wrap at current position
				if( nLastBreakPos == 0 || bBreakable)
				{
					if (nLineCharCount <= nMaxLineWidth || nCharCount - nCharCountPrev > nMaxLineWidth)
					{
						nLastBreakPos = i;
						nLastCharBreakPos = nCharCount;
					}
					else
					{
						nLastBreakPos = previ;
						nLastCharBreakPos = nCharCountPrev;
					}
				}

				if (nLastBreakPos < nLineLength)
				{
					nLineCharCount = nCharCount - nLastCharBreakPos;
					if (anBreaks)
						anBreaks->push_back(nLastBreakPos);
					nLastBreakPosSaved = nLastBreakPos;
					nBreaks++;

					if (nBreaks > nMaxBreaks)
						nMaxBreaks = nBreaks;
					nLastBreakPos = 0;
					bBreakable = false;
				}
			}

			if(ch == '\r' || ch == '\n')
			{
				if (nLastBreakPosSaved != i)
				{
					nLastCharBreakPos = nCharCount;
					nLineCharCount = 0;
					if (anBreaks)
						anBreaks->push_back(i);
					nBreaks++;

					if (nBreaks > nMaxBreaks)
						nMaxBreaks = nBreaks;
					nLastBreakPos = 0;
				}
				bBreakable = false;
			}
		}
		nBreaks = nMaxBreaks;
	}
	else
	{
		//    m_iterChar.setText(reinterpret_cast<const UChar *>(szLine), nLineLength);
		//    for( int i = 0; i < nLineLength; i = m_iterChar.next())
		for( int i = 0; i < nLineLength; i += U16_IS_SURROGATE(szLine[i]) ? 2 : 1)
		{
			tchar_t ch = szLine[i];
			// remember position of whitespace for wrap
			if( bBreakable )
			{
				nLastBreakPos = i;
				nLastCharBreakPos = nCharCount;
				bBreakable = false;
			}

			// increment char counter (evtl. expand tab)
			if( ch == _T('\t') )
			{
				nLineCharCount+= (nTabWidth - nCharCount % nTabWidth);
				nCharCount+= (nTabWidth - nCharCount % nTabWidth);
				// remember whitespace
				bBreakable = true;
			}
			else if (ch >= _T('\x00') && ch <= _T('\x1F'))
			{
				nLineCharCount+= 3;
				nCharCount+= 3;
				bBreakable = true;
			}
			else
			{
#ifndef _UNICODE
				if( IsDBCSLeadByte((BYTE)ch) )
				{
					nLineCharCount += 2;
					nCharCount += 2;
					GetStringTypeA(LOCALE_USER_DEFAULT,CT_CTYPE3, &szLine[i], 2, &wCharType);
					// remember whitespace
					if( (wCharType & (C3_IDEOGRAPH | C3_HIRAGANA | C3_KATAKANA)))
						bBreakable = true;
				}
				else
				{
					nLineCharCount ++;
					nCharCount ++;
					// remember whitespace
					if( ch == _T(' ') )
						bBreakable = true;
				}
#else
				if (ch & 0xff80)
				{
					int n = m_pTextView->GetCharCellCountFromChar(szLine + i);
					nLineCharCount += n;
					nCharCount += n;
					GetStringTypeW(CT_CTYPE3, &ch, 1, &wCharType);
					// remember whitespace
					if( wCharType & (C3_IDEOGRAPH | C3_HIRAGANA | C3_KATAKANA) )
						bBreakable = true;
				}
				else
				{
					nLineCharCount ++;
					nCharCount ++;
					// remember whitespace
					if( ch == _T(' ') )
						bBreakable = true;
				}
#endif
			}

			// wrap line
			if( nLineCharCount >= nMaxLineWidth )
			{
				// if no wrap position found, but line is to wide, 
				// wrap at current position
				if( nLastBreakPos == 0 )
				{
					nLastBreakPos = i;
					nLastCharBreakPos = nCharCount;
				}
				if( anBreaks )
					anBreaks->push_back(nLastBreakPos);
				++nBreaks;

				nLineCharCount = nCharCount - nLastCharBreakPos;
				nLastBreakPos = 0;
			}

#ifndef _UNICODE
			if (IsDBCSLeadByte((BYTE)ch))
				i++;
#endif
		}
	}
}
