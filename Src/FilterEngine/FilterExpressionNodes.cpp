/**
 * @file  FilterExpressionNodes.cpp
 *
 * @brief This file implements the filter expression evaluation logic for the FilterExpression.
 */
#include "pch.h"
#include "FilterExpressionNodes.h"
#include "FilterExpression.h"
#include "FileContentRef.h"
#include "FolderStats.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "paths.h"
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

ValueType OrNode::Evaluate(const DIFFITEM& di) const
{
	auto lval = left->Evaluate(di);
	auto lbool = evalAsBool(lval);
	if (lbool && *lbool) return true;

	auto rval = right->Evaluate(di);
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

ValueType AndNode::Evaluate(const DIFFITEM& di) const
{
	auto lval = left->Evaluate(di);
	auto lbool = evalAsBool(lval);
	if (!lbool) return std::monostate{};
	if (!*lbool) return false;

	auto rval = right->Evaluate(di);
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

ValueType NotNode::Evaluate(const DIFFITEM& di) const
{
	auto val = right->Evaluate(di);
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
static ExprNode* TryFoldConstants(ExprNode* left, int op, ExprNode* right)
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
			switch (op)
			{
			case TK_EQ: result = Poco::icompare(lStr->value, rStr->value) == 0; break;
			case TK_NE: result = Poco::icompare(lStr->value, rStr->value) != 0; break;
			case TK_LT: result = Poco::icompare(lStr->value, rStr->value) < 0; break;
			case TK_LE: result = Poco::icompare(lStr->value, rStr->value) <= 0; break;
			case TK_GT: result = Poco::icompare(lStr->value, rStr->value) > 0; break;
			case TK_GE: result = Poco::icompare(lStr->value, rStr->value) >= 0; break;
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
	if (ExprNode* folded = TryFoldConstants(left, op, right))
	{
		delete this;
		return folded;
	}
	if (op == TK_RECONTAINS || op == TK_MATCHES)
	{
		if (auto strNode = dynamic_cast<StringLiteral*>(right))
		{
			right = new RegularExpressionLiteral(strNode->value);
			delete strNode;
		}
	}
	return this;
}

static auto compute(int op, const ValueType& lval, const ValueType& rval) -> ValueType
{
	auto lvalArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&lval);
	auto rvalArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&rval);
	if (!lvalArray && !rvalArray)
	{
		if (auto lvalDouble = std::get_if<double>(&lval))
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
				if (op == TK_EQ) return Poco::icompare(*lvalString, *rvalString) == 0;
				if (op == TK_NE) return Poco::icompare(*lvalString, *rvalString) != 0;
				if (op == TK_LT) return Poco::icompare(*lvalString, *rvalString) < 0;
				if (op == TK_LE) return Poco::icompare(*lvalString, *rvalString) <= 0;
				if (op == TK_GT) return Poco::icompare(*lvalString, *rvalString) > 0;
				if (op == TK_GE) return Poco::icompare(*lvalString, *rvalString) >= 0;
				if (op == TK_PLUS) return *lvalString + *rvalString;
				if (op == TK_CONTAINS)
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
				if (op == TK_RECONTAINS)
				{
					try
					{
						Poco::RegularExpression regex(*rvalString, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8);
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
					Poco::Glob glob(*rvalString, Poco::Glob::GLOB_CASELESS);
					return glob.match(*lvalString);
				}
				if (op == TK_MATCHES)
				{
					try
					{
						Poco::RegularExpression regex(*rvalString, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8);
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
				if (op == TK_CONTAINS) return (*lvalContent)->Contains(*rvalString);
				if (op == TK_RECONTAINS)
				{
					try
					{
						Poco::RegularExpression regex(*rvalString, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8);
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
			result->emplace_back(ValueType2{ compute(op, item.value, rval) });
		return result;
	}
	else if (!lvalArray && rvalArray)
	{
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *(rvalArray->get()))
			result->emplace_back(ValueType2{ compute(op, lval, item.value) });
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
				ValueType eq = compute(TK_EQ, lv, rv);
				if (!std::holds_alternative<bool>(eq) || !std::get<bool>(eq))
					return (op == TK_NE);
			}
			return (op == TK_EQ);
		}
		const size_t maxSize = (std::max)((*lvalArray)->size(), (*rvalArray)->size());
		const size_t minSize = (std::min)((*lvalArray)->size(), (*rvalArray)->size());
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (size_t i = 0; i < minSize; ++i)
			result->emplace_back(ValueType2{ compute(op, (*lvalArray)->at(i).value, (*rvalArray)->at(i).value) });
		for (size_t i = 0; i < maxSize - minSize; ++i)
			result->emplace_back(ValueType2{ std::monostate{} });
		return result;
	}
}

ValueType BinaryOpNode::Evaluate(const DIFFITEM& di) const
{
	auto lval = left->Evaluate(di);
	auto rval = right->Evaluate(di);
	return compute(op, lval, rval);
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

ValueType NegateNode::Evaluate(const DIFFITEM& di) const
{
	auto rval = right->Evaluate(di);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
	if (auto rvalDouble = std::get_if<double>(&rval))
		return -*rvalDouble;
	return std::monostate{};
}

static auto ExistsField(int index, const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType
{
	return di.diffcode.exists(index);
}

template<typename Func>
static auto FolderStatField(int index, const FilterExpression* ctxt, const DIFFITEM& di, Func func, bool recursive) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};

	const String relpath = paths::ConcatPath(di.diffFileInfo[index].path, di.diffFileInfo[index].filename);
	const String fullPath = paths::ConcatPath(ctxt->ctxt->GetPath(index), relpath);

	FolderStats::FolderStatsResult stats = FolderStats::ScanFolder(fullPath, recursive);
	return func(stats);
}

static auto FilesField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.fileCount); }, false);
}

static auto RecursiveFilesField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.fileCount); }, true);
}

static auto ItemsField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType {
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.itemCount); }, false);
}

static auto RecursiveItemsField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return static_cast<int64_t>(s.itemCount); }, true);
}

static auto TotalSizeField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return s.totalSize; }, false);
}

static auto RecursiveTotalSizeField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return FolderStatField(index, ctxt, di, [](const FolderStats::FolderStatsResult& s) { return s.totalSize; }, true);
}

static auto NameField(int index, const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(di.diffFileInfo[index].filename.get());
}

static auto ExtensionField(int index, const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	const std::string ext = ucr::toUTF8(paths::FindExtension(di.diffFileInfo[index].filename.get()));
	return std::string(ext.c_str() + strspn(ext.c_str(), "."));
}

static auto FolderField(int index, const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType
{
	return ucr::toUTF8(di.diffFileInfo[index].path.get());
}

static auto SizeField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(di.diffFileInfo[index].size);
}

static auto DateField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return di.diffFileInfo[index].mtime;
}

static auto DateStrField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	Poco::LocalDateTime ldt(Poco::Timezone::tzd(), di.diffFileInfo[index].mtime);
	return Poco::DateTimeFormatter::format(ldt, "%Y-%m-%d");
}

static auto CreationTimeField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return di.diffFileInfo[index].ctime;
}

static auto FileVersionField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	if (di.diffFileInfo[index].version.IsCleared())
		ctxt->ctxt->UpdateVersion(const_cast<DIFFITEM&>(di), index);
	return static_cast<int64_t>(di.diffFileInfo[index].version.GetFileVersionQWORD());
}

static auto AttributesField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(di.diffFileInfo[index].flags.attributes);
}

static auto AttrStrField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(di.diffFileInfo[index].flags.ToString());
}

static auto CodepageField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return static_cast<int64_t>(di.diffFileInfo[index].encoding.m_codepage);
}

static auto DiffCodeField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return static_cast<int64_t>(di.diffcode.diffcode);
}

static auto DifferencesField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return static_cast<int64_t>(di.nsdiffs);
}

static auto IgnoredDiffsField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	return static_cast<int64_t>(di.nidiffs);
}

static auto EncodingField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	return ucr::toUTF8(di.diffFileInfo[index].encoding.GetName());
}

static auto FullPathField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	const String relpath = paths::ConcatPath(di.diffFileInfo[index].path, di.diffFileInfo[index].filename);
	return ucr::toUTF8(paths::ConcatPath(ctxt->ctxt->GetPath(index), relpath));
}

static auto ContentField(int index, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	const String relpath = paths::ConcatPath(di.diffFileInfo[index].path, di.diffFileInfo[index].filename);
	std::shared_ptr<FileContentRef> content{ new FileContentRef };
	content->path = paths::ConcatPath(ctxt->ctxt->GetPath(index), relpath);
	content->item.size = di.diffFileInfo[index].size;
	content->item.flags = di.diffFileInfo[index].flags;
	content->item.mtime = di.diffFileInfo[index].mtime;
	content->item.ctime = di.diffFileInfo[index].ctime;
	content->item.version = di.diffFileInfo[index].version;
	content->item.encoding = di.diffFileInfo[index].encoding;
	return content;
}

FieldNode::FieldNode(const FilterExpression* ctxt, const std::string& v) : ctxt(ctxt), field(v)
{
	int prefixlen = 0;
	int side = 0;
	std::string vl = Poco::toLower(v);
	if (vl.compare(0, 4, "left") == 0)
	{
		side = 0;
		prefixlen = 4;
	}
	else if (vl.compare(0, 6, "middle") == 0)
	{
		side = 1;
		prefixlen = 6;
	}
	else if (vl.compare(0, 5, "right") == 0)
	{
		side = -1;
		prefixlen = 5;
	}
	ValueType (*functmp)(int, const FilterExpression*, const DIFFITEM&) = nullptr;
	const char* p = vl.c_str() + prefixlen;
	if (strcmp(p, "exists") == 0)
		functmp = ExistsField;
	else if (strcmp(p, "files") == 0)
		functmp = FilesField;
	else if (strcmp(p, "items") == 0)
		functmp = ItemsField;
	else if (strcmp(p, "totalsize") == 0)
		functmp = TotalSizeField;
	else if (strcmp(p, "recursivefiles") == 0)
		functmp = RecursiveFilesField;
	else if (strcmp(p, "recursiveitems") == 0)
		functmp = RecursiveItemsField;
	else if (strcmp(p, "recursivetotalsize") == 0)
		functmp = RecursiveTotalSizeField;
	else if (strcmp(p, "name") == 0)
		functmp = NameField;
	else if (strcmp(p, "extension") == 0)
		functmp = ExtensionField;
	else if (strcmp(p, "fullpath") == 0)
		functmp = FullPathField;
	else if (strcmp(p, "folder") == 0)
		functmp = FolderField;
	else if (strcmp(p, "size") == 0)
		functmp = SizeField;
	else if (strcmp(p, "datestr") == 0)
		functmp = DateStrField;
	else if (strcmp(p, "date") == 0)
		functmp = DateField;
	else if (strcmp(p, "attributes") == 0)
		functmp = AttributesField;
	else if (strcmp(p, "attrstr") == 0)
		functmp = AttrStrField;
	else if (strcmp(p, "creationtime") == 0)
		functmp = CreationTimeField;
	else if (strcmp(p, "version") == 0)
		functmp = FileVersionField;
	else if (strcmp(p, "codepage") == 0)
		functmp = CodepageField;
	else if (strcmp(p, "encoding") == 0)
		functmp = EncodingField;
	else if (strcmp(p, "diffcode") == 0)
	{
		functmp = DiffCodeField;
		side = -2;
	}
	else if (strcmp(p, "differences") == 0)
	{
		functmp = DifferencesField;
		side = -2;
	}
	else if (strcmp(p, "ignoreddiffs") == 0)
	{
		functmp = IgnoredDiffsField;
		side = -2;
	}
	else if (strcmp(p, "content") == 0)
		functmp = ContentField;
	else
		throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));
	if (prefixlen > 0)
		func = [side, functmp](const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType { return functmp(side < 0 ? ctxt->ctxt->GetCompareDirs() + side: side, ctxt, di); };
	else
		func = [functmp](const FilterExpression* ctxt, const DIFFITEM& di)-> ValueType {
			const int dirs = ctxt->ctxt->GetCompareDirs();
			std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
			for (int i = 0; i < dirs; ++i)
				values->emplace_back(ValueType2{ functmp(i, ctxt, di) });
			return values;
		};
}

ValueType FieldNode::Evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di);
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

static auto AbsFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto absFn = [](const ValueType& val)->ValueType
		{
			if (auto arg1Int = std::get_if<int64_t>(&val))
				return abs(*arg1Int);
			if (auto arg1Double = std::get_if<double>(&val))
				return abs(*arg1Double);
			return std::monostate{};
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, absFn);
}

static auto AnyOfFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(di);
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

static auto AllOfFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(di);
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

static auto AllEqualFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType first = args->at(0)->Evaluate(di);
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
			ValueType val = args->at(i)->Evaluate(di);
			if (!valueEquals(val, val))
				return false;
		}
		return true;
	}
}

static auto ArrayFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto result = std::make_shared<std::vector<ValueType2>>();
	if (args)
	{
		for (ExprNode* arg : *args)
		{
			ValueType val = arg->Evaluate(di);
			result->emplace_back(ValueType2{ std::move(val) });
		}
	}
	return result;
}

static auto AtFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(di);
	if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&arg1))
	{
		ValueType arg2 = args->at(1)->Evaluate(di);
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

static auto StrlenFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto strlenFn = [](const ValueType& val)->ValueType
		{
			if (auto arg1Str = std::get_if<std::string>(&val))
				return static_cast<int64_t>(arg1Str->length());
			return std::monostate{};
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, strlenFn);
}

static auto SubstrFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2 || args->size() > 3)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(di);
	auto argStart = (*args)[1]->Evaluate(di);
	std::optional<ValueType> argLen;
	if (args->size() == 3)
		argLen = (*args)[2]->Evaluate(di);

	const int64_t* start = std::get_if<int64_t>(&argStart);
	const int64_t* len = argLen ? std::get_if<int64_t>(&*argLen) : nullptr;

	if (!start)
		return std::monostate{};

	if (auto argStrArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argStr))
	{
		auto result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *argStrArray->get())
		{
			const std::string* str = std::get_if<std::string>(&item.value);
			if (!str)
			{
				result->emplace_back(ValueType2{ std::monostate{} });
				continue;
			}

			int64_t s = *start;
			if (s < 0)
				s += static_cast<int64_t>(str->length());
			if (s < 0 || s >= str->length())
			{
				result->emplace_back(ValueType2{ std::string{} });
				continue;
			}

			if (!len)
			{
				result->emplace_back(ValueType2{ str->substr(static_cast<size_t>(s)) });
				continue;
			}

			int64_t actualLen = (*len >= 0) ? *len : static_cast<int64_t>(str->length()) - s + *len;
			if (actualLen < 0)
				actualLen = 0;
			result->emplace_back(ValueType2{ str->substr(static_cast<size_t>(s), static_cast<size_t>(actualLen)) });
		}
		return result;
	}

	const std::string* str = std::get_if<std::string>(&argStr);
	if (!str)
		return std::monostate{};

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
}

static auto LineCountFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto lineCountFn = [](const ValueType& val) -> ValueType
		{
			if (auto contentRef = std::get_if<std::shared_ptr<FileContentRef>>(&val))
				return static_cast<int64_t>((*contentRef)->LineCount());
			return std::monostate{};
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, lineCountFn);
}

static auto SublinesFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2 || args->size() > 3)
		return std::monostate{};

	auto argContentRef = (*args)[0]->Evaluate(di);
	auto argStart = (*args)[1]->Evaluate(di);
	std::optional<ValueType> argLen;
	if (args->size() == 3)
		argLen = (*args)[2]->Evaluate(di);

	const auto contentref = std::get_if<std::shared_ptr<FileContentRef>>(&argContentRef);
	const auto contentrefArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argContentRef);
	const int64_t* start = std::get_if<int64_t>(&argStart);
	const int64_t* len = argLen ? std::get_if<int64_t>(&*argLen) : nullptr;

	if ((!contentref && !contentrefArray) || !start)
		return std::monostate{};

	if (contentrefArray && *contentrefArray)
	{
		std::shared_ptr<std::vector<ValueType2>> result = std::make_shared<std::vector<ValueType2>>();
		for (const auto& item : *contentrefArray->get())
		{
			if (auto contentRef = std::get_if<std::shared_ptr<FileContentRef>>(&item.value))
				result->emplace_back(ValueType2{ static_cast<std::string>((*contentRef)->Sublines(*start, len ? *len : -1)) });
			else
				result->emplace_back(ValueType2{ std::monostate{} });
		}
		return result;

	}
	if (contentref && *contentref)
		return (*contentref)->Sublines(*start, len ? *len : -1);
	return std::monostate{};
}

static auto ReplaceFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	if (!args || args->size() != 3)
		return std::monostate{};

	auto argStr = (*args)[0]->Evaluate(di);
	auto argFrom = (*args)[1]->Evaluate(di);
	auto argTo = (*args)[2]->Evaluate(di);

	const std::string* from = std::get_if<std::string>(&argFrom);
	const std::string* to = std::get_if<std::string>(&argTo);

	if (!from || !to || from->empty())
		return std::monostate{};

	if (auto argStrArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&argStr))
	{
		auto& vec = **argStrArray;
		auto result = std::make_shared<std::vector<ValueType2>>();
		result->reserve(vec.size());

		for (const auto& item : vec)
		{
			const std::string* str = std::get_if<std::string>(&item.value);
			if (!str)
			{
				result->emplace_back(ValueType2{ std::monostate{} });
				continue;
			}

			const auto replaced = Poco::replace(*str, *from, *to);
			result->emplace_back(ValueType2{ replaced });
		}

		return result;
	}

	const std::string* str = std::get_if<std::string>(&argStr);
	if (!str)
		return std::monostate{};
	return Poco::replace(*str, *from, *to);
}

static auto TodayFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return *ctxt->today;
}

static auto NowFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return *ctxt->now;
}

static auto StartOfWeekFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
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
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, startOfWeek);
}

static auto StartOfMonthFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto startOfMonth = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			auto& ldt = *ldtOpt;
			return Poco::LocalDateTime(ldt.year(), ldt.month(), 1, 0, 0, 0, 0, 0).utc().timestamp();
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, startOfMonth);
}

static auto StartOfYearFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto startOfYear = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			auto& ldt = *ldtOpt;
			return Poco::LocalDateTime(ldt.year(), 1, 1, 0, 0, 0, 0, 0).utc().timestamp();
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, startOfYear);
}

static auto ToDateStrFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto toDateStr = [](const ValueType& val) -> ValueType
		{
			auto ldtOpt = toLocalDateTime(val);
			if (!ldtOpt) return std::monostate{};
			return Poco::DateTimeFormatter::format(*ldtOpt, "%Y-%m-%d");
		};
	auto arg = (*args)[0]->Evaluate(di);
	return applyToScalarOrArray(arg, toDateStr);
}

static auto IsWithinFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg2 = args->at(1)->Evaluate(di);
	ValueType arg3 = args->at(2)->Evaluate(di);

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
						int cmpMin = Poco::icompare(*valString, *minString);
						int cmpMax = Poco::icompare(*valString, *maxString);
						return (cmpMin >= 0 && cmpMax <= 0);
					}
				}
			}
			return std::monostate{};
		};

	auto arg1 = args->at(0)->Evaluate(di);
	return applyToScalarOrArray(arg1, isWithinFn);
}

static auto InRangeFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg2 = args->at(1)->Evaluate(di);
	ValueType arg3 = args->at(2)->Evaluate(di);

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
						int cmpMin = Poco::icompare(*valString, *minString);
						int cmpMax = Poco::icompare(*valString, *maxString);
						return (cmpMin >= 0 && cmpMax < 0);
					}
				}
			}
			return std::monostate{};
		};

	auto arg1 = args->at(0)->Evaluate(di);
	return applyToScalarOrArray(arg1, inRangeFn);
}

static std::string ToStringValue(const ValueType& val)
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

static auto LogFunc(int level, const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType val;
	std::string msg;
	for (size_t i = 0; i < args->size(); ++i)
	{
		val = args->at(i)->Evaluate(di);
		if (i > 0)
			msg += " ";
		msg += ToStringValue(val);
	}
	ctxt->logger(level, msg);
	return val;
}

static auto LogErrorFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(0, ctxt, di, args);
}

static auto LogWarnFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(1, ctxt, di, args);
}

static auto LogInfoFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return LogFunc(2, ctxt, di, args);
}

static auto IfFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType condition = args->at(0)->Evaluate(di);
	auto condBool = evalAsBool(condition);
	if (condBool && *condBool)
		return args->at(1)->Evaluate(di);
	else
		return args->at(2)->Evaluate(di);
}

template<typename MapFunc>
static auto ApplyToScalarOrArrayWithContext(const ValueType& input, MapFunc mapFunc) -> ValueType
{
	auto inputArray = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&input);
	
	if (!inputArray)
		return mapFunc(input);
	
	auto result = std::make_shared<std::vector<ValueType2>>();
	for (const auto& item : **inputArray)
		result->emplace_back(ValueType2{ mapFunc(item.value) });
	
	return result;
}

static auto IfEachFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType argCond = args->at(0)->Evaluate(di);
	ValueType argTrue = args->at(1)->Evaluate(di);
	ValueType argFalse = args->at(2)->Evaluate(di);

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

	return ApplyToScalarOrArrayWithContext(argCond, selectValue);
}

static auto ChooseFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2)
		return std::monostate{};
	
	ValueType indexVal = args->at(0)->Evaluate(di);
	auto indexInt = std::get_if<int64_t>(&indexVal);
	
	if (!indexInt)
		return std::monostate{};
	
	int64_t idx = *indexInt;
	if (idx < 0)
		idx = 0;
	
	size_t choiceIdx = static_cast<size_t>(idx) + 1;
	if (choiceIdx >= args->size())
		choiceIdx = args->size() - 1;
	
	return args->at(choiceIdx)->Evaluate(di);
}

static auto ChooseEachFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	if (args->size() < 2)
		return std::monostate{};
	
	ValueType indexVal = args->at(0)->Evaluate(di);
	
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
			
			return args->at(choiceIdx)->Evaluate(di);
		};
	
	return ApplyToScalarOrArrayWithContext(indexVal, selectFromIndex);
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

static auto AndEachFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(di);
	ValueType arg2 = args->at(1)->Evaluate(di);
	return BinaryLogicalEachFunc(arg1, arg2, [](bool a, bool b) { return a && b; });
}

static auto OrEachFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg1 = args->at(0)->Evaluate(di);
	ValueType arg2 = args->at(1)->Evaluate(di);
	return BinaryLogicalEachFunc(arg1, arg2, [](bool a, bool b) { return a || b; });
}

static auto NotEachFunc(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType arg = args->at(0)->Evaluate(di);
	
	auto notFunc = [](const ValueType& val) -> ValueType
		{
			auto boolVal = evalAsBool(val);
			if (boolVal)
				return !*boolVal;
			return std::monostate{};
		};
	
	return ApplyToScalarOrArrayWithContext(arg, notFunc);
}

struct FunctionInfo
{
	using FuncPtr = auto (*)(const FilterExpression*, const DIFFITEM&, std::vector<ExprNode*>*) -> ValueType;
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
	{"choose", ChooseFunc, 2, -1},
	{"chooseeach", ChooseEachFunc, 2, -1},
	{"if", IfFunc, 3, 3},
	{"ifeach", IfEachFunc, 3, 3},
	{"inrange", InRangeFunc, 3, 3},
	{"iswithin", IsWithinFunc, 3, 3},
	{"linecount", LineCountFunc, 1, 1},
	{"logerror", LogErrorFunc, 1, -1},
	{"loginfo", LogInfoFunc, 1, -1},
	{"logwarn", LogWarnFunc, 1, -1},
	{"noteach", NotEachFunc, 1, 1},
	{"now", NowFunc, 0, 0},
	{"oreach", OrEachFunc, 2, 2},
	{"replace", ReplaceFunc, 3, 3},
	{"startofmonth", StartOfMonthFunc, 1, 1},
	{"startofweek", StartOfWeekFunc, 1, 1},
	{"startofyear", StartOfYearFunc, 1, 1},
	{"strlen", StrlenFunc, 1, 1},
	{"sublines", SublinesFunc, 2, 3},
	{"substr", SubstrFunc, 2, 3},
	{"todatestr", ToDateStrFunc, 1, 1},
	{"today", TodayFunc, 0, 0},
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

FunctionNode::FunctionNode(const FilterExpression* ctxt, const std::string& name, std::vector<ExprNode*>* args)
	: ctxt(ctxt), functionName(Poco::toLower(name)), args(args)
{
	// Special handling for prop functions
	if (functionName == "prop")
	{
		SetPropFunc();
		return;
	}
	if (functionName == "leftprop" || functionName == "middleprop" || functionName == "rightprop")
	{
		SetLeftMiddleRightPropFunc();
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
			auto* result = new IntLiteral(std::get<int64_t>(func(ctxt, di, args)));
			delete this;
			return result;
		}
		if (auto doubleLit = args ? dynamic_cast<DoubleLiteral*>((*args)[0]) : nullptr)
		{
			auto* result = new DoubleLiteral(std::get<double>(func(ctxt, di, args)));
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
			auto* result = new IntLiteral(std::get<int64_t>(func(ctxt, di, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "substr")
	{
		if (args && dynamic_cast<StringLiteral*>((*args)[0]) && dynamic_cast<IntLiteral*>((*args)[1])
			&& (args->size() == 2 || dynamic_cast<IntLiteral*>((*args)[2])))

		{
			auto* result = new StringLiteral(std::get<std::string>(func(ctxt, di, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "replace")
	{
		if (args && dynamic_cast<StringLiteral*>((*args)[0]) && dynamic_cast<StringLiteral*>((*args)[2]) && dynamic_cast<StringLiteral*>((*args)[2]))
		{
			auto* result = new StringLiteral(std::get<std::string>(func(ctxt, di, args)));
			delete this;
			return result;
		}
	}
	else if (functionName == "startofweek" || functionName == "startofmonth" || functionName == "startofyear")
	{
		if (auto dtLit = args ? dynamic_cast<DateTimeLiteral*>((*args)[0]) : nullptr)
		{
			auto* result = new DateTimeLiteral(std::get<Poco::Timestamp>(func(ctxt, di, args)));
			delete this;
			return result;
		}
	}
	return this;
}

ValueType FunctionNode::Evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di, args);
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
	static const std::vector<std::string> formats = {
		Poco::DateTimeFormat::ISO8601_FORMAT,
		Poco::DateTimeFormat::ISO8601_FRAC_FORMAT,
		"%Y/%m/%d %H:%M:%S",
		"%Y.%m.%d %H:%M:%S",
		"%d-%b-%Y %H:%M:%S",
	};

	Poco::DateTime dt;
	bool parsed = false;
	for (const auto& fmt : formats)
	{
		try
		{
			int tz = 0;
			Poco::DateTimeParser::parse(fmt, v, dt, tz);
			dt.makeUTC((v.find_first_of("zZ+") != std::string::npos) ? tz : Poco::Timezone::tzd());
			value = dt.timestamp();
			parsed = true;
			break;
		}
		catch (Poco::SyntaxException&)
		{
			// Try next format
		}
	}
	if (!parsed)
	{
		throw std::invalid_argument("Unrecognized date/time format: " + v);
	}
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

RegularExpressionLiteral::RegularExpressionLiteral(const std::string& v)
{
	value.reset(new Poco::RegularExpression(v, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8));
}

