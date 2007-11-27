#include "stdafx.h"
#include "UniMarkdownFile.h"
#include "markdown.h"
#include "unicoder.h"

UniMarkdownFile::UniMarkdownFile()
: m_pMarkdown(NULL)
{
}

bool UniMarkdownFile::DoOpen(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess)
{
	m_depth = 0;
	bool bOpen = UniMemFile::DoOpen(filename, dwOpenAccess, dwOpenShareMode, dwOpenCreationDispostion, dwMappingProtect, dwMapViewAccess);
	if (bOpen)
	{
		m_pMarkdown = new CMarkdown((const char *)m_current, (const char *)m_base + m_filesize);
		Move();
	}
	return bOpen;
}

void UniMarkdownFile::Close()
{
	UniMemFile::Close();
	delete m_pMarkdown;
	m_pMarkdown = NULL;
}

bool UniMarkdownFile::ReadBom()
{
	bool bReadBom = UniMemFile::ReadBom();
	if (bReadBom && m_unicoding == ucr::UTF8)
		m_codepage = CP_UTF8;
	return bReadBom;
}

static void CollapseWhitespace(CString &line)
{
	int nEatSpace = -2;
	for (int i = line.GetLength() ; i-- ; )
	{
		switch (line.GetAt(i))
		{
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			if (++nEatSpace < 0 || nEatSpace == 0 && line.GetAt(i + 1) == '<')
				++nEatSpace;
			line.SetAt(i, ' ');
			break;
		case '>':
			if (nEatSpace >= 0)
				++nEatSpace;
		default:
			if (nEatSpace > 0)
				line.Delete(i + 1, nEatSpace);
			nEatSpace = -1;
			break;
		}
	}
	if (++nEatSpace > 0)
		line.Delete(0, nEatSpace);
}

void UniMarkdownFile::Move()
{
	m_bMove = m_pMarkdown->Move();
	const char *first = m_pMarkdown->first;
	const char *ahead = m_pMarkdown->ahead;
	m_transparent = NULL;
	if (first < ahead)
	{
		switch (*++first)
		{
		case '?':
			m_pMarkdown->Scan();
			m_transparent = (LPBYTE)m_pMarkdown->upper;
			break;
		case '!':
			if (first < ahead)
			{
				switch (*++first)
				{
				case '[':
				case '-':
					m_pMarkdown->Scan();
					m_transparent = (LPBYTE)m_pMarkdown->upper;
					break;
				}
			}
			break;
		}
	}
}

BOOL UniMarkdownFile::ReadString(CString &line, CString &eol, bool *lossy)
{
	line.ReleaseBuffer(0);
	eol.ReleaseBuffer(0);
	int nDepth = 0;
	bool bDone = false;
	if (m_current < (LPBYTE)m_pMarkdown->lower)
	{
		line = ucr::maketstring((const char *)m_current, m_pMarkdown->lower - (const char *)m_current, m_codepage, lossy);
		CollapseWhitespace(line);
		bDone = !line.IsEmpty();
		m_current = (LPBYTE)m_pMarkdown->lower;
	}
	while (m_current < m_base + m_filesize && !bDone)
	{
		if (m_current < m_transparent)
		{
			// Leave whitespace alone when inside <? ?>, <!-- -->, or <![*[ ]]>.
			LPBYTE current = m_current;
			if (m_current == (LPBYTE)m_pMarkdown->first)
			{
				nDepth = m_depth;
			}
			while (m_current < m_transparent && *m_current != '\r' && *m_current != '\n')
			{
				++m_current;
			}
			line = ucr::maketstring((const char *)current, m_current - current, m_codepage, lossy);
			if (m_current < m_transparent)
			{
				current = m_current;
				BYTE eol = *m_current++;
				if (m_current < m_transparent && *m_current == (eol ^ ('\r'^'\n')))
				{
					++m_current;
					++m_txtstats.ncrlfs;
				}
				else
				{
					++(eol == '\r' ? m_txtstats.ncrs : m_txtstats.nlfs);
				}
			}
			bDone = true;
		}
		else
		{
			while (m_current < m_base + m_filesize && isspace(*m_current))
			{
				LPBYTE current = m_current;
				BYTE eol = *m_current++;
				if (eol == '\r' || eol == '\n')
				{
					if (m_current < m_base + m_filesize && *m_current == (eol ^ ('\r'^'\n')))
					{
						++m_current;
						++m_txtstats.ncrlfs;
					}
					else
					{
						++(eol == '\r' ? m_txtstats.ncrs : m_txtstats.nlfs);
					}
				}
			}
			nDepth = m_depth;
			bool bPull = false;
			if (m_bMove && m_pMarkdown->Pull())
			{
				++m_depth;
				bPull = true;
			}
			Move();
			bDone = m_bMove;
			if (!bDone)
			{
				--m_depth;
				bDone = m_pMarkdown->Push();
				if (bPull && bDone)
					Move();
			}
			if (bDone)
			{
				line = ucr::maketstring((const char *)m_current, m_pMarkdown->first - (const char *)m_current, m_codepage, lossy);
				CollapseWhitespace(line);
				m_current = (LPBYTE)m_pMarkdown->first;
			}
			else if (m_current < m_base + m_filesize)
			{
				bDone = true;
				line = ucr::maketstring((const char *)m_current, m_base + m_filesize - m_current, m_codepage, lossy);
				CollapseWhitespace(line);
				m_current = m_base + m_filesize;
			}
			bDone = !line.IsEmpty();
		}
	}
	ASSERT(line.FindOneOf(_T("\r\n")) == -1);
	if (nDepth > 0)
		line.Insert(0, CString('\t', nDepth));
	if (bDone)
		eol = _T("\n");
	return bDone;
}
