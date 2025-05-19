#pragma once

#include "FilterParser.h"
#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <Poco/Timestamp.h>

struct FilterContext;
class DIFFITEM;
typedef std::variant<std::monostate, int64_t, Poco::Timestamp, std::string, bool> ValueType;

struct ExprNode
{
	virtual ~ExprNode()
	{
	}
	virtual ValueType evaluate(const DIFFITEM& di) const = 0;
};

struct OrNode : public ExprNode
{
	ExprNode* left;
	ExprNode* right;
	OrNode(ExprNode* l, ExprNode* r) : left(l), right(r)
	{
	}
	virtual ~OrNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct AndNode : public ExprNode
{
	ExprNode* left;
	ExprNode* right;
	AndNode(ExprNode* l, ExprNode* r) : left(l), right(r)
	{
	}
	virtual ~AndNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct NotNode : public ExprNode
{
	ExprNode* expr;
	NotNode(ExprNode* e) : expr(e)
	{
	}
	virtual ~NotNode()
	{
		delete expr;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct ComparisonNode : public ExprNode
{
	int op;
	ExprNode* left;
	ExprNode* right;
	ComparisonNode(ExprNode* l, const std::string& o, ExprNode* r) : left(l), right(r)
	{
		if (o == "==")
			op = EQ;
		else if (o == "!=")
			op = NE;
		else if (o == "<")
			op = LT;
		else if (o == "<=")
			op = LE;
		else if (o == ">")
			op = GT;
		else if (o == ">=")
			op = GE;
		else if (o == "CONTAINS")
			op = CONTAINS;
		else if (o == "MATCHES")
			op = MATCHES;
		else
			op = EQ;
	}
	virtual ~ComparisonNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct ArithmeticNode : public ExprNode
{
	char op;
	ExprNode* left;
	ExprNode* right;
	ArithmeticNode(ExprNode* l, char o, ExprNode* r) : left(l), op(o), right(r)
	{
	}
	virtual ~ArithmeticNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct NegateNode : public ExprNode
{
	ExprNode* right;
	NegateNode(ExprNode* r) : right(r)
	{
	}
	virtual ~NegateNode()
	{
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct FieldNode : public ExprNode
{
	const FilterContext* ctxt;
	std::string field;
	std::function<ValueType(const FilterContext* ctxt, const DIFFITEM& di)> func;
	FieldNode(const FilterContext* ctxt, const std::string& v);
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct FunctionNode : public ExprNode
{
	const FilterContext* ctxt;
	std::string functionName;
	std::vector<ExprNode*>* args;
	std::function<ValueType(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args)> func;
	FunctionNode(const FilterContext* ctxt, const std::string& name, std::vector<ExprNode*>* args);
	virtual ~FunctionNode()
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
	ValueType evaluate(const DIFFITEM& di) const override;
};

struct BoolLiteral : public ExprNode
{
	bool value;
	BoolLiteral(bool v) : value(v)
	{
	}
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};

struct IntLiteral : public ExprNode
{
	int64_t value;
	IntLiteral(int64_t v) : value(v)
	{
	}
	inline ValueType evaluate(const DIFFITEM& di) const override
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
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};

