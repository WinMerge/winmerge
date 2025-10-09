/**
 * @file  FunctionNodeProperty.cpp
 * @brief Implementation file for property-related functions in filter expressions
 */
#include "pch.h"
#include "FilterExpressionNodes.h"
#include "FilterExpression.h"
#include "DiffContext.h"
#include "paths.h"

static ValueType ConvertPROPVARIANTToValueType(const PROPVARIANT& propvalue)
{
	switch (propvalue.vt)
	{
	case VT_EMPTY:
		return std::monostate{};
	case VT_I4:
		return static_cast<int64_t>(propvalue.lVal);
	case VT_I8:
		return static_cast<int64_t>(propvalue.hVal.QuadPart);
	case VT_R8:
		return static_cast<double>(propvalue.dblVal);
	case VT_LPWSTR:
		return ucr::toUTF8(propvalue.pwszVal);
	case VT_BOOL:
		return propvalue.boolVal != VARIANT_FALSE;
	case VT_VECTOR|VT_UI1:
	{
		const CAUB& buf = propvalue.caub;
		std::string value;
		for (unsigned i = 0; i < buf.cElems; ++i)
		{
			BYTE c = buf.pElems[i];
			value += "0123456789abcdef"[c >> 4];
			value += "0123456789abcdef"[c & 0xf];
		}
		return value;
	}
	default:
		return std::monostate{};
	}
}

static auto prop(int index, const String& name, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	if (!di.diffcode.exists(index))
		return std::monostate{};
	auto& properties = const_cast<DIFFITEM&>(di).diffFileInfo[index].m_pAdditionalProperties;
	if (!di.diffFileInfo[index].m_pAdditionalProperties)
	{
		properties.reset(new PropertyValues());
		if (di.diffcode.exists(index))
		{
			const String relpath = paths::ConcatPath(di.diffFileInfo[index].path, di.diffFileInfo[index].filename);
			const String path = paths::ConcatPath(ctxt->ctxt->GetPath(index), relpath);
			ctxt->ctxt->m_pPropertySystem->GetPropertyValues(path, *properties);
		}
		else
		{
			size_t numprops = ctxt->ctxt->m_pPropertySystem->GetCanonicalNames().size();
			properties->Resize(numprops);
		}
	}
	const int propindex = ctxt->ctxt->m_pPropertySystem->GetPropertyIndex(name);
	if (propindex < 0)
		return std::monostate{};
	return ConvertPROPVARIANTToValueType((*properties)[propindex]);
}

static auto propary(const String& name, const FilterExpression* ctxt, const DIFFITEM& di) -> ValueType
{
	std::shared_ptr<std::vector<ValueType2>> values = std::make_shared<std::vector<ValueType2>>();
	const int dirs = ctxt->ctxt->GetCompareDirs();
	for (int i = 0; i < dirs; ++i)
		values->emplace_back(ValueType2{ prop(i, name, ctxt, di) });
	return values;
}

void FunctionNode::SetPropFunc()
{
	if (!args || args->size() != 1)
		throw std::invalid_argument("prop function requires 1 arguments");
	auto strLit = dynamic_cast<StringLiteral*>((*args)[0]);
	if (!strLit)
		throw std::invalid_argument("prop function requires a string literal as argument");
	String propName = ucr::toTString(strLit->value);
	PropertySystem propSys({ propName });
	const int propindex = propSys.GetPropertyIndex(propName);
	if (propindex < 0)
		throw InvalidPropertyNameError(strLit->value);
	func = [propName](const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
		{ return propary(propName, ctxt, di); };
}

void FunctionNode::SetLeftMiddleRightPropFunc()
{
	if (!args || args->size() != 1)
		throw std::invalid_argument(functionName + " function requires 1 arguments");
	auto strLit = dynamic_cast<StringLiteral*>((*args)[0]);
	if (!strLit)
		throw std::invalid_argument(functionName + " function requires a string literal as argument");
	String propName = ucr::toTString(strLit->value);
	PropertySystem propSys({ propName });
	const int propindex = propSys.GetPropertyIndex(propName);
	if (propindex < 0)
		throw InvalidPropertyNameError(strLit->value);
	int side = 0;
	if (functionName == "leftprop")
		side = 0;
	else if (functionName == "middleprop")
		side = 1;
	else if (functionName == "rightprop")
		side = -1;
	func = [propName, side](const FilterExpression* ctxt, const DIFFITEM& di, std::vector<ExprNode*>* args) -> ValueType
		{ return prop((side < 0) ? (ctxt->ctxt->GetCompareDirs() - 1) : side, propName, ctxt, di); };
}
