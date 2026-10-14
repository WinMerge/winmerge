#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{

TEST_CLASS(RtfTests)
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

  static unsigned Parse(unsigned cookie, const tchar_t* line,
                        std::vector<CrystalLineParser::TEXTBLOCK>& blocks)
  {
    return CrystalLineParser::ParseLineRtf(cookie, line,
                                          static_cast<int>(tc::tcslen(line)),
                                          &blocks);
  }

public:
  TEST_METHOD(ControlWordsGroupsAndText)
  {
    const tchar_t* line = LR"({\rtf1\ansi Hello \b-1 bold})";
    std::vector<CrystalLineParser::TEXTBLOCK> blocks;
    Assert::AreEqual(0u, Parse(0, line, blocks));
    Assert::AreEqual(static_cast<int>(COLORINDEX_OPERATOR), ColorAt(blocks, 0));
    Assert::AreEqual(static_cast<int>(COLORINDEX_KEYWORD), ColorAt(blocks, 1));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NUMBER), ColorAt(blocks, 5));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NORMALTEXT), ColorAt(blocks, 12));
    Assert::AreEqual(static_cast<int>(COLORINDEX_KEYWORD), ColorAt(blocks, 18));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NUMBER), ColorAt(blocks, 20));
    Assert::AreEqual(static_cast<int>(COLORINDEX_OPERATOR),
                     ColorAt(blocks, static_cast<int>(tc::tcslen(line)) - 1));
  }

  TEST_METHOD(EscapedDelimitersAndHexCharacters)
  {
    const tchar_t* line = LR"({\*\foo \{\}\\\'e9\~\u-233?})";
    std::vector<CrystalLineParser::TEXTBLOCK> blocks;
    Assert::AreEqual(0u, Parse(0, line, blocks));
    Assert::AreEqual(static_cast<int>(COLORINDEX_PREPROCESSOR), ColorAt(blocks, 1));
    Assert::AreEqual(static_cast<int>(COLORINDEX_KEYWORD), ColorAt(blocks, 3));
    Assert::AreEqual(static_cast<int>(COLORINDEX_STRING), ColorAt(blocks, 8));
    Assert::AreEqual(static_cast<int>(COLORINDEX_STRING), ColorAt(blocks, 14));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NUMBER), ColorAt(blocks, 22));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NORMALTEXT), ColorAt(blocks, 26));
  }

  TEST_METHOD(BinaryDataMayContainBracesAndBackslashes)
  {
    std::vector<CrystalLineParser::TEXTBLOCK> blocks;
    Assert::AreEqual(0u, Parse(0, LR"(\bin4 {\}x)", blocks));
    Assert::AreEqual(static_cast<int>(COLORINDEX_STRING), ColorAt(blocks, 6));
    Assert::AreEqual(static_cast<int>(COLORINDEX_STRING), ColorAt(blocks, 8));

    blocks.clear();
    unsigned cookie = Parse(0, LR"(\bin4 ab)", blocks);
    Assert::AreEqual(2u, cookie);
    blocks.clear();
    cookie = Parse(cookie, _T("{}Z"), blocks);
    Assert::AreEqual(0u, cookie);
    Assert::AreEqual(static_cast<int>(COLORINDEX_STRING), ColorAt(blocks, 0));
    Assert::AreEqual(static_cast<int>(COLORINDEX_NORMALTEXT), ColorAt(blocks, 2));
  }

  TEST_METHOD(EmptyLinePreservesBinaryState)
  {
    std::vector<CrystalLineParser::TEXTBLOCK> blocks;
    const unsigned cookie = Parse(0, LR"(\bin4 a)", blocks);
    Assert::AreEqual(3u, cookie);
    blocks.clear();
    Assert::AreEqual(cookie, Parse(cookie, _T(""), blocks));
  }
};

} // namespace test
