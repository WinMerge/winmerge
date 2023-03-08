#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/parsers/crystallineparser.h"
#include "../editlib/SyntaxColors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(LuaTests)
	{
	public:
		TEST_METHOD(LuaSyntax)
		{
			std::unique_ptr<CrystalLineParser::TEXTBLOCK[]> pblocks(new CrystalLineParser::TEXTBLOCK[256]);
			struct TestData {
				unsigned dwCookie;
				tchar_t *pszChars;
			} data[] = {
				//                                             1         2         3         4         5
				//                                   012345678901234567890123456789012345678901234657890123456
				{0,                             LR"!(a="\"abc";)!" },
				{0,                             LR"!(ab='\'abc';)!" },
				{0,                              _T("return")},
				{0,                              _T("--comment")},
				{0,                              _T("return abc(1)")},
				{0,                              _T("--[[comment]]--[=[]]]=]--[==[]==][[]][=[s]=][==[[[]]]==]a")},
				{0,                              _T("--[[a")},
				{COOKIE_EXT_COMMENT,             _T("]]--[=[") },
				{COOKIE_EXT_COMMENT | (1 << 28), _T("]=]--[==[") },
				{COOKIE_EXT_COMMENT | (2 << 28), _T("]==][[") },
				{COOKIE_RAWSTRING,               _T("]][=[") },
				{COOKIE_RAWSTRING | (1 << 28),   _T("]=][==[") },
				{COOKIE_RAWSTRING | (2 << 28),   _T("]==];") },
			};
			struct Expected {
				unsigned dwCookie;
				CrystalLineParser::TEXTBLOCK pblocks[10];
				size_t nblocks;
			} expected[] = {
				{ 0, {
					{0,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{1,  COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					{2,  COLORINDEX_STRING,     COLORINDEX_BKGND},
					{9,  COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					}, 4},
				{ 0, {
					{0,  COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					{2,  COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					{3,  COLORINDEX_STRING,     COLORINDEX_BKGND},
					{10, COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					}, 4},
				{ 0, {
					{0,  COLORINDEX_KEYWORD,    COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_COMMENT,    COLORINDEX_BKGND},
					}, 1},
				{ 0, {
					{0,  COLORINDEX_KEYWORD,    COLORINDEX_BKGND},
					{6,  COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					{7,  COLORINDEX_FUNCNAME,   COLORINDEX_BKGND},
					{10, COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					{11, COLORINDEX_NUMBER,    COLORINDEX_BKGND},
					{12, COLORINDEX_OPERATOR,   COLORINDEX_BKGND},
					}, 6},
				{ 0, {
					{0,  COLORINDEX_COMMENT,    COLORINDEX_BKGND},
					{33, COLORINDEX_STRING,     COLORINDEX_BKGND},
					{56, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND},
					}, 3},
				{ COOKIE_EXT_COMMENT,             {{0, COLORINDEX_COMMENT, COLORINDEX_BKGND}}, 1 },
				{ COOKIE_EXT_COMMENT | (1 << 28), {{0, COLORINDEX_COMMENT, COLORINDEX_BKGND}}, 1 },
				{ COOKIE_EXT_COMMENT | (2 << 28), {{0, COLORINDEX_COMMENT, COLORINDEX_BKGND}}, 1 },
				{ COOKIE_RAWSTRING,               {{0, COLORINDEX_COMMENT, COLORINDEX_BKGND}, {4, COLORINDEX_STRING,  COLORINDEX_BKGND}}, 2 },
				{ COOKIE_RAWSTRING | (1 << 28),   {{0, COLORINDEX_STRING,  COLORINDEX_BKGND}}, 1 },
				{ COOKIE_RAWSTRING | (2 << 28),   {{0, COLORINDEX_STRING,  COLORINDEX_BKGND}}, 1 },
				{ 0,                              {{0, COLORINDEX_STRING,  COLORINDEX_BKGND}, {4, COLORINDEX_OPERATOR, COLORINDEX_BKGND}}, 2 },
			};
			for (size_t i = 0; i < std::size(expected); ++i)
			{
				int nActualItems = 0;
				std::wstring msg = L"index: " + std::to_wstring(i);
				Assert::AreEqual(
					static_cast<unsigned>(expected[i].dwCookie),
					CrystalLineParser::ParseLineLua(data[i].dwCookie, data[i].pszChars, static_cast<int>(tc::tcslen(data[i].pszChars)), pblocks.get(), nActualItems), msg.c_str());
				Assert::AreEqual(static_cast<int>(expected[i].nblocks), nActualItems, msg.c_str());
				for (int j = 0; j < nActualItems; ++j)
				{
					std::wstring msg = L"index: " + std::to_wstring(i) + L"," + std::to_wstring(j);
					Assert::AreEqual(expected[i].pblocks[j].m_nCharPos, pblocks[j].m_nCharPos, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nColorIndex, pblocks[j].m_nColorIndex, msg.c_str());
					Assert::AreEqual(expected[i].pblocks[j].m_nBgColorIndex, pblocks[j].m_nBgColorIndex, msg.c_str());
				}
			}
		}

	};
}