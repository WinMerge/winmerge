/** 
 * @file  LineFilterHelper.cpp
 *
 * @brief Implementation file for LineFilterHelper class
 */

#include "pch.h"
#include "LineFilterHelper.h"
#include "UnicodeString.h"
#include "DiffItem.h"

LineFilterHelper::LineFilterHelper()
{
}

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
	SetStringOrExpression(AddToExpression(m_filter, expr, op));
}

String LineFilterHelper::RemovePrefix(const String& filter)
{
	if (filter.find(_T("le:")) != String::npos)
		return filter.substr(3);
	return filter;
}

String LineFilterHelper::AddToExpression(const String& filter, const String& expr, const String& op)
{
	String result = RemovePrefix(ConvertToLineContainsExpression(filter));
	auto [filterDirectives, filterExpr] = FilterExpression::SplitDirectivesAndExpr(result);
	auto [exprDirectives, exprBody] = FilterExpression::SplitDirectivesAndExpr(expr);
	String mergedDirectives = FilterExpression::MergeDirectives(filterDirectives, exprDirectives);
	result += (result.empty() ? _T("") : _T(" ") + op + _T(" ")) + exprBody;
	return BuildFilter((mergedDirectives.empty() ? _T("") : mergedDirectives + _T(" ")), result);
}

String LineFilterHelper::Quote(const String& text)
{
	String tmp = text;
	strutils::replace(tmp, _T("\""), _T("\"\""));
	return _T("\"") + tmp + _T("\"");
}

String LineFilterHelper::ConvertToLineContainsExpression(const String& text)
{
	if (text.empty() || text.find(_T("le:")) == 0)
		return text;
	return _T("le:Line contains ") + Quote(text);
}
