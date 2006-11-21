// StringDifferencing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StringDifferencing.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/ui/text/TestRunner.h>

// Includes for testcases
#include "TestCase1.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		/**
		 * This is the code running the tests.
		 * Tests are found automatically once they are created with
		 * CppUnit's macros, you don't need to run single tests here.
		 */
		CPPUNIT_NS::TestResult controller;

		// Add a listener that colllects test result
		CPPUNIT_NS::TestResultCollector result;
		controller.addListener( &result );

		// Add a listener that print dots as test run.
		CPPUNIT_NS::BriefTestProgressListener progress;
		controller.addListener( &progress );

		// Add the top suite to the test runner
		CPPUNIT_NS::TestRunner runner;
		runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
		runner.run( controller );

		// Print test in a compiler compatible format.
		CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
		outputter.write();
	}

	getc(stdin);

	return nRetCode;
}


