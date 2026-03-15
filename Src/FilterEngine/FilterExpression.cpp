/**
 * @file  FilterExpression.cpp
 *
 * @brief Filter engine implementation.
 */
#include "pch.h"
#include "FilterExpression.h"
#include "FilterExpressionNodes.h"
#include "FilterLexer.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <Poco/LocalDateTime.h>
#include <algorithm>
#include <cctype>

extern void Parse(void* yyp, int yymajor, YYSTYPE yyminor, FilterExpression* pCtx);
extern void* ParseAlloc(void* (*mallocProc)(size_t));
extern void ParseFree(void* yyp, void (*freeProc)(void*));

void YYSTYPEDestructor(YYSTYPE& yystype)
{
	if (yystype.nodeList)
	{
		for (auto& node : *yystype.nodeList)
			delete node;
		delete yystype.nodeList;
	}
	delete yystype.node;
	yystype.node = nullptr;
	yystype.nodeList = nullptr;
}

FilterExpression::FilterExpression()
{
}

FilterExpression::FilterExpression(const FilterExpression& other)
	: optimize(other.optimize)
	, caseSensitive(other.caseSensitive)
	, name(other.name)
	, ctxt(other.ctxt)
	, now(other.now ? new Poco::Timestamp(*other.now) : nullptr)
	, today(other.today ? new Poco::Timestamp(*other.today) : nullptr)
	, expression(other.expression)
{
	Parse(expression);
}

FilterExpression::FilterExpression(const std::string& expression)
{
	Parse(expression);
}

FilterExpression::~FilterExpression()
{
	Clear();
}

void FilterExpression::Clear()
{
	now.reset();
	today.reset();
	rootNode.reset();
	errorCode = FILTER_ERROR_NO_ERROR;
	errorPosition = -1;
}

template <typename Func>
void traverseExprTree(const ExprNode* node, Func&& func)
{
	if (!node) return;

	func(node);

	if (const auto node2 = dynamic_cast<const NotNode*>(node))
	{
		traverseExprTree(node2->right, func);
	}
	else if (const auto node3 = dynamic_cast<const NegateNode*>(node))
	{
		traverseExprTree(node3->right, func);
	}
	else if (const auto node4 = dynamic_cast<const OrNode*>(node))
	{
		traverseExprTree(node4->left, func);
		traverseExprTree(node4->right, func);
	}
	else if (const auto node5 = dynamic_cast<const AndNode*>(node))
	{
		traverseExprTree(node5->left, func);
		traverseExprTree(node5->right, func);
	}
	else if (const auto node6 = dynamic_cast<const BinaryOpNode*>(node))
	{
		traverseExprTree(node6->left, func);
		traverseExprTree(node6->right, func);
	}
	else if (const auto funcNode = dynamic_cast<const FunctionNode*>(node))
	{
		for (const auto& arg : *funcNode->args)
			traverseExprTree(arg, func);
	}
}

static void collectPropertyNames(const ExprNode* root, std::vector<std::string>& names)
{
	traverseExprTree(root, [&](const ExprNode* node)
		{
			const auto funcNode = dynamic_cast<const FunctionNode*>(node);
			if (!funcNode) return;

			const auto& fn = funcNode->functionName;
			if (fn == "prop" || fn == "leftprop" || fn == "middleprop" || fn == "rightprop")
			{
				if (funcNode->args && funcNode->args->size() == 1)
				{
					if (const auto strLit = dynamic_cast<const StringLiteral*>((*funcNode->args)[0]))
					{
						if (std::find(names.begin(), names.end(), strLit->value) == names.end())
							names.push_back(strLit->value);
					}
				}
			}
		});
}

std::vector<std::string> FilterExpression::GetPropertyNames() const
{
	std::vector<std::string> names;
	if (rootNode)
		collectPropertyNames(rootNode.get(), names);
	return names;
}

void FilterExpression::UpdateTimestamp()
{
	now.reset(new Poco::Timestamp());
	Poco::LocalDateTime ldt(*now);
	Poco::LocalDateTime midnight(ldt.year(), ldt.month(), ldt.day(), 0, 0, 0, 0, 0);
	today.reset(new Poco::Timestamp(midnight.utc().timestamp()));
}

static int getErrorPosition(const FilterExpression* pCtx, const FilterLexer& lexer)
{
	if (pCtx->errorCode == FILTER_ERROR_UNDEFINED_IDENTIFIER)
	{
		return static_cast<int>(lexer.yycursor - strlen(lexer.yylval.string) - pCtx->expression.c_str());
	}
	return static_cast<int>(lexer.yycursor - pCtx->expression.c_str());
}

bool FilterExpression::Parse()
{
	Clear();
	UpdateTimestamp();
	if (expression.empty())
	{
		errorCode = FILTER_ERROR_EMPTY_EXPRESSION;
		errorPosition = 0;
		return false;
	}
	FilterLexer lexer(expression);
	void* prs = ParseAlloc(malloc);
	int token;
	FilterErrorCode firstError = FILTER_ERROR_NO_ERROR;
	while ((token = lexer.yylex()) != 0)
	{
		if (token < 0)
		{
			firstError = static_cast<FilterErrorCode>(-token);
			errorPosition = getErrorPosition(this, lexer);
			break;
		}
		::Parse(prs, token, lexer.yylval, this);
		if (errorCode != 0)
		{
			firstError = errorCode;
			errorPosition = getErrorPosition(this, lexer);
			break;
		}
		lexer.yycursor = lexer.YYCURSOR;
	}
	::Parse(prs, 0, lexer.yylval, this);
	if (firstError == 0 && errorCode != 0)
	{
		firstError = errorCode;
		errorPosition = getErrorPosition(this, lexer);
	}
	::ParseFree(prs, free);
	if (firstError != 0)
		errorCode = firstError;
	return (errorCode == 0 && rootNode != nullptr);
}

bool FilterExpression::ParseDirective(const std::string& directive)
{
	// Remove '@' prefix
	std::string dir = directive.substr(1);

	// Split into key and value at '='
	std::string key = dir;
	std::string value;
	size_t eqPos = dir.find('=');
	if (eqPos != std::string::npos)
	{
		key = dir.substr(0, eqPos);
		value = dir.substr(eqPos + 1);
	}

	// Convert key to lowercase for case-insensitive comparison
	std::string keyLower = key;
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	// Parse name directive
	if (keyLower == "name")
	{
		if (value.empty())
			return false;

		// Check if the value is quoted
		if (value[0] == '"')
		{
			size_t endQuote = value.find('"', 1);
			if (endQuote == std::string::npos)
				return false; // Missing closing quote
			name = value.substr(1, endQuote - 1);
		}
		else
		{
			name = value;
		}
		return true;
	}

	// Helper for flag-based directives (no values allowed)
	auto setFlagDirective = [&](const std::initializer_list<const char*>& keys, bool& flag, bool flagValue) -> bool
	{
		for (const char* k : keys)
		{
			if (keyLower == k)
			{
				if (!value.empty())
					return false; // No values allowed
				flag = flagValue;
				return true;
			}
		}
		return false;
	};

	// Parse flag-based directives
	if (setFlagDirective({"cs", "casesensitive"}, caseSensitive, true)) return true;
	if (setFlagDirective({"ci", "caseinsensitive"}, caseSensitive, false)) return true;
	if (setFlagDirective({"optimize", "opt"}, optimize, true)) return true;
	if (setFlagDirective({"nooptimize", "noopt"}, optimize, false)) return true;

	return false; // Unknown directive
}

bool FilterExpression::ParseAllDirectives(const std::string& expressionStr, std::string& actualExpression)
{
	actualExpression = expressionStr;
	size_t pos = 0;

	// Process multiple directives
	while (true)
	{
		// Skip leading whitespace
		while (pos < actualExpression.size() && 
			   std::isspace(static_cast<unsigned char>(actualExpression[pos])))
			++pos;

		// Check if this is a directive
		if (pos >= actualExpression.size() || actualExpression[pos] != '@')
			break;

		size_t startPos = pos;
		++pos; // Skip '@'

		// Find the end of the directive (whitespace or end of string)
		// Handle quoted strings within directive (e.g., @name="abc def")
		size_t endPos = pos;
		bool inQuote = false;
		while (endPos < actualExpression.size())
		{
			char ch = actualExpression[endPos];
			if (ch == '"')
				inQuote = !inQuote;
			else if (!inQuote && std::isspace(static_cast<unsigned char>(ch)))
				break;
			++endPos;
		}

		std::string directive = actualExpression.substr(startPos, endPos - startPos);

		// Parse the directive
		if (!ParseDirective(directive))
		{
			errorCode = FILTER_ERROR_INVALID_DIRECTIVE;
			errorPosition = static_cast<int>(startPos);
			errorMessage = "Invalid directive: " + directive;
			return false;
		}

		// Move to the end of this directive
		pos = endPos;
	}

	// Skip any remaining whitespace after directives
	while (pos < actualExpression.size() && 
		   std::isspace(static_cast<unsigned char>(actualExpression[pos])))
		++pos;

	// Extract the actual expression (after all directives)
	actualExpression = actualExpression.substr(pos);

	return true;
}

bool FilterExpression::Parse(const std::string& expressionStr)
{
	expression = expressionStr;

	std::string actualExpression;
	if (!ParseAllDirectives(expressionStr, actualExpression))
		return false;

	// Update the expression to parse
	expression = actualExpression;

	return Parse();
}

static bool ContainsTrue(const ValueType& value)
{
	if (const auto boolVal = std::get_if<bool>(&value))
		return *boolVal;

	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&value))
	{
		for (const auto& item : *arrayVal->get())
		{
			if (ContainsTrue(item.value))
				return true;
		}
	}
	return false;
}

bool FilterExpression::Evaluate(const DIFFITEM& di)
{
	try
	{
		const auto result = rootNode->Evaluate(di);
		return ContainsTrue(result);
	}
	catch (const Poco::RegularExpressionException& e)
	{
		errorCode = FILTER_ERROR_INVALID_REGULAR_EXPRESSION;
		errorPosition = -1;
		errorMessage = e.message();
		if (logger)
			logger(0, "FilterExpression evaluation error: " + errorMessage);
		return false;
	}
	catch (const std::exception& e)
	{
		errorCode = FILTER_ERROR_EVALUATION_FAILED;
		errorPosition = -1;
		errorMessage = e.what();
		if (logger)
			logger(0, "FilterExpression evaluation error: " + errorMessage);
		return false;
	}
}

static std::vector<String> ConvertStringArray(const ValueType& value)
{
	std::vector<String> result;
	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&value))
	{
		for (const auto& item : *arrayVal->get())
			result.push_back(ucr::toTString(ToStringValue(item.value)));
	}
	else
	{
		result.push_back(ucr::toTString(ToStringValue(value)));
	}
	return result;
}

std::vector<String> FilterExpression::EvaluateKeys(const DIFFITEM& di)
{
	try
	{
		const auto result = rootNode->Evaluate(di);
		return ConvertStringArray(result);
	}
	catch (const Poco::RegularExpressionException& e)
	{
		errorCode = FILTER_ERROR_INVALID_REGULAR_EXPRESSION;
		errorPosition = -1;
		errorMessage = e.message();
		if (logger)
			logger(0, "FilterExpression evaluation error: " + errorMessage);
		return std::vector<String>();
	}
	catch (const std::exception& e)
	{
		errorCode = FILTER_ERROR_EVALUATION_FAILED;
		errorPosition = -1;
		errorMessage = e.what();
		if (logger)
			logger(0, "FilterExpression evaluation error: " + errorMessage);
		return std::vector<String>();
	}
}

bool FilterExpression::HasCaseSensitiveDirective(const String& expression)
{
	String directives = ExtractDirectivesPrefix(expression);
	if (directives.empty())
		return false;

	// Check for case-sensitive directives in directives part only
	return (directives.find(_T("@cs")) != String::npos || 
			directives.find(_T("@caseSensitive")) != String::npos || 
			directives.find(_T("@casesensitive")) != String::npos);
}

String FilterExpression::AddCaseSensitiveDirective(const String& expression)
{
	if (HasCaseSensitiveDirective(expression))
		return expression;

	String directives = ExtractDirectivesPrefix(expression);
	String body = RemoveAllDirectives(expression);

	if (body.empty())
		return _T("@cs ");

	if (directives.empty())
		return _T("@cs ") + body;
	else
		return directives + _T(" @cs ") + body;
}

String FilterExpression::RemoveCaseSensitiveDirective(const String& expression)
{
	String directives = ExtractDirectivesPrefix(expression);
	String body = RemoveAllDirectives(expression);

	if (directives.empty())
		return body;

	// Remove case-sensitive directives from the directives string
	const tchar_t* csDirectives[] = { 
		_T("@cs "), _T("@cs"),
		_T("@caseSensitive "), _T("@caseSensitive"),
		_T("@casesensitive "), _T("@casesensitive")
	};

	for (const tchar_t* directive : csDirectives)
	{
		size_t pos = directives.find(directive);
		if (pos != String::npos)
		{
			directives.erase(pos, String(directive).length());
			directives = strutils::trim_ws(directives);
			break;
		}
	}

	// Reconstruct expression
	if (directives.empty())
		return body;
	else
		return directives + _T(" ") + body;
}

static size_t FindDirectivesEnd(const String& expression)
{
	String expr = strutils::trim_ws(expression);
	size_t pos = 0;

	// Skip all leading directives
	while (true)
	{
		// Skip leading whitespace
		while (pos < expr.size() && tc::istspace(expr[pos]))
			++pos;

		// Check if this is a directive
		if (pos >= expr.size() || expr[pos] != _T('@'))
			break;

		++pos; // Skip '@'

		// Find the end of the directive (whitespace or end of string)
		// Handle quoted strings within directive (e.g., @name="abc def")
		size_t endPos = pos;
		bool inQuote = false;
		while (endPos < expr.size())
		{
			tchar_t ch = expr[endPos];
			if (ch == _T('"'))
				inQuote = !inQuote;
			else if (!inQuote && tc::istspace(ch))
				break;
			++endPos;
		}

		// Move to the end of this directive
		pos = endPos;
	}

	return pos;
}

String FilterExpression::ExtractDirectivesPrefix(const String& expression)
{
	String expr = strutils::trim_ws(expression);
	size_t endPos = FindDirectivesEnd(expr);

	if (endPos == 0)
		return _T("");

	return strutils::trim_ws(expr.substr(0, endPos));
}

String FilterExpression::RemoveAllDirectives(const String& expression)
{
	String expr = strutils::trim_ws(expression);
	size_t pos = FindDirectivesEnd(expr);

	// Skip any remaining whitespace after directives
	while (pos < expr.size() && tc::istspace(expr[pos]))
		++pos;

	// Return the expression without directives
	return pos < expr.size() ? expr.substr(pos) : _T("");
}

