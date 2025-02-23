#include "pch.h"
#include "Logger.h"
#include "unicoder.h"

void Logger::Log(LogLevel level, const String& msg)
{
	if (m_func)
		m_func(level, msg);
}

void Logger::Log(LogLevel level, const std::string& msg)
{
	if (m_func)
		m_func(level, ucr::toTString(msg));
}
