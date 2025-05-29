#pragma once

#include "FilterParser.h"
#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <Poco/Exception.h>
#include <Poco/Timestamp.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

struct FilterContext;
class DIFFITEM;
struct ValueType2;
using ValueType = std::variant<std::monostate, bool, int64_t, Poco::Timestamp, std::string, std::unique_ptr<std::vector<ValueType2>>>;
struct ValueType2 { ValueType value; };

struct ExprNode
{
	virtual ~ExprNode()
	{
	}
	virtual ValueType evaluate(const DIFFITEM& di) const = 0;
};

struct OrNode : public ExprNode
{
	OrNode(ExprNode* l, ExprNode* r) : left(l), right(r)
	{
	}
	virtual ~OrNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
	ExprNode* left;
	ExprNode* right;
};

struct AndNode : public ExprNode
{
	AndNode(ExprNode* l, ExprNode* r) : left(l), right(r)
	{
	}
	virtual ~AndNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
	ExprNode* left;
	ExprNode* right;
};

struct NotNode : public ExprNode
{
	NotNode(ExprNode* e) : expr(e)
	{
	}
	virtual ~NotNode()
	{
		delete expr;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
	ExprNode* expr;
};

struct BinaryOpNode : public ExprNode
{
	BinaryOpNode(ExprNode* l, const std::string& o, ExprNode* r);
	virtual ~BinaryOpNode()
	{
		delete left;
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
	int op;
	ExprNode* left;
	ExprNode* right;
};

struct NegateNode : public ExprNode
{
	NegateNode(ExprNode* r) : right(r)
	{
	}
	virtual ~NegateNode()
	{
		delete right;
	}
	ValueType evaluate(const DIFFITEM& di) const override;
	ExprNode* right;
};

struct FieldNode : public ExprNode
{
	FieldNode(const FilterContext* ctxt, const std::string& v);
	ValueType evaluate(const DIFFITEM& di) const override;
	const FilterContext* ctxt;
	std::string field;
	std::function<ValueType(const FilterContext* ctxt, const DIFFITEM& di)> func;
};

struct FunctionNode : public ExprNode
{
	FunctionNode(const FilterContext* ctxt, const std::string& name, std::vector<ExprNode*>* args);
	virtual ~FunctionNode();
	ValueType evaluate(const DIFFITEM& di) const override;
	const FilterContext* ctxt;
	std::string functionName;
	std::vector<ExprNode*>* args;
	std::function<ValueType(const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args)> func;
};

struct BoolLiteral : public ExprNode
{
	BoolLiteral(bool v) : value(v) { }
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	bool value;
};

struct IntLiteral : public ExprNode
{
	IntLiteral(int64_t v) : value(v) { }
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct StringLiteral : public ExprNode
{
	StringLiteral(const std::string& v) : value(v) { }
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	std::string value;
};

struct SizeLiteral : public ExprNode
{
	SizeLiteral(const std::string& v);
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct DateTimeLiteral : public ExprNode
{
	DateTimeLiteral(const std::string& v);
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	Poco::Timestamp value;
};

struct DurationLiteral : public ExprNode
{
	DurationLiteral(const std::string& v);
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct VersionLiteral : public ExprNode
{
	VersionLiteral(const std::string& v);
	inline ValueType evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

