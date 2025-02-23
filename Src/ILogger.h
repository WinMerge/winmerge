#pragma once

#include "UnicodeString.h"

class ILogger {
public:
	virtual ~ILogger() = default;

	virtual void info(const String& message) = 0;
	virtual void warn(const String& message) = 0;
	virtual void error(const String& message) = 0;
	virtual void debug(const String& message) = 0;
};
