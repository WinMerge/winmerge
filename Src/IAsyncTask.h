#pragma once

#include "UnicodeString.h"
#include <atomic>

struct IAsyncTask
{
	virtual String RunAndGetMessage(std::atomic_bool& cancelFlag) = 0;
};
