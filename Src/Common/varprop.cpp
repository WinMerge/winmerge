/**
 *  @file varprop.cpp
 *
 *  @brief Implementation of generic named property classes
 */ 
// ID line follows -- this is updated by SVN
// $Id$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "varprop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace varprop
{
/**
 * @brief Default constructor.
 */
VariantValue::VariantValue()
: m_vtype(VT_NULL), m_bvalue(false), m_ivalue(0), m_fvalue(0),
  m_tvalue(0), m_svalue(NULL)
{
}

/**
 * @brief Copy constructor.
 * @param [in] value Object to copy.
 */
VariantValue::VariantValue(const VariantValue &value)
{
	m_vtype = value.m_vtype;
	m_bvalue = value.m_bvalue;
	m_ivalue = value.m_ivalue;
	m_fvalue = value.m_fvalue;
	if (value.m_svalue != NULL)
	{
		m_svalue = _tcsdup(value.m_svalue);
	}
	else
		m_svalue = NULL;
	m_tvalue = value.m_tvalue;
}

/**
 * @brief Destructor.
 * Clean up possible memory allocations for values.
 */
VariantValue::~VariantValue()
{
	if (m_svalue != NULL)
	{
		ASSERT(_CrtIsValidHeapPointer(m_svalue));
		free(m_svalue);
		m_svalue = NULL;
	}
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
		free(m_svalue);
		if (value.m_svalue != NULL)
		{
			m_svalue = _tcsdup(value.m_svalue);
		}
		else
			m_svalue = NULL;
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
 * @param [in] sz String value to set. Can be a NULL.
 */
void VariantValue::SetString(const TCHAR *sz)
{
	Clear();
	m_vtype = VT_STRING;
	if (sz != NULL)
		m_svalue = _tcsdup(sz);
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
	if (m_svalue != NULL)
	{
		free(m_svalue);
		m_svalue = NULL;
	}
	m_tvalue = 0;
}

/**
 * @brief Get boolean value.
 * @return Boolean value.
 */
bool VariantValue::GetBool() const
{
	ASSERT(m_vtype == VT_BOOL);
	return m_bvalue;
}

/**
 * @brief Get integer value.
 * @return Integer value.
 */
int VariantValue::GetInt() const
{
	ASSERT(m_vtype == VT_INT);
	return m_ivalue;
}

/**
 * @brief Get floating point value.
 * @return Floating point value.
 */
double VariantValue::GetFloat() const
{
	ASSERT(m_vtype == VT_FLOAT);
	return m_fvalue;
}

/**
 * @brief Get string value.
 * @return String value.
 * @note Returned string is a new copy which must be freed with free() after use.
 */
TCHAR * VariantValue::GetString() const
{
	ASSERT(m_vtype == VT_STRING);
	if (m_svalue != NULL)
		return _tcsdup(m_svalue);
	else
		return NULL;
}

/**
 * @brief Get time value.
 * @return Time value.
 */
time_t VariantValue::GetTime() const
{
	ASSERT(m_vtype == VT_TIME);
	return m_tvalue;
}

} // namespace
