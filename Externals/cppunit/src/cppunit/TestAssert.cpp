#if HAVE_CMATH
#   include <cmath>
#else
#   include <math.h>
#endif

#include <cppunit/TestAssert.h>


CPPUNIT_NS_BEGIN


void 
assertDoubleEquals( double expected,
                    double actual,
                    double delta,
                    SourceLine sourceLine,
                    const std::string &message )
{
  AdditionalMessage msg( "Delta   : " + 
                         assertion_traits<double>::toString(delta) );
  msg.addDetail( AdditionalMessage(message) );

  Asserter::failNotEqualIf( fabs( expected - actual ) > delta,
                            assertion_traits<double>::toString(expected),
                            assertion_traits<double>::toString(actual),
                            sourceLine, 
                            msg, 
                            "double equality assertion failed" );
}


CPPUNIT_NS_END
