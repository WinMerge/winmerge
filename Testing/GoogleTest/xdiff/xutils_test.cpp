#include "pch.h"
#include <gtest/gtest.h>
extern "C"
{
	#include "../../Externals/xdiff/xinclude.h"
}

#define sl(x) x, static_cast<long>(strlen(x))

TEST(xutils, xdl_recmatch)
{
	// flags=0
	EXPECT_EQ(1, xdl_recmatch(sl("1234a"), sl("1234a"), 0));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a"), sl("2345a"), 0));

	// XDF_IGNORE_NUMBERS
	EXPECT_EQ(1, xdl_recmatch(sl("1234a"), sl("2345a"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\r\n"), sl("2345a\r\n"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a"), sl("2345b"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a\r"), sl("2345a\n"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(1, xdl_recmatch(sl("a1234"), sl("a2345"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(1, xdl_recmatch(sl("a1234\r\n"), sl("a2345\r\n"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(0, xdl_recmatch(sl("a1234"), sl("b2345"), XDF_IGNORE_NUMBERS));
	EXPECT_EQ(0, xdl_recmatch(sl("a1234\n"), sl("a2345\r\n"), XDF_IGNORE_NUMBERS));

	// XDF_IGNORE_WHITESPACE
	EXPECT_EQ(1, xdl_recmatch(sl("1234a"), sl("1234a"), XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  "), sl("  1234a "), XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \n"), sl("  1234a \n"), XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a  "), sl("  2345a "), XDF_IGNORE_WHITESPACE));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  "), sl("  2345a "), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \n"), sl("  2345a \n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234b  "), sl("  2345a "), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234  \n"), sl("2345a \r"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));

	// XDF_IGNORE_WHITESPACE_CHANGE
	EXPECT_EQ(1, xdl_recmatch(sl("1234a"), sl("1234a"), XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(1, xdl_recmatch(sl(" 1234a"), sl("  1234a"), XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(1, xdl_recmatch(sl(" 1234a  "), sl("  1234a "), XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a"), sl("2345a"), XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(0, xdl_recmatch(sl(" 1234a"), sl("1234a"), XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a b\n"), sl("1234ab\n"), XDF_IGNORE_WHITESPACE_CHANGE));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_CHANGE
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  "), sl("2345a "), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \r\n"), sl("2345a \r\n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a  "), sl(" 2345a "), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_CHANGE));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a  \r\n"), sl(" 2345a \r\n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_CHANGE));

	// XDF_IGNORE_WHITESPACE_AT_EOL
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\r\n"), sl("1234a\r\n"), XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\r\n"), sl("1234a \r\n"), XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \r\n"), sl("1234a \r\n"), XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a\r\n"), sl("2345a\r\n"), XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a  \r\n"), sl("2345a \r\n"), XDF_IGNORE_WHITESPACE_AT_EOL));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_AT_EOL
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  "), sl("2345a "), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \r\n"), sl("2345a \r\n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("  1234a  \r\n"), sl(" 2345a \r\n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a  \r\n"), sl("2345a \r"), XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE_AT_EOL));

	// XDF_IGNORE_CR_AT_EOL 
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\r\n"), sl("1234a\n"), XDF_IGNORE_CR_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\n"), sl("1234a\r\n"), XDF_IGNORE_CR_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a\r\n"), sl("2345a\n"), XDF_IGNORE_CR_AT_EOL));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL 
	EXPECT_EQ(1, xdl_recmatch(sl("1234a"), sl("2345a"), XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_EQ(1, xdl_recmatch(sl("1234a\r\n"), sl("2345a\n"), XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a"), sl("2345b"), XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_EQ(0, xdl_recmatch(sl("1234a\n"), sl("2345a"), XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
}

TEST(xutils, xdl_hash_record)
{
	auto hash_comp = [](const char* data1, const char* data2, int flags)
	{
		const char* ptr1 = data1, * ptr2 = data2;
		return xdl_hash_record(&ptr1, ptr1 + strlen(ptr1), flags) == xdl_hash_record(&ptr2, ptr2 + strlen(ptr2), flags);
	};
	// flags = 0
	EXPECT_TRUE(hash_comp("", "", 0));
	EXPECT_TRUE(hash_comp("1234a", "1234a", 0));
	EXPECT_FALSE(hash_comp("1234a", "1234b", 0));
	EXPECT_FALSE(hash_comp("a1234", "b1234", 0));

	// XDF_IGNORE_NUMBERS
	EXPECT_TRUE(hash_comp("1234a", "1234a", XDF_IGNORE_NUMBERS));
	EXPECT_TRUE(hash_comp("1234a", "2a", XDF_IGNORE_NUMBERS));
	EXPECT_TRUE(hash_comp("a1234", "a", XDF_IGNORE_NUMBERS));
	EXPECT_FALSE(hash_comp("1234a", "2345b", XDF_IGNORE_NUMBERS));
	EXPECT_FALSE(hash_comp("a1234", "b2345", XDF_IGNORE_NUMBERS));

	// XDF_IGNORE_WHITESPACE
	EXPECT_TRUE(hash_comp("1234a", "1234a", XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp(" 1234a", "1234a", XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("1234a", " 1234a", XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("1234a ", "1234a", XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("1234a", "1234a ", XDF_IGNORE_WHITESPACE));
	EXPECT_FALSE(hash_comp("1234a", "1234b", XDF_IGNORE_WHITESPACE));
	EXPECT_FALSE(hash_comp("a1234", "b1234", XDF_IGNORE_WHITESPACE));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE
	EXPECT_TRUE(hash_comp("1234a", "1234a", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp(" 1234a", "12a", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("1234a", " a", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("a ", "1234a", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_TRUE(hash_comp("1234a", "1a ", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_FALSE(hash_comp("1234a", "1234b", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));
	EXPECT_FALSE(hash_comp("a1234", "b1234", XDF_IGNORE_NUMBERS | XDF_IGNORE_WHITESPACE));

	// XDF_IGNORE_CR_AT_EOL 
	EXPECT_TRUE(hash_comp("\r", "\n", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a", "1234a", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\n", "1234a\n", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\r", "1234a\n", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\n", "1234a\r", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\r", "1234a\r", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\r\n", "1234a\r", XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\r", "1234a\r\n", XDF_IGNORE_CR_AT_EOL));
	EXPECT_FALSE(hash_comp("1234a\r\n", "1234b\r\n", XDF_IGNORE_CR_AT_EOL));
	EXPECT_FALSE(hash_comp("a1234\r\n", "b1234\r\n", XDF_IGNORE_CR_AT_EOL));

	// XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL 
	EXPECT_TRUE(hash_comp("\r", "\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a", "1234a", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\n", "34a\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("a\r", "1234a\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\n", "12a\r", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("12a\r", "1234a\r", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("1234a\r\n", "a\r", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_TRUE(hash_comp("a\r", "1234a\r\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_FALSE(hash_comp("1234a\r\n", "1234b\r\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
	EXPECT_FALSE(hash_comp("a1234\r\n", "b1234\r\n", XDF_IGNORE_NUMBERS | XDF_IGNORE_CR_AT_EOL));
}
