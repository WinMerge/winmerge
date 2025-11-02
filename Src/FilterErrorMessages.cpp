#include "pch.h"
#include "FilterEngine/FilterError.h"
#include "FilterEngine/FilterExpression.h"
#include "FileFilter.h"
#include "UnicodeString.h"
#include "I18n.h"
#include "unicoder.h"

String GetFilterErrorMessage(FilterErrorCode code)
{
	switch (code)
	{
	case FILTER_ERROR_NO_ERROR:
		return _("No error");
	case FILTER_ERROR_EMPTY_EXPRESSION:
		return _("Filter expression is empty");
	case FILTER_ERROR_UNKNOWN_CHAR:
		return _("Unknown character in filter expression");
	case FILTER_ERROR_UNTERMINATED_STRING:
		return _("Unterminated string literal");
	case FILTER_ERROR_SYNTAX_ERROR:
		return _("Syntax error in filter expression");
	case FILTER_ERROR_PARSE_FAILURE:
		return _("Failed to parse filter expression");
	case FILTER_ERROR_INVALID_LITERAL:
		return _("Invalid literal value");
	case FILTER_ERROR_INVALID_ARGUMENT_COUNT:
		return _("Invalid number of arguments");
	case FILTER_ERROR_INVALID_REGULAR_EXPRESSION:
		return _("Invalid regular expression");
	case FILTER_ERROR_UNDEFINED_IDENTIFIER:
		return _("Undefined identifier");
	case FILTER_ERROR_FILTER_NAME_NOT_FOUND:
		return _("Filter name not found");
	case FILTER_ERROR_DIVIDE_BY_ZERO:
		return _("Division by zero in filter expression");
	case FILTER_ERROR_INVALID_PROPERTY_NAME:
		return _("Invalid property name");
	default:
		return _("Unknown error");
	}
}

String FormatFilterErrorSummary(const FileFilterErrorInfo& fei)
{
	if (fei.errorCode == FILTER_ERROR_NO_ERROR)
		return _T("");
	String msg;
	msg = GetFilterErrorMessage(fei.errorCode);
	if (fei.errorPosition >= 0)
		msg += _T(" ") + _("at position") + _T(" ") + strutils::to_str(fei.errorPosition + 1);
	msg += _T(": ") + ucr::toTString(fei.srcText);
	if (!fei.context.empty())
		msg += _T(" (") + fei.context + _T(")");
	return msg;
}

String FormatFilterErrorSummary(const FilterExpression& fe)
{
	if (fe.errorCode == FILTER_ERROR_NO_ERROR)
		return _T("");
	String msg;
	msg = GetFilterErrorMessage(fe.errorCode);
	if (fe.errorPosition >= 0)
		msg += _T(" ") + _("at position") + _T(" ") + strutils::to_str(fe.errorPosition + 1);
	msg += _T(": ") + ucr::toTString(fe.expression);
	return msg;
}