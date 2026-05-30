/** 
 * @file  LineFilterHelper.h
 *
 * @brief Declaration file for LineFilterHelper
 */
#pragma once

#include "UnicodeString.h"
#include "FilterEngine/FilterExpression.h"

class LineFilterHelper
{
public:
	LineFilterHelper();
	~LineFilterHelper();

	const String& GetStringOrExpression() const { return m_filter; }
	bool SetStringOrExpression(const String& filter);
	FilterExpression& GetFilterExpression() { return m_expr; }
	void AddToExpression(const String& expr, const String& op);
	static String AddToExpression(const String& filter, const String& expr, const String& op);
	static String RemoveLePrefix(const String& filter);
	static String Quote(const String& text);
	static String ConvertToLineContainsExpression(const String& text);
	// Helper: Build "le:" prefixed result with optional directives
	static String BuildLeFilter(const String& directives, const String& expr) {
		return _T("le:") + (directives.empty() ? _T("") : strutils::trim_ws(directives) + _T(" ")) + expr;
	}
private:
	FilterExpression m_expr;
	String m_filter;
};
