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
	return rbool && *rbool;
}

ValueType AndNode::evaluate(const DIFFITEM& di) const
{
	auto lval = left->evaluate(di);
	auto lbool = std::get_if<bool>(&lval);
	if (!lbool || !*lbool) return false;

	auto rval = right->evaluate(di);
	auto rbool = std::get_if<bool>(&rval);
	return rbool && *rbool;
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
	int64_t index;
	if (v.find(L"Left") == 0)
		index = 0;
	else if (v.find(L"Middle") == 0)
		index = 1;
	else if (v.find(L"Right") == 0)
		index = ctxt->GetCompareDirs() < 3 ? 1 : 2;
	if (v.find(L"Size") == v.length() - 4)
		func = [index](const CDiffContext* ctxt, const DIFFITEM& di) { return (int64_t)di.diffFileInfo[index].size; };
	else if (v.find(L"Name") == v.length() - 4)
		func = [index](const CDiffContext* ctxt, const DIFFITEM& di) { return di.diffFileInfo[index].filename.get(); };
	else
	{
//		std::cerr << "Error: Invalid field'" << field << "' and value." << std::endl;
	}
}
ValueType FieldNode::evaluate(const DIFFITEM& di) const
{
	return func(ctxt, di);
}

