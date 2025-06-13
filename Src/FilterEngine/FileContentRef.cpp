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
#include <Poco/Exception.h>
using NTSTATUS = LONG;
#include "HashCalc.h"

static std::vector<uint8_t> CalculateHashValue(const String& path, const tchar_t* algoname)
{
	std::vector<uint8_t> hash;
	HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CalculateHashValue(hFile, algoname, hash);
		CloseHandle(hFile);
	}
	return hash;
}

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
	std::vector<uint8_t> hash1 = CalculateHashValue(path, _T("SHA256"));
	if (hash1.empty())
		return false;
	std::vector<uint8_t> hash2 = CalculateHashValue(other.path, _T("SHA256"));
	if (hash2.empty())
		return false;
	return hash1 == hash2;
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

