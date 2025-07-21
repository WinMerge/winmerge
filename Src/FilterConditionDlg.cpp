/** 
 * @file  FilterConditionDlg.cpp
 *
 * @brief Implementation of the dialog used to select table properties
 */

#include "stdafx.h"
#include "FilterConditionDlg.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg dialog


CFilterConditionDlg::CFilterConditionDlg(CWnd* pParent /*= nullptr*/)
: CTrDialog(CFilterConditionDlg::IDD, pParent)
, m_bDiff(false)
, m_nSide(0)
{
}

CFilterConditionDlg::CFilterConditionDlg(bool diff, int side, const String& field, const String& ope, const String& transform, CWnd* pParent /* = nullptr*/)
: CTrDialog(CFilterConditionDlg::IDD, pParent)
, m_bDiff(diff)
, m_nSide(side)
, m_sField(field)
, m_sOperator(ope)
, m_sLHS(transform)
{
	//{{AFX_DATA_INIT(CFilterConditionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFilterConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterConditionDlg)
	DDX_CBString(pDX, IDC_CONDITION_VALUE1, m_sValue1);
	DDX_CBString(pDX, IDC_CONDITION_VALUE2, m_sValue2);
	DDX_Control(pDX, IDC_CONDITION_VALUE1, m_ctlValue1);
	DDX_Control(pDX, IDC_CONDITION_VALUE2, m_ctlValue2);
	DDX_DateTimeCtrl(pDX, IDC_CONDITION_VALUEDTP1, m_tmValue1);
	DDX_DateTimeCtrl(pDX, IDC_CONDITION_VALUEDTP2, m_tmValue2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilterConditionDlg, CTrDialog)
	//{{AFX_MSG_MAP(CFilterConditionDlg)
	ON_CBN_SELCHANGE(IDC_CONDITION_OPERATOR, OnCbnSelchangeOperator)
	ON_CBN_EDITCHANGE(IDC_CONDITION_VALUE1, OnCbnEditchangeValue)
	ON_CBN_EDITCHANGE(IDC_CONDITION_VALUE2, OnCbnEditchangeValue)
	ON_CBN_SELCHANGE(IDC_CONDITION_VALUE1, OnCbnSelchangeValue)
	ON_CBN_SELCHANGE(IDC_CONDITION_VALUE2, OnCbnSelchangeValue)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_CONDITION_VALUEDTP1, OnDateTimeChange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_CONDITION_VALUEDTP2, OnDateTimeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

String CFilterConditionDlg::GetLHS() const
{
	String lhs;
	if (!m_bDiff)
	{
		const String Sides[] = { _T(""), _T("Left"), _T("Middle"), _T("Right")};
		lhs = strutils::format_string1(m_sLHS, Sides[m_nSide] + m_sField);
	}
	else
	{
		const String DiffSides1[] = { _T("Left"), _T("Left"), _T("Middle") };
		const String DiffSides2[] = { _T("Right"), _T("Middle"), _T("Right") };
		lhs = strutils::format_string2(m_sLHS, DiffSides1[m_nSide] + m_sField, DiffSides2[m_nSide] + m_sField);
	}
	return lhs;
}

String CFilterConditionDlg::GetExpression()
{
	auto expressionptr = (intptr_t)GetDlgItemDataCurSel(IDC_CONDITION_OPERATOR);
	if (expressionptr == -1)
		return _T("");
	String expression = (const wchar_t*)expressionptr;
	const String lhs = GetLHS();
	String result;
	if (m_sField == _T("Size") || m_sField == _T("Date") || m_sLHS == _T("lineCount(%1)"))
	{
		result = strutils::format_string3(expression, lhs, m_sValue1, m_sValue2);
	}
	else
	{
		String value1;
		String value2;
		if (m_sField == _T("DateStr"))
		{
			value1 = m_tmValue1.Format(_T("%Y-%m-%d"));
			value2 = m_tmValue2.Format(_T("%Y-%m-%d"));
		}
		else
		{
			value1 = m_sValue1;
			value2 = m_sValue2;
		}
		strutils::replace(value1, _T("\""), _T("\"\""));
		strutils::replace(value2, _T("\""), _T("\"\""));
		value1 = _T("\"") + value1 + _T("\"");
		value2 = _T("\"") + value2 + _T("\"");
		result = strutils::format_string3(expression, lhs, value1, value2);
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg message handlers

/**
 * @brief Dialog initializer function.
 */
BOOL CFilterConditionDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	SetDlgItemText(IDC_CONDITION_LHS, GetLHS());

	// Initialize the operator combo box
	if (m_sField == _T("Size") || m_sField == _T("Date") || m_sField == _T("DateStr") || m_sLHS == _T("lineCount(%1)"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_OPERATOR,
			{
				{ _("Equals"), L"%1 = %2" },
				{ _("Does not equal"), L"%1 != %2" },
				{ _("Less than"), L"%1 < %2" },
				{ _("Less than or equal to"), L"%1 <= %2" },
				{ _("Greater than or equal to"), L"%1 >= %2" },
				{ _("Greater than"), L"%1 > %2" },
				{ _("Between"), L"(%1 >= %2 and %1 <= %3)" },
				{ _("Not Between"), L"not (%1 >= %2 and %1 <= %3)" },
			}, m_sOperator);
	}
	else if (m_sField == _T("Content"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_OPERATOR,
			{
				{ _("Contains"), L"%1 contains %2" },
				{ _("Not Contains"), L"%1 not contains %2" },
				{ _("Contains (regex)"), L"%1 recontains %2" },
				{ _("Not Contains (regex)"), L"%1 not recontains %2" },
			}, m_sOperator);
	} 

	// Initialize the value combo boxes
	if (m_sField == _T("Size"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE1, { _("0B"), _("1B"), _("10B"), _("100B"), _("1KB"), _("10KB"), _("100KB"), _("1MB"), _("10MB"), _("100MB"), _("1GB") });
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE2, { _("0B"), _("1B"), _("10B"), _("100B"), _("1KB"), _("10KB"), _("100KB"), _("1MB"), _("10MB"), _("100MB"), _("1GB") });
		m_sValue1 = _T("0B");
		m_sValue2 = _T("0B");
	}
	else if (m_sLHS == _T("lineCount(%1)"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE1, { _("0"), _("1"), _("10"), _("100"),_("1000"), _("10000"), _("100000") });
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE2, { _("0"), _("1"), _("10"), _("100"),_("1000"), _("10000"), _("100000") });
		m_sValue1 = _T("0");
		m_sValue2 = _T("0");
	}
	else if (m_sField == _T("Date"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE1, { _("0second"), _("1second"), _("1minute"), _("1hour"), _("1day"), _("1week") });
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE2, { _("0second"), _("1second"), _("1minute"), _("1hour"), _("1day"), _("1week") });
		m_sValue1 = _T("0second");
		m_sValue2 = _T("0second");
	}
	else if (m_sField == _T("DateStr"))
	{
		m_tmValue1 = CTime::GetCurrentTime();
		m_tmValue2 = CTime::GetCurrentTime();
	}
	else if (m_sField == _T("Content"))
	{
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE1, { _("") });
		SetDlgItemComboBoxList(IDC_CONDITION_VALUE2, { _("") });
	}

	OnCbnSelchangeOperator();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterConditionDlg::OnCbnSelchangeOperator()
{
	auto expressionptr = (intptr_t)GetDlgItemDataCurSel(IDC_CONDITION_OPERATOR);
	if (expressionptr == -1)
		return;
	String expression = (const wchar_t*)expressionptr;
	const bool showValue2 = expression.find(_T("%3")) != String::npos;
	const bool showDatePicker = (m_sField == _T("DateStr"));
	ShowDlgItem(IDC_CONDITION_VALUE1, !showDatePicker);
	ShowDlgItem(IDC_CONDITION_VALUE2, !showDatePicker && showValue2);
	ShowDlgItem(IDC_CONDITION_VALUEDTP1, showDatePicker);
	ShowDlgItem(IDC_CONDITION_VALUEDTP2, showDatePicker && showValue2);
	SetDlgItemText(IDC_CONDITION_EXPRESSION, GetExpression());
}

void CFilterConditionDlg::OnCbnEditchangeValue()
{
	UpdateData(TRUE);
	SetDlgItemText(IDC_CONDITION_EXPRESSION, GetExpression());
}

void CFilterConditionDlg::OnCbnSelchangeValue()
{
	CString value;
	const int sel1 = m_ctlValue1.GetCurSel();
	if (sel1 == CB_ERR)
	{
		m_sValue1.clear();
	}
	else
	{
		m_ctlValue1.GetLBText(sel1, value);
		m_sValue1 = value.GetString();
	}
	const int sel2 = m_ctlValue2.GetCurSel();
	if (sel2 == CB_ERR)
	{
		m_sValue2.clear();
	}
	else
	{
		m_ctlValue2.GetLBText(sel2, value);
		m_sValue2 = value.GetString();
	}
	SetDlgItemText(IDC_CONDITION_EXPRESSION, GetExpression());
}

void CFilterConditionDlg::OnDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateData(TRUE);
	SetDlgItemText(IDC_CONDITION_EXPRESSION, GetExpression());
	*pResult = 0;
}

void CFilterConditionDlg::OnOK()
{
	UpdateData(TRUE);
	m_sExpression = GetExpression();
	CDialog::OnOK();
}

