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
	static String RemovePrefix(const String& filter);
	static String Quote(const String& text);
private:
	FilterExpression m_expr;
	String m_filter;
};
