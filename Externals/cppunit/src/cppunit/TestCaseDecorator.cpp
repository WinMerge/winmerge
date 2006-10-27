#include <cppunit/extensions/TestCaseDecorator.h>

CPPUNIT_NS_BEGIN


TestCaseDecorator::TestCaseDecorator( TestCase *test )
    : m_test( test )
    , TestCase( test->getName() )
{ 
}


TestCaseDecorator::~TestCaseDecorator()
{
  delete m_test;
}


std::string 
TestCaseDecorator::getName() const
{ 
  return m_test->getName(); 
}


void 
TestCaseDecorator::setUp()
{
  m_test->setUp();
}


void 
TestCaseDecorator::tearDown()
{
  m_test->tearDown();
}


void 
TestCaseDecorator::runTest()
{
  m_test->runTest();
}


CPPUNIT_NS_END
