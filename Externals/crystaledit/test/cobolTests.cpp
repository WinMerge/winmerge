// SPDX-License-Identifier: BSL-1.0
#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"
#include "../editlib/SyntaxColors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(CobolTests)
	{
	public:
		TEST_METHOD(CobolSyntax)
		{
			struct TestData {
				unsigned dwCookie;
				const tchar_t *pszChars;
			} data[] = {
				//                1         2         3         4         5         6         7
				//      01234567890123456789012345678901234567890123456789012345678901234567890123456789
				{0, _T("000100 IDENTIFICATION DIVISION.")},
				{0, _T("000200*THIS IS A COMMENT LINE")},
				{0, _T("       01  WS-NAME       PIC IS X(30).")},
				{0, _T("       MAIN-PARA.")},
				{0, _T("           MOVE \"IT\"\"S OK\" TO WS-NAME *> inline")},
				{0, _T("           DISPLAY FUNCTION UPPER-CASE(WS-NAME) X\"0D0A\"")},
				{0, _T("      D    DISPLAY 'DEBUG'")},
				{0, _T("           PERFORM 0000-INIT THRU 0000-EXIT.                          ABC12345")},
				{0, _T(">>SOURCE FORMAT FREE")},
				{0, _T("MOVE A TO B.")},
			};
			struct Expected {
				unsigned dwCookie;
				CrystalLineParser::TEXTBLOCK pblocks[16];
				size_t nblocks;
			} expected[] = {
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{21, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{22, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{30, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 5},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_NUMBER, COLORINDEX_BKGND},
					{9,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{25, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{28, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{29, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{31, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{32, COLORINDEX_USER1, COLORINDEX_BKGND},
					{37, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 9},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_FUNCNAME, COLORINDEX_BKGND},
					{16, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 3},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{11, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{15, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{16, COLORINDEX_STRING, COLORINDEX_BKGND},
					{26, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{27, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{29, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{38, COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 9},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{11, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{18, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{19, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{27, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{28, COLORINDEX_FUNCNAME, COLORINDEX_BKGND},
					{38, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{39, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{46, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{47, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{48, COLORINDEX_STRING, COLORINDEX_BKGND},
					}, 12},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{6,  COLORINDEX_PREPROCESSOR, COLORINDEX_BKGND},
					{7,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{11, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{18, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{19, COLORINDEX_STRING, COLORINDEX_BKGND},
					}, 6},
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					{7,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{11, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{18, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{29, COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{33, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{43, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{44, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{72, COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 9},
				{ 0, {
					{0,  COLORINDEX_PREPROCESSOR, COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{4,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{7,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{9,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{11, COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					}, 5},
			};
			for (size_t i = 0; i < std::size(expected); ++i)
			{
				std::vector<CrystalLineParser::TEXTBLOCK> blocks;
				std::wstring msg = L"index: " + std::to_wstring(i);
				Assert::AreEqual(
					static_cast<unsigned>(expected[i].dwCookie),
					CrystalLineParser::ParseLineCobol(data[i].dwCookie, data[i].pszChars, static_cast<int>(tc::tcslen(data[i].pszChars)), &blocks), msg.c_str());
				int nActualItems = static_cast<int>(blocks.size());
				Assert::AreEqual(static_cast<int>(expected[i].nblocks), nActualItems, msg.c_str());
				for (int j = 0; j < nActualItems; ++j)
				{
					std::wstring msg = L"index: " + std::to_wstring(i) + L", block: " + std::to_wstring(j);
					Assert::AreEqual(expected[i].pblocks[j].m_nCharPos, blocks[j].m_nCharPos, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nColorIndex, blocks[j].m_nColorIndex, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nBgColorIndex, blocks[j].m_nBgColorIndex, msg.c_str());
				}
			}
		}

	};
}
