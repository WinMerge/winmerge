#pragma once

#include "UnicodeString.h"
#include <functional>

class Logger
{
public:
	enum class LogLevel : int { ERROR, WARN, INFO };

	static Logger& Get() { static Logger logger; return logger; }
	template<class T> void Info(const T& msg) { if (m_level >= LogLevel::INFO) Log(LogLevel::INFO, msg); }
	template<class T> void Warn(const T& msg) { if (m_level >= LogLevel::WARN) Log(LogLevel::WARN, msg); }
	template<class T> void Error(const T& msg) { if (m_level >= LogLevel::ERROR) Log(LogLevel::ERROR, msg); }
	void Log(LogLevel level, const String& msg);
	void Log(LogLevel level, const std::string& msg);
	void SetOutputFunction(std::function<void(LogLevel level, const String& msg)> func) { m_func = func; }

private:
	LogLevel m_level;
	std::function<void(LogLevel level, const String& msg)> m_func;
};

namespace RootLogger
{
	template<class T> static void Info(const T& msg) { Logger::Get().Info(msg); }
	template<class T> static void Warn(const T& msg) { Logger::Get().Warn(msg); }
	template<class T> static void Error(const T& msg) { Logger::Get().Error(msg); }
}
