// Copyright (c) 2020 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"
#include "../editlib/SyntaxColors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(BatchTests)
	{
	public:
		TEST_METHOD(BatchSyntax)
		{
			struct TestData {
				unsigned dwCookie;
				tchar_t *pszChars;
			} data[] = {
				//                                             1         2         3         4         5
				//                                   012345678901234567890123456789012345678901234657890123456
				{0,                              _T("rem test")},
				{0,                              _T(" rem test rem ^")},
				{0,                              _T("dir rem test")},
			};
			struct Expected {
				unsigned dwCookie;
				CrystalLineParser::TEXTBLOCK pblocks[10];
				size_t nblocks;
			} expected[] = {
				{ 0, {
					{0,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{1,  COLORINDEX_COMMENT, COLORINDEX_BKGND},
					}, 2},
				{ 0, {
					{0,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{3,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{4,  COLORINDEX_KEYWORD, COLORINDEX_BKGND},
					{7,  COLORINDEX_OPERATOR, COLORINDEX_BKGND},
					{8,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					}, 5},
			};
			for (size_t i = 0; i < std::size(expected); ++i)
			{
				std::vector<CrystalLineParser::TEXTBLOCK> blocks;
				std::wstring msg = L"index: " + std::to_wstring(i);
				Assert::AreEqual(
					static_cast<unsigned>(expected[i].dwCookie),
					CrystalLineParser::ParseLineBatch(data[i].dwCookie, data[i].pszChars, static_cast<int>(tc::tcslen(data[i].pszChars)), blocks), msg.c_str());
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
