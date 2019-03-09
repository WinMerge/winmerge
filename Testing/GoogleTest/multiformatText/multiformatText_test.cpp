#include "pch.h"
#include <gtest/gtest.h>
#include "multiformatText.h"

namespace
{
	// The fixture for testing string differencing functions.
	class storageForPluginsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		storageForPluginsTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~storageForPluginsTest()
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

	TEST_F(storageForPluginsTest, Test1)
	{
		{
			storageForPlugins bufferData;
			// detect Ansi or Unicode file
			bufferData.SetDataFileUnknown(_T("../../Data/Unicode/UTF-8/DiffItem.h"), false);
			BSTR *bstr = bufferData.GetDataBufferUnicode();
			VARIANT *var = bufferData.GetDataBufferAnsi();
			String filename = bufferData.GetDataFileUnicode();
			filename = bufferData.GetDataFileAnsi();
			bstr = bufferData.GetDataBufferUnicode();
			}

		{
			storageForPlugins bufferData;
			bufferData.SetDataFileUnknown(_T("../../Data/Unicode/UTF-8-NOBOM/DiffItem.h"), false);
			BSTR *bstr = bufferData.GetDataBufferUnicode();
			VARIANT *var = bufferData.GetDataBufferAnsi();
			String filename = bufferData.GetDataFileUnicode();
			filename = bufferData.GetDataFileAnsi();
		}

		{
			storageForPlugins bufferData;
			bufferData.SetDataFileUnknown(_T("../../Data/Unicode/UCS-2LE/DiffItem.h"), false);
			BSTR *bstr = bufferData.GetDataBufferUnicode();
			VARIANT *var = bufferData.GetDataBufferAnsi();
			String filename = bufferData.GetDataFileUnicode();
			filename = bufferData.GetDataFileAnsi();
		}

		{
			storageForPlugins bufferData;
			bufferData.SetDataFileUnknown(_T("../../Data/Unicode/UCS-2BE/DiffItem.h"), false);
			BSTR *bstr = bufferData.GetDataBufferUnicode();
			VARIANT *var = bufferData.GetDataBufferAnsi();
			String filename = bufferData.GetDataFileUnicode();
			filename = bufferData.GetDataFileAnsi();
		}
	}

}  // namespace
