#include <gtest/gtest.h>
#include "diff.h"
#include "CompareEngines/ByteCompare.h"
#include "CompareOptions.h"
#include "FileLocation.h"
#include "DiffItem.h"
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

namespace
{
	// The fixture for testing paths functions.
	class ByteCompareTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		ByteCompareTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~ByteCompareTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(ByteCompareTest, IgnoreEOLDifference)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		FileLocation location[2];
		file_data filedata[2];
		std::string filename_crlf = "_tmp_cflf.txt";
		std::string filename_lf   = "_tmp_lf.txt";

		option.m_bIgnoreEOLDifference = true;
		bc.SetCompareOptions(option);

		// same
		{
			std::ofstream ostr_crlf(filename_crlf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			std::ofstream ostr_lf(filename_lf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			for (int i = 0; i < 10000; i++)
			{
				ostr_crlf << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\r\n";
				ostr_lf   << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\n";
			}
		}		
		filedata[0].desc = _open(filename_crlf.c_str(), O_RDONLY | O_BINARY, _S_IREAD);
		filedata[1].desc = _open(filename_lf.c_str(), O_RDONLY | O_BINARY, _S_IREAD);
		bc.SetFileData(2, filedata);
		EXPECT_EQ(DIFFCODE::TEXT|DIFFCODE::SAME, bc.CompareFiles(location));
		_close(filedata[0].desc);
		_close(filedata[1].desc);

		// diff
		{
			std::ofstream ostr_crlf(filename_crlf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			std::ofstream ostr_lf(filename_lf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			for (int i = 0; i < 10000; i++)
			{
				ostr_crlf << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\r\n";
				ostr_lf   << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\n";
			}
			ostr_lf << "etc";
		}		
		filedata[0].desc = _open(filename_crlf.c_str(), O_RDONLY | O_BINARY, _S_IREAD);
		filedata[1].desc = _open(filename_lf.c_str(), O_RDONLY | O_BINARY, _S_IREAD);
		bc.SetFileData(2, filedata);
		EXPECT_EQ(DIFFCODE::TEXT|DIFFCODE::DIFF, bc.CompareFiles(location));
		_close(filedata[0].desc);
		_close(filedata[1].desc);

		remove(filename_crlf.c_str());
		remove(filename_lf.c_str());
	}


}  // namespace
