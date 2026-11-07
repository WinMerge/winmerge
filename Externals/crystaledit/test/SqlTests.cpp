#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
TEST_CLASS(SqlTests)
{
  static int ColorAt(const std::vector<CrystalLineParser::TEXTBLOCK>& blocks,
					 int position)
  {
	int color = COLORINDEX_NORMALTEXT;
	for (const auto& block : blocks)
	  {
		if (block.m_nCharPos > position)
		  break;
		color = block.m_nColorIndex;
	  }
	return color;
  }

  static unsigned ParseDefault(unsigned cookie, const tchar_t* line,
							   std::vector<CrystalLineParser::TEXTBLOCK>& blocks)
  {
	return CrystalLineParser::ParseLineSql(cookie, line,
											static_cast<int>(tc::tcslen(line)),
											&blocks);
  }

  static unsigned ParsePostgreSql(unsigned cookie, const tchar_t* line,
								  std::vector<CrystalLineParser::TEXTBLOCK>& blocks)
  {
	return CrystalLineParser::ParseLineSqlPostgreSql(cookie, line,
													  static_cast<int>(tc::tcslen(line)),
													  &blocks);
  }

  static unsigned ParseMySql(unsigned cookie, const tchar_t* line,
							 std::vector<CrystalLineParser::TEXTBLOCK>& blocks)
  {
	return CrystalLineParser::ParseLineSqlMySql(cookie, line,
												 static_cast<int>(tc::tcslen(line)),
												 &blocks);
  }

public:
  TEST_METHOD(TSqlBracketIdentifierContinuesAcrossLines)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	unsigned cookie = ParseDefault(0, _T("SELECT [name"), blocks);
	Assert::AreEqual(static_cast<unsigned>(COOKIE_SQL_QUOTED_IDENTIFIER | COOKIE_SQL_BRACKET_IDENTIFIER), cookie);

	blocks.clear();
	cookie = ParseDefault(cookie, _T("#name] SELECT 1"), blocks);
	Assert::AreEqual(0u, cookie);
  }

  TEST_METHOD(PostgreSqlEscapeStringContinuesAcrossLines)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	unsigned cookie = ParsePostgreSql(0, LR"(SELECT E'it\'s still)", blocks);
	Assert::IsTrue((cookie & COOKIE_CHAR) != 0);
	Assert::IsTrue((cookie & COOKIE_SQL_POSTGRES_ESCAPE_STRING) != 0);

	blocks.clear();
	cookie = ParsePostgreSql(cookie, LR"(open'; SELECT 1)", blocks);
	Assert::AreEqual(0u, cookie);
  }

  TEST_METHOD(MySqlBacktickIdentifierIgnoresHashOnContinuationLine)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	unsigned cookie = ParseMySql(0, _T("SELECT `name"), blocks);
	Assert::AreEqual(static_cast<unsigned>(COOKIE_SQL_QUOTED_IDENTIFIER), cookie);

	blocks.clear();
	cookie = ParseMySql(cookie, _T("#name` SELECT 1"), blocks);
	Assert::AreEqual(0u, cookie);
  }

  TEST_METHOD(MySqlHashCommentEndsAtNewline)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	const unsigned cookie = ParseMySql(0, LR"(# note\)", blocks);
	Assert::AreEqual(0u, cookie);
  }

  TEST_METHOD(ExecutableCommentIsMySqlSpecific)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	unsigned cookie = ParseMySql(0, LR"(/*! SELECT 1)", blocks);
	Assert::AreEqual(static_cast<unsigned>(COOKIE_EXT_EXECUTABLE_COMMENT), cookie);

	blocks.clear();
	cookie = ParsePostgreSql(0, LR"(/*! SELECT 1)", blocks);
	Assert::AreEqual(static_cast<unsigned>(COOKIE_EXT_COMMENT), cookie);
  }

  TEST_METHOD(PostgreSqlArrayBracketIsNotQuotedIdentifier)
  {
	std::vector<CrystalLineParser::TEXTBLOCK> blocks;
	const unsigned cookie = ParsePostgreSql(0, _T("SELECT ARRAY [1, /* comment */ 2]"), blocks);
	Assert::AreEqual(0u, cookie);
	Assert::AreEqual(static_cast<int>(COLORINDEX_COMMENT), ColorAt(blocks, 17));
  }
};
} // namespace test
