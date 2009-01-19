#include <gtest/gtest.h>
#include <windows.h>
#include "paths.h"

namespace
{
	// The fixture for testing class Foo.
	class PathTest :	public testing::Test {
	protected:
		// You can remove	any	or all of the following	functions if its body
		// is	empty.

		PathTest()	{
			// You can do set-up work for each test	here.
		}

		virtual ~PathTest() {
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test,	you	can	define the following methods:

		virtual void SetUp() {
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown() {
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests	in the test	case for Foo.
	};

	// Tests that the Foo::Bar() method	does Abc.
	TEST_F(PathTest, EndsWithSlash)
	{
		const TCHAR input_filepath[] = "this/package/testdata/myinputfile.dat";
		//const string output_filepath = "this/package/testdata/myoutputfile.dat";
		//Foo f;
		EXPECT_EQ(0, paths_EndsWithSlash(input_filepath));
	}

	// Tests that Foo does Xyz.
	//TEST_F(FooTest,	DoesXyz) {
		// Exercises the Xyz feature of Foo.
	//}

}  // namespace

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
