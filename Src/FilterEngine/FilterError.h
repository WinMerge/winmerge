/**
 * @file FilterError.h
 * @brief Defines error codes for the filter engine.
 */
#pragma once

#include <stdexcept>

enum FilterErrorCode
{
	FILTER_ERROR_NO_ERROR = 0,
	FILTER_ERROR_UNKNOWN_CHAR = 1, // LEXER_ERR_UNKNOWN_CHAR(-1)
	FILTER_ERROR_UNTERMINATED_STRING = 2, // LEXER_ERR_UNTERMINATED_STRING(-2)
	FILTER_ERROR_EMPTY_EXPRESSION = 3,
	FILTER_ERROR_SYNTAX_ERROR = 4,
	FILTER_ERROR_PARSE_FAILURE = 5,
	FILTER_ERROR_INVALID_LITERAL = 6,
	FILTER_ERROR_INVALID_ARGUMENT_COUNT = 7,
	FILTER_ERROR_INVALID_REGULAR_EXPRESSION = 8,
	FILTER_ERROR_UNDEFINED_IDENTIFIER = 9,
	FILTER_ERROR_FILTER_NAME_NOT_FOUND = 10,
	FILTER_ERROR_EVALUATION_FAILED = 11,
	FILTER_ERROR_DIVIDE_BY_ZERO = 12,
	FILTER_ERROR_INVALID_PROPERTY_NAME = 13,
	FILTER_ERROR_INVALID_DIRECTIVE = 14,
	FILTER_ERROR_INVALID_ARGUMENT = 15,
};

class InvalidPropertyNameError : public std::invalid_argument
{
public:
	explicit InvalidPropertyNameError(const std::string& name)
		: std::invalid_argument("unknown property name: " + name), propertyName(name) {}
	std::string propertyName;
};
class DivisionByZeroError : public std::invalid_argument
{
	using std::invalid_argument::invalid_argument;
};
class InvalidArgumentCountError : public std::invalid_argument
{
	using std::invalid_argument::invalid_argument;
};
class InvalidArgumentError : public std::invalid_argument
{
	using std::invalid_argument::invalid_argument;
};
