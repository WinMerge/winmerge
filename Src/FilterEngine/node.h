#pragma once

#include <iostream>
#include <string>
#include <map>
#include <variant>

typedef std::variant<int, std::string, bool> ValueType;

struct ExprNode
{
	virtual ~ExprNode() {}
	virtual ValueType evaluate(const std::map<std::string, ValueType>& data) const = 0;
};

struct OrNode : public ExprNode
{
	ExprNode* left;
	ExprNode* right;
	OrNode(ExprNode* l, ExprNode* r) : left(l), right(r) {}
	~OrNode() { delete left; delete right; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		auto lval = left->evaluate(data);
		auto lbool = std::get_if<bool>(&lval);
		if (lbool && *lbool) return true;

		auto rval = right->evaluate(data);
		auto rbool = std::get_if<bool>(&rval);
		return rbool && *rbool;
	}
};

struct AndNode : public ExprNode
{
	ExprNode* left;
	ExprNode* right;
	AndNode(ExprNode* l, ExprNode* r) : left(l), right(r) {}
	~AndNode() { delete left; delete right; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		auto lval = left->evaluate(data);
		auto lbool = std::get_if<bool>(&lval);
		if (!lbool || !*lbool) return false;

		auto rval = right->evaluate(data);
		auto rbool = std::get_if<bool>(&rval);
		return rbool && *rbool;
	}
};

struct NotNode : public ExprNode
{
	ExprNode* expr;
	NotNode(ExprNode* e) : expr(e) {}
	~NotNode() { delete expr; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		auto val = expr->evaluate(data);
		auto boolVal = std::get_if<bool>(&val);
		return *boolVal ? false : true;
	}
};

struct ComparisonNode : public ExprNode
{
	std::string op;
	ExprNode* left;
	ExprNode* right;
	ComparisonNode(ExprNode* l, const std::string& o, ExprNode* r) : left(l), op(o), right(r) {}
	~ComparisonNode() { delete left; delete right; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		/*
		if (data.find(field) == data.end()) {
			std::cerr << "Error: Field '" << field << "' not found." << std::endl;
			return false;
		}
		*/
		auto lval = left->evaluate(data);
		auto rval = right->evaluate(data);
		if (auto lvalInt = std::get_if<int>(&lval))
		{
			if (auto rvalInt = std::get_if<int>(&rval))
			{
				if (op == "==") return *rvalInt == *lvalInt;
				if (op == "!=") return *rvalInt != *lvalInt;
				if (op == "<")  return *rvalInt < *lvalInt;
				if (op == "<=") return *rvalInt <= *lvalInt;
				if (op == ">")  return *rvalInt > *lvalInt;
				if (op == ">=") return *rvalInt >= *lvalInt;
			}
		}
		else if (auto lvalString = std::get_if<std::string>(&lval))
		{
			if (auto rvalString = std::get_if<std::string>(&rval))
			{
				if (op == "==") return *rvalString == *lvalString;
				if (op == "!=") return *rvalString != *lvalString;
			}
		}
		else if (auto lvalBool = std::get_if<bool>(&lval))
		{
			if (auto rvalBool = std::get_if<bool>(&rval))
			{
				if (op == "==") return *rvalBool == *lvalBool;
				if (op == "!=") return *rvalBool != *lvalBool;
			}
		}
//		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
		return false;
	}
};

struct ArithmeticNode : public ExprNode
{
	std::string op;
	ExprNode* left;
	ExprNode* right;
	ArithmeticNode(ExprNode* l, const std::string& o, ExprNode* r) : left(l), op(o), right(r) {}
	~ArithmeticNode() { delete left; delete right; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		auto lval = left->evaluate(data);
		auto rval = right->evaluate(data);
		if (auto lvalInt = std::get_if<int>(&lval))
		{
			if (auto rvalInt = std::get_if<int>(&rval))
			{
				if (op == "+") return *rvalInt + *lvalInt;
				if (op == "-") return *rvalInt - *lvalInt;
			}
		}
		else if (auto lvalString = std::get_if<std::string>(&lval))
		{
			if (auto rvalString = std::get_if<std::string>(&rval))
			{
				if (op == "+") return *rvalString + *lvalString;
			}
		}
		else if (auto lvalBool = std::get_if<bool>(&lval))
		{
			if (auto rvalBool = std::get_if<bool>(&rval))
			{
				if (op == "+") return *rvalBool + *lvalBool;
			}
		}
//		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
		return false;
	}
};

struct FieldNode : public ExprNode
{
	std::string field;
	FieldNode(const std::string& v) : field(v) {}
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		if (data.find(field) == data.end())
		{
			std::cerr << "Error: Field '" << field << "' not found." << std::endl;
			return false;
		}
		return data.at(field);
	}
};

struct BoolLiteral : public ExprNode
{
	bool value;
	BoolLiteral(bool v) : value(v) {}
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		return value;
	}
};

struct IntLiteral : public ExprNode
{
	int value;
	IntLiteral(int v) : value(v) {}
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		return value;
	}
};

struct StringLiteral : public ExprNode
{
	std::string value;
	StringLiteral(const std::string& v) : value(v)
	{
	}
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		return value;
	}
};

