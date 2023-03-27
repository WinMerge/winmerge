/**
 *  @file varprop.cpp
 *
 *  @brief Implementation of generic named property classes
 */ 

#include "pch.h"
#include "varprop.h"
#include <cassert>

namespace varprop
{
/**
 * @brief Default constructor.
 */
VariantValue::VariantValue()
: m_vtype(VT_NULL), m_bvalue(false), m_ivalue(0), m_fvalue(0),
  m_tvalue(0)
{
}

/**
 * @brief Copy constructor.
 * @param [in] value Object to copy.
 */
VariantValue::VariantValue(const VariantValue &value)
	: m_vtype(value.m_vtype)
	, m_bvalue(value.m_bvalue)
	, m_ivalue(value.m_ivalue)
	, m_fvalue(value.m_fvalue)
	, m_tvalue(value.m_tvalue)
	, m_svalue(value.m_svalue)
{
}

/**
 * @brief Assignment operator override.
 * @param [in] Object to copy.
 * @return Copy of given object.
 */
VariantValue& VariantValue::operator=(const VariantValue& value)
{
	if (this != &value)
	{
		m_vtype = value.m_vtype;
		m_bvalue = value.m_bvalue;
		m_ivalue = value.m_ivalue;
		m_fvalue = value.m_fvalue;
		m_svalue = value.m_svalue;
		m_tvalue = value.m_tvalue;
	}
	return *this;
}

/**
 * @brief Set boolean value.
 * @param [in] v Boolean value to set.
 */
void VariantValue::SetBool(bool v)
{
	Clear();
	m_vtype = VT_BOOL;
	m_bvalue = v;
}

/**
 * @brief Set integer value.
 * @param [in] v Integer value to set.
 */
void VariantValue::SetInt(int v)
{ 
	Clear();
	m_vtype = VT_INT;
	m_ivalue = v;
}

/**
 * @brief Set floating point value.
 * @param [in] v Floating point value to set.
 */
void VariantValue::SetFloat(double v)
{
	Clear();
	m_vtype = VT_FLOAT;
	m_fvalue = v;
}

/**
 * @brief Set string value.
 * @param [in] sz String value to set. Can be a `nullptr`.
 */
void VariantValue::SetString(const tchar_t *sz)
{
	Clear();
	m_vtype = VT_STRING;
	if (sz != nullptr)
		m_svalue = sz;
}

/**
 * @brief Set string value.
 * @param [in] sz String value to set.
 */
void VariantValue::SetString(const String& sz)
{
	m_svalue = sz;
	m_vtype = VT_STRING;
}

/**
 * @brief Set time value.
 * @param [in] v Time value to set.
 */
void VariantValue::SetTime(time_t v)
{
	Clear();
	m_vtype = VT_TIME;
	m_tvalue = v;
}

/**
 * @brief Clear variant's value (reset to defaults).
 */
void VariantValue::Clear()
{
	m_vtype = VT_NULL;
	m_bvalue = false;
	m_ivalue = 0;
	m_fvalue = 0;
	m_svalue.erase();
	m_tvalue = 0;
}

/**
 * @brief Get boolean value.
 * @return Boolean value.
 */
bool VariantValue::GetBool() const
{
	assert(m_vtype == VT_BOOL);
	return m_bvalue;
}

/**
 * @brief Get integer value.
 * @return Integer value.
 */
int VariantValue::GetInt() const
{
	assert(m_vtype == VT_INT);
	return m_ivalue;
}

/**
 * @brief Get floating point value.
 * @return Floating point value.
 */
double VariantValue::GetFloat() const
{
	assert(m_vtype == VT_FLOAT);
	return m_fvalue;
}

/**
 * @brief Get string value.
 * @return String value.
 */
const String& VariantValue::GetString() const
{
	assert(m_vtype == VT_STRING);
	return m_svalue;
}

/**
 * @brief Get time value.
 * @return Time value.
 */
time_t VariantValue::GetTime() const
{
	assert(m_vtype == VT_TIME);
	return m_tvalue;
}

} // namespace
