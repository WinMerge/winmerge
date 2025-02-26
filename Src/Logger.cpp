#include "pch.h"
#include "Logger.h"
#include "unicoder.h"
#include <chrono>

void Logger::Log(LogLevel level, const String& msg)
{
	if (m_func)
		m_func(level, std::chrono::system_clock::now(), msg);
}

void Logger::Log(LogLevel level, const std::string& msg)
{
	if (m_func)
		m_func(level, std::chrono::system_clock::now(), ucr::toTString(msg));
}


String LogMessage::format() const
{
	auto timeT = std::chrono::system_clock::to_time_t(tp);
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

	std::tm tm;
	localtime_s(&tm, &timeT);

	std::basic_ostringstream<tchar_t> oss;
	oss << std::put_time(&tm, _T("%Y-%m-%dT%H:%M:%S"))
		<< '.' << std::setw(3) << std::setfill(_T('0')) << millis.count();

	const tchar_t* levelStr = (level == Logger::LogLevel::ERR) ? _T("ERROR") :
		(level == Logger::LogLevel::WARN) ? _T("WARN") : _T("INFO");

	oss << _T(" [") << levelStr << _T("] ") << text << std::endl;

	return oss.str();
}
