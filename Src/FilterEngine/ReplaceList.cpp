/**
 * @file ReplaceList.cpp
 * @brief Implementation of ReplaceList functions
 */
#include "pch.h"
#include "ReplaceList.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "UniFile.h"
#include "paths.h"
#include "MergeApp.h"
#include "codepage_detect.h"
#include "Environment.h"
#include "FilterExpression.h"
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>

namespace ReplaceList
{

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

template<typename ProcessLineFn>
static std::shared_ptr<std::vector<ValueType2>> LoadListImpl(
	const FilterExpression& ctxt,
	const String& path,
	const char* fileTypeDesc,
	ProcessLineFn processLine)
{
	auto list = std::make_shared<std::vector<ValueType2>>();
	UniMemFile file;
	if (!file.OpenReadOnly(env::ExpandEnvironmentVariables(path)))
	{
		if (ctxt.logger)
		{
			ctxt.logger(0, std::string("Failed to open ") + fileTypeDesc + " file: " +
				ucr::toUTF8(path) + ", error: " + ucr::toUTF8(file.GetLastUniError().GetError()));
		}
		return list;
	}

	GuessEncoding(file, path);

	int lineNumber = 0;
	bool linesToRead = true;
	do
	{
		bool lossy;
		String line, eol;
		linesToRead = file.ReadString(line, eol, &lossy);
		lineNumber++;

		if (line.empty())
			continue;

		if (line[0] == _T('#'))
			continue;

		size_t tabPos = line.find(_T('\t'));
		if (tabPos == String::npos)
			continue;

		String from = line.substr(0, tabPos);
		String to = line.substr(tabPos + 1);

		size_t secondTabPos = to.find(_T('\t'));
		if (secondTabPos != String::npos)
			to = to.substr(0, secondTabPos);

		if (auto pair = processLine(ctxt, lineNumber, from, to, path))
			list->push_back(ValueType2{ pair });

	} while (linesToRead);

	file.Close();

	return list;
}

std::shared_ptr<std::vector<ValueType2>> LoadList(const FilterExpression& ctxt, const String& path)
{
	return LoadListImpl(ctxt, path, "replace list",
		[](const FilterExpression& ctxt, int lineNumber, const String& from, const String& to, const String& path)
		-> std::shared_ptr<std::vector<ValueType2>>
		{
			std::string fromUtf8 = ucr::toUTF8(from);
			std::string toUtf8 = ucr::toUTF8(to);

			try
			{
				// Escape and compile regex for case-insensitive replacement
				std::string pattern;
				pattern.reserve(fromUtf8.size());
				for (char c : fromUtf8)
				{
					if (c == '.' || c == '^' || c == '$' || c == '*' || c == '+' || c == '?' ||
						c == '{' || c == '}' || c == '[' || c == ']' || c == '\\' || c == '|' ||
						c == '(' || c == ')')
					{
						pattern += '\\';
					}
					pattern += c;
				}

				auto regex = std::make_shared<Poco::RegularExpression>(
					pattern,
					Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8
				);

				auto pair = std::make_shared<std::vector<ValueType2>>();
				pair->push_back(ValueType2{ regex });
				pair->push_back(ValueType2{ toUtf8 });

				return pair;
			}
			catch (const Poco::RegularExpressionException& e)
			{
				if (ctxt.logger)
				{
					ctxt.logger(0, "Invalid pattern at line " + std::to_string(lineNumber) +
						" in file: " + ucr::toUTF8(path) + ", pattern: " + fromUtf8 +
						", error: " + e.message());
				}
				return nullptr;
			}
		});
}

std::shared_ptr<std::vector<ValueType2>> LoadRegexList(const FilterExpression& ctxt, const String& path)
{
	return LoadListImpl(ctxt, path, "regex replace list",
		[](const FilterExpression& ctxt, int lineNumber, const String& pattern, const String& replacement, const String& path)
		-> std::shared_ptr<std::vector<ValueType2>>
		{
			std::string patternUtf8 = ucr::toUTF8(pattern);
			std::string replacementUtf8 = ucr::toUTF8(replacement);

			try
			{
				auto regex = std::make_shared<Poco::RegularExpression>(
					patternUtf8,
					Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8
				);

				auto pair = std::make_shared<std::vector<ValueType2>>();
				pair->push_back(ValueType2{ regex });
				pair->push_back(ValueType2{ replacementUtf8 });

				return pair;
			}
			catch (const Poco::RegularExpressionException& e)
			{
				if (ctxt.logger)
				{
					ctxt.logger(0, "Invalid regular expression at line " + std::to_string(lineNumber) +
						" in file: " + ucr::toUTF8(path) + ", pattern: " + patternUtf8 +
						", error: " + e.message());
				}
				return nullptr;
			}
		});
}

}