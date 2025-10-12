#include "pch.h"
#include "FileContentRef.h"
#include "UnicodeString.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "UniFile.h"
#include "codepage_detect.h"
#include "paths.h"
#include "MergeApp.h"
#include <algorithm>
#include <functional>
#include <Poco/RegularExpression.h>
#include <Poco/FileStream.h>
#include <Poco/Exception.h>

static void GuessEncoding(UniMemFile& file, const String& path)
{
	file.ReadBom();
	if (!file.HasBom())
	{
		int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
		int64_t fileSize = file.GetFileSize();
		FileTextEncoding encoding = codepage_detect::Guess(
			paths::FindExtension(path), file.GetBase(), static_cast<size_t>(
				fileSize < static_cast<int64_t>(codepage_detect::BufSize) ?
				fileSize : static_cast<int64_t>(codepage_detect::BufSize)),
			iGuessEncodingType);
		file.SetCodepage(encoding.m_codepage);
	}
}

bool FileContentRef::operator==(const FileContentRef& other) const
{
	try {
		Poco::FileInputStream fs1(ucr::toUTF8(path), std::ios::binary);
		Poco::FileInputStream fs2(ucr::toUTF8(other.path), std::ios::binary);

		if (!fs1.good() || !fs2.good()) return false;

		const size_t bufferSize = 4096;
		char buffer1[bufferSize];
		char buffer2[bufferSize];

		while (true) {
			fs1.read(buffer1, bufferSize);
			fs2.read(buffer2, bufferSize);

			std::streamsize count1 = fs1.gcount();
			std::streamsize count2 = fs2.gcount();

			if (count1 != count2)
				return false;
			if (count1 == 0)
				return true; // end of both

			if (std::memcmp(buffer1, buffer2, static_cast<size_t>(count1)) != 0)
				return false;
		}
	}
	catch (const Poco::Exception&)
	{
		return false;
	}
}

bool FileContentRef::Contains(const std::string& str) const
{
	UniMemFile file;
	if (!file.OpenReadOnly(path))
		return false;
	GuessEncoding(file, path);
	String searchStr = ucr::toTString(str);
	strutils::makelower(searchStr);
	std::boyer_moore_horspool_searcher<String::const_iterator> searcher(searchStr.begin(), searchStr.end());
	bool linesToRead = true;
	bool found = false;
	do
	{
		bool lossy;
		String line, eol;
		linesToRead = file.ReadString(line, eol, &lossy);
		strutils::makelower(line);
		using iterator = String::const_iterator;
		std::pair<iterator, iterator> result = searcher(line.begin(), line.end());
		if (result.first != result.second)
		{
			found = true;
			break;
		}
	} while (linesToRead);
	file.Close();
	return found;
}

bool FileContentRef::REContains(const Poco::RegularExpression& regexp) const
{
	UniMemFile file;
	if (!file.OpenReadOnly(path))
		return false;
	GuessEncoding(file, path);
	bool linesToRead = true;
	bool found = false;
	try
	{
		do
		{
			bool lossy;
			String line, eol;
			linesToRead = file.ReadString(line, eol, &lossy);
			Poco::RegularExpression::Match match;
			if (regexp.match(ucr::toUTF8(line), match) > 0)
			{
				found = true;
				break;
			}
		} while (linesToRead);
	}
	catch (const Poco::RegularExpressionException&)
	{
	}
	file.Close();
	return found;
}

std::string FileContentRef::Sublines(ptrdiff_t start, ptrdiff_t len) const
{
	UniMemFile file;
	if (!file.OpenReadOnly(path))
		return "";
	GuessEncoding(file, path);
	bool linesToRead = true;
	std::vector<String> lines;
	if (start >= 0 && len >= 0)
	{
		size_t count = 0;
		do
		{
			bool lossy;
			String line, eol;
			linesToRead = file.ReadString(line, eol, &lossy);
			if (count >= start && count < start + len && (!line.empty() || !eol.empty()))
				lines.push_back(line + eol);
			if (lines.size() >= static_cast<size_t>(len))
				break;
			++count;
		} while (linesToRead);
		file.Close();
		return ucr::toUTF8(strutils::join(lines.begin(), lines.end(), _T("")));
	}
	do
	{
		bool lossy;
		String line, eol;
		linesToRead = file.ReadString(line, eol, &lossy);
		if (!line.empty() || !eol.empty())
			lines.push_back(line + eol);
	} while (linesToRead);
	if (start < 0)
	{
		start = static_cast<ptrdiff_t>(lines.size()) + start;
		if (start < 0)
			start = 0;
	}
	if (start >= static_cast<ptrdiff_t>(lines.size()))
		return "";
	if (len < 0)
		len = static_cast<ptrdiff_t>(lines.size()) - start + len + 1;
	if (len < 0)
		return "";
	file.Close();
	return ucr::toUTF8(strutils::join(lines.begin() + start, lines.begin() + start + len, _T("")));
}

size_t FileContentRef::LineCount() const
{
	UniMemFile file;
	if (!file.OpenReadOnly(path))
		return static_cast<size_t>(-1);
	GuessEncoding(file, path);
	bool linesToRead = true;
	size_t count = 0;
	do
	{
		bool lossy;
		String line, eol;
		linesToRead = file.ReadString(line, eol, &lossy);
		if (!line.empty() || !eol.empty())
			++count;
	} while (linesToRead);
	file.Close();
	return count;
}

