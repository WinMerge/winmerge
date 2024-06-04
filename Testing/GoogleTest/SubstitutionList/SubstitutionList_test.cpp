#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "SubstitutionList.h"
#include "Environment.h"
#include "paths.h"

namespace
{
	// The fixture for testing string differencing functions.
	class SubstitutionListTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		SubstitutionListTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~SubstitutionListTest()
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
		SubstitutionList m_substitutionList;
	};

	TEST_F(SubstitutionListTest, test)
	{
		SubstitutionList list;
		EXPECT_FALSE(list.HasRegExps());
		list.Add("abc", "\\a\\b\\f\\n\\r\\t\\v", Poco::RegularExpression::RE_MULTILINE);
		EXPECT_TRUE(list.HasRegExps());
		list.Add("([0-9]+) ([A-Z]+)", "\\2-\\1", Poco::RegularExpression::RE_MULTILINE);
		list.Add("def", "\\x01\\xEF\\xab\\x", Poco::RegularExpression::RE_MULTILINE);
		EXPECT_EQ(3, list.GetCount());
		EXPECT_EQ("-\a\b\f\n\r\t\v-", list.Subst("-abc-"));
		EXPECT_EQ("-ABCD-0123-", list.Subst("-0123 ABCD-"));
		EXPECT_EQ("\x01\xef\xab\\x", list.Subst("def"));
		list.RemoveAllFilters();
		EXPECT_EQ(0, list.GetCount());
	}



}  // namespace
