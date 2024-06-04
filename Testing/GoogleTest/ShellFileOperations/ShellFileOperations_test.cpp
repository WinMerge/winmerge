#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "UnicodeString.h"
#include "ShellFileOperations.h"
#include "paths.h"

namespace
{
	// The fixture for testing command line parsing.
	class ShellFileOperationsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		ShellFileOperationsTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~ShellFileOperationsTest()
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

	TEST_F(ShellFileOperationsTest, CopyFile1)
	{
		//TCHAR curFolder[MAX_PATH] = {0};
		//_tgetcwd(curFolder, MAX_PATH);
		paths::CreateIfNeeded(_T("..\\..\\Data\\ShellFileOpTest"));

		ShellFileOperations ops;
		ops.AddSourceAndDestination(paths::GetLongPath(_T("..\\..\\Data\\File1.txt")),
				paths::GetLongPath(_T("..\\..\\Data\\ShellFileOpTest\\File1.txt")));
		ops.SetOperation(FO_COPY, FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION);
		EXPECT_TRUE(ops.Run());
		EXPECT_FALSE(ops.IsCanceled());
	}

}
