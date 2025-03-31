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


String LogMessage::format(const String& dateTimePattern, bool milliseconds) const
{
	auto timeT = std::chrono::system_clock::to_time_t(tp);
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

	std::tm tm;
	localtime_s(&tm, &timeT);

	std::array<tchar_t, 64> dateTimeBuffer{};
	tc::tcsftime(dateTimeBuffer.data(), dateTimeBuffer.size(), dateTimePattern.c_str(), &tm);

	String dateTimeStr = dateTimeBuffer.data();
	if (milliseconds)
		dateTimeStr += strutils::format(_T(".%03d"), millis.count());

	const tchar_t* levelStr = (level == Logger::LogLevel::ERR) ? _T("ERROR") :
		(level == Logger::LogLevel::WARN) ? _T("WARN") : _T("INFO");

	String msg = text;
	strutils::replace(msg, _T("\n"), _T("\n  "));

	return dateTimeStr + _T(" [") + levelStr + _T("] ") + msg;
}
