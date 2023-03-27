#pragma once

inline void DDX_Check(CDataExchange* pDX, int nIDC, bool& value)
{
	BOOL BOOL_value = value;
	DDX_Check(pDX, nIDC, BOOL_value);
	value = !!BOOL_value;
}

inline void DDX_Radio(CDataExchange* pDX, int nIDC, bool& value)
{
	BOOL BOOL_value = value;
	DDX_Radio(pDX, nIDC, BOOL_value);
	value = !!BOOL_value;
}

template<typename T>
inline void DDX_Text(CDataExchange* pDX, int nIDC, std::basic_string<T>& value)
{
	CString cstrValue = value.c_str();
	DDX_Text(pDX, nIDC, cstrValue);
	value = cstrValue;
}

template<typename T>
inline void DDX_CBString(CDataExchange* pDX, int nIDC, std::basic_string<T>& value)
{
	CString cstrValue = value.c_str();
	DDX_CBString(pDX, nIDC, cstrValue);
	value = cstrValue;
}

template<typename T>
inline void DDX_CBStringExact(CDataExchange* pDX, int nIDC, std::basic_string<T>& value)
{
	CString cstrValue = value.c_str();
	DDX_CBStringExact(pDX, nIDC, cstrValue);
	value = cstrValue;
}

