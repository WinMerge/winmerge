#include "pch.h"
#include "FilterExpression.h"
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
			if (op == L"==") return *lvalInt == *rvalInt;
			if (op == L"!=") return *lvalInt != *rvalInt;
			if (op == L"<")  return *lvalInt <  *rvalInt;
			if (op == L"<=") return *lvalInt <= *rvalInt;
			if (op == L">")  return *lvalInt >  *rvalInt;
			if (op == L">=") return *lvalInt >= *rvalInt;
		}
	}
	else if (auto lvalString = std::get_if<std::wstring>(&lval))
	{
		if (auto rvalString = std::get_if<std::wstring>(&rval))
		{
			if (op == L"==") return *lvalString == *rvalString;
			if (op == L"!=") return *lvalString != *rvalString;
		}
	}
	else if (auto lvalBool = std::get_if<bool>(&lval))
	{
		if (auto rvalBool = std::get_if<bool>(&rval))
		{
			if (op == L"==") return *lvalBool == *rvalBool;
			if (op == L"!=") return *lvalBool != *rvalBool;
		}
	}
//		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
	return false;
}

ValueType ArithmeticNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto rval = right->evaluate(di);
	if (auto lvalInt = std::get_if<int64_t>(&lval))
	{
		if (auto rvalInt = std::get_if<int64_t>(&rval))
		{
			if (op == L"+") return *lvalInt + *rvalInt;
			if (op == L"-") return *lvalInt - *rvalInt;
			if (op == L"*") return *lvalInt * *rvalInt;
			if (op == L"/") return *lvalInt / *rvalInt;
			if (op == L"%") return *lvalInt % *rvalInt;
		}
	}
	else if (auto lvalString = std::get_if<std::wstring>(&lval))
	{
		if (auto rvalString = std::get_if<std::wstring>(&rval))
		{
			if (op == L"+") return *rvalString + *lvalString;
		}
	}
	else if (auto lvalBool = std::get_if<bool>(&lval))
	{
		if (auto rvalBool = std::get_if<bool>(&rval))
		{
			if (op == L"+") return *rvalBool + *lvalBool;
		}
	}
//		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
	return false;
}

ValueType NegateNode::evaluate(const DIFFITEM& di) const
{
	auto rval = right->evaluate(di);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
//		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
	return false;
}

FieldNode::FieldNode(const CDiffContext* ctxt, const std::wstring& v) : ctxt(ctxt), field(v)
{
	int prefixlen = 0;
	const wchar_t* p = v.c_str();
	int64_t index = 0;
	if (v.compare(0, 4, L"Left") == 0)
	{
		index = 0;
		prefixlen = 4;
	}
	else if (v.compare(0, 6, L"Middle") == 0)
	{
		index = 1;
		prefixlen = 6;
	}
	else if (v.compare(0, 5, L"Right") == 0)
	{
		index = ctxt->GetCompareDirs() < 3 ? 1 : 2;
		prefixlen = 5;
	}
	if (v.compare(prefixlen, 4, L"Size") == 0)
		func = [index](const CDiffContext* ctxt, const DIFFITEM& di) -> int64_t { return static_cast<int64_t>(di.diffFileInfo[index].size); };
	else if (v.compare(prefixlen, 4, L"Date") == 0)
		func = [index](const CDiffContext* ctxt, const DIFFITEM& di) -> Poco::Timestamp { return di.diffFileInfo[index].mtime; };
	else if (v.compare(prefixlen, 4, L"Name") == 0)
		func = [index](const CDiffContext* ctxt, const DIFFITEM& di)-> std::wstring { return di.diffFileInfo[index].filename.get(); };
	else
	{
//		std::cerr << "Error: Invalid field'" << field << "' and value." << std::endl;
	}
}

ValueType FieldNode::evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di);
}

FunctionNode::FunctionNode(const CDiffContext* ctxt, const std::wstring& name, std::vector<ExprNode*>* args)
	: functionName(name), args(args)
{
	if (functionName == L"abs")
	{
		if (args->size() != 1)
			throw std::runtime_error("abs function requires 1 arguments");
		func = [](const CDiffContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType { 
			auto arg1 = (*args)[0]->evaluate(di);
			if (auto arg1Int = std::get_if<int64_t>(&arg1))
				return abs(*arg1Int);
			throw std::runtime_error("abs function requires 1 integer arguments");
		};
	}
	else
	{
//		std::cerr << "Error: Invalid field'" << field << "' and value." << std::endl;
	}
}

ValueType FunctionNode::evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di, args);
}

