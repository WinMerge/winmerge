#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include "varprop.h"

// NOTE:
// Use namespace varprop:: with type names as there are conflicting values in VT_*

namespace
{
	// The fixture for testing command line parsing.
	class VariantValueTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		VariantValueTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~VariantValueTest()
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

	// Initialize without type
	TEST_F(VariantValueTest, Create1)
	{
		varprop::VariantValue val;
		EXPECT_EQ(varprop::VT_NULL, val.GetType());
		EXPECT_FALSE(val.IsBool());
		EXPECT_FALSE(val.IsFloat());
		EXPECT_FALSE(val.IsInt());
		EXPECT_FALSE(val.IsString());
		EXPECT_FALSE(val.IsTime());
	}

	// Set and read boolean value
	TEST_F(VariantValueTest, SetBool1)
	{
		varprop::VariantValue val;
		val.SetBool(false);
		EXPECT_TRUE(val.IsBool());
		EXPECT_FALSE(val.GetBool());
	}

	// Set and read boolean value
	TEST_F(VariantValueTest, SetBool2)
	{
		varprop::VariantValue val;
		val.SetBool(true);
		EXPECT_TRUE(val.IsBool());
		EXPECT_TRUE(val.GetBool());
	}

	// Set and read integer value
	TEST_F(VariantValueTest, SetInt1)
	{
		varprop::VariantValue val;
		val.SetInt(0);
		EXPECT_TRUE(val.IsInt());
		EXPECT_EQ(0, val.GetInt());
	}

	// Set and read integer value
	TEST_F(VariantValueTest, SetInt2)
	{
		varprop::VariantValue val;
		val.SetInt(-5);
		EXPECT_TRUE(val.IsInt());
		EXPECT_EQ(-5, val.GetInt());
	}

	// Set and read integer value
	TEST_F(VariantValueTest, SetInt3)
	{
		varprop::VariantValue val;
		val.SetInt(15);
		EXPECT_TRUE(val.IsInt());
		EXPECT_EQ(15, val.GetInt());
	}

	// Set and read float value
	TEST_F(VariantValueTest, SetFloat1)
	{
		varprop::VariantValue val;
		val.SetFloat(0.0);
		EXPECT_TRUE(val.IsFloat());
		EXPECT_EQ(0.0, val.GetFloat());
	}

	// Set and read float value
	TEST_F(VariantValueTest, SetFloat2)
	{
		varprop::VariantValue val;
		val.SetFloat(-3.5);
		EXPECT_TRUE(val.IsFloat());
		EXPECT_EQ(-3.5, val.GetFloat());
	}

	// Set and read float value
	TEST_F(VariantValueTest, SetFloat3)
	{
		varprop::VariantValue val;
		val.SetFloat(7.2);
		EXPECT_TRUE(val.IsFloat());
		EXPECT_EQ(7.2, val.GetFloat());
	}

	// Set and read String value
	TEST_F(VariantValueTest, SetString1)
	{
		varprop::VariantValue val;
		val.SetString(_T(""));
		EXPECT_TRUE(val.IsString());
		EXPECT_EQ(_T(""), val.GetString());
	}

	// Set and read String value
	TEST_F(VariantValueTest, SetString2)
	{
		varprop::VariantValue val;
		val.SetString(_T("Boo"));
		EXPECT_TRUE(val.IsString());
		EXPECT_EQ(_T("Boo"), val.GetString());
	}

	// Set and read time value
	TEST_F(VariantValueTest, SetTime1)
	{
		varprop::VariantValue val;
		val.SetTime(0);
		EXPECT_TRUE(val.IsTime());
		EXPECT_EQ(0, val.GetTime());
	}

	// Set and read time value
	TEST_F(VariantValueTest, SetTime2)
	{
		varprop::VariantValue val;
		val.SetTime(382);
		EXPECT_TRUE(val.IsTime());
		EXPECT_EQ(382, val.GetTime());
	}

	// Set and clear value
	TEST_F(VariantValueTest, Clear1)
	{
		varprop::VariantValue val;
		val.SetInt(382);
		EXPECT_TRUE(val.IsInt());
		EXPECT_EQ(382, val.GetInt());
		val.Clear();
		EXPECT_EQ(varprop::VT_NULL, val.GetType());
		EXPECT_FALSE(val.IsBool());
		EXPECT_FALSE(val.IsFloat());
		EXPECT_FALSE(val.IsInt());
		EXPECT_FALSE(val.IsString());
		EXPECT_FALSE(val.IsTime());
	}
}
