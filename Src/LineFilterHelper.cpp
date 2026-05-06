/** 
 * @file  LineFilterHelper.cpp
 *
 * @brief Implementation file for LineFilterHelper class
 */

#include "pch.h"
#include "LineFilterHelper.h"
#include "UnicodeString.h"
#include "DiffItem.h"

/** 
 * @brief Constructor, creates new filtermanager.
 */
LineFilterHelper::LineFilterHelper()
{
}

/** 
 * @brief Destructor, deletes filtermanager.
 */
LineFilterHelper::~LineFilterHelper() = default;


bool LineFilterHelper::SetStringOrExpression(const String& filter)
{
	m_filter = filter;
	std::string expr;
	if (filter.find(_T("le:")) != String::npos)
		expr = ucr::toUTF8(filter.substr(3));
	else
		expr = "Line contains " + ucr::toUTF8(Quote(filter));
	m_expr.diritem = false;
	return m_expr.Parse(expr);
}

void LineFilterHelper::AddToExpression(const String& expr, const String& op)
{
	String filter = m_filter;
	if (filter.find(_T("le:")) == String::npos)
	{
		if (filter.empty())
			filter = _T("le:");
		else
			filter = _T("le:Line contains ") + Quote(filter);
	}
	filter += (filter.length() == 3 ? _T("") : _T(" ") + op + _T(" ")) + expr;
	SetStringOrExpression(filter);
}

String LineFilterHelper::Quote(const String& text)
{
	String tmp = text;
	strutils::replace(tmp, _T("\""), _T("\"\""));
	return _T("\"") + tmp + _T("\"");
}
