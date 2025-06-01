/**
 * @file  FilterExpression.cpp
 *
 * @brief This file implements the filter expression evaluation logic for the FilterEngine.
 */
#include "pch.h"
#include "FilterExpression.h"
#include "FilterLexer.h"
#include "FilterEngine.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <string>
#include <variant>
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

static std::optional<bool> evalAsBool(const ValueType& val)
{
	auto boolVal = std::get_if<bool>(&val);
	if (boolVal) return *boolVal;

	auto ary = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&val);
	if (ary)
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
	if (!expr)
		return this;
	expr = expr->Optimize();
	auto boolVal = dynamic_cast<BoolLiteral*>(expr);
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
	auto val = expr->Evaluate(di);
	auto boolVal = evalAsBool(val);
	if (!boolVal) return std::monostate{};
	return *boolVal ? false : true;
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

static ExprNode* TryFoldConstants(ExprNode* left, int op, ExprNode* right)
{
	auto lInt = getConstIntValue(left);
	auto rInt = getConstIntValue(right);
	if (lInt && rInt)
	{
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
			default: return nullptr;
			}
			return new BoolLiteral(result);
		}

		int64_t result = 0;
		switch (op)
		{
		case TK_PLUS:  result = *lInt + *rInt; break;
		case TK_MINUS: result = *lInt - *rInt; break;
		case TK_STAR:  result = *lInt * *rInt; break;
		case TK_SLASH: if (*rInt != 0) result = *lInt / *rInt; else return nullptr; break;
		case TK_MOD:   if (*rInt != 0) result = *lInt % *rInt; else return nullptr; break;
		default: return nullptr;
		}
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
			case TK_EQ: result = lStr->value == rStr->value; break;
			case TK_NE: result = lStr->value != rStr->value; break;
			case TK_LT: result = lStr->value < rStr->value; break;
			case TK_LE: result = lStr->value <= rStr->value; break;
			case TK_GT: result = lStr->value > rStr->value; break;
			case TK_GE: result = lStr->value >= rStr->value; break;
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

		Poco::Timestamp result;
		switch (op)
		{
		case TK_MINUS: result = lDateTime->value - rDateTime->value; break;
		default: return nullptr;
		}
		return new DateTimeLiteral(result);
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
	if (op == TK_MATCHES)
	{
		if (auto strNode = dynamic_cast<StringLiteral*>(right))
		{
			right = new RegularExpressionLiteral(strNode->value);
			delete strNode;
		}
	}
	return this;
}

ValueType BinaryOpNode::Evaluate(const DIFFITEM& di) const
{
	auto lval = left->Evaluate(di);
	auto rval = right->Evaluate(di);
	auto compute = [](int op, const ValueType& lval, const ValueType& rval) -> ValueType
		{
			if (auto lvalInt = std::get_if<int64_t>(&lval))
			{
				if (auto rvalInt = std::get_if<int64_t>(&rval))
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
					if (op == TK_SLASH) return *lvalInt / *rvalInt;
					if (op == TK_MOD) return *lvalInt % *rvalInt;
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
					if (op == TK_EQ) return *lvalString == *rvalString;
					if (op == TK_NE) return *lvalString != *rvalString;
					if (op == TK_LT) return *lvalString < *rvalString;
					if (op == TK_LE) return *lvalString <= *rvalString;
					if (op == TK_GT) return *lvalString > *rvalString;
					if (op == TK_GE) return *lvalString >= *rvalString;
					if (op == TK_PLUS) return *rvalString + *lvalString;
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
					if (op == TK_MATCHES)
					{
						Poco::RegularExpression regex(*rvalString, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8);
						return regex.match(*lvalString);
					}
				}
				if (auto rvalRegexp = std::get_if<std::shared_ptr<Poco::RegularExpression>>(&rval))
				{
					if (op == TK_MATCHES)
						return rvalRegexp->get()->match(*lvalString);
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
			if (op == TK_EQ)
				return false;
			else if (op == TK_NE)
				return true;
			return std::monostate{};
		};
	auto lvalArray = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&lval);
	auto rvalArray = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&rval);
	if (!lvalArray && !rvalArray)
	{
		return compute(op, lval, rval);
	}
	else if (lvalArray && !rvalArray)
	{
		std::unique_ptr<std::vector<ValueType2>> result = std::make_unique<std::vector<ValueType2>>();
		for (const auto& item : *(lvalArray->get()))
			result->emplace_back(ValueType2{ compute(op, item.value, rval) });
		return result;
	}
	else if (!lvalArray && rvalArray)
	{
		std::unique_ptr<std::vector<ValueType2>> result = std::make_unique<std::vector<ValueType2>>();
		for (const auto& item : *(rvalArray->get()))
			result->emplace_back(ValueType2{ compute(op, lval, item.value) });
		return result;
	}
	else
	{
		const size_t maxSize = (std::max)(lvalArray->get()->size(), rvalArray->get()->size());
		const size_t minSize = (std::min)(lvalArray->get()->size(), rvalArray->get()->size());
		std::unique_ptr<std::vector<ValueType2>> result = std::make_unique<std::vector<ValueType2>>();
		for (size_t i = 0; i < minSize; ++i)
			result->emplace_back(ValueType2{ compute(op, lvalArray->get()->at(i).value, rvalArray->get()->at(i).value) });
		for (size_t i = 0; i < maxSize - minSize; ++i)
			result->emplace_back(ValueType2{ std::monostate{} });
		return result;
	}
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
	return this;
}

ValueType NegateNode::Evaluate(const DIFFITEM& di) const
{
	auto rval = right->Evaluate(di);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
	return std::monostate{};
}

static auto ExistsField(int index, const FilterContext* ctxt, const DIFFITEM& di)-> ValueType
{
	return di.diffcode.exists(index);
}

static auto NameField(int index, const FilterContext* ctxt, const DIFFITEM& di)-> ValueType
{
	if (di.diffcode.exists(index))
		return ucr::toUTF8(di.diffFileInfo[index].filename.get());
	return std::monostate{};
}

static auto PathField(int index, const FilterContext* ctxt, const DIFFITEM& di)-> ValueType
{
	return ucr::toUTF8(di.diffFileInfo[index].path.get());
}

static auto SizeField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return static_cast<int64_t>(di.diffFileInfo[index].size);
	return std::monostate{};
}

static auto DateField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return di.diffFileInfo[index].mtime;
	return std::monostate{};
}

static auto CreationTimeField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return di.diffFileInfo[index].ctime;
	return std::monostate{};
}

static auto FileVersionField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return di.diffFileInfo[index].version.GetFileVersionQWORD();
	return std::monostate{};
}

static auto AttributesField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return di.diffFileInfo[index].flags.attributes;
	return std::monostate{};
}

static auto AttrStrField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return ucr::toUTF8(di.diffFileInfo[index].flags.ToString());
	return std::monostate{};
}

static auto CodepageField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return di.diffFileInfo[index].encoding.m_codepage;
	return std::monostate{};
}

static auto EncodingField(int index, const FilterContext* ctxt, const DIFFITEM& di) -> ValueType
{
	if (di.diffcode.exists(index))
		return ucr::toUTF8(di.diffFileInfo[index].encoding.GetName());
	return std::monostate{};
}

FieldNode::FieldNode(const FilterContext* ctxt, const std::string& v) : ctxt(ctxt), field(v)
{
	int prefixlen = 0;
	const char* p = v.c_str();
	int index = 0;
	if (v.compare(0, 4, "Left") == 0)
	{
		index = 0;
		prefixlen = 4;
	}
	else if (v.compare(0, 6, "Middle") == 0)
	{
		index = 1;
		prefixlen = 6;
	}
	else if (v.compare(0, 5, "Right") == 0)
	{
		index = ctxt->ctxt->GetCompareDirs() < 3 ? 1 : 2;
		prefixlen = 5;
	}
	ValueType (*functmp)(int, const FilterContext*, const DIFFITEM&) = nullptr;
	if (v.compare(prefixlen, 6, "Exists") == 0)
		functmp = ExistsField;
	else if (v.compare(prefixlen, 4, "Name") == 0)
		functmp = NameField;
	else if (v.compare(prefixlen, 4, "Path") == 0)
		functmp = PathField;
	else if (v.compare(prefixlen, 4, "Size") == 0)
		functmp = SizeField;
	else if (v.compare(prefixlen, 4, "Date") == 0)
		functmp = DateField;
	else if (v.compare(prefixlen, 10, "Attributes") == 0)
		functmp = AttributesField;
	else if (v.compare(prefixlen, 10, "AttrStr") == 0)
		functmp = AttrStrField;
	else if (v.compare(prefixlen, 12, "CreationTime") == 0)
		functmp = CreationTimeField;
	else if (v.compare(prefixlen, 11, "FileVersion") == 0)
		functmp = FileVersionField;
	else if (v.compare(prefixlen, 8, "Codepage") == 0)
		functmp = CodepageField;
	else if (v.compare(prefixlen, 8, "Encoding") == 0)
		functmp = EncodingField;
	else
		throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));
	if (prefixlen > 0)
		func = [index, functmp](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType { return functmp(index, ctxt, di); };
	else
		func = [functmp](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
			const int dirs = ctxt->ctxt->GetCompareDirs();
			std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
			for (int i = 0; i < dirs; ++i)
				values->emplace_back(ValueType2{ functmp(i, ctxt, di) });
			return values;
		};
}

ValueType FieldNode::Evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di);
}

static auto AbsFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{ 
	auto arg1 = (*args)[0]->Evaluate(di);
	if (auto arg1Int = std::get_if<int64_t>(&arg1))
		return abs(*arg1Int);
	return std::monostate{};
}

static auto AnyOfFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(di);
	if (const auto arrayVal = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&arg1))
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

static auto AllOfFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	auto arg1 = (*args)[0]->Evaluate(di);
	if (const auto arrayVal = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&arg1))
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

static auto AllEqualFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	ValueType first = args->at(0)->Evaluate(di);
	if (auto pArray = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&first); pArray && *pArray)
	{
		const auto& vec = **pArray;
		if (vec.size() <= 1)
			return true;
		const ValueType& base = vec[0].value;
		for (size_t i = 1; i < vec.size(); ++i)
		{
			if (!(vec[i].value == base))
				return false;
		}
		return true;
	}
	else
	{
		for (size_t i = 1; i < args->size(); ++i)
		{
			ValueType val = args->at(i)->Evaluate(di);
			if (!(val == first))
				return false;
		}
		return true;
	}
}

static auto TodayFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return *ctxt->today;
}

static auto NowFunc(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
{
	return *ctxt->now;
}

FunctionNode::FunctionNode(const FilterContext* ctxt, const std::string& name, std::vector<ExprNode*>* args)
	: ctxt(ctxt), functionName(name), args(args)
{
	if (functionName == "abs")
	{
		if (!args || args->size() != 1)
			throw std::invalid_argument("abs function requires 1 arguments");
		func = AbsFunc;
	}
	else if (functionName == "anyof")
	{
		if (!args || args->size() != 1)
			throw std::invalid_argument("anyof function requires 1 arguments");
		func = AnyOfFunc;
	}
	else if (functionName == "allof")
	{
		if (!args || args->size() != 1)
			throw std::invalid_argument("allof function requires 1 arguments");
		func = AllOfFunc;
	}
	else if (functionName == "allequal")
	{
		if (!args || args->size() < 1)
			throw std::invalid_argument("allequal function requires at least 1 arguments");
		func = AllEqualFunc;
	}
	else if (functionName == "today")
	{
		if (args && args->size() != 0)
			throw std::invalid_argument("today function requires 0 arguments");
		func = TodayFunc;
	}
	else if (functionName == "now")
	{
		if (args && args->size() != 0)
			throw std::invalid_argument("now function requires 0 arguments");
		func = NowFunc;
	}
	else
	{
		throw std::runtime_error("Unknown function: " + std::string(functionName.begin(), functionName.end()));
	}
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

ValueType FunctionNode::Evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di, args);
}

SizeLiteral::SizeLiteral(const std::string& v)
{
	std::string numberPart = v.substr(0, v.find_first_not_of("0123456789"));
	std::string unitPart = v.substr(numberPart.length());
	int64_t number = std::stoll(numberPart);
	if (unitPart == "KB" || unitPart == "kb")
		value = number * 1024LL;
	else if (unitPart == "MB" || unitPart == "mb")
		value = number * 1024LL * 1024LL;
	else if (unitPart == "GB" || unitPart == "gb")
		value = number * 1024LL * 1024LL * 1024LL;
	else if (unitPart == "TB" || unitPart == "tb")
		value = number * 1024LL * 1024LL * 1024LL * 1024LL;
	else
		value = number;
}

DateTimeLiteral::DateTimeLiteral(const std::string& v)
{
	static const std::vector<std::string> formats = {
		Poco::DateTimeFormat::ISO8601_FORMAT,
		Poco::DateTimeFormat::ISO8601_FRAC_FORMAT,
		"%Y-%m-%d %H:%M:%S",
		"%Y-%m-%d",
		"%Y/%m/%d %H:%M:%S",
		"%Y/%m/%d",
		"%Y.%m.%d %H:%M:%S",
		"%Y.%m.%d",
		"%d-%b-%Y %H:%M:%S",
		"%d-%b-%Y",
	};

	Poco::DateTime dt;
	int tz = 0;
	bool parsed = false;
	for (const auto& fmt : formats)
	{
		try
		{
			Poco::DateTimeParser::parse(fmt, v, dt, tz);
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
	std::string numberPart = v.substr(0, v.find_first_not_of("0123456789"));
	std::string unitPart = v.substr(numberPart.length());
	int64_t number = std::stoll(numberPart);
	if (unitPart == "w" || unitPart == "week" || unitPart == "weeks")
		value = number * 604800LL * 1000000LL;
	else if (unitPart == "d" || unitPart == "day" || unitPart == "days")
		value = number * 86400LL * 1000000LL;
	else if (unitPart == "h" || unitPart == "hr" || unitPart == "hour" || unitPart == "hours")
		value = number * 3600LL * 1000000LL;
	else if (unitPart == "m" || unitPart == "min" || unitPart == "minute" || unitPart == "minutes")
		value = number * 60LL * 1000000LL;
	else if (unitPart == "s" || unitPart == "sec" || unitPart == "second" || unitPart == "seconds")
		value = number * 1LL * 1000000LL;
	else if (unitPart == "ms" || unitPart == "msec" || unitPart == "millisecond" || unitPart == "milliseconds")
		value = number * 1000LL;
	else
		value = number;
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

