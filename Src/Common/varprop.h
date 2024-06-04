/**
 *  @file varprop.h
 *
 *  @brief Declaration of generic named property classes
 */ 
#pragma once

#include <ctime>
#include "UnicodeString.h"

namespace varprop
{

/**
 * @brief Types that variant type can contain.
 */
typedef enum
{
	VT_NULL,     /**< No type */
	VT_BOOL,     /**< Boolean type */
	VT_INT,      /**< Integer type */
	VT_FLOAT,    /**< Floating point type */
	VT_STRING,   /**< String type */
	VT_TIME      /**< Time type */
} VT_TYPE;

/**
 * @brief A variant class.
 * This class can hold several base types. Type is set when the value is set.
 * @todo Add function to get/set string values as UTF-8 string - to avoid
 *  conversions in call sites.
 */
class VariantValue
{
public:
	VariantValue();
	VariantValue(const VariantValue &value);
	VariantValue& operator=(const VariantValue& value);

	bool IsBool() const { return m_vtype == VT_BOOL; }
	bool IsInt() const { return m_vtype == VT_INT; }
	bool IsFloat() const { return m_vtype == VT_FLOAT; }
	bool IsString() const { return m_vtype == VT_STRING; }
	bool IsTime() const { return m_vtype == VT_TIME; }
	VT_TYPE GetType() const { return m_vtype; }

	void SetBool(bool v);
	void SetInt(int v);
	void SetFloat(double v);
	void SetString(const tchar_t *sz);
	void SetString(const String& sz);
	void SetTime(time_t v);

	void Clear();

	bool GetBool() const;
	int GetInt() const;
	double GetFloat() const;
	const String& GetString() const;
	time_t GetTime() const;

private:
	VT_TYPE m_vtype;  /**< Type of the variant. */
	bool m_bvalue;    /**< Boolean value of the variant. */
	int m_ivalue;     /**< Integer value of the variant. */
	double m_fvalue;  /**< Floating point value of the variant. */
	String m_svalue;  /**< String value of the variant. */
	time_t m_tvalue;  /**< Time value of the variant. */
};

} // namespace
