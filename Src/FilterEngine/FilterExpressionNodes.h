/**
 * @file  FilterExpressionNodes.h
 *
 * @brief Filter expression evaluation classes.
 */
#pragma once

#include "FilterParser.h"
#include "FileContentRef.h"
#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <Poco/Timestamp.h>

namespace Poco { class RegularExpression; }
struct FilterExpression;
class DIFFITEM;
struct ValueType2;
using ValueType = std::variant<std::monostate, bool, int64_t, Poco::Timestamp, std::shared_ptr<Poco::RegularExpression>, std::string, FileContentRef, std::unique_ptr<std::vector<ValueType2>>>;
struct ValueType2 { ValueType value; };

struct ExprNode
{
	virtual ~ExprNode() { }
	virtual ExprNode* Optimize() { return this; }
	virtual ValueType Evaluate(const DIFFITEM& di) const = 0;
};

struct OrNode : public ExprNode
{
	OrNode(ExprNode* l, ExprNode* r) : left(l), right(r) { }
	virtual ~OrNode()
	{
		delete left;
		delete right;
	}
	ExprNode* Optimize() override;
	ValueType Evaluate(const DIFFITEM& di) const override;
	ExprNode* left;
	ExprNode* right;
};

struct AndNode : public ExprNode
{
	AndNode(ExprNode* l, ExprNode* r) : left(l), right(r) { }
	virtual ~AndNode()
	{
		delete left;
		delete right;
	}
	ExprNode* Optimize() override;
	ValueType Evaluate(const DIFFITEM& di) const override;
	ExprNode* left;
	ExprNode* right;
};

struct NotNode : public ExprNode
{
	NotNode(ExprNode* e) : expr(e) { }
	virtual ~NotNode()
	{
		delete expr;
	}
	ExprNode* Optimize() override;
	ValueType Evaluate(const DIFFITEM& di) const override;
	ExprNode* expr;
};

struct BinaryOpNode : public ExprNode
{
	BinaryOpNode(ExprNode* l, int o, ExprNode* r) : left(l), right(r), op(o) { }
	virtual ~BinaryOpNode()
	{
		delete left;
		delete right;
	}
	ExprNode* Optimize() override;
	ValueType Evaluate(const DIFFITEM& di) const override;
	int op;
	ExprNode* left;
	ExprNode* right;
};

struct NegateNode : public ExprNode
{
	NegateNode(ExprNode* r) : right(r) { }
	virtual ~NegateNode()
	{
		delete right;
	}
	ExprNode* Optimize() override;
	ValueType Evaluate(const DIFFITEM& di) const override;
	ExprNode* right;
};

struct FieldNode : public ExprNode
{
	FieldNode(const FilterExpression* ctxt, const std::string& v);
	ValueType Evaluate(const DIFFITEM& di) const override;
	const FilterExpression* ctxt;
	std::string field;
	std::function<ValueType(const FilterExpression* ctxt, const DIFFITEM& di)> func;
};

struct FunctionNode : public ExprNode
{
	FunctionNode(const FilterExpression* ctxt, const std::string& name, std::vector<ExprNode*>* args);
	virtual ~FunctionNode();
	ValueType Evaluate(const DIFFITEM& di) const override;
	const FilterExpression* ctxt;
	std::string functionName;
	std::vector<ExprNode*>* args;
	std::function<ValueType(const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args)> func;
};

struct BoolLiteral : public ExprNode
{
	BoolLiteral(bool v) : value(v) { }
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	bool value;
};

struct IntLiteral : public ExprNode
{
	IntLiteral(int64_t v) : value(v) { }
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct StringLiteral : public ExprNode
{
	StringLiteral(const std::string& v) : value(v) { }
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	std::string value;
};

struct SizeLiteral : public ExprNode
{
	SizeLiteral(const std::string& v);
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct DateTimeLiteral : public ExprNode
{
	DateTimeLiteral(const std::string& v);
	DateTimeLiteral(const Poco::Timestamp& v) : value(v) { }
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	Poco::Timestamp value;
};

struct DurationLiteral : public ExprNode
{
	DurationLiteral(const std::string& v);
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct VersionLiteral : public ExprNode
{
	VersionLiteral(const std::string& v);
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	int64_t value;
};

struct RegularExpressionLiteral : public ExprNode
{
	RegularExpressionLiteral(const std::string& v);
	inline ValueType Evaluate(const DIFFITEM& di) const override { return value; }
	std::shared_ptr<Poco::RegularExpression> value;
};

