#pragma once

#include "ILogger.h"

namespace Poco { class Logger; }

class PocoLogger : public ILogger
{
public:
	PocoLogger();
	void info(const std::string& message) override;
	void warn(const std::string& message) override;
	void error(const std::string& message) override;
	void debug(const std::string& message) override;

private:
	Poco::Logger* logger;
};
