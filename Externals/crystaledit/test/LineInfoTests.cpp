// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#include "pch.h"
#include "CppUnitTest.h"
#include "../editlib/LineInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(LineInfoTests)
	{
	public:
		TEST_METHOD(Test1)
		{
			LineInfo li{ _T("AB"), 2 };
			Assert::AreEqual(_T("AB"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(2), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.Append(_T("0123456789"), 10);
			Assert::AreEqual(_T("AB0123456789"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(12), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.Append(_T("0123456789\r\n"), 12);
			Assert::AreEqual(_T("AB01234567890123456789\r\n"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(22), li.Length());
			Assert::AreEqual(_T("\r\n"), li.GetEol());
			Assert::IsTrue( li.HasEol());

			li.DeleteEnd(2);
			Assert::AreEqual(_T("AB"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(2), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.ChangeEol(_T("\n"));
			Assert::AreEqual(_T("AB\n"), li.GetLine());
			Assert::AreEqual(_T("\n"), li.GetEol());
			Assert::IsTrue( li.HasEol());

			li.ChangeEol(_T(""));
			Assert::AreEqual(_T("AB"), li.GetLine());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

		}

		TEST_METHOD(Test2)
		{
			LineInfo li;
			li.CreateEmpty();
			Assert::AreEqual(_T(""), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(0), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.Append(_T("0123456789"), 10);
			Assert::AreEqual(_T("0123456789"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(10), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.Delete(3, 5);
			Assert::AreEqual(_T("01256789"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(8), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());

			li.Delete(0, 8);
			Assert::AreEqual(_T(""), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(0), li.Length());
			Assert::AreEqual(nullptr, li.GetEol());
			Assert::IsFalse( li.HasEol());
		}

		TEST_METHOD(Test3)
		{
			LineInfo li;
			li.Create(_T("ABC\r"), 4);
			Assert::AreEqual(_T("ABC\r"), li.GetLine());
			Assert::AreEqual(static_cast<size_t>(3), li.Length());
			Assert::AreEqual(_T("\r"), li.GetEol());
			Assert::IsTrue( li.HasEol());
		}
	};
}
