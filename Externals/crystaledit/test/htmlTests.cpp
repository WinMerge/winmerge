#include "stdafx.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"
#include "../editlib/SyntaxColors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(HTMLTests)
	{
	public:
		TEST_METHOD(HTMLSyntax)
		{
			std::unique_ptr<CrystalLineParser::TEXTBLOCK[]> pblocks(new CrystalLineParser::TEXTBLOCK[256]);
			struct TestData {
				unsigned dwCookie;
				tchar_t *pszChars;
			} data[] = {
				//                                             1         2         3         4         5
				//                                   012345678901234567890123456789012345678901234657890123456
				{0,                              _T("<!---->")},
				{0,                              _T(" <!-- test --> <!-- test -->")},
				{0,                              _T("<!-- test --><!-- test -->")},
				{0,                              _T("<!--")},
				{COOKIE_EXT_COMMENT,             _T("-->")},
				{0,                              _T("<!--<!--")},
				{COOKIE_EXT_COMMENT,             _T("---->")},
				{0,                              _T("<style><!-- --></style>")},
				{0,                              _T("<script><!-- --></script>")},
			};
			struct Expected {
				unsigned dwCookie;
				CrystalLineParser::TEXTBLOCK pblocks[10];
				size_t nblocks;
			} expected[] = {
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 2},
				{ 0, {
					{0,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{1,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{14,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{15,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{28,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 5},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{26,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 2},
				{ COOKIE_EXT_COMMENT, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{3,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 2},
				{ COOKIE_EXT_COMMENT, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{5,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 2},
				{ 0, {
					{0,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{1,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{6,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{16, COLORINDEX_PREPROCESSOR, COLORINDEX_BKGND},
					{17, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{22, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 6},
				{ 0, {
					{0,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{1,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{7,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{17, COLORINDEX_PREPROCESSOR, COLORINDEX_BKGND},
					{18, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{24, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 6},
			};
			for (size_t i = 0; i < std::size(expected); ++i)
			{
				int nActualItems = 0;
				std::wstring msg = L"index: " + std::to_wstring(i);
				Assert::AreEqual(
					static_cast<unsigned>(expected[i].dwCookie),
					CrystalLineParser::ParseLineHtml(data[i].dwCookie, data[i].pszChars, static_cast<int>(_tcslen(data[i].pszChars)), pblocks.get(), nActualItems), msg.c_str());
				Assert::AreEqual(static_cast<int>(expected[i].nblocks), nActualItems, msg.c_str());
				for (int j = 0; j < nActualItems; ++j)
				{
					std::wstring msg = L"index: " + std::to_wstring(i) + L", block: " + std::to_wstring(j);
					Assert::AreEqual(expected[i].pblocks[j].m_nCharPos, pblocks[j].m_nCharPos, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nColorIndex, pblocks[j].m_nColorIndex, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nBgColorIndex, pblocks[j].m_nBgColorIndex, msg.c_str());
				}
			}
		}

	};
}
