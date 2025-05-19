#include "pch.h"
#include "FilterExpression.h"
#include "FilterEngineInternal.h"
#include "FilterEngine.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <iostream>
#include <string>
#include <map>
#include <variant>

ValueType OrNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto lbool = std::get_if<bool>(&lval);
	if (lbool && *lbool) return true;

	auto rval = right->evaluate(di);
	auto rbool = std::get_if<bool>(&rval);
	if (rbool && *rbool) return true;

	if (lbool || rbool) return false;
	return std::monostate{};
}

ValueType AndNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto lbool = std::get_if<bool>(&lval);
	if (!lbool) return std::monostate{};
	if (!*lbool) return false;

	auto rval = right->evaluate(di);
	auto rbool = std::get_if<bool>(&rval);
	if (!rbool) return std::monostate{};
	if (!*rbool) return false;
	return true;
}

ValueType NotNode::evaluate(const DIFFITEM& di) const
{
	auto val = expr->evaluate(di);
	auto boolVal = std::get_if<bool>(&val);
	return *boolVal ? false : true;
}

ValueType ComparisonNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto rval = right->evaluate(di);
	if (auto lvalInt = std::get_if<int64_t>(&lval))
	{
		if (auto rvalInt = std::get_if<int64_t>(&rval))
		{
			if (op == EQ) return *lvalInt == *rvalInt;
			if (op == NE) return *lvalInt != *rvalInt;
			if (op == LT) return *lvalInt <  *rvalInt;
			if (op == LE) return *lvalInt <= *rvalInt;
			if (op == GT) return *lvalInt >  *rvalInt;
			if (op == GE) return *lvalInt >= *rvalInt;
		}
	}
	else if (auto lvalTimestamp = std::get_if<Poco::Timestamp>(&lval))
	{
		if (auto rvalTimestamp = std::get_if<Poco::Timestamp>(&rval))
		{
			if (op == EQ) return *lvalTimestamp == *rvalTimestamp;
			if (op == NE) return *lvalTimestamp != *rvalTimestamp;
			if (op == LT) return *lvalTimestamp <  *rvalTimestamp;
			if (op == LE) return *lvalTimestamp <= *rvalTimestamp;
			if (op == GT) return *lvalTimestamp >  *rvalTimestamp;
			if (op == GE) return *lvalTimestamp >= *rvalTimestamp;
		}
	}
	else if (auto lvalString = std::get_if<std::string>(&lval))
	{
		if (auto rvalString = std::get_if<std::string>(&rval))
		{
			if (op == EQ) return *lvalString == *rvalString;
			if (op == NE) return *lvalString != *rvalString;
			if (op == CONTAINS)
			{
				std::string l = ucr::toUTF8(strutils::makelower(ucr::toTString(*lvalString)));
				std::string r = ucr::toUTF8(strutils::makelower(ucr::toTString(*rvalString)));
				return (l.find(r) != std::string::npos);
			}
			if (op == MATCHES)
			{
				return *lvalString != *rvalString;
			}
		}
	}
	else if (auto lvalBool = std::get_if<bool>(&lval))
	{
		if (auto rvalBool = std::get_if<bool>(&rval))
		{
			if (op == EQ) return *lvalBool == *rvalBool;
			if (op == NE) return *lvalBool != *rvalBool;
		}
	}
	if (op == EQ)
		return false;
	else if (op == NE)
		return true;
	return std::monostate{};
}

ValueType ArithmeticNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto rval = right->evaluate(di);
	if (auto lvalInt = std::get_if<int64_t>(&lval))
	{
		if (auto rvalInt = std::get_if<int64_t>(&rval))
		{
			if (op == '+') return *lvalInt + *rvalInt;
			if (op == '-') return *lvalInt - *rvalInt;
			if (op == '*') return *lvalInt * *rvalInt;
			if (op == '/') return *lvalInt / *rvalInt;
			if (op == '%') return *lvalInt % *rvalInt;
		}
	}
	if (auto lvalTimestamp = std::get_if<Poco::Timestamp>(&lval))
	{
		if (auto rvalTimestamp = std::get_if<Poco::Timestamp>(&rval))
		{
			if (op == '-') return *lvalTimestamp - *rvalTimestamp;
		}
		if (auto rvalInt = std::get_if<int64_t>(&rval))
		{
			if (op == '+') return *lvalTimestamp + *rvalInt;
			if (op == '-') return *lvalTimestamp - *rvalInt;
		}
	}
	else if (auto lvalString = std::get_if<std::string>(&lval))
	{
		if (auto rvalString = std::get_if<std::string>(&rval))
		{
			if (op == '+') return *rvalString + *lvalString;
		}
	}
	else if (auto lvalBool = std::get_if<bool>(&lval))
	{
		if (auto rvalBool = std::get_if<bool>(&rval))
		{
			if (op == '+') return *rvalBool + *lvalBool;
		}
	}
	return std::monostate{};
}

ValueType NegateNode::evaluate(const DIFFITEM& di) const
{
	auto rval = right->evaluate(di);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
	return std::monostate{};
}

FieldNode::FieldNode(const FilterContext* ctxt, const std::string& v) : ctxt(ctxt), field(v)
{
	int prefixlen = 0;
	const char* p = v.c_str();
	int64_t index = 0;
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
	if (v.compare(prefixlen, 4, "Size") == 0)
		func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> int64_t { return static_cast<int64_t>(di.diffFileInfo[index].size); };
	else if (v.compare(prefixlen, 4, "Date") == 0)
		func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> Poco::Timestamp { return di.diffFileInfo[index].mtime; };
	else if (v.compare(prefixlen, 4, "Name") == 0)
		func = [index](const FilterContext* ctxt, const DIFFITEM& di)-> std::string { return ucr::toUTF8(di.diffFileInfo[index].filename.get()); };
	else
	{
		throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));
	}
}

ValueType FieldNode::evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di);
}

FunctionNode::FunctionNode(const FilterContext* ctxt, const std::string& name, std::vector<ExprNode*>* args)
	: ctxt(ctxt), functionName(name), args(args)
{
	if (functionName == "abs")
	{
		if (args->size() != 1)
			throw std::runtime_error("abs function requires 1 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType { 
			auto arg1 = (*args)[0]->evaluate(di);
			if (auto arg1Int = std::get_if<int64_t>(&arg1))
				return abs(*arg1Int);
			return std::monostate{};
		};
	}
	else if (functionName == "today")
	{
		if (args && args->size() != 0)
			throw std::runtime_error("today function requires 0 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType {
				return *ctxt->today;
			};
	}
	else if (functionName == "now")
	{
		if (args && args->size() != 0)
			throw std::runtime_error("now function requires 0 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType {
				return *ctxt->now;
			};
	}
	else
	{
		throw std::runtime_error("Unknown function: " + std::string(functionName.begin(), functionName.end()));
	}
}

ValueType FunctionNode::evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di, args);
}

