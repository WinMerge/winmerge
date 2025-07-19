#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "UnicodeString.h"
#include "DirWatcher.h"
#include "TFile.h"

using std::vector;

// TODO:
// - add helper functions for checking the changes in registry

namespace
{
	// The fixture for testing command line parsing.
	class DirWatcherTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		DirWatcherTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~DirWatcherTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(DirWatcherTest, Add)
	{
		DirWatcher watcher;
		int counter = 0;
		bool completed = false;
		auto func = [&](const String& path, DirWatcher::ACTION action) {
			wprintf(L"path=%s action=%d\n", path.c_str(), action);
			counter++;
			if (counter >= 10)
				completed = true;
		};
		EXPECT_TRUE(watcher.Add(1, true,  L"..\\TestData\\DirWatcher\\", func));
		EXPECT_TRUE(watcher.Add(2, true,  L"..\\TestData\\DirWatcher\\", func));
		EXPECT_TRUE(watcher.Add(3, false, L"..\\TestData\\DirWatcher\\test.txt", func));
		EXPECT_TRUE(watcher.Add(4, false, L"..\\TestData\\DirWatcher\\test2.txt", func));

		TFile file1(L"..\\TestData\\DirWatcher\\test2.txt");
		file1.setWriteable(false);
		file1.setWriteable(true);
		TFile file2(L"..\\TestData\\DirWatcher\\Subdir1\\test.txt");
		file2.setWriteable(false);
		file2.setWriteable(true);

//		while (!completed)
//			Sleep(1);
	}
}