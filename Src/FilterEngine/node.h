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
	std::string field;
	std::string op;
	ExprNode* value;
	ComparisonNode(const std::string& f, const std::string& o, ExprNode* v) : field(f), op(o), value(v) {}
	~ComparisonNode() { delete value; }
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override {
		if (data.find(field) == data.end()) {
			std::cerr << "Error: Field '" << field << "' not found." << std::endl;
			return false;
		}
		const auto& fieldValue = data.at(field);
		auto val = value->evaluate(data);
		if (auto intVal = std::get_if<int>(&val))
		{
			if (auto dataInt = std::get_if<int>(&fieldValue))
			{
				if (op == "==") return *dataInt == *intVal;
				if (op == "!=") return *dataInt != *intVal;
				if (op == "<")  return *dataInt < *intVal;
				if (op == "<=") return *dataInt <= *intVal;
				if (op == ">")  return *dataInt > *intVal;
				if (op == ">=") return *dataInt >= *intVal;
			}
		}
		else if (auto stringVal = std::get_if<std::string>(&val))
		{
			if (auto dataString = std::get_if<std::string>(&fieldValue))
			{
				if (op == "==") return *dataString == *stringVal;
				if (op == "!=") return *dataString != *stringVal;
			}
		}
		else if (auto boolVal = std::get_if<bool>(&val))
		{
			if (auto dataBool = std::get_if<bool>(&fieldValue))
			{
				if (op == "==") return *dataBool == *boolVal;
				if (op == "!=") return *dataBool != *boolVal;
			}
		}
		std::cerr << "Error: Invalid comparison between field '" << field << "' and value." << std::endl;
		return false;
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
	StringLiteral(const std::string& v) : value(v) {}
	ValueType evaluate(const std::map<std::string, ValueType>& data) const override
	{
		return value;
	}
};

