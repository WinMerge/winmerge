#include "pch.h"
#include <gtest/gtest.h>
extern "C"
{
	#include "../../Src/diffutils/src/diff.h"
}

#define sl(x) x, strlen(x)

TEST(diffutils, line_cmp)
{
	auto reset_flags = []() {
		ignore_case_flag = false;
		ignore_space_change_flag = false;
		ignore_all_space_flag = false;
		ignore_eol_diff = false;
		ignore_numbers_flag = false;
	};

	// flags=0
	reset_flags();
	EXPECT_EQ(0, line_cmp(sl("1234a"), sl("1234a")));
	EXPECT_EQ(1, line_cmp(sl("1234a"), sl("2345a")));

	// ignore_numbers_flag
	reset_flags();
	ignore_numbers_flag = true;
	EXPECT_EQ(0, line_cmp(sl("1234a"), sl("2345a")));
	EXPECT_EQ(0, line_cmp(sl("1234a\r\n"), sl("2345a\r\n")));
	EXPECT_EQ(1, line_cmp(sl("1234a"), sl("2345b")));
	EXPECT_EQ(1, line_cmp(sl("1234a\r"), sl("2345a\n")));
	EXPECT_EQ(0, line_cmp(sl("a1234"), sl("a2345")));
	EXPECT_EQ(0, line_cmp(sl("a1234\r\n"), sl("a2345\r\n")));
	EXPECT_EQ(1, line_cmp(sl("a1234"), sl("b2345")));
	EXPECT_EQ(1, line_cmp(sl("a1234\n"), sl("a2345\r\n")));

	// ignore_all_space_flag
	reset_flags();
	ignore_all_space_flag = true;
	EXPECT_EQ(0, line_cmp(sl("1234a"), sl("1234a")));
	EXPECT_EQ(0, line_cmp(sl("1234a  "), sl("  1234a ")));
	EXPECT_EQ(0, line_cmp(sl("1234a  \n"), sl("  1234a \n")));
	EXPECT_EQ(1, line_cmp(sl("1234a  "), sl("  2345a ")));

	// ignore_numbers | ignore_all_space_flag
	reset_flags();
	ignore_numbers_flag = true;
	ignore_all_space_flag = true;
	EXPECT_EQ(0, line_cmp(sl("1234a  "), sl("  2345a ")));
	EXPECT_EQ(0, line_cmp(sl("1234a  \n"), sl("  2345a \n")));
	EXPECT_EQ(1, line_cmp(sl("1234b  "), sl("  2345a ")));
	EXPECT_EQ(1, line_cmp(sl("1234  \n"), sl("2345a \r")));

	// ignore_space_change_flag
	reset_flags();
	ignore_space_change_flag = true;
	EXPECT_EQ(0, line_cmp(sl("1234a"), sl("1234a")));
	EXPECT_EQ(0, line_cmp(sl(" 1234a"), sl("  1234a")));
	EXPECT_EQ(0, line_cmp(sl(" 1234a  "), sl("  1234a ")));
	EXPECT_EQ(1, line_cmp(sl("1234a"), sl("2345a")));
	EXPECT_EQ(1, line_cmp(sl(" 1234a"), sl("1234a")));
	EXPECT_EQ(1, line_cmp(sl("1234a b\n"), sl("1234ab\n")));

	// ignore_numbers_flag | ignore_space_change_flag
	reset_flags();
	ignore_numbers_flag = true;
	ignore_space_change_flag = true;
	EXPECT_EQ(0, line_cmp(sl("1234a  "), sl("2345a ")));
	EXPECT_EQ(0, line_cmp(sl("1234a  \r\n"), sl("2345a \r\n")));
	EXPECT_EQ(1, line_cmp(sl("1234a  "), sl(" 2345a ")));
	EXPECT_EQ(1, line_cmp(sl("1234a  \r\n"), sl(" 2345a \r\n")));

	// ignore_case_flag
	reset_flags();
	ignore_case_flag = true;
	EXPECT_EQ(0, line_cmp(sl("a1234\r\n"), sl("A1234\r\n")));
	EXPECT_EQ(0, line_cmp(sl("1234a"), sl("1234A")));
	EXPECT_EQ(1, line_cmp(sl("1234a"), sl("1234B")));

	// ignore_numbers_flag | ignore_case_flag
	reset_flags();
	ignore_numbers_flag = true;
	ignore_case_flag = true;
	EXPECT_EQ(0, line_cmp(sl("a1234\r\n"), sl("A\r\n")));
	EXPECT_EQ(0, line_cmp(sl("a"), sl("1234A")));
	EXPECT_EQ(1, line_cmp(sl("1234a"), sl("1234B")));
}

