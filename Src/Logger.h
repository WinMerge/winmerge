#pragma once

#include "UnicodeString.h"
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

class Logger
{
public:
	enum class LogLevel : int { ERR, WARN, INFO };

	static Logger& Get() { static Logger logger; return logger; }
	template<class T> void Info(const T& msg) { if (m_level >= LogLevel::INFO) Log(LogLevel::INFO, msg); }
	template<class T> void Warn(const T& msg) { if (m_level >= LogLevel::WARN) Log(LogLevel::WARN, msg); }
	template<class T> void Error(const T& msg) { if (m_level >= LogLevel::ERR) Log(LogLevel::ERR, msg); }
	void Log(LogLevel level, const String& msg);
	void Log(LogLevel level, const std::string& msg);
	void SetLogLevel(LogLevel level) { m_level = level; }
	void SetOutputFunction(std::function<void(LogLevel level, const std::chrono::system_clock::time_point& tp, const String& msg)> func) { m_func = func; }

private:
	LogLevel m_level = LogLevel::INFO;
	std::function<void(LogLevel level, const std::chrono::system_clock::time_point& tp, const String& msg)> m_func;
};

namespace RootLogger
{
	template<class T> static void Info(const T& msg) { Logger::Get().Info(msg); }
	template<class T> static void Warn(const T& msg) { Logger::Get().Warn(msg); }
	template<class T> static void Error(const T& msg) { Logger::Get().Error(msg); }
}

struct LogMessage
{
	LogMessage(Logger::LogLevel level, const std::chrono::system_clock::time_point& tp, const String& text)
		: level(level), tp(tp), text(text) { }
	LogMessage(LogMessage&& other) noexcept
		: level(other.level), tp(other.tp), text(std::move(other.text)) { }
	String format(const String& dateTimePattern, bool milliseconds) const;
	Logger::LogLevel level;
	std::chrono::system_clock::time_point tp;
	String text;
};

