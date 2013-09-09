/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  HexDump.cpp
 *
 * @brief Hex dump class implementation.
 *
 */
#include "precomp.h"
#include "HexDump.h"
#include "simparr.h"

HexDump::HexDump()
: m_pData(NULL)
, m_pBuffer(NULL)
, m_bytesPerLine(0)
, m_charsPerLine(0)
, m_offsetMaxLen(0)
, m_offsetMinLen(0)
, m_partialOffset(0)
, m_partialStats(false)
, m_byteSpace(0)
, m_charSpace(0)
{
}

HexDump::~HexDump()
{
	delete [] m_pBuffer;
}

void HexDump::Settings(int bytesPerLine, int charsPerLine,
		bool partialStats, INT64 partialOffset,
		int byteSpace, int charSpace, int charset)
{
	m_bytesPerLine = bytesPerLine;
	m_charsPerLine = charsPerLine;
	m_partialStats = partialStats;
	m_partialOffset = partialOffset;
	m_byteSpace = byteSpace;
	m_charSpace = charSpace;
	m_charset = charset;
}

void HexDump::SetOffsets(int minLen, int maxLen)
{
	m_offsetMinLen = minLen;
	m_offsetMaxLen = maxLen;
}

void HexDump::SetArray(const SimpleArray<BYTE> *dataArray)
{
	m_pData = const_cast<SimpleArray<BYTE>*>(dataArray);
}

void HexDump::CreateBuffer(unsigned size)
{
	m_pBuffer = new char[size];
	for (unsigned i = 0; i < size; i++)
		m_pBuffer[i] = ' ';
}

char *HexDump::GetBuffer() const
{
	return m_pBuffer;
}

void HexDump::Write(int startInd, int endInd)
{
	char buf2[128];

	// Write hexdump.
	// a = first byte of first line of hexdump.
	// b = first byte of last line of hexdump.
	//b = iCopyHexdumpDlgEnd;
	// a = Offset of current line.
	// k = Offset in text array.
	for (int k = 0, a = startInd; a <= endInd;
			a += m_bytesPerLine, k += m_charsPerLine + 2)
	{
		// Write offset.
		int m = sprintf(buf2, "%*.*x", m_offsetMinLen, m_offsetMinLen,
				m_partialStats ? a + m_partialOffset : a);

		for (int i = m; i < m_offsetMaxLen + m_byteSpace - m; i++)
			buf2[i] = ' ';
		buf2[m_offsetMaxLen + m_byteSpace] = '\0';

		int l = 0; // l = Offset in line, relative to k.
		int n = 0;
		while (buf2[n] != '\0')
			m_pBuffer[k + (l++)] = buf2[n++]; // Copy Offset. l = next empty place after spaces.
		// Write bytes and chars.
		for (int j = 0 ; j < m_bytesPerLine ; j++)
		{
			if (a + j > m_pData->GetUpperBound())
			{
				// Nonexistant byte.
				m_pBuffer[k + l + j * 3    ] = ' ';
				m_pBuffer[k + l + j * 3 + 1] = ' ';
				m_pBuffer[k + l + j * 3 + 2] = ' ';
				// Nonexistant char.
				m_pBuffer[k + l + m_bytesPerLine * 3 + m_charSpace + j] = ' ';
			}
			else
			{
				// Write byte.
				sprintf(buf2, "%2.2x ", (*m_pData)[(int)a + j]);
				m_pBuffer[k + l + j*3    ] = buf2[0];
				m_pBuffer[k + l + j*3 + 1] = buf2[1];
				m_pBuffer[k + l + j*3 + 2] = buf2[2];
				// Write char.
				if (m_charset == OEM_FIXED_FONT && (*m_pData)[(int)a + j] != 0)
					m_pBuffer[k + l + m_bytesPerLine*3 + m_charSpace + j] = (*m_pData)[(int)a + j];
				else if (((*m_pData)[(int)a + j] >= 32 && (*m_pData)[(int)a + j] <= 126) ||
						((*m_pData)[(int)a + j]>=160 && (*m_pData)[(int)a + j] <= 255) ||
						((*m_pData)[(int)a + j] >= 145 && (*m_pData)[(int)a + j] <= 146))
				{
					m_pBuffer[k + l + m_bytesPerLine*3 + m_charSpace + j] = (*m_pData)[(int)a + j];
				}
				else
					m_pBuffer[k + l + m_bytesPerLine*3 + m_charSpace + j] = '.';
			}
		}
		m_pBuffer[k + m_charsPerLine] = '\r';
		m_pBuffer[k + m_charsPerLine + 1] = '\n';
	}
}
