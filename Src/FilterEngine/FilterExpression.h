#pragma once

#include "FilterParser.h"
#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <Poco/Exception.h>
#include <Poco/Timestamp.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

struct FilterContext;
class DIFFITEM;
typedef std::variant<std::monostate, bool, int64_t, Poco::Timestamp, std::string> ValueType;

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

struct SizeLiteral : public ExprNode
{
	int64_t value;
	SizeLiteral(const std::string& v)
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
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};


struct DateTimeLiteral : public ExprNode
{
	Poco::Timestamp value;
	DateTimeLiteral(const std::string& v)
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
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};

struct TimeLiteral : public ExprNode
{
	int64_t value;
	TimeLiteral(const std::string& v)
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
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};

struct VersionLiteral : public ExprNode
{
	int64_t value;
	VersionLiteral(const std::string& v)
	{
		int major = 0, minor = 0, build = 0, revision = 0;
		sscanf_s(v.c_str(), "%d.%d.%d.%d", &major, &minor, &build, &revision);
		value = (static_cast<int64_t>(major) << 48) + (static_cast<int64_t>(minor) << 32) + (build << 16) + revision;
	}
	inline ValueType evaluate(const DIFFITEM& di) const override
	{
		return value;
	}
};

