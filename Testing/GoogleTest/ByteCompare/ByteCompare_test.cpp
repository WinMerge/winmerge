#include "pch.h"
#include <gtest/gtest.h>
#include "diff.h"
#include "CompareEngines/ByteCompare.h"
#include "CompareOptions.h"
#include "FileLocation.h"
#include "DiffItem.h"
#include "DiffFileData.h"
#include "cio.h"
#include "unicoder.h"
#include <fstream>

namespace
{
	const int WMCMPBUFF = 32 * 1024;

	struct TempFile
	{
		TempFile(const std::string& filename, const char *data, size_t len) : m_filename(filename)
		{
			std::ofstream ostr(filename.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			ostr.write(data, len);
		}
		~TempFile()
		{
			remove(m_filename.c_str());
		}
		std::string m_filename;
	};

	struct FilePair
	{
		FilePair(const std::string& left, const std::string& right)
		{
			diffData.OpenFiles(ucr::toTString(left), ucr::toTString(right));
		}

		~FilePair()
		{
		}

		DiffFileData diffData;
	};

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

	TEST_F(ByteCompareTest, FileTextStats)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 3];
		char buf_right[WMCMPBUFF * 3 + 1];

		memset(buf_left, 'A', sizeof(buf_left));;

		buf_left[              5] = '\0';         
		buf_left[             10] = '\0';         
		buf_left[WMCMPBUFF +  10] = '\0';         

		buf_left[WMCMPBUFF -   1] = '\r';
		buf_left[WMCMPBUFF      ] = '\n';
		buf_left[            100] = '\r';
		buf_left[            101] = '\n';
		buf_left[WMCMPBUFF + 100] = '\r';
		buf_left[WMCMPBUFF + 101] = '\n';

		buf_left[            200] = '\n';
		buf_left[            201] = '\n';
		buf_left[WMCMPBUFF + 200] = '\n';

		buf_left[            300] = '\r';
		buf_left[            301] = '\r';
		buf_left[WMCMPBUFF + 300] = '\r';
		buf_left[WMCMPBUFF*2 - 1] = '\r';
		buf_left[sizeof(buf_left) - 1] = '\r';

		memcpy(buf_right, buf_left, sizeof(buf_left));
		buf_right[sizeof(buf_right) - 1] = '\r';

		TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
		TempFile file_right(filename_right, buf_right, sizeof(buf_right));

		FilePair pair(filename_left, filename_right);

		bc.SetCompareOptions(option);
		EXPECT_EQ(int(DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		FileTextStats stats[2];
		stats[0] = pair.diffData.m_textStats[0];
		stats[1] = pair.diffData.m_textStats[1];

//		EXPECT_EQ(3, stats[0].nzeros);
//		EXPECT_EQ(3, stats[1].nzeros);
		EXPECT_EQ(3, stats[0].ncrlfs);
		EXPECT_EQ(3, stats[1].ncrlfs);
		EXPECT_EQ(3, stats[0].nlfs);
		EXPECT_EQ(3, stats[1].nlfs);
		EXPECT_EQ(5, stats[0].ncrs);
		EXPECT_EQ(6, stats[1].ncrs);

	}

	TEST_F(ByteCompareTest, IgnoreAllSpace)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2 - 1];

		option.m_ignoreWhitespace = WHITESPACE_IGNORE_ALL;
		bc.SetCompareOptions(option);

		memset(buf_left,  'A', sizeof(buf_left));
		memset(buf_right, 'A', sizeof(buf_right));

		buf_left [              5] = ' ';
		buf_left [              6] = ' ';
		buf_right[              5] = ' ';

		buf_left [             10] = ' ';         
		buf_right[             11] = ' ';         
		buf_right[             12] = ' ';         

		buf_left [             20] = ' ';
		buf_right[             40] = ' ';

		buf_left [WMCMPBUFF -   1] = ' ';
		buf_left [WMCMPBUFF      ] = ' ';
		buf_right[WMCMPBUFF +   1] = ' ';
		buf_right[WMCMPBUFF +   2] = ' ';

		buf_left [sizeof(buf_left) - 1] = ' ';

		{// same
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// diff
			buf_right[WMCMPBUFF +  10] = ' ';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{// all space
			memset(buf_left,  ' ', sizeof(buf_left));
			memset(buf_right, ' ', sizeof(buf_right));

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// empty right
			memset(buf_left,  ' ', sizeof(buf_left));

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, "", 0);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// empty left
			memset(buf_right,  ' ', sizeof(buf_right));

			TempFile file_left (filename_left,  "", 0);
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{
			memset(buf_left,  ' ', sizeof(buf_left));
			buf_left [WMCMPBUFF - 1] = 'A';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, "A", 1);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{
			memset(buf_right,  ' ', sizeof(buf_right));
			buf_right[WMCMPBUFF - 1] = 'A';

			TempFile file_left (filename_left,  "A", 1);
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

	}

	TEST_F(ByteCompareTest, IgnoreSpaceChange)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2 - 1];

		option.m_ignoreWhitespace = WHITESPACE_IGNORE_CHANGE;
		bc.SetCompareOptions(option);

		memset(buf_left,  'A', sizeof(buf_left));
		memset(buf_right, 'A', sizeof(buf_right));

		buf_left [              5] = ' ';
		buf_left [              6] = ' ';
		buf_right[              5] = ' ';

		buf_left [             10] = ' ';
		buf_right[              9] = ' ';
		buf_right[             10] = ' ';

		buf_left [             40] = ' ';
		buf_left [             41] = '\n';
		buf_right[             40] = '\n';

		buf_left [             51] = '\n';
		buf_right[             50] = ' ';
		buf_right[             51] = '\n';

		buf_left [WMCMPBUFF -   1] = ' ';
		buf_left [WMCMPBUFF      ] = ' ';
		buf_left [WMCMPBUFF +   1] = ' ';
		buf_right[WMCMPBUFF -   1] = ' ';
		buf_right[WMCMPBUFF      ] = ' ';

		{// same1
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// diff
			buf_left [WMCMPBUFF +  10] = ' ';
			buf_right[WMCMPBUFF +  20] = ' ';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{// same2
			memset(buf_left,  'A', sizeof(buf_left));
			memset(buf_right, 'A', sizeof(buf_right));

			buf_left [10] = ' ';
			buf_left [11] = ' ';
			buf_right[10] = ' ';
			buf_right[11] = ' ';
			buf_right[12] = ' ';

			buf_left [WMCMPBUFF   - 1] = ' ';
			buf_left [WMCMPBUFF      ] = ' ';
			buf_left [WMCMPBUFF   + 1] = ' ';
			buf_right[WMCMPBUFF      ] = ' ';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// same3
			memset(buf_left,  'A', sizeof(buf_left));
			memset(buf_right, 'A', sizeof(buf_right));

			buf_left [10] = ' ';
			buf_left [11] = ' ';
			buf_left [12] = ' ';
			buf_right[10] = ' ';
			buf_right[11] = ' ';

			buf_left [WMCMPBUFF      ] = ' ';
			buf_left [WMCMPBUFF   + 1] = ' ';
			buf_right[WMCMPBUFF   - 1] = ' ';
			buf_right[WMCMPBUFF      ] = ' ';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

	}

	TEST_F(ByteCompareTest, IgnoreEOLDifference)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_crlf = "_tmp_cflf.txt";
		std::string filename_lf   = "_tmp_lf.txt";

		option.m_bIgnoreEOLDifference = true;
		bc.SetCompareOptions(option);

		// same
		{
			{
				std::ofstream ostr_crlf(filename_crlf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
				std::ofstream ostr_lf(filename_lf.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
				for (int i = 0; i < 10000; i++)
				{
					ostr_crlf << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\r\n";
					ostr_lf   << "testdata_" << i << "_01234567890123456789012345678901234567890123456789\n";
				}
			}		

			FilePair pair(filename_crlf, filename_lf);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		// diff
		{
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

			FilePair pair(filename_crlf, filename_lf);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		remove(filename_crlf.c_str());
		remove(filename_lf.c_str());

		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		for (int i = 0; i < 3; i++)
		{
			{// diff left: LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\r';
				buf_right[10] = '\n';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
		for (int i = 0; i < 3; i++)
		{
			{// diff left: CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\r';
				buf_left[11] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';
				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\r';
				buf_left[11] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';
				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
	}

	TEST_F(ByteCompareTest, IgnoreCase)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2];

		option.m_bIgnoreCase = true;
		bc.SetCompareOptions(option);

		memset(buf_left,  'A', sizeof(buf_left));
		memset(buf_right, 'a', sizeof(buf_right));

		for (int i = 0, c = 'A'; c <= 'Z'; ++c, ++i)
		{
			buf_left [i] = static_cast<char>(c);
			buf_right[i] = static_cast<char>(c - 'A' + 'a');
		}

		{// same
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// diff
			buf_left [WMCMPBUFF +  20] = 'a';
			buf_right[WMCMPBUFF +  20] = 'b';

			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left));
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

	}

	TEST_F(ByteCompareTest, IgnoreBlankLines)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2];

		option.m_ignoreWhitespace = WHITESPACE_COMPARE_ALL;
		option.m_bIgnoreBlankLines = true;
		bc.SetCompareOptions(option);

		{// same
			strcpy_s(buf_left, 
"\r\n/* abc */\r\nabc(aaa);\r\n\r\n");
			strcpy_s(buf_right, 
"  \r\n/* abc */\r\nabc(aaa);\r\n  \r\n");
			TempFile file_left (filename_left,  buf_left,  strlen(buf_left));
			TempFile file_right(filename_right, buf_right, strlen(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// diff 
			strcpy_s(buf_left, 
"\r\n /* abc */\r\nabc(aaa);\r\n\r\n");
			strcpy_s(buf_right, 
"  \r\n  /* abc */\r\nabc(aaa);\r\n  \r\n");
			TempFile file_left (filename_left,  buf_left,  strlen(buf_left));
			TempFile file_right(filename_right, buf_right, strlen(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}
	}

	TEST_F(ByteCompareTest, IgnoreBlankLinesAndIgnoreEOLDifference)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2];

		option.m_ignoreWhitespace = WHITESPACE_COMPARE_ALL;
		option.m_bIgnoreEOLDifference = true;
		option.m_bIgnoreBlankLines = true;
		bc.SetCompareOptions(option);

		{// same
			strcpy_s(buf_left, 
"\n/* abc */\nabc(aaa);\n\n");
			strcpy_s(buf_right, 
"  \r\n/* abc */\r\nabc(aaa);\r\n  \r\n");
			TempFile file_left (filename_left,  buf_left,  strlen(buf_left));
			TempFile file_right(filename_right, buf_right, strlen(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
		}

		{// diff 
			strcpy_s(buf_left, 
"\n/* abc */ \nabc(aaa);\n\n");
			strcpy_s(buf_right, 
"  \r\n/* abc */\r\nabc(aaa);\r\n  \r\n");
			TempFile file_left (filename_left,  buf_left,  strlen(buf_left));
			TempFile file_right(filename_right, buf_right, strlen(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}
	}

	TEST_F(ByteCompareTest, CompareAppendedFile)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";
		char buf_left [WMCMPBUFF * 2];
		char buf_right[WMCMPBUFF * 2];

		bc.SetCompareOptions(option);

		memset(buf_left,  'A', sizeof(buf_left));
		memset(buf_right, 'A', sizeof(buf_right));

		{
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left)     );
			TempFile file_right(filename_right, buf_right, sizeof(buf_right) - 1);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left)  - 1);
			TempFile file_right(filename_right, buf_right, sizeof(buf_right)    );

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{
			TempFile file_left (filename_left,  buf_left,  WMCMPBUFF);
			TempFile file_right(filename_right, buf_right, sizeof(buf_right));

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_right));
			TempFile file_right(filename_right, buf_right, WMCMPBUFF);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{
			TempFile file_left (filename_left,  buf_left,  WMCMPBUFF + 1);
			TempFile file_right(filename_right, buf_right, sizeof(buf_right) - 1);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

		{
			TempFile file_left (filename_left,  buf_left,  sizeof(buf_left) - 1);
			TempFile file_right(filename_right, buf_right, WMCMPBUFF + 1);

			FilePair pair(filename_left, filename_right);

			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
		}

	}

	TEST_F(ByteCompareTest, IgnoreMissingTrailingEol)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";

		option.m_bIgnoreMissingTrailingEol = true;
		bc.SetCompareOptions(option);

		for (int i = 0; i < 3; i++)
		{
			{// same left: LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// same left: CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+A - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+A
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: LF+LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 2] = '\n';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: LF+LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 2] = '\n';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
		
		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+CR - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+CR
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 4; i++)
		{
			{// diff left: CRLF+CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 4 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[buf_left.size() - 4] = '\r';
				buf_left[buf_left.size() - 3] = '\n';
				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CRLF+CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 4 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[buf_right.size() - 4] = '\r';
				buf_right[buf_right.size() - 3] = '\n';
				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
	}

	TEST_F(ByteCompareTest, IgnoreMissingTrailingEolAndIgnoreEOLDifference)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";

		option.m_bIgnoreMissingTrailingEol = true;
		option.m_bIgnoreEOLDifference = true;
		bc.SetCompareOptions(option);

		for (int i = 0; i < 3; i++)
		{
			{// same left: LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// same left: CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[19] = '\r';
				buf_right[20] = '\r';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[19] = '\r';
				buf_left[20] = '\r';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+A - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[19] = '\r';
				buf_right[20] = '\r';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+A
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[19] = '\r';
				buf_left[20] = '\r';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: LF+LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[19] = '\r';
				buf_right[20] = '\r';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\n';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: LF+LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[19] = '\r';
				buf_left[20] = '\r';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\n';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
		
		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+CR - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[19] = '\r';
				buf_right[20] = '\r';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+CR
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[19] = '\r';
				buf_left[20] = '\r';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 4; i++)
		{
			{// diff left: CRLF+CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 4 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 4 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\r';
				buf_right[11] = '\n';

				buf_left[19] = '\r';
				buf_right[20] = '\r';
				buf_right[21] = '\n';

				buf_left[28] = '\r';
				buf_right[30] = '\r';
				buf_right[31] = '\n';

				buf_left[buf_left.size() - 4] = '\r';
				buf_left[buf_left.size() - 3] = '\n';
				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CRLF+CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 4 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 4 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\r';
				buf_left[11] = '\n';

				buf_right[19] = '\r';
				buf_left[20] = '\r';
				buf_left[21] = '\n';

				buf_right[28] = '\r';
				buf_left[30] = '\r';
				buf_left[31] = '\n';

				buf_right[buf_right.size() - 4] = '\r';
				buf_right[buf_right.size() - 3] = '\n';
				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
	}

	TEST_F(ByteCompareTest, IgnoreMissingTrailingEolAndIgnoreBlankLines)
	{
		CompareEngines::ByteCompare bc;
		QuickCompareOptions option;
		std::string filename_left  = "_tmp_.txt";
		std::string filename_right = "_tmp_2.txt";

		option.m_bIgnoreMissingTrailingEol = true;
		option.m_bIgnoreBlankLines = true;
		bc.SetCompareOptions(option);

		for (int i = 0; i < 3; i++)
		{
			{// same left: LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 1 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 1 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// same left: CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[19] = '\n';
				buf_right[20] = '\n';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}

			{// same left: no EOL - right: CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[19] = '\n';
				buf_left[20] = '\n';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::SAME), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+A - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[19] = '\n';
				buf_right[20] = '\n';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+A
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[19] = '\n';
				buf_left[20] = '\n';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = 'A';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 3; i++)
		{
			{// diff left: LF+LF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[19] = '\n';
				buf_right[20] = '\n';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\n';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: LF+LF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[19] = '\n';
				buf_left[20] = '\n';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\n';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
		
		for (int i = 0; i < 3; i++)
		{
			{// diff left: CR+CR - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[19] = '\n';
				buf_right[20] = '\n';
				buf_right[21] = '\n';

				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CR+CR
				std::vector<char> buf_left(WMCMPBUFF * 2 - 2 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 2 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[19] = '\n';
				buf_left[20] = '\n';
				buf_left[21] = '\n';

				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\r';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}

		for (int i = 0; i < 4; i++)
		{
			{// diff left: CRLF+CRLF - right: no EOL
				std::vector<char> buf_left(WMCMPBUFF * 2 - 4 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 4 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_left[10] = '\n';
				buf_right[10] = '\n';
				buf_right[11] = '\n';

				buf_left[19] = '\n';
				buf_right[20] = '\n';
				buf_right[21] = '\n';

				buf_left[28] = '\n';
				buf_right[30] = '\n';
				buf_right[31] = '\n';

				buf_left[buf_left.size() - 4] = '\r';
				buf_left[buf_left.size() - 3] = '\n';
				buf_left[buf_left.size() - 2] = '\r';
				buf_left[buf_left.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}

			{// diff left: no EOL - right: CRLF+CRLF
				std::vector<char> buf_left(WMCMPBUFF * 2 - 4 + i);
				std::vector<char> buf_right(WMCMPBUFF * 2 - 4 + i);

				memset(buf_left.data(), 'A', buf_left.size());
				memset(buf_right.data(), 'A', buf_right.size());

				buf_right[10] = '\n';
				buf_left[10] = '\n';
				buf_left[11] = '\n';

				buf_right[19] = '\n';
				buf_left[20] = '\n';
				buf_left[21] = '\n';

				buf_right[28] = '\n';
				buf_left[30] = '\n';
				buf_left[31] = '\n';

				buf_right[buf_right.size() - 4] = '\r';
				buf_right[buf_right.size() - 3] = '\n';
				buf_right[buf_right.size() - 2] = '\r';
				buf_right[buf_right.size() - 1] = '\n';

				TempFile file_left(filename_left, buf_left.data(), buf_left.size());
				TempFile file_right(filename_right, buf_right.data(), buf_right.size());

				FilePair pair(filename_left, filename_right);

				EXPECT_EQ(int(DIFFCODE::TEXT | DIFFCODE::DIFF), bc.CompareFiles(&pair.diffData));
			}
		}
	}

}  // namespace
