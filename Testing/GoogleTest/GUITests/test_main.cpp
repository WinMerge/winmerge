#include "pch.h"
#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>

int _tmain(int argc, TCHAR **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
