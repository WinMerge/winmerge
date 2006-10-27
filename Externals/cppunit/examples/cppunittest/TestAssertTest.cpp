#include "CoreSuite.h"
#include "TestAssertTest.h"
#include <algorithm>

/*
 Note:
 - tests need to be added to test asserEquals() template function and
 use of assertion traits. Some check may need to be added to check
 the message content in Exception.
 - code need to be refactored with the use of a test caller that expect
 an exception.
 */


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TestAssertTest,
                                       coreSuiteName() );


TestAssertTest::TestAssertTest()
{
}


TestAssertTest::~TestAssertTest()
{
}


void 
TestAssertTest::setUp()
{
}


void 
TestAssertTest::tearDown()
{
}


void 
TestAssertTest::testAssertThrow()
{
   CPPUNIT_ASSERT_THROW( throw std::string(), std::string );

   try
   {
      CPPUNIT_ASSERT_THROW( 1234, std::string );
   }
   catch ( CPPUNIT_NS::Exception & )
   {
      return;
   }

   throw std::exception();
}


void 
TestAssertTest::testAssertNoThrow()
{
   CPPUNIT_ASSERT_NO_THROW( 1234 );

   try
   {
      CPPUNIT_ASSERT_NO_THROW( throw std::exception() );
   }
   catch ( CPPUNIT_NS::Exception & )
   {
      return;
   }
   throw std::exception();
}


void 
TestAssertTest::testAssertAssertionFail()
{
   CPPUNIT_ASSERT_ASSERTION_FAIL( throw CPPUNIT_NS::Exception() );

   try
   {
      CPPUNIT_ASSERT_ASSERTION_FAIL( 1234 );
   }
   catch ( CPPUNIT_NS::Exception & )
   {
      return;
   }

   throw std::exception();
}


void 
TestAssertTest::testAssertAssertionPass()
{
   CPPUNIT_ASSERT_ASSERTION_PASS( 1234 );

   try
   {
      CPPUNIT_ASSERT_ASSERTION_PASS( throw CPPUNIT_NS::Exception() );
   }
   catch ( CPPUNIT_NS::Exception & )
   {
      return;
   }

   throw std::exception();
}


void 
TestAssertTest::testAssert()
{
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT( true ) );
  
  CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( false ) );
}


static int foo() { return 1; }


void 
TestAssertTest::testAssertEqual()
{
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT_EQUAL( 1, 1 ) );
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT_EQUAL( 1, foo() ) );
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT_EQUAL( 12345678, 12345678 ) );

  CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT_EQUAL( 1, 2 ) );
}


void 
TestAssertTest::testAssertMessageTrue()
{
  CPPUNIT_ASSERT_ASSERTION_PASS( 
     CPPUNIT_ASSERT_MESSAGE( "This test should not failed", true ) );
}


void 
TestAssertTest::testAssertMessageFalse()
{
  bool exceptionCaught = false;
  std::string message( "This test message should not be seen" );
  try
  {
    CPPUNIT_ASSERT_MESSAGE( message, 2==3 );
  }
  catch( CPPUNIT_NS::Exception &e )
  {
    exceptionCaught = true; // ok, we were expecting an exception.
    checkMessageContains( &e, message );
  }

  CPPUNIT_ASSERT( exceptionCaught );
}


void 
TestAssertTest::testAssertDoubleEquals()
{
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.1, 1.2, 0.101 ) );
  CPPUNIT_ASSERT_ASSERTION_PASS( CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.2, 1.1, 0.101 ) );

  CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.1, 1.2, 0.09 ) );
  CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.2, 1.1, 0.09 ) );
}


void 
TestAssertTest::testFail()
{
  bool exceptionCaught = false;
  std::string failure( "FailureMessage" );
  try
  {
    CPPUNIT_FAIL( failure );
  }
  catch( CPPUNIT_NS::Exception &e )
  {
    exceptionCaught = true;
    checkMessageContains( &e, failure );
  }
  CPPUNIT_ASSERT( exceptionCaught );
}


void 
TestAssertTest::checkMessageContains( CPPUNIT_NS::Exception *e,
                                      std::string expected )
{
  std::string actual = e->what();
  CPPUNIT_ASSERT_MESSAGE( "Expected message not found: " + expected +
                          ", was: " + actual,
      std::search( actual.begin(), actual.end(), 
                   expected.begin(), expected.end() ) != actual.end() );
}
