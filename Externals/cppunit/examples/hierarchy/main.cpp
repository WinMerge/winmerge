#include <cppunit/ui/text/TestRunner.h>

#include "BoardGame.h"
#include "Chess.h"
#include "BoardGameTest.h"
#include "ChessTest.h"



int 
main(int argc, char** argv)
{
  CPPUNIT_NS::TextUi::TestRunner runner;

  runner.addTest( BoardGameTest<BoardGame>::suite() );
  runner.addTest( ChessTest<Chess>::suite() );

  bool wasSucessful = runner.run();

  // should be:
  //  return wasSuccessful ? 0 : 1;
  return 0;
}
