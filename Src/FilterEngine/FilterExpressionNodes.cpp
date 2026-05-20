/**
 * @file  FilterExpressionNodes.cpp
 *
 * @brief This file implements the filter expression evaluation logic for the FilterExpression.
 */
#include "pch.h"
#include "FilterExpressionNodes.h"
#include "FilterExpression.h"
#include "FileContentRef.h"
#include "ReplaceList.h"
#include "FolderStats.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "ILineDataProvider.h"
#include "paths.h"
#include "unicoder.h"
#include "FileTransform.h"
#include <string>
#include <variant>
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>
#include <Poco/LocalDateTime.h>
#include <Poco/Timezone.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/String.h>
#include <Poco/Glob.h>

static std::optional<bool> evalAsBool(const ValueType& val)
{
	auto boolVal = std::get_if<bool>(&val);
	if (boolVal) return *boolVal;

	auto ary = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&val);
	if (ary && *ary)
	{
		const auto& vec = *ary->get();
		return std::any_of(vec.begin(), vec.end(), [](const ValueType2& item) {
			const auto boolVal = std::get_if<bool>(&item.value);
			return boolVal && *boolVal;
			});
	}
	return std::nullopt;
}

static std::optional<std::string> getAsString(const ValueType& val)
{
	if (auto str = std::get_if<std::string>(&val))
		return *str;
	if (std::holds_alternative<std::monostate>(val))
		return std::nullopt;
	return ToStringValue(val);
}

static std::string escapeRegex(const std::string& str)
{
	std::string result;
	result.reserve(str.size());
	for (char c : str)
	{
		if (c == '.' || c == '^' || c == '$' || c == '*' || c == '+' || c == '?' ||
			c == '{' || c == '}' || c == '[' || c == ']' || c == '\\' || c == '|' ||
			c == '(' || c == ')')
		{
			result += '\\';
		}
		result += c;
	}
	return result;
}

ExprNode* OrNode::Optimize()
{
	if (!left || !right)
		return this;
	left = left->Optimize();
	right = right->Optimize();
	auto lBool = dynamic_cast<BoolLiteral*>(left);
	auto rBool = dynamic_cast<BoolLiteral*>(right);
	if (lBool && rBool)
	{
		const bool result = lBool->value || rBool->value;
		delete this;
		return new BoolLiteral(result);
	}
	return this;
}

ValueType OrNode::Evaluate(const FilterEvalContext& ectxt) const
{
	auto lval = left->Evaluate(ectxt);
	auto lbool = evalAsBool(lval);
	if (lbool && *lbool) return true;

	auto rval = right->Evaluate(ectxt);
	auto rbool = evalAsBool(rval);
	if (rbool && *rbool) return true;

	if (lbool || rbool) return false;
	return std::monostate{};
}

ExprNode* AndNode::Optimize()
{
	if (!left || !right)
		return this;
	left = left->Optimize();
	right = right->Optimize();
	auto lBool = dynamic_cast<BoolLiteral*>(left);
	auto rBool = dynamic_cast<BoolLiteral*>(right);
	if (lBool && rBool)
	{
		const bool result = lBool->value && rBool->value;
		delete this;
		return new BoolLiteral(result);
	}
	return this;
}

ValueType AndNode::Evaluate(const FilterEvalContext& ectxt) const
{
	auto lval = left->Evaluate(ectxt);
	auto lbool = evalAsBool(lval);
	if (!lbool) return std::monostate{};
	if (!*lbool) return false;

	auto rval = right->Evaluate(ectxt);
	auto rbool = evalAsBool(rval);
	if (!rbool) return std::monostate{};
	if (!*rbool) return false;
	return true;
}

ExprNode* NotNode::Optimize()
{
	if (!right)
		return this;
	right = right->Optimize();
	auto boolVal = dynamic_cast<BoolLiteral*>(right);
	if (boolVal)
	{
		const bool result = !boolVal->value;
		delete this;
		return new BoolLiteral(result);
	}
	return this;
}

ValueType NotNode::Evaluate(const FilterEvalContext& ectxt) const
{
	auto val = right->Evaluate(ectxt);
	auto boolVal = evalAsBool(val);
	if (!boolVal) return std::monostate{};
	return !*boolVal;
}

static std::optional<int64_t> getConstIntValue(const ExprNode* node)
{
	if (auto intNode = dynamic_cast<const IntLiteral*>(node))
		return intNode->value;
	if (auto sizeNode = dynamic_cast<const SizeLiteral*>(node))
		return sizeNode->value;
	if (auto durationNode = dynamic_cast<const DurationLiteral*>(node))
		return durationNode->value;
	if (auto versionNode = dynamic_cast<const VersionLiteral*>(node))
		return versionNode->value;
	return std::nullopt;
}

/**
 * @brief Attempts to fold constants in an expression tree.
 *
 * This function evaluates constant expressions involving two nodes and an operator.
 * If both nodes represent constant values, it computes the result and returns a new
 * literal node representing the folded constant. Otherwise, it returns nullptr.
 *
 * @param left The left-hand side expression node.
 * @param op The operator to apply (e.g., TK_PLUS, TK_EQ).
 * @param right The right-hand side expression node.
 * @return A new ExprNode representing the folded constant, or nullptr if folding is not possible.
 */
static ExprNode* TryFoldConstants(ExprNode* left, int op, ExprNode* right, bool caseSensitive)
{
	// If both nodes are constants, attempt to fold them.
	auto lDouble = dynamic_cast<DoubleLiteral*>(left);
	auto rDouble = dynamic_cast<DoubleLiteral*>(right);
	if (lDouble && rDouble)
	{
		// Handle comparison operators (e.g., ==, !=, <, <=, >, >=).
		if (op >= TK_EQ && op <= TK_GE)
		{
			bool result;
			switch (op)
			{
			case TK_EQ: result = lDouble->value == rDouble->value; break;
			case TK_NE: result = lDouble->value != rDouble->value; break;
			case TK_LT: result = lDouble->value < rDouble->value; break;
			case TK_LE: result = lDouble->value <= rDouble->value; break;
			case TK_GT: result = lDouble->value > rDouble->value; break;
			case TK_GE: result = lDouble->value >= rDouble->value; break;
			default: return nullptr; // Invalid operator.
			}
			// Return a boolean literal representing the comparison result.
			return new BoolLiteral(result);
		}

		// Handle arithmetic operators (e.g., +, -, *, /, %).
		double result = 0.0;
		switch (op)
		{
		case TK_PLUS:  result = lDouble->value + rDouble->value; break;
		case TK_MINUS: result = lDouble->value - rDouble->value; break;
		case TK_STAR:  result = lDouble->value * rDouble->value; break;
		case TK_SLASH:
			if (rDouble->value == 0)
				throw std::invalid_argument("Division by zero is not allowed.");
			result = lDouble->value / rDouble->value;
			break;
		default: return nullptr; // Invalid operator.
		}
		// Return an double literal representing the arithmetic result.
		return new DoubleLiteral(result);
	}
	// Extract constant integer values from the left and right nodes.
	auto lInt = getConstIntValue(left);
	auto rInt = getConstIntValue(right);
	if (lInt && rInt)
	{
		// Handle comparison operators (e.g., ==, !=, <, <=, >, >=).
		if (op >= TK_EQ && op <= TK_GE)
		{
			bool result;
			switch (op)
			{
			case TK_EQ: result = *lInt == *rInt; break;
			case TK_NE: result = *lInt != *rInt; break;
			case TK_LT: result = *lInt < *rInt; break;
			case TK_LE: result = *lInt <= *rInt; break;
			case TK_GT: result = *lInt > *rInt; break;
			case TK_GE: result = *lInt >= *rInt; break;
			default: return nullptr; // Invalid operator.
			}
			// Return a boolean literal representing the comparison result.
			return new BoolLiteral(result);
		}

		// Handle arithmetic operators (e.g., +, -, *, /, %).
		int64_t result = 0;
		switch (op)
		{
		case TK_PLUS:  result = *lInt + *rInt; break;
		case TK_MINUS: result = *lInt - *rInt; break;
		case TK_STAR:  result = *lInt * *rInt; break;
		case TK_SLASH:
			if (*rInt == 0)
				throw std::invalid_argument("Division by zero is not allowed.");
			result = *lInt / *rInt;
			break;
		case TK_MOD:
			// Avoid modulo by zero.
			if (*rInt == 0)
				throw std::invalid_argument("Division by zero is not allowed.");
			result = *lInt % *rInt;
			break;
		default: return nullptr; // Invalid operator.
		}
		// Return an integer literal representing the arithmetic result.
		return new IntLiteral(result);
	}
	auto lStr = dynamic_cast<StringLiteral*>(left);
	auto rStr = dynamic_cast<StringLiteral*>(right);
	if (lStr && rStr)
	{
		if (op >= TK_EQ && op <= TK_GE)
		{
			bool result = false;
			if (!caseSensitive)
			{
				switch (op)
				{
				case TK_EQ: result = Poco::icompare(lStr->value, rStr->value) == 0; break;
				case TK_NE: result = Poco::icompare(lStr->value, rStr->value) != 0; break;
				case TK_LT: result = Poco::icompare(lStr->value, rStr->value) < 0; break;
				case TK_LE: result = Poco::icompare(lStr->value, rStr->value) <= 0; break;
				case TK_GT: result = Poco::icompare(lStr->value, rStr->value) > 0; break;
				case TK_GE: result = Poco::icompare(lStr->value, rStr->value) >= 0; break;
				}
			}
			else
			{
				switch (op)
				{
				case TK_EQ: result = lStr->value == rStr->value; break;
				case TK_NE: result = lStr->value != rStr->value; break;
				case TK_LT: result = lStr->value < rStr->value; break;
				case TK_LE: result = lStr->value <= rStr->value; break;
				case TK_GT: result = lStr->value > rStr->value; break;
				case TK_GE: result = lStr->value >= rStr->value; break;
				}
			}
			return new BoolLiteral(result);
		}

		std::string result;
		switch (op)
		{
		case TK_PLUS:  result = lStr->value + rStr->value; break;
		default: return nullptr;
		}
		return new StringLiteral(result);
	}
	auto lBool = dynamic_cast<BoolLiteral*>(left);
	auto rBool = dynamic_cast<BoolLiteral*>(right);
	if (lBool && rBool)
	{
		if (op >= TK_EQ && op <= TK_GE)
		{
			bool result = false;
			switch (op)
			{
			case TK_EQ: result = lBool->value == rBool->value; break;
			case TK_NE: result = lBool->value != rBool->value; break;
			case TK_LT: result = lBool->value < rBool->value; break;
			case TK_LE: result = lBool->value <= rBool->value; break;
			case TK_GT: result = lBool->value > rBool->value; break;
			case TK_GE: result = lBool->value >= rBool->value; break;
			default: return nullptr;
			}
			return new BoolLiteral(result);
		}

		int64_t result;
		switch (op)
		{
		case TK_PLUS: result = lBool->value + rBool->value; break;
		case TK_MINUS: result = lBool->value - rBool->value; break;
		default: return nullptr;
		}
		return new IntLiteral(result);
	}
	auto lDateTime = dynamic_cast<DateTimeLiteral*>(left);
	auto rDateTime = dynamic_cast<DateTimeLiteral*>(right);
	if (lDateTime && rDateTime)
	{
		if (op >= TK_EQ && op <= TK_GE)
		{
			bool result = false;
			switch (op)
			{
			case TK_EQ: result = lDateTime->value == rDateTime->value; break;
			case TK_NE: result = lDateTime->value != rDateTime->value; break;
			case TK_LT: result = lDateTime->value < rDateTime->value; break;
			case TK_LE: result = lDateTime->value <= rDateTime->value; break;
			case TK_GT: result = lDateTime->value > rDateTime->value; break;
			case TK_GE: result = lDateTime->value >= rDateTime->value; break;
			default: return nullptr;
			}
			return new BoolLiteral(result);
		}

		int64_t  result;
		switch (op)
		{
		case TK_MINUS: result = lDateTime->value - rDateTime->value; break;
		default: return nullptr;
		}
		return new IntLiteral(result);
	}
	if (lDateTime && rInt)
	{
		if (op == TK_PLUS) return new DateTimeLiteral(lDateTime->value + *rInt);
		if (op == TK_MINUS) return new DateTimeLiteral(lDateTime->value - *rInt);
	}
	return nullptr;
}

ExprNode* BinaryOpNode::Optimize()
{
	if (!left || !right)
		return this;
	left = left->Optimize();
	right = right->Optimize();
	if (ExprNode* folded = TryFoldConstants(left, op, right, ctxt->caseSensitive))
	{
		delete this;
		return folded;
	}
	if (op == TK_RECONTAINS || op == TK_MATCHES)
	{
		if (auto strNode = dynamic_cast<StringLiteral*>(right))
		{
			right = new RegularExpressionLiteral(strNode->value, ctxt->caseSensitive);
			delete strNode;
		}
	}
	return this;
}

static auto compute(int op, const ValueType& lval, const ValueType& rval, bool caseSensitive) -> ValueType
{
	auto lvalArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&lval);
	auto rvalArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&rval);
	if (!lvalArray && !rvalArray)
	{
		if (op == TK_PLUS && lval.index() != rval.index() &&
		   (std::holds_alternative<std::string>(lval) || std::holds_alternative<std::string>(rval)))
		{
			if (std::holds_alternative<std::monostate>(lval) || std::holds_alternative<std::monostate>(rval))
				return std::monostate{};
			return ToStringValue(lval) + ToStringValue(rval);
		}
		else if (auto lvalDouble = std::get_if<double>(&lval))
		{
			double r;
			if (auto rvalDouble = std::get_if<double>(&rval))
				r = *rvalDouble;
			else if (auto rvalInt = std::get_if<int64_t>(&rval))
				r = static_cast<double>(*rvalInt);
			else
				return std::monostate{};
			if (op == TK_EQ) return *lvalDouble == r;
			if (op == TK_NE) return *lvalDouble != r;
			if (op == TK_LT) return *lvalDouble < r;
			if (op == TK_LE) return *lvalDouble <= r;
			if (op == TK_GT) return *lvalDouble > r;
			if (op == TK_GE) return *lvalDouble >= r;
			if (op == TK_PLUS)  return *lvalDouble + r;
			if (op == TK_MINUS) return *lvalDouble - r;
			if (op == TK_STAR)  return *lvalDouble * r;
			if (op == TK_SLASH)
			{
				if (r == 0.0)
					throw std::invalid_argument("Division by zero in filter expression");
				return *lvalDouble / r;
			}
		}
		else if (auto lvalInt = std::get_if<int64_t>(&lval))
		{
			if (auto rvalDouble = std::get_if<double>(&rval))
			{
				double l = static_cast<double>(*lvalInt);
				if (op == TK_EQ) return l == *rvalDouble;
				if (op == TK_NE) return l != *rvalDouble;
				if (op == TK_LT) return l < *rvalDouble;
				if (op == TK_LE) return l <= *rvalDouble;
				if (op == TK_GT) return l > *rvalDouble;
				if (op == TK_GE) return l >= *rvalDouble;
				if (op == TK_PLUS)  return l + *rvalDouble;
				if (op == TK_MINUS) return l - *rvalDouble;
				if (op == TK_STAR)  return l * *rvalDouble;
				if (op == TK_SLASH)
				{
					if (*rvalDouble == 0.0)
						throw std::invalid_argument("Division by zero in filter expression");
					return l / *rvalDouble;
				}
			}
			else if (auto rvalInt = std::get_if<int64_t>(&rval))
			{
				if (op == TK_EQ) return *lvalInt == *rvalInt;
				if (op == TK_NE) return *lvalInt != *rvalInt;
				if (op == TK_LT) return *lvalInt < *rvalInt;
				if (op == TK_LE) return *lvalInt <= *rvalInt;
				if (op == TK_GT) return *lvalInt > *rvalInt;
				if (op == TK_GE) return *lvalInt >= *rvalInt;
				if (op == TK_PLUS) return *lvalInt + *rvalInt;
				if (op == TK_MINUS) return *lvalInt - *rvalInt;
				if (op == TK_STAR) return *lvalInt * *rvalInt;
				if (op == TK_SLASH)
				{
					if (*rvalInt == 0)
						throw std::invalid_argument("Division by zero in filter expression");
					return *lvalInt / *rvalInt;
				}
				if (op == TK_MOD)
				{
					if (*rvalInt == 0)
						throw std::invalid_argument("Division by zero in filter expression");
					return *lvalInt % *rvalInt;
				}
			}
		}
		else if (auto lvalTimestamp = std::get_if<Poco::Timestamp>(&lval))
		{
			if (auto rvalTimestamp = std::get_if<Poco::Timestamp>(&rval))
			{
				if (op == TK_EQ) return *lvalTimestamp == *rvalTimestamp;
				if (op == TK_NE) return *lvalTimestamp != *rvalTimestamp;
				if (op == TK_LT) return *lvalTimestamp < *rvalTimestamp;
				if (op == TK_LE) return *lvalTimestamp <= *rvalTimestamp;
				if (op == TK_GT) return *lvalTimestamp > *rvalTimestamp;
				if (op == TK_GE) return *lvalTimestamp >= *rvalTimestamp;
				if (op == TK_MINUS) return *lvalTimestamp - *rvalTimestamp;
			}
			else if (auto rvalInt = std::get_if<int64_t>(&rval))
			{
				if (op == TK_PLUS) return *lvalTimestamp + *rvalInt;
				if (op == TK_MINUS) return *lvalTimestamp - *rvalInt;
			}
		}
		else if (auto lvalString = std::get_if<std::string>(&lval))
		{
			if (auto rvalString = std::get_if<std::string>(&rval))
			{
				if (!caseSensitive)
				{
					if (op == TK_EQ) return Poco::icompare(*lvalString, *rvalString) == 0;
					if (op == TK_NE) return Poco::icompare(*lvalString, *rvalString) != 0;
					if (op == TK_LT) return Poco::icompare(*lvalString, *rvalString) < 0;
					if (op == TK_LE) return Poco::icompare(*lvalString, *rvalString) <= 0;
					if (op == TK_GT) return Poco::icompare(*lvalString, *rvalString) > 0;
					if (op == TK_GE) return Poco::icompare(*lvalString, *rvalString) >= 0;
				}
				else
				{
					if (op == TK_EQ) return *lvalString == *rvalString;
					if (op == TK_NE) return *lvalString != *rvalString;
					if (op == TK_LT) return *lvalString < *rvalString;
					if (op == TK_LE) return *lvalString <= *rvalString;
					if (op == TK_GT) return *lvalString > *rvalString;
					if (op == TK_GE) return *lvalString >= *rvalString;
				}
				if (op == TK_PLUS) return *lvalString + *rvalString;
				if (op == TK_CONTAINS)
				{
					if (!caseSensitive)
					{
						auto searcher = std::boyer_moore_horspool_searcher(
							rvalString->cbegin(), rvalString->cend(), std::hash<char>(),
							[](char a, char b) {
								return std::tolower(static_cast<unsigned char>(a)) ==
									std::tolower(static_cast<unsigned char>(b));
							}
						);
						using iterator = std::string::const_iterator;
						std::pair<iterator, iterator> result = searcher(lvalString->begin(), lvalString->end());
						return (result.first != result.second);
					}
					else
					{
						auto searcher = std::boyer_moore_horspool_searcher(
							rvalString->cbegin(), rvalString->cend());
						using iterator = std::string::const_iterator;
						std::pair<iterator, iterator> result = searcher(lvalString->begin(), lvalString->end());
						return (result.first != result.second);
					}
				}
				if (op == TK_RECONTAINS)
				{
					try
					{
						const int flags = Poco::RegularExpression::RE_UTF8 | (!caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
						Poco::RegularExpression regex(*rvalString, flags);
						Poco::RegularExpression::Match match;
						return (regex.match(*lvalString, match) > 0);
					}
					catch (const Poco::RegularExpressionException&)
					{
						return false;
					}
				}
				if (op == TK_LIKE)
				{
					const int flags = (!caseSensitive) ? Poco::Glob::GLOB_CASELESS : 0;
					Poco::Glob glob(*rvalString, flags);
					return glob.match(*lvalString);
				}
				if (op == TK_MATCHES)
				{
					try
					{
						const int flags = Poco::RegularExpression::RE_UTF8 | (!caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
						Poco::RegularExpression regex(*rvalString, flags);
						return regex.match(*lvalString);
					}
					catch (const Poco::RegularExpressionException&)
					{
						return false;
					}
				}
			}
			if (auto rvalRegexp = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&rval))
			{
				if (op == TK_RECONTAINS)
				{
					try
					{
						Poco::RegularExpression::Match match;
						return ((*rvalRegexp)->match(*lvalString, match) > 0);
					}
					catch (const Poco::RegularExpressionException&)
					{
						return false;
					}
				}
				if (op == TK_MATCHES)
					return (*rvalRegexp)->match(*lvalString);
			}
		}
		else if (auto lvalContent = std::get_if<std::shared_ptr<FileContentRef>>(&lval))
		{
			if (auto rvalContent = std::get_if<std::shared_ptr<FileContentRef>>(&rval))
			{
				if (op == TK_EQ) return (*lvalContent)->operator==(**rvalContent);
				if (op == TK_NE) return !(*lvalContent)->operator==(**rvalContent);
			}
			if (auto rvalString = std::get_if<std::string>(&rval))
			{
				if (op == TK_CONTAINS) return (*lvalContent)->Contains(*rvalString, caseSensitive);
				if (op == TK_RECONTAINS)
				{
					try
					{
						const int flags = Poco::RegularExpression::RE_UTF8 | (!caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
						Poco::RegularExpression regex(*rvalString, flags);
						return (*lvalContent)->REContains(regex);
					}
					catch (const Poco::RegularExpressionException&)
					{
						return false;
					}
				}
			}
			if (auto rvalRegexp = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&rval))
			{
				if (op == TK_RECONTAINS) return (*lvalContent)->REContains(*rvalRegexp->get());
			}
		}
		else if (auto lvalBool = std::get_if<bool>(&lval))
		{
			if (auto rvalBool = std::get_if<bool>(&rval))
			{
				if (op == TK_EQ) return *lvalBool == *rvalBool;
				if (op == TK_NE) return *lvalBool != *rvalBool;
				if (op == TK_LT) return *lvalBool < *rvalBool;
				if (op == TK_LE) return *lvalBool <= *rvalBool;
				if (op == TK_GT) return *lvalBool > *rvalBool;
				if (op == TK_GE) return *lvalBool >= *rvalBool;
				if (op == TK_PLUS) return static_cast<int64_t>(*rvalBool + *lvalBool);
				if (op == TK_MINUS) return static_cast<int64_t>(*rvalBool - *lvalBool);
			}
		}
		else if (std::holds_alternative<std::monostate>(lval))
		{
			if (std::holds_alternative<std::monostate>(rval))
			{
				if (op == TK_EQ)
					return true;
				else if (op == TK_NE)
					return false;
			}
		}
		if (op == TK_EQ)
			return false;
		else if (op == TK_NE)
			return true;
		return std::monostate{};
	}
	else if (lvalArray && !rvalArray)
	{
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *(lvalArray->get()))
			result->emplace_back(ValueType2{ compute(op, item.value, rval, caseSensitive) });
		return result;
	}
	else if (!lvalArray && rvalArray)
	{
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *(rvalArray->get()))
			result->emplace_back(ValueType2{ compute(op, lval, item.value, caseSensitive) });
		return result;
	}
	else
	{
		if (op == TK_EQ || op == TK_NE)
		{
			if ((*lvalArray)->size() != (*rvalArray)->size())
				return (op == TK_NE);

			for (size_t i = 0; i < (*lvalArray)->size(); ++i)
			{
				ValueType lv = (*lvalArray)->at(i).value;
				ValueType rv = (*rvalArray)->at(i).value;
				ValueType eq = compute(TK_EQ, lv, rv, caseSensitive);
				if (!std::holds_alternative<bool>(eq) || !std::get<bool>(eq))
					return (op == TK_NE);
			}
			return (op == TK_EQ);
		}
		const size_t maxSize = (std::max)((*lvalArray)->size(), (*rvalArray)->size());
		const size_t minSize = (std::min)((*lvalArray)->size(), (*rvalArray)->size());
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (size_t i = 0; i < minSize; ++i)
			result->emplace_back(ValueType2{ compute(op, (*lvalArray)->at(i).value, (*rvalArray)->at(i).value, caseSensitive) });
		for (size_t i = 0; i < maxSize - minSize; ++i)
			result->emplace_back(ValueType2{ std::monostate{} });
		return result;
	}
}

ValueType BinaryOpNode::Evaluate(const FilterEvalContext& ectxt) const
{
	auto lval = left->Evaluate(ectxt);
	auto rval = right->Evaluate(ectxt);
	return compute(op, lval, rval, ctxt->caseSensitive);
}

ExprNode* NegateNode::Optimize()
{
	if (!right)
		return this;
	right = right->Optimize();
	auto rInt = getConstIntValue(right);
	if (rInt)
	{
		delete this;
		return new IntLiteral(-*rInt);
	}
	auto rDouble = dynamic_cast<DoubleLiteral*>(right);
	if (rDouble)
	{
		const double doubleVal = -rDouble->value;
		delete this;
		return new DoubleLiteral(doubleVal);
	}
	return this;
}

ValueType NegateNode::Evaluate(const FilterEvalContext& ectxt) const
{
	auto rval = right->Evaluate(ectxt);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
	if (auto rvalDouble = std::get_if<double>(&rval))
		return -*rvalDouble;
	return std::monostate{};
}

static auto FileExistsField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	return ectxt.di->diffcode.exists(index);
}

static auto FileMissingField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	return !ectxt.di->diffcode.exists(index);
}

static auto IsFolderField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	return ectxt.di->diffcode.isDirectory();
}

template<typename Func>
static auto FolderStatField(int index, const FilterEvalContext& ectxt, Func func, bool recursive) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};

	const String relpath = paths::ConcatPath(ectxt.di->diffFileInfo[index].path, ectxt.di->diffFileInfo[index].filename);
	const String fullPath = paths::ConcatPath(ectxt.expr->ctxt->GetPath(index), relpath);
	FolderStats::FolderStatsResult stats = FolderStats::ScanFolder(fullPath, recursive);
	return func(stats);
}

static auto FilesField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.fileCount); }, false);
}

static auto RecursiveFilesField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.fileCount); }, true);
}

static auto ItemsField(int index, const FilterEvalContext& ectxt) -> ValueType {
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.itemCount); }, false);
}

static auto RecursiveItemsField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.itemCount); }, true);
}

static auto TotalSizeField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return s.totalSize; }, false);
}

static auto RecursiveTotalSizeField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return FolderStatField(index, ectxt, [](const FolderStats::FolderStatsResult& s) { return s.totalSize; }, true);
}

static auto NameField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(ectxt.di->diffFileInfo[index].filename.get());
}

static auto BaseNameField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(paths::RemoveExtension(ectxt.di->diffFileInfo[index].filename.get()));
}

static auto ExtensionField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	const std::string ext = ucr::toUTF8(paths::FindExtension(ectxt.di->diffFileInfo[index].filename.get()));
	return std::string(ext.c_str() + strspn(ext.c_str(), "."));
}

// Helper function to determine EOL style from FileTextStats
static unsigned GetEOLStyleFromStats(const FileTextStats& stats)
{
	// Check if file has more than one EOL type (mixed)
	int eolCount = 0;
	if (stats.ncrlfs > 0) ++eolCount;
	if (stats.ncrs > 0) ++eolCount;
	if (stats.nlfs > 0) ++eolCount;

	if (eolCount > 1)
		return ILineDataProvider::EOL_MIXED;

	// Pure EOL style detection
	if (stats.ncrlfs > 0)
		return ILineDataProvider::EOL_CRLF;
	if (stats.nlfs > 0)
		return ILineDataProvider::EOL_LF;
	if (stats.ncrs > 0)
		return ILineDataProvider::EOL_CR;
	return ILineDataProvider::EOL_NONE;
}

static auto FileEOLField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return GetEOLStyleFromStats(ectxt.di->diffFileInfo[index].m_textStats);
}

static std::string EOLStr(unsigned eolFlags)
{
	switch (eolFlags)
	{
	case ILineDataProvider::EOL_LF: return "LF";
	case ILineDataProvider::EOL_CR: return "CR";
	case ILineDataProvider::EOL_CRLF: return "CRLF";
	case ILineDataProvider::EOL_MIXED: return "Mixed";
	default: return "None";
	}
}

static auto FileEOLStrField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return EOLStr(GetEOLStyleFromStats(ectxt.di->diffFileInfo[index].m_textStats));
}

// Helper function to parse date/time strings with multiple format support
static std::optional<Poco::Timestamp> ParseDateTime(const std::string& str)
{
	static const std::vector<std::string> formats = {
		Poco::DateTimeFormat::ISO8601_FORMAT,
		Poco::DateTimeFormat::ISO8601_FRAC_FORMAT,
		"%Y/%m/%d %H:%M:%S",
		"%Y.%m.%d %H:%M:%S",
		"%d-%b-%Y %H:%M:%S",
		"%Y-%m-%d",
		"%Y/%m/%d",
		"%Y.%m.%d",
	};

	Poco::DateTime dt;
	for (const auto& fmt : formats)
	{
		try
		{
			int tz = 0;
			Poco::DateTimeParser::parse(fmt, str, dt, tz);
			dt.makeUTC((str.find_first_of("zZ+") != std::string::npos) ? tz : Poco::Timezone::tzd());
			return dt.timestamp();
		}
		catch (Poco::SyntaxException&)
		{
			// Try next format
		}
	}
	return std::nullopt;
}

static auto FolderField(int index, const FilterEvalContext& ectxt)-> ValueType
{
	return ucr::toUTF8(ectxt.di->diffFileInfo[index].path.get());
}

static auto SizeField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(ectxt.di->diffFileInfo[index].size);
}

static auto DateField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ectxt.di->diffFileInfo[index].mtime;
}

static auto DateStrField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	Poco::LocalDateTime ldt(Poco::Timezone::tzd(), ectxt.di->diffFileInfo[index].mtime);
	return Poco::DateTimeFormatter::format(ldt, "%Y-%m-%d");
}

static auto CreationTimeField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ectxt.di->diffFileInfo[index].ctime;
}

static auto FileVersionField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	if (ectxt.di->diffFileInfo[index].version.IsCleared())
		ectxt.expr->ctxt->UpdateVersion(const_cast<DIFFITEM&>(*ectxt.di), index);
	return static_cast<int64_t>(ectxt.di->diffFileInfo[index].version.GetFileVersionQWORD());
}

static auto AttributesField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(ectxt.di->diffFileInfo[index].flags.attributes);
}

static auto AttrStrField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(ectxt.di->diffFileInfo[index].flags.ToString());
}

static auto CodepageField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(ectxt.di->diffFileInfo[index].encoding.m_codepage);
}

static auto DiffCodeField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return static_cast<int64_t>(ectxt.di->diffcode.diffcode);
}

static auto DifferencesField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return static_cast<int64_t>(ectxt.di->nsdiffs);
}

static auto IgnoredDiffsField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return static_cast<int64_t>(ectxt.di->nidiffs);
}

static auto FileIdenticalField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return ectxt.di->diffcode.isResultSame();
}

static auto FileDifferentField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return ectxt.di->diffcode.isResultDiff();
}

static auto SkippedField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return ectxt.di->diffcode.isResultFiltered();
}

static auto FileMovedField(int, const FilterEvalContext& ectxt) -> ValueType
{
	return ectxt.di->renameMoveGroupId != -1;
}

static auto EncodingField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(ectxt.di->diffFileInfo[index].encoding.GetName());
}

static auto HasBOMField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	return ectxt.di->diffFileInfo[index].encoding.m_bom;
}

static auto BinaryField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	const unsigned diffcode = ectxt.di->diffcode.diffcode;
	const unsigned binarySide = (index == 0) ? DIFFCODE::BINSIDE1 : (index == 1) ? DIFFCODE::BINSIDE2 : DIFFCODE::BINSIDE3;
	return (diffcode & binarySide) != 0;
}

static auto FileDifferentLeftMiddleField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (ectxt.expr->ctxt->GetCompareDirs() < 3)
		return std::monostate{};
	return (ectxt.di->diffcode.diffcode & DIFFCODE::DIFF1STONLY) != 0;
}

static auto FileDifferentMiddleRightField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (ectxt.expr->ctxt->GetCompareDirs() < 3)
		return std::monostate{};
	return (ectxt.di->diffcode.diffcode & DIFFCODE::DIFF2NDONLY) != 0;
}

static auto FileDifferentLeftRightField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (ectxt.expr->ctxt->GetCompareDirs() >= 3)
		return (ectxt.di->diffcode.diffcode & DIFFCODE::DIFF3RDONLY) != 0;
	else
		return ectxt.di->diffcode.existAll() && ectxt.di->diffcode.isResultDiff();
}

static auto RelPathField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	const String relpath = paths::ConcatPath(ectxt.di->diffFileInfo[index].path, ectxt.di->diffFileInfo[index].filename);
	return ucr::toUTF8(relpath);
}

static auto FullPathField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	const String relpath = paths::ConcatPath(ectxt.di->diffFileInfo[index].path, ectxt.di->diffFileInfo[index].filename);
	return ucr::toUTF8(paths::ConcatPath(ectxt.expr->ctxt->GetPath(index), relpath));
}

static auto ContentField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.di->diffcode.exists(index))
		return std::monostate{};
	const String relpath = paths::ConcatPath(ectxt.di->diffFileInfo[index].path, ectxt.di->diffFileInfo[index].filename);
	std::shared_ptr<FileContentRef> content{ new FileContentRef };
	content->path = paths::ConcatPath(ectxt.expr->ctxt->GetPath(index), relpath);
	content->item.size = ectxt.di->diffFileInfo[index].size;
	content->item.flags = ectxt.di->diffFileInfo[index].flags;
	content->item.mtime = ectxt.di->diffFileInfo[index].mtime;
	content->item.ctime = ectxt.di->diffFileInfo[index].ctime;
	content->item.version = ectxt.di->diffFileInfo[index].version;
	content->item.encoding = ectxt.di->diffFileInfo[index].encoding;
	return content;
}

static auto UnpackerField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (ectxt.di->diffcode.isDirectory())
		return std::monostate{};
	PackingInfo* pInfoUnpacker = nullptr;
	PrediffingInfo* pInfoPrediffer = nullptr;
	String filteredFilenames = ectxt.expr->ctxt->GetFilteredFilenames(*ectxt.di);
	const_cast<CDiffContext*>(ectxt.expr->ctxt)->FetchPluginInfos(filteredFilenames, &pInfoUnpacker, &pInfoPrediffer);
	return pInfoUnpacker ? ucr::toUTF8(pInfoUnpacker->GetPluginPipeline()) : std::string("");
}

static auto PredifferField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (ectxt.di->diffcode.isDirectory())
		return std::monostate{};
	PackingInfo* pInfoUnpacker = nullptr;
	PrediffingInfo* pInfoPrediffer = nullptr;
	String filteredFilenames = ectxt.expr->ctxt->GetFilteredFilenames(*ectxt.di);
	const_cast<CDiffContext*>(ectxt.expr->ctxt)->FetchPluginInfos(filteredFilenames, &pInfoUnpacker, &pInfoPrediffer);
	return pInfoPrediffer ? ucr::toUTF8(pInfoPrediffer->GetPluginPipeline()) : std::string("");
}

static auto LineField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	std::string line = ectxt.provider->GetLine(index, ectxt.lineIndex);
	if (line.empty() && (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0)
		return std::monostate{};
	return line;
}

static auto LineLengthField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	auto line = ectxt.provider->GetLine(index, ectxt.lineIndex);
	if (line.empty() && (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0)
		return std::monostate{};
	return static_cast<int64_t>(ucr::stringlen_of_utf8(line.data(), line.length()));
}

static auto LineNumberField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	if ((ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0)
		return std::monostate{};
	return static_cast<int64_t>(ectxt.provider->GetRealLineNumber(index, ectxt.lineIndex) + 1);
}

static auto ViewLineNumberField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	return static_cast<int>(ectxt.lineIndex + 1);
}

static auto LineEOLField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	if ((ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0)
		return std::monostate{};
	return static_cast<int64_t>(ectxt.provider->GetLineEol(index, ectxt.lineIndex));
}

static auto LineEOLStrField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	if ((ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0)
		return std::monostate{};
	return EOLStr(ectxt.provider->GetLineEol(index, ectxt.lineIndex));
}

static auto LineExistsField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) == 0;
}

static auto LineMissingField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_GHOST) != 0;
}

static auto LineMovedField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_MOVED) != 0;
}

static auto LineBookmarkedField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(index, ectxt.lineIndex) & ILineDataProvider::LF_BOOKMARKS) != 0;
}

static auto LineIdenticalField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(0, ectxt.lineIndex) & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) == 0;
}

static auto LineDifferentField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(0, ectxt.lineIndex) & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) != 0;
}

static auto LineDifferentLeftMiddleField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider || ectxt.expr->ctxt->GetCompareDirs() < 3)
		return std::monostate{};
	unsigned leftflags = ectxt.provider->GetLineFlags(0, ectxt.lineIndex);
	return ((leftflags & ILineDataProvider::LF_SNP) == 0) && ((leftflags & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) != 0);
}

static auto LineDifferentMiddleRightField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider || ectxt.expr->ctxt->GetCompareDirs() < 3)
		return std::monostate{};
	unsigned middleflags = ectxt.provider->GetLineFlags(1, ectxt.lineIndex);
	return (middleflags & ILineDataProvider::LF_SNP) == 0 && (middleflags & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) != 0;
}

static auto LineDifferentLeftRightField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	unsigned leftflags = ectxt.provider->GetLineFlags(0, ectxt.lineIndex);
	if (ectxt.expr->ctxt->GetCompareDirs() >= 3)
	{
		unsigned middleflags = ectxt.provider->GetLineFlags(1, ectxt.lineIndex);
		return (leftflags & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) != 0 && (middleflags & ILineDataProvider::LF_SNP) == 0;
	}
	return (leftflags & (ILineDataProvider::LF_DIFF | ILineDataProvider::LF_GHOST)) != 0;
}

static auto LineTrivialField(int, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return (ectxt.provider->GetLineFlags(0, ectxt.lineIndex) & ILineDataProvider::LF_TRIVIAL) != 0;
}

static int GetColumnIndexByName(int index, const std::string& columnName, const FilterEvalContext& ectxt)
{
	if (!ectxt.provider || ectxt.provider->GetLineCount() == 0)
		return -1;

	int colCount = ectxt.provider->GetColumnCount(index, 0);
	for (int i = 0; i < colCount; ++i)
	{
		std::string headerName = ectxt.provider->GetColumn(index, 0, i);
		bool match = ectxt.expr->caseSensitive ? 
			(headerName == columnName) : 
			(Poco::icompare(headerName, columnName) == 0);
		if (match)
			return i;
	}
	return -1;
}

static auto ColumnField(int index, int columnIndex, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	if (columnIndex < 0 || columnIndex >= ectxt.provider->GetColumnCount(index, ectxt.lineIndex))
		return std::monostate{};
	return ectxt.provider->GetColumn(index, ectxt.lineIndex, columnIndex);
}

static auto ColumnFieldByName(int index, const std::string& columnName, const FilterEvalContext& ectxt) -> ValueType
{
	int columnIndex = GetColumnIndexByName(index, columnName, ectxt);
	if (columnIndex < 0)
		return std::monostate{};
	return ColumnField(index, columnIndex, ectxt);
}

static auto ColumnCountField(int index, const FilterEvalContext& ectxt) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	return static_cast<int64_t>(ectxt.provider->GetColumnCount(index, ectxt.lineIndex));
}

static std::pair<int, int> GetDistancesToRanges(int line, const std::vector<RangeInfo>& ranges)
{
	int prevEnd = -1;

	for (const auto& range : ranges)
	{
		if (range.start <= line && line <= range.end)
			return { 0, 0 };

		if (line < range.start)
		{
			int after = range.start - line;
			if (prevEnd == -1)
				return { -1, after };
			int before = line - prevEnd;
			return { before, after };
		}

		prevEnd = range.end;
	}

	if (prevEnd != -1)
		return { line - prevEnd, -1 };

	return { -1, -1 };
}

template <typename Predicate>
std::vector<std::pair<int, int>> ExtractRanges(int count, Predicate pred)
{
	std::vector<std::pair<int, int>> ranges;

	int start = -1;

	for (int i = 0; i < count; ++i)
	{
		if (pred(i))
		{
			if (start == -1)
				start = i;
		}
		else
		{
			if (start != -1)
			{
				ranges.emplace_back(start, i - 1);
				start = -1;
			}
		}
	}

	if (start != -1)
		ranges.emplace_back(start, count - 1);

	return ranges;
}

struct AttrParseResult { int side; int prefixLen; };

AttrParseResult ParseAttributeName(std::string_view name)
{
	if (name.compare(0, 4, "left") == 0)
		return { 0, 4 };
	else if (name.compare(0, 6, "middle") == 0)
		return { 1, 6 };
	else if (name.compare(0, 5, "right") == 0)
		return { -1, 5 };
	return { 0, 0 };
}

struct FieldInfo
{
	using FieldFunc = ValueType (*)(int, const FilterEvalContext&);
	const char* name;
	FieldFunc funcForDir;      // Function when diritem == true (or always if funcForLine == nullptr)
	FieldFunc funcForLine;     // Function when diritem == false (nullptr if same as funcForDir)
	bool allowPrefix;          // Whether left/middle/right prefix is allowed
	bool forceGlobalSide;      // Whether to force side = -2 (no side differentiation)
};

// Compile-time constant array - zero runtime initialization cost
static constexpr FieldInfo fieldTable[] = {
	// name, funcForDir, funcForLine, allowPrefix, forceGlobalSide
	{"attributes", AttributesField, nullptr, true, false},
	{"attrstr", AttrStrField, nullptr, true, false},
	{"basename", BaseNameField, nullptr, true, false},
	{"binary", BinaryField, nullptr, true, false},
	{"bookmarked", LineBookmarkedField, nullptr, true, false},
	{"codepage", CodepageField, nullptr, true, false},
	{"columncount", ColumnCountField, nullptr, true, false},
	{"content", ContentField, nullptr, true, false},
	{"creationtime", CreationTimeField, nullptr, true, false},
	{"date", DateField, nullptr, true, false},
	{"datestr", DateStrField, nullptr, true, false},
	{"diffcode", DiffCodeField, nullptr, false, true},
	{"differences", DifferencesField, nullptr, false, true},
	{"different", FileDifferentField, LineDifferentField, false, true},
	{"differentleftmiddle", FileDifferentLeftMiddleField, LineDifferentLeftMiddleField, false, true},
	{"differentleftright", FileDifferentLeftRightField, LineDifferentLeftRightField, false, true},
	{"differentmiddleright", FileDifferentMiddleRightField, LineDifferentMiddleRightField, false, true},
	{"encoding", EncodingField, nullptr, true, false},
	{"eol", FileEOLField, LineEOLField, true, false},
	{"eolstr", FileEOLStrField, LineEOLStrField, true, false},
	{"exists", FileExistsField, LineExistsField, true, false},
	{"extension", ExtensionField, nullptr, true, false},
	{"filedifferent", FileDifferentField, nullptr, false, true},
	{"filedifferentleftmiddle", FileDifferentLeftMiddleField, nullptr, false, true},
	{"filedifferentleftright", FileDifferentLeftRightField, nullptr, false, true},
	{"filedifferentmiddleright", FileDifferentMiddleRightField, nullptr, false, true},
	{"fileexists", FileExistsField, nullptr, true, false},
	{"fileidentical", FileIdenticalField, nullptr, false, true},
	{"filemissing", FileMissingField, nullptr, true, false},
	{"filemoved", FileMovedField, nullptr, true, false},
	{"files", FilesField, nullptr, true, false},
	{"folder", FolderField, nullptr, true, false},
	{"fullpath", FullPathField, nullptr, true, false},
	{"hasbom", HasBOMField, nullptr, true, false},
	{"identical", FileIdenticalField, LineIdenticalField, false, true},
	{"ignoreddiffs", IgnoredDiffsField, nullptr, false, true},
	{"isfolder", IsFolderField, nullptr, true, false},
	{"items", ItemsField, nullptr, true, false},
	{"line", LineField, nullptr, true, false},
	{"linelength", LineLengthField, nullptr, true, false},
	{"linenumber", LineNumberField, nullptr, true, false},
	{"missing", FileMissingField, LineMissingField, true, false},
	{"moved", FileMovedField, LineMovedField, true, false},
	{"name", NameField, nullptr, true, false},
	{"prediffer", PredifferField, nullptr, false, true},
	{"recursivefiles", RecursiveFilesField, nullptr, true, false},
	{"recursiveitems", RecursiveItemsField, nullptr, true, false},
	{"recursivetotalsize", RecursiveTotalSizeField, nullptr, true, false},
	{"relpath", RelPathField, nullptr, true, false},
	{"size", SizeField, nullptr, true, false},
	{"skipped", SkippedField, nullptr, false, true},
	{"totalsize", TotalSizeField, nullptr, true, false},
	{"trivial", LineTrivialField, nullptr, false, true},
	{"unpacker", UnpackerField, nullptr, false, true},
	{"version", FileVersionField, nullptr, true, false},
	{"viewlinenumber", ViewLineNumberField, nullptr, false, true},
};

static constexpr size_t fieldTableSize = sizeof(fieldTable) / sizeof(fieldTable[0]);

// Binary search in sorted array - O(log n)
static const FieldInfo* findField(const char* name)
{
	auto it = std::lower_bound(std::begin(fieldTable), std::end(fieldTable), name,
		[](const FieldInfo& info, const char* n) {
			return strcmp(info.name, n) < 0;
		}
	);

	if (it != std::end(fieldTable) && strcmp(it->name, name) == 0)
		return it;

	return nullptr;
}

FieldNode::FieldNode(const FilterExpression* ctxt, const std::string& v) : ctxt(ctxt), field(v)
{
	std::string vl = Poco::toLower(v);
	auto [side, prefixlen] = ParseAttributeName(vl);
	ValueType (*functmp)(int, const FilterEvalContext&) = nullptr;
	const char* p = vl.c_str() + prefixlen;

	// Handle dynamic column fields (column1, column2, etc.)
	if (strncmp(p, "column", 6) == 0 && isdigit(p[6]))
	{
		// Parse column number (Column1, Column2, etc.)
		int columnIndex = atoi(p + 6) - 1;
		if (prefixlen == 0)
		{
			func = [columnIndex](const FilterEvalContext& ectxt) -> ValueType {
				const int dirs = ectxt.expr->ctxt->GetCompareDirs();
				std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
					values->emplace_back(ValueType2{ ColumnField(i, columnIndex, ectxt) });
				return values;
				};
			return;
		}
		else
		{
			func = [side, columnIndex](const FilterEvalContext& ectxt) -> ValueType {
				return ColumnField((side < 0) ? (ectxt.expr->ctxt->GetCompareDirs() + side) : side, columnIndex, ectxt);
				};
			return;
		}
	}
	// Lookup in field table
	else if (const FieldInfo* info = findField(p))
	{
		// Check if prefix is allowed for this field
		if (prefixlen > 0 && !info->allowPrefix)
			throw std::runtime_error("Field does not support left/middle/right prefix: " + std::string(v.begin(), v.end()));

		// Select appropriate function based on diritem context
		functmp = (info->funcForLine && !ctxt->diritem) ? info->funcForLine : info->funcForDir;

		// Override side if this is a global field
		if (info->forceGlobalSide)
			side = -2;
	}
	else
		throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));

	// Set up the wrapper function based on side/prefix
	if (prefixlen > 0)
		func = [side, functmp](const FilterEvalContext& ectxt)-> ValueType { return functmp(side < 0 ? ectxt.expr->ctxt->GetCompareDirs() + side: side, ectxt); };
	else
	{
		if (side == -2)
		{
			func = [functmp](const FilterEvalContext& ectxt)-> ValueType { return functmp(-2, ectxt); };
		}
		else
		{
			func = [functmp](const FilterEvalContext& ectxt)-> ValueType {
				const int dirs = ectxt.expr->ctxt->GetCompareDirs();
				std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
					values->emplace_back(ValueType2{ functmp(i, ectxt) });
				return values;
				};
		}
	}
}

ValueType FieldNode::Evaluate(const FilterEvalContext& ectxt) const
{
	return func(ectxt);
}

inline static std::optional<Poco::LocalDateTime> toLocalDateTime(const ValueType& val)
{
	if (auto ts = std::get_if<Poco::Timestamp>(&val))
		return Poco::LocalDateTime(*ts);
	if (auto str = std::get_if<std::string>(&val))
		return Poco::LocalDateTime(DateTimeLiteral(*str).value);
	return std::nullopt;
}

template <typename Func>
inline static ValueType applyToScalarOrArray(const ValueType& arg, Func func)
{
	if (auto argArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg))
	{
		auto result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *(*argArray))
			result->emplace_back(ValueType2{ func(item.value) });
		return result;
	}
	return func(arg);
}

static auto AbsFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto absFn = [](const ValueType& val)->ValueType
		{
			if (auto arg1Int = std::get_if<int64_t>(&val))
				return abs(*arg1Int);
			if (auto arg1Double = std::get_if<double>(&val))
				return abs(*arg1Double);
			return std::monostate{};
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, absFn);
}

static auto AnyOfFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(ectxt);
	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg1))
	{
		const auto& vec = *arrayVal->get();
		return std::any_of(vec.begin(), vec.end(), [](const ValueType2& item) {
			const auto boolVal = std::get_if<bool>(&item.value);
			return boolVal && *boolVal;
			});
	}
	if (auto arg1Bool = std::get_if<bool>(&arg1))
		return *arg1Bool;
	return std::monostate{};
}

static auto AllOfFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(ectxt);
	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg1))
	{
		const auto& vec = *arrayVal->get();
		return std::all_of(vec.begin(), vec.end(), [](const ValueType2& item) {
			const auto boolVal = std::get_if<bool>(&item.value);
			return boolVal && *boolVal;
			});
	}
	if (auto arg1Bool = std::get_if<bool>(&arg1))
		return *arg1Bool;
	return std::monostate{};
}

static bool valueEquals(const ValueType& a, const ValueType& b)
{
	if (a.index() != b.index())
		return false;

	if (auto arrA = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&a))
	{
		auto arrB = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&b);
		if (!arrB || (*arrA)->size() != (*arrB)->size())
			return false;
		for (size_t i = 0; i < (*arrA)->size(); ++i)
			if (!valueEquals((*arrA)->at(i).value, (*arrB)->at(i).value))
				return false;
		return true;
	}
	if (auto fcA = std::get_if<std::shared_ptr<FileContentRef>>(&a))
	{
		auto fcB = std::get_if<std::shared_ptr<FileContentRef>>(&b);
		return fcB && **fcA == **fcB;
	}
	return a == b;
}

static auto AllEqualFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType first = args->at(0)->Evaluate(ectxt);
	if (auto pArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&first); pArray && *pArray)
	{
		const auto& vec = **pArray;
		if (vec.size() <= 1)
			return true;
		const ValueType& base = vec[0].value;
		for (size_t i = 1; i < vec.size(); ++i)
		{
			if (!valueEquals(base, vec[i].value))
				return false;
		}
		return true;
	}
	else
	{
		for (size_t i = 1; i < args->size(); ++i)
		{
			ValueType val = args->at(i)->Evaluate(ectxt);
			if (!valueEquals(first, val))
				return false;
		}
		return true;
	}
}

static auto ArrayFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto result = std::make_shared<std::vector<ValueType2>>();
	if (args)
	{
		for (ExprNode* arg : *args)
		{
			ValueType val = arg->Evaluate(ectxt);
			result->emplace_back(ValueType2{ std::move(val) });
		}
	}
	return result;
}

static auto AtFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(ectxt);
	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg1))
	{
		ValueType arg2 = args->at(1)->Evaluate(ectxt);
		if (auto arg2Int = std::get_if<int64_t>(&arg2))
		{
			int64_t idx = *arg2Int;
			if (idx < 0)
				idx += static_cast<int64_t>((*arrayVal)->size());
			if (idx < 0 || idx >= static_cast<int64_t>((*arrayVal)->size()))
				return std::monostate{};
			return (*arrayVal)->at(idx).value;
		}
	}
	return std::monostate{};
}

static auto StrlenFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto strlenFn = [](const ValueType& val)->ValueType
		{
			if (auto arg1Str = std::get_if<std::string>(&val))
				return static_cast<int64_t>(ucr::stringlen_of_utf8(arg1Str->c_str(), arg1Str->length()));
			if (std::holds_alternative<std::monostate>(val))
				return std::monostate{};
			auto str = ToStringValue(val);
			return static_cast<int64_t>(ucr::stringlen_of_utf8(str.c_str(), str.length()));
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, strlenFn);
}

static auto LenFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg = (*args)[0]->Evaluate(ectxt);

	// Handle array: return element count
	if (auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg))
		return static_cast<int64_t>((*arrayVal)->size());

	// Handle string: return UTF-8 character count
	if (auto strVal = std::get_if<std::string>(&arg))
		return static_cast<int64_t>(ucr::stringlen_of_utf8(strVal->c_str(), strVal->length()));

	// Handle monostate: return 0
	if (std::holds_alternative<std::monostate>(arg))
		return static_cast<int64_t>(0);

	// Handle other scalar values: return 1
	return static_cast<int64_t>(1);
}

static auto SubstrFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2 || args->size() > 3)
		return std::monostate{};

	auto argStart = (*args)[1]->Evaluate(ectxt);
	std::optional<ValueType> argLen;
	if (args->size() == 3)
		argLen = (*args)[2]->Evaluate(ectxt);

	const int64_t* start = std::get_if<int64_t>(&argStart);
	const int64_t* len = argLen ? std::get_if<int64_t>(&*argLen) : nullptr;

	if (!start)
		return std::monostate{};

	auto substrFn = [start, len](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};
			const std::string* str = &*strOpt;

			int64_t s = *start;
			if (s < 0)
				s += static_cast<int64_t>(str->length());
			if (s < 0 || s >= str->length())
				return std::string{};

			if (!len)
				return str->substr(static_cast<size_t>(s));

			int64_t actualLen = (*len >= 0) ? *len : static_cast<int64_t>(str->length()) - s + *len;
			if (actualLen < 0)
				actualLen = 0;
			return str->substr(static_cast<size_t>(s), static_cast<size_t>(actualLen));
		};

	auto argStr = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(argStr, substrFn);
}

static auto LineCountFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto lineCountFn = [](const ValueType& val) -> ValueType
		{
			if (auto contentRef = std::get_if<std::shared_ptr<FileContentRef>>(&val))
				return static_cast<int64_t>((*contentRef)->LineCount());
			return std::monostate{};
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, lineCountFn);
}

static auto SublinesFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2 || args->size() > 3)
		return std::monostate{};

	auto argContentRef = (*args)[0]->Evaluate(ectxt);
	auto argStart = (*args)[1]->Evaluate(ectxt);
	std::optional<ValueType> argLen;
	if (args->size() == 3)
		argLen = (*args)[2]->Evaluate(ectxt);

	const int64_t* start = std::get_if<int64_t>(&argStart);
	const int64_t* len = argLen ? std::get_if<int64_t>(&*argLen) : nullptr;

	if (!start)
		return std::monostate{};

	auto sublinesFn = [start, len](const ValueType& val) -> ValueType
		{
			if (auto contentRef = std::get_if<std::shared_ptr<FileContentRef>>(&val))
				return static_cast<std::string>((*contentRef)->Sublines(*start, len ? *len : -1));
			return std::monostate{};
		};

	return applyToScalarOrArray(argContentRef, sublinesFn);
}

static auto ReplaceFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!args || args->size() != 3)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(ectxt);
	auto argFrom = (*args)[1]->Evaluate(ectxt);
	auto argTo = (*args)[2]->Evaluate(ectxt);

	const std::string* from = std::get_if<std::string>(&argFrom);
	const auto fromRegex = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&argFrom);
	const std::string* to = std::get_if<std::string>(&argTo);

	if ((!from && !fromRegex) || !to)
		return std::monostate{};

	if (fromRegex)
	{
		// Use pre-compiled regex from Optimize()
		auto replaceFn = [fromRegex, to](const ValueType& val) -> ValueType
			{
				auto strOpt = getAsString(val);
				if (!strOpt)
					return std::monostate{};
				std::string result = *strOpt;
				(*fromRegex)->subst(result, *to, Poco::RegularExpression::RE_GLOBAL);
				return result;
			};
		return applyToScalarOrArray(argStr, replaceFn);
	}
	else if (from)
	{
		// Dynamic case: compile regex at runtime
		if (from->empty())
			return std::monostate{};

		try
		{
			std::string pattern = escapeRegex(*from);
			const int flags = Poco::RegularExpression::RE_UTF8 | (!ectxt.expr->caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
			auto regex = std::make_shared<Poco::RegularExpression>(pattern, flags);

			auto replaceFn = [regex, to](const ValueType& val) -> ValueType
				{
					auto strOpt = getAsString(val);
					if (!strOpt)
						return std::monostate{};
					std::string result = *strOpt;
					regex->subst(result, *to, Poco::RegularExpression::RE_GLOBAL);
					return result;
				};

			return applyToScalarOrArray(argStr, replaceFn);
		}
		catch (const Poco::RegularExpressionException&)
		{
			return std::monostate{};
		}
	}

	return std::monostate{};
}

static auto RegexReplaceWithListFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!args || args->size() != 2)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(ectxt);
	auto argPathOrList = (*args)[1]->Evaluate(ectxt);

	const std::string* path = std::get_if<std::string>(&argPathOrList);
	auto list = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argPathOrList);

	std::shared_ptr<std::vector<ValueType2>> list2;
	if (path)
	{
		list2 = ReplaceList::LoadRegexList(*ectxt.expr, ucr::toTString(*path));
		list = &list2;
	}

	auto regexReplaceWithListFn = [&list, &ectxt](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt || !list->get())
				return val;
			std::string result = *strOpt;
			for (const auto& item : (*list->get()))
			{
				if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&item.value))
				{
					if ((*arrayVal)->size() != 2)
						continue;
					auto replacement = std::get_if<std::string>(&(*arrayVal)->at(1).value);
					if (!replacement)
						continue;
					try
					{
						if (auto rePattern = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&(*arrayVal)->at(0).value))
						{
							(*rePattern)->subst(result, *replacement, Poco::RegularExpression::RE_GLOBAL);
						}
						else if (auto strPattern = std::get_if<std::string>(&(*arrayVal)->at(0).value))
						{
							const int flags = Poco::RegularExpression::RE_UTF8 | (!ectxt.expr->caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
							Poco::RegularExpression regex(*strPattern, flags);
							regex.subst(result, *replacement, Poco::RegularExpression::RE_GLOBAL);
						}
					}
					catch (const Poco::RegularExpressionException&)
					{
						continue;
					}
				}
			}
			return result;
		};

	return applyToScalarOrArray(argStr, regexReplaceWithListFn);
}

static auto ReplaceWithListFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!args || args->size() != 2)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(ectxt);
	auto argPathOrList = (*args)[1]->Evaluate(ectxt);

	const std::string* path = std::get_if<std::string>(&argPathOrList);
	auto list = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argPathOrList);

	std::shared_ptr<std::vector<ValueType2>> list2;
	if (path)
	{
		list2 = ReplaceList::LoadList(*ectxt.expr, ucr::toTString(*path));
		list = &list2;
	}

	auto replaceWithListFn = [&list](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt || !list->get())
				return val;
			std::string result = *strOpt;
			// Use pre-compiled regex objects from LoadList()
			for (const auto& item : (*list->get()))
			{
				if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&item.value))
				{
					if ((*arrayVal)->size() != 2)
						continue;
					// Get pre-compiled regex and replacement string
					if (auto regex = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&(*arrayVal)->at(0).value))
					{
						if (auto replacement = std::get_if<std::string>(&(*arrayVal)->at(1).value))
						{
							(*regex)->subst(result, *replacement, Poco::RegularExpression::RE_GLOBAL);
						}
					}
				}
			}
			return result;
		};

	return applyToScalarOrArray(argStr, replaceWithListFn);
}

static auto TodayFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return *ectxt.expr->today;
}

static auto NowFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return *ectxt.expr->now;
}

static auto StartOfWeekFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto startOfWeek = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			auto& ldt = *ldtOpt;
			Poco::LocalDateTime startOfDay(ldt.year(), ldt.month(), ldt.day(), 0, 0, 0, 0, 0);
			int dow = ldt.dayOfWeek();
			Poco::Timespan offset(dow, 0, 0, 0, 0);
			return (startOfDay - offset).utc().timestamp();
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, startOfWeek);
}

static auto StartOfMonthFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto startOfMonth = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			auto& ldt = *ldtOpt;
			return Poco::LocalDateTime(ldt.year(), ldt.month(), 1, 0, 0, 0, 0, 0).utc().timestamp();
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, startOfMonth);
}

static auto StartOfYearFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto startOfYear = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			auto& ldt = *ldtOpt;
			return Poco::LocalDateTime(ldt.year(), 1, 1, 0, 0, 0, 0, 0).utc().timestamp();
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, startOfYear);
}

static auto ToDateStrFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto toDateStr = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			return Poco::DateTimeFormatter::format(*ldtOpt, "%Y-%m-%d");
		};
	auto arg = (*args)[0]->Evaluate(ectxt);
	return applyToScalarOrArray(arg, toDateStr);
}

static auto IsWithinFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg2 = args->at(1)->Evaluate(ectxt);
	ValueType arg3 = args->at(2)->Evaluate(ectxt);

	auto isWithinFn = [&](const ValueType& val) -> ValueType
		{
			if (auto valInt = std::get_if<int64_t>(&val))
			{
				if (auto minInt = std::get_if<int64_t>(&arg2))
				{
					if (auto maxInt = std::get_if<int64_t>(&arg3))
						return (*valInt >= *minInt && *valInt <= *maxInt);
					if (auto maxDouble = std::get_if<double>(&arg3))
						return (*valInt >= *minInt && static_cast<double>(*valInt) <= *maxDouble);
				}
				if (auto minDouble = std::get_if<double>(&arg2))
				{
					if (auto maxDouble = std::get_if<double>(&arg3))
						return (static_cast<double>(*valInt) >= *minDouble && static_cast<double>(*valInt) <= *maxDouble);
					if (auto maxInt = std::get_if<int64_t>(&arg3))
						return (static_cast<double>(*valInt) >= *minDouble && *valInt <= *maxInt);
				}
			}
			else if (auto valDouble = std::get_if<double>(&val))
			{
				double minVal = 0.0, maxVal = 0.0;
				if (auto minDouble = std::get_if<double>(&arg2))
					minVal = *minDouble;
				else if (auto minInt = std::get_if<int64_t>(&arg2))
					minVal = static_cast<double>(*minInt);
				else
					return std::monostate{};

				if (auto maxDouble = std::get_if<double>(&arg3))
					maxVal = *maxDouble;
				else if (auto maxInt = std::get_if<int64_t>(&arg3))
					maxVal = static_cast<double>(*maxInt);
				else
					return std::monostate{};

				return (*valDouble >= minVal && *valDouble <= maxVal);
			}
			else if (auto valTimestamp = std::get_if<Poco::Timestamp>(&val))
			{
				if (auto minTimestamp = std::get_if<Poco::Timestamp>(&arg2))
				{
					if (auto maxTimestamp = std::get_if<Poco::Timestamp>(&arg3))
						return (*valTimestamp >= *minTimestamp && *valTimestamp <= *maxTimestamp);
				}
			}
			else if (auto valString = std::get_if<std::string>(&val))
			{
				if (auto minString = std::get_if<std::string>(&arg2))
				{
					if (auto maxString = std::get_if<std::string>(&arg3))
					{
						int cmpMin, cmpMax;
						if (!ectxt.expr->caseSensitive)
						{
							cmpMin = Poco::icompare(*valString, *minString);
							cmpMax = Poco::icompare(*valString, *maxString);
						}
						else
						{
							cmpMin = valString->compare(*minString);
							cmpMax = valString->compare(*maxString);
						}
						return (cmpMin >= 0 && cmpMax <= 0);
					}
				}
			}
			return std::monostate{};
		};

	auto arg1 = args->at(0)->Evaluate(ectxt);
	return applyToScalarOrArray(arg1, isWithinFn);
}

static auto InRangeFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg2 = args->at(1)->Evaluate(ectxt);
	ValueType arg3 = args->at(2)->Evaluate(ectxt);

	auto inRangeFn = [&](const ValueType& val) -> ValueType
		{
			if (auto valInt = std::get_if<int64_t>(&val))
			{
				if (auto minInt = std::get_if<int64_t>(&arg2))
				{
					if (auto maxInt = std::get_if<int64_t>(&arg3))
						return (*valInt >= *minInt && *valInt < *maxInt);
					if (auto maxDouble = std::get_if<double>(&arg3))
						return (*valInt >= *minInt && static_cast<double>(*valInt) < *maxDouble);
				}
				if (auto minDouble = std::get_if<double>(&arg2))
				{
					if (auto maxDouble = std::get_if<double>(&arg3))
						return (static_cast<double>(*valInt) >= *minDouble && static_cast<double>(*valInt) < *maxDouble);
					if (auto maxInt = std::get_if<int64_t>(&arg3))
						return (static_cast<double>(*valInt) >= *minDouble && *valInt < *maxInt);
				}
			}
			else if (auto valDouble = std::get_if<double>(&val))
			{
				double minVal = 0.0, maxVal = 0.0;
				if (auto minDouble = std::get_if<double>(&arg2))
					minVal = *minDouble;
				else if (auto minInt = std::get_if<int64_t>(&arg2))
					minVal = static_cast<double>(*minInt);
				else
					return std::monostate{};

				if (auto maxDouble = std::get_if<double>(&arg3))
					maxVal = *maxDouble;
				else if (auto maxInt = std::get_if<int64_t>(&arg3))
					maxVal = static_cast<double>(*maxInt);
				else
					return std::monostate{};

				return (*valDouble >= minVal && *valDouble < maxVal);
			}
			else if (auto valTimestamp = std::get_if<Poco::Timestamp>(&val))
			{
				if (auto minTimestamp = std::get_if<Poco::Timestamp>(&arg2))
				{
					if (auto maxTimestamp = std::get_if<Poco::Timestamp>(&arg3))
						return (*valTimestamp >= *minTimestamp && *valTimestamp < *maxTimestamp);
				}
			}
			else if (auto valString = std::get_if<std::string>(&val))
			{
				if (auto minString = std::get_if<std::string>(&arg2))
				{
					if (auto maxString = std::get_if<std::string>(&arg3))
					{
						int cmpMin, cmpMax;
						if (!ectxt.expr->caseSensitive)
						{
							cmpMin = Poco::icompare(*valString, *minString);
							cmpMax = Poco::icompare(*valString, *maxString);
						}
						else
						{
							cmpMin = valString->compare(*minString);
							cmpMax = valString->compare(*maxString);
						}
						return (cmpMin >= 0 && cmpMax < 0);
					}
				}
			}
			return std::monostate{};
		};

	auto arg1 = args->at(0)->Evaluate(ectxt);
	return applyToScalarOrArray(arg1, inRangeFn);
}

std::string ToStringValue(const ValueType& val)
{
	if (auto intVal = std::get_if<int64_t>(&val))
		return std::to_string(*intVal);
	if (auto doubleVal = std::get_if<double>(&val))
		return std::to_string(*doubleVal);
	if (auto strVal = std::get_if<std::string>(&val))
		return *strVal;
	if (auto boolVal = std::get_if<bool>(&val))
		return *boolVal ? "true" : "false";
	if (auto tsVal = std::get_if<Poco::Timestamp>(&val))
	{
		Poco::LocalDateTime ldt(*tsVal);
		return Poco::DateTimeFormatter::format(ldt, "%Y-%m-%d %H:%M:%S");
	}
	if (auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&val))
	{
		std::string result = "[";
		bool first = true;
		for (const auto& item : **arrayVal)
		{
			if (!first) result += ", ";
			result += ToStringValue(item.value);
			first = false;
		}
		result += "]";
		return result;
	}
	if (auto contentVal = std::get_if<std::shared_ptr<FileContentRef>>(&val))
		return std::string("<FileContent:") + ucr::toUTF8((*contentVal)->path) + ">";
	if (auto regexVal = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&val))
		return "<Regex>";
	if (std::holds_alternative<std::monostate>(val))
		return "<undefined>";
	return "<unknown>";
}

static auto RegexReplaceFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!args || args->size() != 3)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(ectxt);
	auto argPattern = (*args)[1]->Evaluate(ectxt);
	auto argReplacement = (*args)[2]->Evaluate(ectxt);

	const std::string* strPattern = std::get_if<std::string>(&argPattern);
	const auto rePattern = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&argPattern);
	const std::string* replacement = std::get_if<std::string>(&argReplacement);

	if ((!strPattern && !rePattern) || !replacement)
		return std::monostate{};

	auto regexReplaceFn = [&ectxt, strPattern, rePattern, replacement](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};
			try
			{
				if (rePattern)
				{
					std::string result = *strOpt;
					(*rePattern)->subst(result, *replacement, Poco::RegularExpression::RE_GLOBAL);
					return result;
				}
				const int flags = Poco::RegularExpression::RE_UTF8 | (!ectxt.expr->caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
				Poco::RegularExpression regex(*strPattern, flags);
				std::string result = *strOpt;
				regex.subst(result, *replacement, Poco::RegularExpression::RE_GLOBAL);
				return result;
			}
			catch (const Poco::RegularExpressionException&)
			{
				return std::monostate{};
			}
		};

	return applyToScalarOrArray(argStr, regexReplaceFn);
}

static auto LogFunc(int level, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType val;
	std::string msg;

	// Add filter name prefix if available
	if (!ectxt.expr->name.empty())
		msg = "[" + ectxt.expr->name + "] ";

	for (size_t i = 0; i < args->size(); ++i)
	{
		val = args->at(i)->Evaluate(ectxt);
		if (i > 0)
			msg += " ";
		msg += ToStringValue(val);
	}
	ectxt.expr->logger(level, msg);
	return val;
}

static auto LogErrorFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(0, ectxt, args);
}

static auto LogWarnFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(1, ectxt, args);
}

static auto LogInfoFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(2, ectxt, args);
}

static auto IfFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType condition = args->at(0)->Evaluate(ectxt);
	auto condBool = evalAsBool(condition);
	if (condBool && *condBool)
		return args->at(1)->Evaluate(ectxt);
	else
		return args->at(2)->Evaluate(ectxt);
}

static auto IfEachFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType argCond = args->at(0)->Evaluate(ectxt);
	ValueType argTrue = args->at(1)->Evaluate(ectxt);
	ValueType argFalse = args->at(2)->Evaluate(ectxt);

	auto trueArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argTrue);
	auto falseArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argFalse);

	size_t index = 0;
	auto selectValue = [&](const ValueType& cond) -> ValueType
		{
			auto condBool = evalAsBool(cond);
			ValueType trueVal = trueArray ? 
				(index < (*trueArray)->size() ? (*trueArray)->at(index).value : std::monostate{}) : argTrue;
			ValueType falseVal = falseArray ? 
				(index < (*falseArray)->size() ? (*falseArray)->at(index).value : std::monostate{}) : argFalse;
			++index;
			
			if (condBool && *condBool)
				return trueVal;
			else
				return falseVal;
		};

	return applyToScalarOrArray(argCond, selectValue);
}

static auto ChooseFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2)
		return std::monostate{};
	
	ValueType indexVal = args->at(0)->Evaluate(ectxt);
	auto indexInt = std::get_if<int64_t>(&indexVal);
	
	if (!indexInt)
		return std::monostate{};
	
	int64_t idx = *indexInt;
	if (idx < 0)
		idx = 0;
	
	size_t choiceIdx = static_cast<size_t>(idx) + 1;
	if (choiceIdx >= args->size())
		choiceIdx = args->size() - 1;
	
	return args->at(choiceIdx)->Evaluate(ectxt);
}

static auto ChooseEachFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2)
		return std::monostate{};

	ValueType indexVal = args->at(0)->Evaluate(ectxt);

	auto selectFromIndex = [&](const ValueType& val) -> ValueType
		{
			auto indexInt = std::get_if<int64_t>(&val);
			if (!indexInt)
				return std::monostate{};

			int64_t idx = *indexInt;
			if (idx < 0)
				idx = 0;

			size_t choiceIdx = static_cast<size_t>(idx) + 1;
			if (choiceIdx >= args->size())
				choiceIdx = args->size() - 1;

			return args->at(choiceIdx)->Evaluate(ectxt);
		};

	return applyToScalarOrArray(indexVal, selectFromIndex);
}

static std::string removeCommas(const std::string& str)
{
	std::string result;
	result.reserve(str.size());
	for (char c : str)
	{
		if (c != ',')
			result += c;
	}
	return result;
}

static auto ToNumberFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 1)
		return std::monostate{};

	ValueType argVal = args->at(0)->Evaluate(ectxt);

	auto convertToNumber = [](const ValueType& val) -> ValueType
		{
			auto strPtr = std::get_if<std::string>(&val);
			if (!strPtr)
				return std::monostate{};

			std::string cleaned = removeCommas(*strPtr);
			if (cleaned.empty())
				return std::monostate{};

			try
			{
				size_t pos = 0;
				if (cleaned.find('.') != std::string::npos || 
					cleaned.find('e') != std::string::npos || 
					cleaned.find('E') != std::string::npos)
				{
					double d = std::stod(cleaned, &pos);
					if (pos == cleaned.size())
						return d;
				}
				else
				{
					int64_t i = std::stoll(cleaned, &pos);
					if (pos == cleaned.size())
						return i;
				}
			}
			catch (...)
			{
			}
			return std::monostate{};
		};

	return applyToScalarOrArray(argVal, convertToNumber);
}

static auto ToIntFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 1)
		return std::monostate{};

	ValueType argVal = args->at(0)->Evaluate(ectxt);

	auto convertToInt = [](const ValueType& val) -> ValueType
		{
			auto strPtr = std::get_if<std::string>(&val);
			if (!strPtr)
				return std::monostate{};

			std::string cleaned = removeCommas(*strPtr);
			if (cleaned.empty())
				return std::monostate{};

			try
			{
				size_t pos = 0;
				int64_t i = std::stoll(cleaned, &pos);
				if (pos == cleaned.size())
					return i;
			}
			catch (...)
			{
			}
			return std::monostate{};
		};

	return applyToScalarOrArray(argVal, convertToInt);
}

static auto ToDoubleFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 1)
		return std::monostate{};

	ValueType argVal = args->at(0)->Evaluate(ectxt);

	auto convertToDouble = [](const ValueType& val) -> ValueType
		{
			auto strPtr = std::get_if<std::string>(&val);
			if (!strPtr)
				return std::monostate{};

			std::string cleaned = removeCommas(*strPtr);
			if (cleaned.empty())
				return std::monostate{};

			try
			{
				size_t pos = 0;
				double d = std::stod(cleaned, &pos);
				if (pos == cleaned.size())
					return d;
			}
			catch (...)
			{
			}
			return std::monostate{};
		};

	return applyToScalarOrArray(argVal, convertToDouble);
}

static auto ToDateTimeFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 1)
		return std::monostate{};

	ValueType argVal = args->at(0)->Evaluate(ectxt);

	auto convertToDate = [](const ValueType& val) -> ValueType
		{
			auto strPtr = std::get_if<std::string>(&val);
			if (!strPtr || strPtr->empty())
				return std::monostate{};

			auto timestamp = ParseDateTime(*strPtr);
			return timestamp ? *timestamp : ValueType(std::monostate{});
		};

	return applyToScalarOrArray(argVal, convertToDate);
}

template<typename BinaryOp>
static auto BinaryLogicalEachFunc(const ValueType& arg1, const ValueType& arg2, BinaryOp op) -> ValueType
{
	auto arg1Array = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg1);
	auto arg2Array = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg2);
	
	if (!arg1Array && !arg2Array)
	{
		auto bool1 = evalAsBool(arg1);
		auto bool2 = evalAsBool(arg2);
		if (bool1 && bool2)
			return op(*bool1, *bool2);
		return std::monostate{};
	}
	
	auto result = std::make_shared<std::vector<ValueType2>>();
	
	if (arg1Array && arg2Array)
	{
		size_t maxSize = (std::max)((*arg1Array)->size(), (*arg2Array)->size());
		for (size_t i = 0; i < maxSize; ++i)
		{
			ValueType val1 = i < (*arg1Array)->size() ? (*arg1Array)->at(i).value : std::monostate{};
			ValueType val2 = i < (*arg2Array)->size() ? (*arg2Array)->at(i).value : std::monostate{};
			
			auto bool1 = evalAsBool(val1);
			auto bool2 = evalAsBool(val2);
			
			if (bool1 && bool2)
				result->emplace_back(ValueType2{ op(*bool1, *bool2) });
			else
				result->emplace_back(ValueType2{ std::monostate{} });
		}
	}
	else if (arg1Array)
	{
		auto bool2 = evalAsBool(arg2);
		for (const auto& item : **arg1Array)
		{
			auto bool1 = evalAsBool(item.value);
			if (bool1 && bool2)
				result->emplace_back(ValueType2{ op(*bool1, *bool2) });
			else
				result->emplace_back(ValueType2{ std::monostate{} });
		}
	}
	else
	{
		auto bool1 = evalAsBool(arg1);
		for (const auto& item : **arg2Array)
		{
			auto bool2 = evalAsBool(item.value);
			if (bool1 && bool2)
				result->emplace_back(ValueType2{ op(*bool1, *bool2) });
			else
				result->emplace_back(ValueType2{ std::monostate{} });
		}
	}
	
	return result;
}

static auto AndEachFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(ectxt);
	ValueType arg2 = args->at(1)->Evaluate(ectxt);
	return BinaryLogicalEachFunc(arg1, arg2, [](bool a, bool b) { return a && b; });
}

static auto OrEachFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(ectxt);
	ValueType arg2 = args->at(1)->Evaluate(ectxt);
	return BinaryLogicalEachFunc(arg1, arg2, [](bool a, bool b) { return a || b; });
}

static auto NotEachFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(ectxt);
	
	auto notFunc = [](const ValueType& val) -> ValueType
		{
			auto boolVal = evalAsBool(val);
			if (boolVal)
				return !*boolVal;
			return std::monostate{};
		};
	
	return applyToScalarOrArray(arg, notFunc);
}

static auto NormalizeUnicodeFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	// Default normalization form: NFC (Normalization Form C)
	ucr::NORMFORM normForm = ucr::NormC;
	
	// If a second argument is provided, it specifies the normalization form
	if (args->size() > 1)
	{
		ValueType arg2 = args->at(1)->Evaluate(ectxt);
		if (auto formStr = std::get_if<std::string>(&arg2))
		{
			std::string form = Poco::toUpper(*formStr);
			if (form == "NFC" || form == "C")
				normForm = ucr::NormC;
			else if (form == "NFD" || form == "D")
				normForm = ucr::NormD;
			else if (form == "NFKC" || form == "KC")
				normForm = ucr::NormKC;
			else if (form == "NFKD" || form == "KD")
				normForm = ucr::NormKD;
			else
				return std::monostate{}; // Invalid normalization form
		}
		else if (auto formInt = std::get_if<int64_t>(&arg2))
		{
			// Support numeric form: 1=NFC, 2=NFD, 5=NFKC, 6=NFKD
			switch (*formInt)
			{
			case 1: normForm = ucr::NormC; break;
			case 2: normForm = ucr::NormD; break;
			case 5: normForm = ucr::NormKC; break;
			case 6: normForm = ucr::NormKD; break;
			default: return std::monostate{}; // Invalid form number
			}
		}
	}
	
	ValueType arg = args->at(0)->Evaluate(ectxt);
	
	auto normalizeUnicodeFunc = [normForm](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};
			// Convert UTF-8 string to String (tchar_t)
			String tstr = ucr::toTString(*strOpt);
			// Use ucr::normalizeString for Unicode normalization
			String normalized = ucr::normalizeString(tstr, normForm);
			// Convert back to UTF-8
			return ucr::toUTF8(normalized);
		};
	
	return applyToScalarOrArray(arg, normalizeUnicodeFunc);
}

static auto ToXFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args, String (*func)(const String&)) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(ectxt);
	auto toXFunc = [func](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};
			return ucr::toUTF8(func(ucr::toTString(*strOpt)));
		};
	
	return applyToScalarOrArray(arg, toXFunc);
}

static auto ToLowerFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toLower);
}

static auto ToUpperFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toUpper);
}

static auto ToHalfWidthFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toHalfWidth);
}

static auto ToFullWidthFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toFullWidth);
}

static auto ToHiraganaFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toHiragana);
}

static auto ToKatakanaFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toKatakana);
}

static auto ToSimplifiedChineseFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toSimplifiedChinese);
}

static auto ToTraditionalChineseFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return ToXFunc(ectxt, args, ucr::toTraditionalChinese);
}

static auto TrimFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(ectxt);

	auto trimFunc = [](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};

			const std::string& str = *strOpt;
			auto start = str.find_first_not_of(" \t\r\n");
			if (start == std::string::npos)
				return std::string{};
			auto end = str.find_last_not_of(" \t\r\n");
			return str.substr(start, end - start + 1);
		};

	return applyToScalarOrArray(arg, trimFunc);
}

static auto TrimLeftFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(ectxt);

	auto trimLeftFunc = [](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};

			const std::string& str = *strOpt;
			auto start = str.find_first_not_of(" \t\r\n");
			if (start == std::string::npos)
				return std::string{};
			return str.substr(start);
		};

	return applyToScalarOrArray(arg, trimLeftFunc);
}

static auto TrimRightFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(ectxt);

	auto trimRightFunc = [](const ValueType& val) -> ValueType
		{
			auto strOpt = getAsString(val);
			if (!strOpt)
				return std::monostate{};

			const std::string& str = *strOpt;
			auto end = str.find_last_not_of(" \t\r\n");
			if (end == std::string::npos)
				return std::string{};
			return str.substr(0, end + 1);
		};

	return applyToScalarOrArray(arg, trimRightFunc);
}

static const auto& GetOrCreateMatchRanges(const FilterEvalContext& ectxt, ExprNode* expr)
{
	auto& matchRanges = ectxt.sharedContext->matchRanges;

	auto it = matchRanges.find(expr);
	if (it == matchRanges.end())
	{
		auto rawRanges = ExtractRanges(
			ectxt.provider->GetLineCount(),
			[&](int i)
			{
				FilterEvalContext lectxt = ectxt;
				lectxt.lineIndex = i;
				auto result = expr->Evaluate(lectxt);
				return evalAsBool(result).value_or(false);
			});

		// Convert to RangeInfo with block indices
		std::vector<RangeInfo> ranges;
		ranges.reserve(rawRanges.size());
		for (size_t i = 0; i < rawRanges.size(); ++i)
		{
			RangeInfo info;
			info.start = rawRanges[i].first;
			info.end = rawRanges[i].second;
			info.blockIndex = static_cast<int>(i + 1); // 1-based
			ranges.push_back(info);
		}

		it = matchRanges.insert_or_assign(expr, std::move(ranges)).first;
	}

	return it->second;
}

static const auto& GetOrCreateInsideRanges(const FilterEvalContext& ectxt, ExprNode* beginExpr, ExprNode* endExpr)
{
	auto& cache = ectxt.sharedContext->insideRanges;

	InsideKey key{ beginExpr, endExpr };

	auto it = cache.find(key);
	if (it != cache.end())
		return it->second;

	std::vector<Range> ranges;

	bool inside = false;
	int start = -1;

	const int lineCount =
		ectxt.provider->GetLineCount();

	for (int i = 0; i < lineCount; ++i)
	{
		auto local = ectxt;
		local.lineIndex = i;

		bool isBegin =
			evalAsBool(beginExpr->Evaluate(local))
			.value_or(false);

		bool isEnd =
			evalAsBool(endExpr->Evaluate(local))
			.value_or(false);

		if (inside && isEnd)
		{
			ranges.emplace_back(start, i);
			inside = false;
			if (isBegin)
			{
				inside = true;
				start = i;
			}
		}
		else if (!inside && isBegin)
		{
			inside = true;
			start = i;
		}
	}

	if (inside)
		ranges.emplace_back(start, lineCount - 1);

	it = cache.insert_or_assign(
		key,
		std::move(ranges)).first;

	return it->second;
}

static auto LineMatchContextFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	auto argBefore = (*args)[1]->Evaluate(ectxt);
	auto argAfter = (*args)[2]->Evaluate(ectxt);
	auto argBeforeInt = std::get_if<int64_t>(&argBefore);
	auto argAfterInt = std::get_if<int64_t>(&argAfter);
	if (!argBeforeInt || !argAfterInt)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	auto distances = GetDistancesToRanges(ectxt.lineIndex, ranges);
	if ((distances.first >= 0 && distances.first <= *argAfterInt) ||
		(distances.second >= 0 && distances.second <= *argBeforeInt))
		return true;
	return false;
}

static auto CountFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	int64_t matchCount = 0;
	for (const auto& range : ranges)
		matchCount += range.end - range.start + 1;

	return matchCount;
}

static auto LineMatchNumberFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	int64_t matchIndex = 0;
	for (const auto& range : ranges)
	{
		if (range.start <= ectxt.lineIndex && ectxt.lineIndex <= range.end)
			return matchIndex + (ectxt.lineIndex - range.start) + 1;
		matchIndex += range.end - range.start + 1;
	}

	return std::monostate{};
}

static auto LineMatchDistanceExFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args, int direction) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	auto distances = GetDistancesToRanges(ectxt.lineIndex, ranges);
	if (direction == -1)
	{
		if (distances.first < 0)
			return std::monostate{};
		return distances.first;
	}
	else if (direction == 1)
	{
		if (distances.second < 0)
			return std::monostate{};
		return distances.second;
	}
	if (distances.first < 0 && distances.second < 0)
		return std::monostate{};
	if (distances.first >= 0 && distances.second < 0)
		return distances.first;
	if (distances.first < 0 && distances.second >= 0)
		return distances.second;
	return (std::min)(distances.first, distances.second);
}

static auto LineMatchDistanceFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LineMatchDistanceExFunc(ectxt, args, 0);
}

static auto LineMatchDistanceBeforeFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LineMatchDistanceExFunc(ectxt, args, -1);
}

static auto LineMatchDistanceAfterFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	return LineMatchDistanceExFunc(ectxt, args, 1);
}

static auto LineMatchInsideFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateInsideRanges(ectxt, (*args)[0], (*args)[1]);

	for (const auto& range : ranges)
	{
		if (range.first <= ectxt.lineIndex && ectxt.lineIndex <= range.second)
			return true;
	}

	return false;
}

// Helper lambda to process a single value and update statistics
static auto ProcessValueForStatistics(
	const ValueType& result,
	const FilterEvalContext& ectxt,
	double& sum,
	int64_t& count,
	std::optional<double>& maxNumber,
	std::optional<double>& minNumber,
	std::optional<std::string>& maxString,
	std::optional<std::string>& minString,
	std::optional<Poco::Timestamp>& maxTimestamp,
	std::optional<Poco::Timestamp>& minTimestamp,
	int64_t& trueCount,
	int64_t& falseCount,
	int& valueType)
{
	// Handle boolean values
	if (auto boolVal = std::get_if<bool>(&result))
	{
		++count;
		if (*boolVal)
			++trueCount;
		else
			++falseCount;
		if (valueType == 0) valueType = 4;
	}
	// Handle numeric values
	else if (auto intVal = std::get_if<int64_t>(&result))
	{
		double value = static_cast<double>(*intVal);
		sum += value;
		++count;
		if (!maxNumber.has_value() || value > *maxNumber)
			maxNumber = value;
		if (!minNumber.has_value() || value < *minNumber)
			minNumber = value;
		if (valueType == 0) valueType = 1;
	}
	else if (auto doubleVal = std::get_if<double>(&result))
	{
		sum += *doubleVal;
		++count;
		if (!maxNumber.has_value() || *doubleVal > *maxNumber)
			maxNumber = *doubleVal;
		if (!minNumber.has_value() || *doubleVal < *minNumber)
			minNumber = *doubleVal;
		if (valueType == 0) valueType = 1;
	}
	// Handle string values
	else if (auto strVal = std::get_if<std::string>(&result))
	{
		++count;
		if (!maxString.has_value())
		{
			maxString = *strVal;
			minString = *strVal;
		}
		else
		{
			int cmpMax = ectxt.expr->caseSensitive ? 
				strVal->compare(*maxString) : Poco::icompare(*strVal, *maxString);
			int cmpMin = ectxt.expr->caseSensitive ? 
				strVal->compare(*minString) : Poco::icompare(*strVal, *minString);
			if (cmpMax > 0) maxString = *strVal;
			if (cmpMin < 0) minString = *strVal;
		}
		if (valueType == 0) valueType = 2;
	}
	// Handle timestamp values
	else if (auto tsVal = std::get_if<Poco::Timestamp>(&result))
	{
		++count;
		if (!maxTimestamp.has_value() || *tsVal > *maxTimestamp)
			maxTimestamp = *tsVal;
		if (!minTimestamp.has_value() || *tsVal < *minTimestamp)
			minTimestamp = *tsVal;
		if (valueType == 0) valueType = 3;
	}
};

static const StatisticsResult& GetOrCreateStatistics(const FilterEvalContext& ectxt, ExprNode* expr, ExprNode* conditionExpr = nullptr)
{
	auto& statsCache = ectxt.sharedContext->statistics;

	// Create cache key that includes condition expression (if any)
	std::pair<ExprNode*, ExprNode*> cacheKey{ expr, conditionExpr };
	auto it = statsCache.find(cacheKey);
	if (it != statsCache.end())
		return it->second;

	if (!ectxt.provider)
	{
		static StatisticsResult emptyResult;
		return emptyResult;
	}

	const int lineCount = ectxt.provider->GetLineCount();
	double sum = 0.0;
	int64_t count = 0;
	std::optional<double> maxNumber;
	std::optional<double> minNumber;
	std::optional<std::string> maxString;
	std::optional<std::string> minString;
	std::optional<Poco::Timestamp> maxTimestamp;
	std::optional<Poco::Timestamp> minTimestamp;
	int64_t trueCount = 0;
	int64_t falseCount = 0;
	int valueType = 0;

	for (int i = 0; i < lineCount; ++i)
	{
		FilterEvalContext lectxt = ectxt;
		lectxt.lineIndex = i;

		// Check condition if provided
		if (conditionExpr)
		{
			ValueType condResult = conditionExpr->Evaluate(lectxt);
			auto condBool = evalAsBool(condResult);
			if (!condBool || !*condBool)
				continue;
		}

		ValueType result = expr->Evaluate(lectxt);

		// Handle array values - process each element
		if (auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&result))
		{
			for (const auto& item : **arrayVal)
			{
				ProcessValueForStatistics(
					item.value, lectxt, sum, count,
					maxNumber, minNumber, maxString, minString,
					maxTimestamp, minTimestamp, trueCount, falseCount, valueType);
			}
		}
		else
		{
			// Handle single value
			ProcessValueForStatistics(
				result, lectxt, sum, count,
				maxNumber, minNumber, maxString, minString,
				maxTimestamp, minTimestamp, trueCount, falseCount, valueType);
		}
	}

	StatisticsResult stats;
	stats.count = count;
	stats.valueType = valueType;
	if (count > 0)
	{
		if (valueType == 1 && maxNumber.has_value())
		{
			stats.sum = sum;
			stats.average = sum / static_cast<double>(count);
			stats.maxNumber = *maxNumber;
			stats.minNumber = *minNumber;
		}
		else if (valueType == 2 && maxString.has_value())
		{
			stats.maxString = *maxString;
			stats.minString = *minString;
		}
		else if (valueType == 3 && maxTimestamp.has_value())
		{
			stats.maxTimestamp = *maxTimestamp;
			stats.minTimestamp = *minTimestamp;
		}
		else if (valueType == 4)
		{
			stats.trueCount = trueCount;
			stats.falseCount = falseCount;
		}
	}

	it = statsCache.insert_or_assign(cacheKey, std::move(stats)).first;
	return it->second;
}

static auto AverageFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	ExprNode* conditionExpr = (args->size() > 1) ? (*args)[1] : nullptr;
	const auto& stats = GetOrCreateStatistics(ectxt, (*args)[0], conditionExpr);

	if (stats.count == 0)
		return std::monostate{};

	if (stats.valueType == 1)
		return stats.average;
	else if (stats.valueType == 4)
		return static_cast<double>(stats.trueCount) / static_cast<double>(stats.count);

	return std::monostate{};
}

static auto MaximumFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	ExprNode* conditionExpr = (args->size() > 1) ? (*args)[1] : nullptr;
	const auto& stats = GetOrCreateStatistics(ectxt, (*args)[0], conditionExpr);

	if (stats.count == 0)
		return std::monostate{};

	if (stats.valueType == 1)
	{
		// Return as int64_t if the result is an integer value
		if (stats.maxNumber == std::floor(stats.maxNumber))
			return static_cast<int64_t>(stats.maxNumber);
		return stats.maxNumber;
	}
	else if (stats.valueType == 2)
	{
		return stats.maxString;
	}
	else if (stats.valueType == 3)
	{
		return stats.maxTimestamp;
	}
	else if (stats.valueType == 4)
	{
		return stats.trueCount > 0 ? true : false;
	}

	return std::monostate{};
}

static auto MinimumFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	ExprNode* conditionExpr = (args->size() > 1) ? (*args)[1] : nullptr;
	const auto& stats = GetOrCreateStatistics(ectxt, (*args)[0], conditionExpr);

	if (stats.count == 0)
		return std::monostate{};

	if (stats.valueType == 1)
	{
		// Return as int64_t if the result is an integer value
		if (stats.minNumber == std::floor(stats.minNumber))
			return static_cast<int64_t>(stats.minNumber);
		return stats.minNumber;
	}
	else if (stats.valueType == 2)
	{
		return stats.minString;
	}
	else if (stats.valueType == 3)
	{
		return stats.minTimestamp;
	}
	else if (stats.valueType == 4)
	{
		return stats.falseCount > 0 ? false : true;
	}

	return std::monostate{};
}

static auto SumFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	ExprNode* conditionExpr = (args->size() > 1) ? (*args)[1] : nullptr;
	const auto& stats = GetOrCreateStatistics(ectxt, (*args)[0], conditionExpr);

	if (stats.count == 0)
		return std::monostate{};

	if (stats.valueType == 1)
	{
		// Return as int64_t if the result is an integer value
		if (stats.sum == std::floor(stats.sum))
			return static_cast<int64_t>(stats.sum);
		return stats.sum;
	}
	else if (stats.valueType == 4)
	{
		return stats.trueCount;
	}

	return std::monostate{};
}

static auto MatchBlockNumberFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	// Find which block contains the current line
	for (const auto& range : ranges)
	{
		if (range.start <= ectxt.lineIndex && ectxt.lineIndex <= range.end)
			return static_cast<int64_t>(range.blockIndex);
	}

	return std::monostate{}; // Line is not in any block
}

static auto BlockCountFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);
	return static_cast<int64_t>(ranges.size());
}

static auto MatchBlockSizeFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	// Find which block contains the current line and calculate its size
	for (const auto& range : ranges)
	{
		if (range.start <= ectxt.lineIndex && ectxt.lineIndex <= range.end)
			return static_cast<int64_t>(range.end - range.start + 1);
	}

	return std::monostate{}; // Line is not in any block
}

static auto MatchBlockOffsetFunc(const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	const auto& ranges = GetOrCreateMatchRanges(ectxt, (*args)[0]);

	// Find which block contains the current line and calculate offset
	for (const auto& range : ranges)
	{
		if (range.start <= ectxt.lineIndex && ectxt.lineIndex <= range.end)
			return static_cast<int64_t>(ectxt.lineIndex - range.start + 1); // 1-based
	}

	return std::monostate{}; // Line is not in any block
}

struct FunctionInfo
{
	using FuncPtr = auto (*)(const FilterEvalContext&, std::vector<ExprNode*>*) -> ValueType;
	const char* name;
	FuncPtr func;
	int minArgs;
	int maxArgs;
};

// Compile-time constant array - zero runtime initialization cost
static constexpr FunctionInfo functionTable[] = {
	{"abs", AbsFunc, 1, 1},
	{"allequal", AllEqualFunc, 1, -1},
	{"allof", AllOfFunc, 1, 1},
	{"andeach", AndEachFunc, 2, 2},
	{"anyof", AnyOfFunc, 1, 1},
	{"array", ArrayFunc, 0, -1},
	{"at", AtFunc, 2, 2},
	{"average", AverageFunc, 1, 2},
	{"blockcount", BlockCountFunc, 1, 1},
	{"choose", ChooseFunc, 2, -1},
	{"chooseeach", ChooseEachFunc, 2, -1},
	{"count", CountFunc, 1, 1},
	{"if", IfFunc, 3, 3},
	{"ifeach", IfEachFunc, 3, 3},
	{"inrange", InRangeFunc, 3, 3},
	{"iswithin", IsWithinFunc, 3, 3},
	{"len", LenFunc, 1, 1},
	{"linecount", LineCountFunc, 1, 1},
	{"logerror", LogErrorFunc, 1, -1},
	{"loginfo", LogInfoFunc, 1, -1},
	{"logwarn", LogWarnFunc, 1, -1},
	{"matchblocknumber", MatchBlockNumberFunc, 1, 1},
	{"matchblockoffset", MatchBlockOffsetFunc, 1, 1},
	{"matchblocksize", MatchBlockSizeFunc, 1, 1},
	{"matchcontext", LineMatchContextFunc, 3, 3},
	{"matchdistance", LineMatchDistanceFunc, 1, 1},
	{"matchdistanceafter", LineMatchDistanceAfterFunc, 1, 1},
	{"matchdistancebefore", LineMatchDistanceBeforeFunc, 1, 1},
	{"matchinside", LineMatchInsideFunc, 2, 2},
	{"matchnumber", LineMatchNumberFunc, 1, 1},
	{"maximum", MaximumFunc, 1, 2},
	{"minimum", MinimumFunc, 1, 2},
	{"normalizeunicode", NormalizeUnicodeFunc, 1, 2},
	{"noteach", NotEachFunc, 1, 1},
	{"now", NowFunc, 0, 0},
	{"oreach", OrEachFunc, 2, 2},
	{"regexreplace", RegexReplaceFunc, 3, 3},
	{"regexreplacewithlist", RegexReplaceWithListFunc, 2, 2},
	{"replace", ReplaceFunc, 3, 3},
	{"replacewithlist", ReplaceWithListFunc, 2, 2},
	{"startofmonth", StartOfMonthFunc, 1, 1},
	{"startofweek", StartOfWeekFunc, 1, 1},
	{"startofyear", StartOfYearFunc, 1, 1},
	{"strlen", StrlenFunc, 1, 1},
	{"sublines", SublinesFunc, 2, 3},
	{"substr", SubstrFunc, 2, 3},
	{"sum", SumFunc, 1, 2},
	{"todatestr", ToDateStrFunc, 1, 1},
	{"todatetime", ToDateTimeFunc, 1, 1},
	{"today", TodayFunc, 0, 0},
	{"todouble", ToDoubleFunc, 1, 1},
	{"tofullwidth", ToFullWidthFunc, 1, 1},
	{"tohalfwidth", ToHalfWidthFunc, 1, 1},
	{"tohiragana", ToHiraganaFunc, 1, 1},
	{"toint", ToIntFunc, 1, 1},
	{"tokatakana", ToKatakanaFunc, 1, 1},
	{"tolower", ToLowerFunc, 1, 1},
	{"tonumber", ToNumberFunc, 1, 1},
	{"tosimplifiedchinese", ToSimplifiedChineseFunc, 1, 1},
	{"totraditionalchinese", ToTraditionalChineseFunc, 1, 1},
	{"toupper", ToUpperFunc, 1, 1},
	{"trim", TrimFunc, 1, 1},
	{"trimleft", TrimLeftFunc, 1, 1},
	{"trimright", TrimRightFunc, 1, 1},
};

static constexpr size_t functionTableSize = sizeof(functionTable) / sizeof(functionTable[0]);

// Binary search in sorted array - O(log n)
static const FunctionInfo* findFunction(const std::string& name)
{
	auto it = std::lower_bound(std::begin(functionTable), std::end(functionTable), name,
		[](const FunctionInfo& info, const std::string& n) {
			return strcmp(info.name, n.c_str()) < 0;
		}
	);
	
	if (it != std::end(functionTable) && strcmp(it->name, name.c_str()) == 0)
		return it;
	
	return nullptr;
}

static auto lineAt(int index, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	auto arg = (*args)[0]->Evaluate(ectxt);
	auto argInt = std::get_if<int64_t>(&arg);
	if (!argInt)
		return std::monostate{};
	int lineNumber = static_cast<int>(*argInt);
	if (lineNumber < 1 || lineNumber > ectxt.provider->GetLineCount())
		return std::monostate{};
	return ectxt.provider->GetLine(index, lineNumber - 1);
}

static auto lineOffsetAt(int index, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};
	auto arg = (*args)[0]->Evaluate(ectxt);
	auto argInt = std::get_if<int64_t>(&arg);
	if (!argInt)
		return std::monostate{};
	int lineNumber = static_cast<int>(ectxt.lineIndex + 1 + *argInt);
	if (lineNumber < 1 || lineNumber > ectxt.provider->GetLineCount())
		return std::monostate{};
	return ectxt.provider->GetLine(index, lineNumber - 1);
}

static auto columnFunc(int index, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	auto argColumnSpec = (*args)[0]->Evaluate(ectxt);

	// Integer: treat as column index (1-based)
	if (auto argInt = std::get_if<int64_t>(&argColumnSpec))
	{
		int columnIndex = static_cast<int>(*argInt) - 1;
		return ColumnField(index, columnIndex, ectxt);
	}
	// String: treat as column name (case-sensitivity respects @cs directive)
	else if (auto argStr = std::get_if<std::string>(&argColumnSpec))
	{
		return ColumnFieldByName(index, *argStr, ectxt);
	}

	return std::monostate{};
}

static auto columnAt(int index, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	// First argument: line number (1-based)
	auto argLine = (*args)[0]->Evaluate(ectxt);
	auto argLineInt = std::get_if<int64_t>(&argLine);
	if (!argLineInt)
		return std::monostate{};
	int lineNumber = static_cast<int>(*argLineInt);
	if (lineNumber < 1 || lineNumber > ectxt.provider->GetLineCount())
		return std::monostate{};
	int lineIndex = lineNumber - 1;

	// Second argument: column index (1-based) or column name (string)
	auto argColumnSpec = (*args)[1]->Evaluate(ectxt);

	// Integer: treat as column index (1-based)
	if (auto argInt = std::get_if<int64_t>(&argColumnSpec))
	{
		int columnIndex = static_cast<int>(*argInt) - 1;
		if (columnIndex < 0 || columnIndex >= ectxt.provider->GetColumnCount(index, lineIndex))
			return std::monostate{};
		return ectxt.provider->GetColumn(index, lineIndex, columnIndex);
	}
	// String: treat as column name (case-sensitivity respects @cs directive)
	else if (auto argStr = std::get_if<std::string>(&argColumnSpec))
	{
		int columnIndex = GetColumnIndexByName(index, *argStr, ectxt);
		if (columnIndex < 0)
			return std::monostate{};
		if (columnIndex >= ectxt.provider->GetColumnCount(index, lineIndex))
			return std::monostate{};
		return ectxt.provider->GetColumn(index, lineIndex, columnIndex);
	}

	return std::monostate{};
}

static auto columnOffsetAt(int index, const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
{
	if (!ectxt.provider)
		return std::monostate{};

	// First argument: line offset
	auto argOffset = (*args)[0]->Evaluate(ectxt);
	auto argOffsetInt = std::get_if<int64_t>(&argOffset);
	if (!argOffsetInt)
		return std::monostate{};
	int lineNumber = static_cast<int>(ectxt.lineIndex + 1 + *argOffsetInt);
	if (lineNumber < 1 || lineNumber > ectxt.provider->GetLineCount())
		return std::monostate{};
	int lineIndex = lineNumber - 1;

	// Second argument: column index (1-based) or column name (string)
	auto argColumnSpec = (*args)[1]->Evaluate(ectxt);

	// Integer: treat as column index (1-based)
	if (auto argInt = std::get_if<int64_t>(&argColumnSpec))
	{
		int columnIndex = static_cast<int>(*argInt) - 1;
		if (columnIndex < 0 || columnIndex >= ectxt.provider->GetColumnCount(index, lineIndex))
			return std::monostate{};
		return ectxt.provider->GetColumn(index, lineIndex, columnIndex);
	}
	// String: treat as column name (case-sensitivity respects @cs directive)
	else if (auto argStr = std::get_if<std::string>(&argColumnSpec))
	{
		int columnIndex = GetColumnIndexByName(index, *argStr, ectxt);
		if (columnIndex < 0)
			return std::monostate{};
		if (columnIndex >= ectxt.provider->GetColumnCount(index, lineIndex))
			return std::monostate{};
		return ectxt.provider->GetColumn(index, lineIndex, columnIndex);
	}

	return std::monostate{};
}

void FunctionNode::SetLineAtFunc(int side, int prefixlen, ValueType (*proc)(int, const FilterEvalContext&, std::vector<ExprNode*>*))
{
	if (!args || args->size() != 1)
		throw std::invalid_argument(functionName + " function requires 1 argument");
	if (prefixlen == 0)
		func = [side, proc](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
		{
			std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
			const int dirs = ectxt.expr->ctxt->GetCompareDirs();
			for (int i = 0; i < dirs; ++i)
				values->emplace_back(ValueType2{ proc(i, ectxt, args) });
			return values;
		};
	else 
		func = [side, proc](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
			{ return proc((side < 0) ? (ectxt.expr->ctxt->GetCompareDirs() - 1) : side, ectxt, args); };
}

void FunctionNode::SetColumnFunc(int side, int prefixlen)
{
	if (!args || args->size() != 1)
		throw std::invalid_argument(functionName + " function requires 1 argument");
	if (prefixlen == 0)
		func = [](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
		{
			std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
			const int dirs = ectxt.expr->ctxt->GetCompareDirs();
			for (int i = 0; i < dirs; ++i)
				values->emplace_back(ValueType2{ columnFunc(i, ectxt, args) });
			return values;
		};
	else 
		func = [side](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
			{ return columnFunc((side < 0) ? (ectxt.expr->ctxt->GetCompareDirs() - 1) : side, ectxt, args); };
}

void FunctionNode::SetColumnAtFunc(int side, int prefixlen, ValueType (*proc)(int, const FilterEvalContext&, std::vector<ExprNode*>*))
{
	if (!args || args->size() != 2)
		throw std::invalid_argument(functionName + " function requires 2 arguments");
	if (prefixlen == 0)
		func = [side, proc](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
		{
			std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
			const int dirs = ectxt.expr->ctxt->GetCompareDirs();
			for (int i = 0; i < dirs; ++i)
				values->emplace_back(ValueType2{ proc(i, ectxt, args) });
			return values;
		};
	else 
		func = [side, proc](const FilterEvalContext& ectxt, std::vector<ExprNode*>* args) -> ValueType
			{ return proc((side < 0) ? (ectxt.expr->ctxt->GetCompareDirs() - 1) : side, ectxt, args); };
}

FunctionNode::FunctionNode(const FilterExpression* ctxt, const std::string& name, std::vector<ExprNode*>* args)
	: ctxt(ctxt), functionName(Poco::toLower(name)), args(args)
{
	auto [side, prefixlen] = ParseAttributeName(functionName);

	// Special handling for prop, lineat, lineoffsetat functions
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "prop") == 0)
	{
		SetPropFunc(side, prefixlen);
		return;
	}
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "lineat") == 0)
	{
		SetLineAtFunc(side, prefixlen, lineAt);
		return;
	}
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "lineoffsetat") == 0)
	{
		SetLineAtFunc(side, prefixlen, lineOffsetAt);
		return;
	}
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "columnat") == 0)
	{
		SetColumnAtFunc(side, prefixlen, columnAt);
		return;
	}
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "columnoffsetat") == 0)
	{
		SetColumnAtFunc(side, prefixlen, columnOffsetAt);
		return;
	}
	if (functionName.compare(prefixlen, functionName.length() - prefixlen, "column") == 0)
	{
		SetColumnFunc(side, prefixlen);
		return;
	}

	// Look up function in table
	const FunctionInfo* info = findFunction(functionName);
	if (!info)
		throw std::runtime_error("Unknown function: " + functionName);
	
	// Validate argument count
	size_t argCount = args ? args->size() : 0;
	if (info->minArgs >= 0 && argCount < static_cast<size_t>(info->minArgs))
	{
		throw std::invalid_argument(functionName + " function requires at least " + 
			std::to_string(info->minArgs) + " argument" + (info->minArgs != 1 ? "s" : ""));
	}
	if (info->maxArgs >= 0 && argCount > static_cast<size_t>(info->maxArgs))
	{
		throw std::invalid_argument(functionName + " function requires at most " + 
			std::to_string(info->maxArgs) + " argument" + (info->maxArgs != 1 ? "s" : ""));
	}

	func = info->func;
}

FunctionNode::~FunctionNode()
{
	if (args)
	{
		for (auto arg : *args)
		{
			delete arg;
		}
	}
	delete args;
}

ExprNode* FunctionNode::Optimize()
{
	DIFFITEM di;
	FilterEvalContext ectxt{ ctxt, &di };
	if (args)
	{
		for (auto& arg : *args)
		{
			if (arg)
				arg = arg->Optimize();
		}
	}
	if (functionName == "now")
	{
		auto* result = new DateTimeLiteral(*ctxt->now);
		delete this;
		return result;
	}
	else if (functionName == "today")
	{
		auto* result = new DateTimeLiteral(*ctxt->today);
		delete this;
		return result;
	}
	else if (functionName == "abs")
	{
		if (auto intLit = args ? dynamic_cast<IntLiteral*>((*args)[0]) : nullptr)
		{
			auto* result = new IntLiteral(std::get<int64_t>(func(ectxt, args)));
			delete this;
			return result;
		}
		if (auto doubleLit = args ? dynamic_cast<DoubleLiteral*>((*args)[0]) : nullptr)
		{
			auto* result = new DoubleLiteral(std::get<double>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "array")
	{
		if (args)
		{
			bool isArrayConst = true;
			for (auto& arg : *args)
			{
				bool isConst = false;
				auto intValue = getConstIntValue(arg);
				if (intValue.has_value())
					isConst = true;
				else if (auto doubleLit = args ? dynamic_cast<DoubleLiteral*>(arg) : nullptr)
					isConst = true;
				else if (auto strLit = args ? dynamic_cast<StringLiteral*>(arg) : nullptr)
					isConst = true;
				else if (auto dateLit = args ? dynamic_cast<DateTimeLiteral*>(arg) : nullptr)
					isConst = true;
				else if (auto boolLit = args ? dynamic_cast<BoolLiteral*>(arg) : nullptr)
					isConst = true;
				if (!isConst)
				{
					isArrayConst = false;
					break;
				}
			}
			if (isArrayConst)
			{
				auto result = std::make_shared<std::vector<ValueType2>>();
				for (auto& arg : *args)
				{
					auto intValue = getConstIntValue(arg);
					if (intValue.has_value())
						result->emplace_back(ValueType2{ *intValue });
					else if (auto doubleLit = dynamic_cast<DoubleLiteral*>(arg))
						result->emplace_back(ValueType2{ doubleLit->value });
					else if (auto strLit = dynamic_cast<StringLiteral*>(arg))
						result->emplace_back(ValueType2{ strLit->value });
					else if (auto dateLit = dynamic_cast<DateTimeLiteral*>(arg))
						result->emplace_back(ValueType2{ dateLit->value });
					else if (auto boolLit = dynamic_cast<BoolLiteral*>(arg))
						result->emplace_back(ValueType2{ boolLit->value });
					else
						result->emplace_back(ValueType2{ std::monostate{} });
				}
				delete this;
				return new ArrayLiteral(result);
			}
		}
	}
	else if (functionName == "at")
	{
		if (args)
		{
			if (auto arrayLit = dynamic_cast<ArrayLiteral*>((*args)[0]))
			{
				if (auto intLit = dynamic_cast<IntLiteral*>((*args)[1]))
				{
					int64_t index = intLit->value;
					if (index < 0)
						index += static_cast<int64_t>(arrayLit->value->size());
					if (index >= 0 && index < static_cast<int64_t>(arrayLit->value->size()))
					{
						auto val = (*arrayLit->value)[static_cast<size_t>(index)].value;
						if (auto strVal = std::get_if<std::string>(&val))
						{
							delete this;
							return new StringLiteral(*strVal);
						}
						else if (auto doubleVal = std::get_if<double>(&val))
						{
							delete this;
							return new DoubleLiteral(*doubleVal);
						}
						else if (auto intVal = std::get_if<int64_t>(&val))
						{
							delete this;
							return new IntLiteral(*intVal);
						}
						else if (auto boolVal = std::get_if<bool>(&val))
						{
							delete this;
							return new BoolLiteral(*boolVal);
						}
						else if (auto dtVal = std::get_if<Poco::Timestamp>(&val))
						{
							delete this;
							return new DateTimeLiteral(*dtVal);
						}
					}
				}
			}
		}
	}
	else if (functionName == "strlen")
	{
		if (args && dynamic_cast<StringLiteral*>((*args)[0]))
		{
			auto* result = new IntLiteral(std::get<int64_t>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "substr")
	{
		if (args && dynamic_cast<StringLiteral*>((*args)[0]) && dynamic_cast<IntLiteral*>((*args)[1])
			&& (args->size() == 2 || dynamic_cast<IntLiteral*>((*args)[2])))
		{
			auto* result = new StringLiteral(std::get<std::string>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "replace")
	{
		// Optimize: convert search string to pre-compiled regex for case-insensitive search
		if (args && args->size() >= 3 && dynamic_cast<StringLiteral*>((*args)[1]))
		{
			try
			{
				std::string pattern = escapeRegex(dynamic_cast<StringLiteral*>((*args)[1])->value);
				auto* re = new RegularExpressionLiteral(pattern, ctxt->caseSensitive);
				delete (*args)[1];
				(*args)[1] = re;
			}
			catch (const Poco::RegularExpressionException&)
			{
				// Invalid pattern, cannot optimize (keep as string)
				return this;
			}
		}
		// If all arguments are literals, evaluate at compile time
		if (args && args->size() >= 3 && dynamic_cast<StringLiteral*>((*args)[0]) && dynamic_cast<RegularExpressionLiteral*>((*args)[1]) && dynamic_cast<StringLiteral*>((*args)[2]))
		{
			auto* result = new StringLiteral(std::get<std::string>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "regexreplace")
	{
		if (args && args->size() >= 3 && dynamic_cast<StringLiteral*>((*args)[1]))
		{
			try
			{
				auto* re = new RegularExpressionLiteral(dynamic_cast<StringLiteral*>((*args)[1])->value, ctxt->caseSensitive);
				delete (*args)[1];
				(*args)[1] = re;
			}
			catch (const Poco::RegularExpressionException&)
			{
				// Invalid regex pattern, cannot optimize
				return this;
			}
		}
		if (args && args->size() >= 3 && dynamic_cast<StringLiteral*>((*args)[0]) && dynamic_cast<RegularExpressionLiteral*>((*args)[1]) && dynamic_cast<StringLiteral*>((*args)[2]))
		{
			auto* result = new StringLiteral(std::get<std::string>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "replacewithlist")
	{
		if (args && args->size() >= 2 && dynamic_cast<StringLiteral*>((*args)[1]))
		{
			auto list = ReplaceList::LoadList(*ctxt, ucr::toTString(dynamic_cast<StringLiteral*>((*args)[1])->value));
			delete (*args)[1];
			(*args)[1] = new ArrayLiteral(list);
		}
	}
	else if (functionName == "regexreplacewithlist")
	{
		if (args && args->size() >= 2 && dynamic_cast<StringLiteral*>((*args)[1]))
		{
			auto list = ReplaceList::LoadRegexList(*ctxt, ucr::toTString(dynamic_cast<StringLiteral*>((*args)[1])->value));
			delete (*args)[1];
			(*args)[1] = new ArrayLiteral(list);
		}
	}
	else if (functionName == "startofweek" || functionName == "startofmonth" || functionName == "startofyear")
	{
		if (auto dtLit = args ? dynamic_cast<DateTimeLiteral*>((*args)[0]) : nullptr)
		{
			auto* result = new DateTimeLiteral(std::get<Poco::Timestamp>(func(ectxt, args)));
			delete this;
			return result;
		}
	}
	return this;
}

ValueType FunctionNode::Evaluate(const FilterEvalContext& ectxt) const
{
	return func(ectxt, args);
}

SizeLiteral::SizeLiteral(const std::string& v)
{
	size_t pos = 0;
	while (pos < v.size() && (isdigit(v[pos]) || v[pos] == '.'))
		++pos;
	std::string numberPart = v.substr(0, pos);
	std::string unitPart = v.substr(pos);
	std::transform(unitPart.begin(), unitPart.end(), unitPart.begin(),
		[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	double number = 0.0;
	try
	{
		number = std::stod(numberPart);
	}
	catch (...) { }
	if (unitPart == "b")
		value = static_cast<int64_t>(number * 1LL);
	else if (unitPart == "kb")
		value = static_cast<int64_t>(number * 1024LL);
	else if (unitPart == "mb")
		value = static_cast<int64_t>(number * 1024LL * 1024LL);
	else if (unitPart == "gb")
		value = static_cast<int64_t>(number * 1024LL * 1024LL * 1024LL);
	else if (unitPart == "tb")
		value = static_cast<int64_t>(number * 1024LL * 1024LL * 1024LL * 1024LL);
	else
		value = static_cast<int64_t>(number);
}

DateTimeLiteral::DateTimeLiteral(const std::string& v)
{
	auto timestamp = ParseDateTime(v);
	if (!timestamp)
		throw std::invalid_argument("Unrecognized date/time format: " + v);
	value = *timestamp;
}

DurationLiteral::DurationLiteral(const std::string& v)
{
	size_t pos = 0;
	while (pos < v.size() && (isdigit(v[pos]) || v[pos] == '.'))
		++pos;
	std::string numberPart = v.substr(0, pos);
	std::string unitPart = v.substr(pos);
	std::transform(unitPart.begin(), unitPart.end(), unitPart.begin(),
		[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	double number = 0.0;
	try
	{
		number = std::stod(numberPart);
	}
	catch (...) { }
	if (unitPart == "w" || unitPart == "week" || unitPart == "weeks")
		value = static_cast<int64_t>(number * 604800LL * 1000000LL);
	else if (unitPart == "d" || unitPart == "day" || unitPart == "days")
		value = static_cast<int64_t>(number * 86400LL * 1000000LL);
	else if (unitPart == "h" || unitPart == "hr" || unitPart == "hour" || unitPart == "hours")
		value = static_cast<int64_t>(number * 3600LL * 1000000LL);
	else if (unitPart == "m" || unitPart == "min" || unitPart == "minute" || unitPart == "minutes")
		value = static_cast<int64_t>(number * 60LL * 1000000LL);
	else if (unitPart == "s" || unitPart == "sec" || unitPart == "second" || unitPart == "seconds")
		value = static_cast<int64_t>(number * 1LL * 1000000LL);
	else if (unitPart == "ms" || unitPart == "msec" || unitPart == "millisecond" || unitPart == "milliseconds")
		value = static_cast<int64_t>(number * 1000LL);
	else
		value = static_cast<int64_t>(number);
}

VersionLiteral::VersionLiteral(const std::string& v)
{
	int major = 0, minor = 0, build = 0, revision = 0;
	sscanf_s(v.c_str(), "%d.%d.%d.%d", &major, &minor, &build, &revision);
	value = (static_cast<int64_t>(major) << 48) + (static_cast<int64_t>(minor) << 32) + (static_cast<int64_t>(build) << 16) + revision;
}

RegularExpressionLiteral::RegularExpressionLiteral(const std::string& v, bool caseSensitive)
{
	const int flags = Poco::RegularExpression::RE_UTF8 | (!caseSensitive ? Poco::RegularExpression::RE_CASELESS : 0);
	value.reset(new Poco::RegularExpression(v, flags));
}

