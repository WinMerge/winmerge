/**
 *  @file UniMarkdownFile.cpp
 *
 *  @brief Implementation of UniMarkdownFile class.
 */ 

#include "pch.h"
#include "UniMarkdownFile.h"
#include <cassert>
#include "markdown.h"

static void CollapseWhitespace(String &line);

/**
 * @brief Constructor.
 */
UniMarkdownFile::UniMarkdownFile()
: m_pMarkdown(nullptr)
, m_depth(0)
, m_bMove(false)
, m_transparent(nullptr)
{
}

/**
 * @brief Open the XML file.
 * @param [in] filename Filename (and path) of the file to open.
 * @param [in] mode access mode.
 * @return true if succeeds, false otherwise.
 */
bool UniMarkdownFile::DoOpen(const String& filename, AccessMode mode)
{
	m_depth = 0;
	bool bOpen = UniMemFile::DoOpen(filename, mode);
	if (bOpen)
	{
		// CMarkdown wants octets, so we may need to transcode to UTF8.
		// As transcoding strips the BOM, we must check for it in advance.
		if (IsUnicode())
			m_codepage = ucr::CP_UTF_8;
		// The CMarkdown::File constructor cares about transcoding.
		CMarkdown::File f(
			reinterpret_cast<const TCHAR *>(m_base),
			static_cast<unsigned>(m_filesize),
			CMarkdown::File::Mapping | CMarkdown::File::Octets);
		// The file mapping may have been recreated due to transcoding.
		m_data = m_current = m_base = reinterpret_cast<unsigned char *>(f.pImage);
		m_filesize = f.cbImage;
		// Prevent the CMarkdown::File destructor from unmapping the view.
		f.pImage = nullptr;
		m_pMarkdown.reset(new CMarkdown(f));
		Move();
	}
	return bOpen;
}

/**
 * @brief Close the file.
 */
void UniMarkdownFile::Close()
{
	UniMemFile::Close();
}

/**
 * @brief Collapse whitespace characters from the given line.
 * @param [in, out] Line to handle.
 */
static void CollapseWhitespace(String &line)
{
	int nEatSpace = -2;
	for (size_t i = line.length() ; i-- ; )
	{
		switch (line[i])
		{
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			if (++nEatSpace < 0 || nEatSpace == 0 && line[i + 1] == '<')
				++nEatSpace;
			line[i] = ' ';
			break;
		case '>':
			if (nEatSpace >= 0 && line[i + 1 + nEatSpace] != '<')
				++nEatSpace;
		default:
			if (nEatSpace > 0)
				line.erase(i + 1, nEatSpace);
			nEatSpace = -1;
			break;
		}
	}
	if (++nEatSpace > 0)
		line.erase(0, nEatSpace);
}

void UniMarkdownFile::Move()
{
	m_bMove = m_pMarkdown->Move();
	const char *first = m_pMarkdown->first;
	const char *ahead = m_pMarkdown->ahead;
	m_transparent = nullptr;
	if (first < ahead)
	{
		switch (*++first)
		{
		case '?':
			m_pMarkdown->Scan();
			m_transparent = (unsigned char *)m_pMarkdown->upper;
			break;
		case '!':
			if (first < ahead)
			{
				switch (*++first)
				{
				case '[':
				case '-':
					m_pMarkdown->Scan();
					m_transparent = (unsigned char *)m_pMarkdown->upper;
					break;
				}
			}
			break;
		}
	}
}

String UniMarkdownFile::maketstring(const char *lpd, size_t len)
{
	bool lossy = false;
	String s;
	ucr::maketstring(s, lpd, len, m_codepage, &lossy);
	if (lossy)
		++m_txtstats.nlosses;
	return s;
}

bool UniMarkdownFile::ReadString(String &line, String &eol, bool *lossy)
{
	line.erase();
	eol.erase();
	int nlosses = m_txtstats.nlosses;
	int nDepth = 0;
	bool bDone = false;
	if (m_current < (const unsigned char *)m_pMarkdown->lower)
	{
		line = maketstring((const char *)m_current, m_pMarkdown->lower - (const char *)m_current);
		CollapseWhitespace(line);
		bDone = !line.empty();
		m_current = (unsigned char *)m_pMarkdown->lower;
	}
	while (m_current < m_base + m_filesize && !bDone)
	{
		if (m_current < m_transparent)
		{
			// Leave whitespace alone when inside <? ?>, <!-- -->, or <![*[ ]]>.
			unsigned char * current = m_current;
			if (m_current == (const unsigned char *)m_pMarkdown->first)
			{
				nDepth = m_depth;
			}
			while (m_current < m_transparent && *m_current != '\r' && *m_current != '\n')
			{
				++m_current;
			}
			line = maketstring((const char *)current, m_current - current);
			if (m_current < m_transparent)
			{
				unsigned char eol1 = *m_current++;
				if (m_current < m_transparent && *m_current == (eol1 ^ ('\r'^'\n')))
				{
					++m_current;
					++m_txtstats.ncrlfs;
				}
				else
				{
					++(eol1 == '\r' ? m_txtstats.ncrs : m_txtstats.nlfs);
				}
			}
			bDone = true;
		}
		else
		{
			while (m_current < m_base + m_filesize && isspace(*m_current))
			{
				unsigned char eol1 = *m_current++;
				if (eol1 == '\r' || eol1 == '\n')
				{
					if (m_current < m_base + m_filesize && *m_current == (eol1 ^ ('\r'^'\n')))
					{
						++m_current;
						++m_txtstats.ncrlfs;
					}
					else
					{
						++(eol1 == '\r' ? m_txtstats.ncrs : m_txtstats.nlfs);
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
				line = maketstring((const char *)m_current, m_pMarkdown->first - (const char *)m_current);
				CollapseWhitespace(line);
				m_current = (unsigned char *)m_pMarkdown->first;
			}
			else if (m_current < m_base + m_filesize)
			{
				bDone = true;
				line = maketstring((const char *)m_current, m_base + m_filesize - m_current);
				CollapseWhitespace(line);
				m_current = m_base + m_filesize;
			}
			bDone = !line.empty();
		}
	}
	assert(line.find_first_of(_T("\r\n")) == String::npos);
	if (nDepth > 0)
		line.insert(0U, nDepth, _T('\t'));
	if (bDone)
		eol = _T("\n");
	if (lossy)
		*lossy = nlosses != m_txtstats.nlosses;
	return bDone;
}
