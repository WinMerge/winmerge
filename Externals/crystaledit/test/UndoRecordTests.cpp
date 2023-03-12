// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/UndoRecord.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(UndoRecordTests)
	{
	public:
		TEST_METHOD(Test1)
		{
			UndoRecord ur, ur2;
			ur.SetText(_T("Test"), 4);
			Assert::AreEqual(static_cast<size_t>(4), ur.GetTextLength());
			Assert::AreEqual(std::basic_string<tchar_t>(L"Test"), std::basic_string<tchar_t>(ur.GetText(), ur.GetTextLength()));
			ur2.Clone(ur);
			Assert::AreEqual(static_cast<size_t>(4), ur2.GetTextLength());
			Assert::AreEqual(std::basic_string<tchar_t>(L"Test"), std::basic_string<tchar_t>(ur2.GetText(), ur2.GetTextLength()));
		}
		TEST_METHOD(Test2)
		{
			UndoRecord ur, ur2;
			ur.SetText(_T("a"), 1);
			Assert::AreEqual(static_cast<size_t>(1), ur.GetTextLength());
			Assert::AreEqual(std::basic_string<tchar_t>(L"a"), std::basic_string<tchar_t>(ur.GetText(), ur.GetTextLength()));
			ur2.Clone(ur);
			Assert::AreEqual(static_cast<size_t>(1), ur2.GetTextLength());
			Assert::AreEqual(std::basic_string<tchar_t>(L"a"), std::basic_string<tchar_t>(ur2.GetText(), ur2.GetTextLength()));
		}
	};
}
