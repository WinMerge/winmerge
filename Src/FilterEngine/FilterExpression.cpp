#include "pch.h"
#include "FilterExpression.h"
#include "FilterLexer.h"
#include "FilterEngine.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <string>
#include <variant>
#include <Poco/RegularExpression.h>

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

ValueType NotNode::Evaluate(const DIFFITEM& di) const
{
	auto val = expr->Evaluate(di);
	auto boolVal = evalAsBool(val);
	if (!boolVal) return std::monostate{};
	return *boolVal ? false : true;
}

BinaryOpNode::BinaryOpNode(ExprNode* l, const std::string& o, ExprNode* r) : left(l), right(r)
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
	else if (o == "+")
		op = PLUS;
	else if (o == "-")
		op = MINUS;
	else if (o == "*")
		op = STAR;
	else if (o == "/")
		op = SLASH;
	else if (o == "%")
		op = MOD;
	else
		op = o.at(0);
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
					if (op == EQ) return *lvalInt == *rvalInt;
					if (op == NE) return *lvalInt != *rvalInt;
					if (op == LT) return *lvalInt < *rvalInt;
					if (op == LE) return *lvalInt <= *rvalInt;
					if (op == GT) return *lvalInt > *rvalInt;
					if (op == GE) return *lvalInt >= *rvalInt;
					if (op == PLUS) return *lvalInt + *rvalInt;
					if (op == MINUS) return *lvalInt - *rvalInt;
					if (op == STAR) return *lvalInt * *rvalInt;
					if (op == SLASH) return *lvalInt / *rvalInt;
					if (op == MOD) return *lvalInt % *rvalInt;
				}
			}
			else if (auto lvalTimestamp = std::get_if<Poco::Timestamp>(&lval))
			{
				if (auto rvalTimestamp = std::get_if<Poco::Timestamp>(&rval))
				{
					if (op == EQ) return *lvalTimestamp == *rvalTimestamp;
					if (op == NE) return *lvalTimestamp != *rvalTimestamp;
					if (op == LT) return *lvalTimestamp < *rvalTimestamp;
					if (op == LE) return *lvalTimestamp <= *rvalTimestamp;
					if (op == GT) return *lvalTimestamp > *rvalTimestamp;
					if (op == GE) return *lvalTimestamp >= *rvalTimestamp;
					if (op == MINUS) return *lvalTimestamp - *rvalTimestamp;
				}
				else if (auto rvalInt = std::get_if<int64_t>(&rval))
				{
					if (op == PLUS) return *lvalTimestamp + *rvalInt;
					if (op == MINUS) return *lvalTimestamp - *rvalInt;
				}
			}
			else if (auto lvalString = std::get_if<std::string>(&lval))
			{
				if (auto rvalString = std::get_if<std::string>(&rval))
				{
					if (op == EQ) return *lvalString == *rvalString;
					if (op == NE) return *lvalString != *rvalString;
					if (op == LT) return *lvalString < *rvalString;
					if (op == LE) return *lvalString <= *rvalString;
					if (op == GT) return *lvalString > *rvalString;
					if (op == GE) return *lvalString >= *rvalString;
					if (op == PLUS) return *rvalString + *lvalString;
					if (op == CONTAINS)
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
					if (op == MATCHES)
					{
						Poco::RegularExpression regex(*rvalString, Poco::RegularExpression::RE_CASELESS | Poco::RegularExpression::RE_UTF8);
						return regex.match(*lvalString);
					}
				}
			}
			else if (auto lvalBool = std::get_if<bool>(&lval))
			{
				if (auto rvalBool = std::get_if<bool>(&rval))
				{
					if (op == EQ) return *lvalBool == *rvalBool;
					if (op == NE) return *lvalBool != *rvalBool;
					if (op == PLUS) return static_cast<int64_t>(*rvalBool + *lvalBool);
				}
			}
			if (op == EQ)
				return false;
			else if (op == NE)
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

ValueType NegateNode::Evaluate(const DIFFITEM& di) const
{
	auto rval = right->Evaluate(di);
	if (auto rvalInt = std::get_if<int64_t>(&rval))
		return -*rvalInt;
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
	if (prefixlen == 0)
	{
		if (v == "Name")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				const int dirs = ctxt->ctxt->GetCompareDirs();
				for (int i = 0; i < dirs; ++i)
				{
					if (di.diffcode.exists(i))
						values->emplace_back(ValueType2{ ucr::toUTF8(di.diffFileInfo[i].filename.get()) });
					else
						values->emplace_back(ValueType2{ std::monostate{} });
				}
				return values;
			};
		}
		else if (v == "Folder")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				const int dirs = ctxt->ctxt->GetCompareDirs();
				for (int i = 0; i < dirs; ++i)
				{
					values->emplace_back(ValueType2{ ucr::toUTF8(di.diffFileInfo[i].path.get()) });
				}
				return values;
			};
		}
		else if (v == "Size")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				const int dirs = ctxt->ctxt->GetCompareDirs();
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
				{
					if (di.diffcode.exists(i))
						values->emplace_back(ValueType2{ static_cast<int64_t>(di.diffFileInfo[i].size) });
					else
						values->emplace_back(ValueType2{ std::monostate{} });
				}
				return values;
			};
		}
		else if (v == "Date")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				const int dirs = ctxt->ctxt->GetCompareDirs();
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
				{
					if (di.diffcode.exists(i))
						values->emplace_back(ValueType2{ di.diffFileInfo[i].mtime });
					else
						values->emplace_back(ValueType2{ std::monostate{} });
				}
				return values;
			};
		}
		else if (v == "CreationTime")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				const int dirs = ctxt->ctxt->GetCompareDirs();
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
				{
					if (di.diffcode.exists(i))
						values->emplace_back(ValueType2{ di.diffFileInfo[i].ctime });
					else
						values->emplace_back(ValueType2{ std::monostate{} });
				}
				return values;
			};
		}
		else if (v == "FileVersion")
		{
			func = [](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
				const int dirs = ctxt->ctxt->GetCompareDirs();
				std::unique_ptr<std::vector<ValueType2>> values = std::make_unique<std::vector<ValueType2>>();
				for (int i = 0; i < dirs; ++i)
				{
					if (di.diffcode.exists(i))
						values->emplace_back(ValueType2{ di.diffFileInfo[i].version.GetFileVersionQWORD() });
					else
						values->emplace_back(ValueType2{ std::monostate{} });
				}
				return values;
			};
		}
		else
			throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));
	}
	else
	{
		if (v.compare(prefixlen, 4, "Name") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
			if (di.diffcode.exists(index)) return ucr::toUTF8(di.diffFileInfo[index].filename.get()); else return std::monostate{};
			};
		else if (v.compare(prefixlen, 6, "Folder") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di)-> ValueType {
			return ucr::toUTF8(di.diffFileInfo[index].path.get());
			};
		else if (v.compare(prefixlen, 4, "Size") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> ValueType {
			if (di.diffcode.exists(index)) return static_cast<int64_t>(di.diffFileInfo[index].size); else return std::monostate{};
			};
		else if (v.compare(prefixlen, 4, "Date") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> ValueType {
			if (di.diffcode.exists(index)) return di.diffFileInfo[index].mtime; else return std::monostate{};
			};
		else if (v.compare(prefixlen, 12, "CreationTime") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> ValueType {
			if (di.diffcode.exists(index)) return di.diffFileInfo[index].ctime; else return std::monostate{};
			};
		else if (v.compare(prefixlen, 11, "FileVersion") == 0)
			func = [index](const FilterContext* ctxt, const DIFFITEM& di) -> ValueType {
			if (di.diffcode.exists(index)) return di.diffFileInfo[index].version.GetFileVersionQWORD() ; else return std::monostate{};
			};
		else
			throw std::runtime_error("Invalid field name: " + std::string(v.begin(), v.end()));
	}
}

ValueType FieldNode::Evaluate(const DIFFITEM& di) const
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
			auto arg1 = (*args)[0]->Evaluate(di);
			if (auto arg1Int = std::get_if<int64_t>(&arg1))
				return abs(*arg1Int);
			return std::monostate{};
		};
	}
	else if (functionName == "anyof")
	{
		if (args->size() != 1)
			throw std::runtime_error("anyof function requires 1 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType { 
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
		};
	}
	else if (functionName == "allof")
	{
		if (args->size() != 1)
			throw std::runtime_error("allof function requires 1 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType { 
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
		};
	}
	else if (functionName == "allequal")
	{
		if (args->size() < 1)
			throw std::runtime_error("allequal function requires at least 1 arguments");
		func = [](const FilterContext* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType { 
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
