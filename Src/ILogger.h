#pragma once

#include <string>

class ILogger {
public:
	virtual ~ILogger() = default;

	virtual void info(const std::string& message) = 0;
	virtual void warn(const std::string& message) = 0;
	virtual void error(const std::string& message) = 0;
	virtual void debug(const std::string& message) = 0;
};
