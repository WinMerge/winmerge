/** 
 * @file ComparisonResultFilterDlg.cpp
 *
 * @brief Implementation of the CComparisonResultFilterDlg class, which implements the dialog for filtering comparison results based on various criteria such as identical, different, left-only, right-only, etc.
 */
#include "StdAfx.h"
#include "ComparisonResultFilterDlg.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CComparisonResultFilterDlg::CComparisonResultFilterDlg(bool is3Way, CWnd* pParent)
	: CTrDialog(CComparisonResultFilterDlg::IDD, pParent)
	, m_is3Way(is3Way)
	, m_nIncludeExclude(0)
	, m_bIdentical(FALSE)
	, m_bDifferent(FALSE)
	, m_bLeftOnly(FALSE)
	, m_bRightOnly(FALSE)
	, m_bSkipped(FALSE)
	, m_bMiddleOnly(FALSE)
	, m_bLeftOnlyDifferent(FALSE)
	, m_bMiddleOnlyDifferent(FALSE)
	, m_bRightOnlyDifferent(FALSE)
	, m_bLeftOnlyMissing(FALSE)
	, m_bMiddleOnlyMissing(FALSE)
	, m_bRightOnlyMissing(FALSE)
{
}

void CComparisonResultFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_INCLUDE, m_nIncludeExclude);
	DDX_Check(pDX, IDC_CHECK_IDENTICAL, m_bIdentical);
	DDX_Check(pDX, IDC_CHECK_DIFFERENT, m_bDifferent);
	DDX_Check(pDX, IDC_CHECK_LEFT_ONLY, m_bLeftOnly);
	DDX_Check(pDX, IDC_CHECK_RIGHT_ONLY, m_bRightOnly);
	DDX_Check(pDX, IDC_CHECK_SKIPPED, m_bSkipped);
	if (m_is3Way)
	{
		DDX_Check(pDX, IDC_CHECK_MIDDLE_ONLY, m_bMiddleOnly);
		DDX_Check(pDX, IDC_CHECK_LEFT_ONLY_DIFFERENT, m_bLeftOnlyDifferent);
		DDX_Check(pDX, IDC_CHECK_MIDDLE_ONLY_DIFFERENT, m_bMiddleOnlyDifferent);
		DDX_Check(pDX, IDC_CHECK_RIGHT_ONLY_DIFFERENT, m_bRightOnlyDifferent);
		DDX_Check(pDX, IDC_CHECK_LEFT_ONLY_MISSING, m_bLeftOnlyMissing);
		DDX_Check(pDX, IDC_CHECK_MIDDLE_ONLY_MISSING, m_bMiddleOnlyMissing);
		DDX_Check(pDX, IDC_CHECK_RIGHT_ONLY_MISSING, m_bRightOnlyMissing);
	}
}

BEGIN_MESSAGE_MAP(CComparisonResultFilterDlg, CTrDialog)
	ON_BN_CLICKED(IDC_RADIO_INCLUDE, OnBnClickedInclude)
	ON_BN_CLICKED(IDC_RADIO_EXCLUDE, OnBnClickedExclude)
END_MESSAGE_MAP()

BOOL CComparisonResultFilterDlg::OnInitDialog()
{
	__super::OnInitDialog();

	if (!m_is3Way)
	{
		GetDlgItem(IDC_CHECK_MIDDLE_ONLY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_LEFT_ONLY_DIFFERENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_MIDDLE_ONLY_DIFFERENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_RIGHT_ONLY_DIFFERENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_LEFT_ONLY_MISSING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_MIDDLE_ONLY_MISSING)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_RIGHT_ONLY_MISSING)->ShowWindow(SW_HIDE);
	}

	UpdateCheckboxStates();
	return TRUE;
}

void CComparisonResultFilterDlg::OnBnClickedInclude()
{
	UpdateData(TRUE);
	UpdateCheckboxStates();
}

void CComparisonResultFilterDlg::OnBnClickedExclude()
{
	UpdateData(TRUE);
	UpdateCheckboxStates();
}

void CComparisonResultFilterDlg::UpdateCheckboxStates()
{
}

void CComparisonResultFilterDlg::OnOK()
{
	UpdateData(TRUE);
	m_sExpression = BuildExpression();
	if (!m_sExpression.empty())
		CDialog::OnOK();
}

String CComparisonResultFilterDlg::BuildExpression() const
{
	std::vector<String> conditions;

	if (m_bIdentical)
		conditions.push_back(_T("Identical"));
	if (m_bDifferent)
		conditions.push_back(_T("Different"));
	if (m_bSkipped)
		conditions.push_back(_T("Skipped"));

	if (m_is3Way)
	{
		if (m_bLeftOnly)
			conditions.push_back(_T("LeftExists and not MiddleExists and not RightExists"));
		if (m_bRightOnly)
			conditions.push_back(_T("not LeftExists and not MiddleExists and RightExists"));
		if (m_bMiddleOnly)
			conditions.push_back(_T("not LeftExists and MiddleExists and not RightExists"));
		if (m_bLeftOnlyDifferent)
			conditions.push_back(_T("DifferentLeftMiddle and not DifferentMiddleRight"));
		if (m_bMiddleOnlyDifferent)
			conditions.push_back(_T("not DifferentLeftMiddle and DifferentMiddleRight and DifferentLeftRight"));
		if (m_bRightOnlyDifferent)
			conditions.push_back(_T("not DifferentLeftMiddle and DifferentMiddleRight and not DifferentLeftRight"));
		if (m_bLeftOnlyMissing)
			conditions.push_back(_T("not LeftExists and MiddleExists and RightExists"));
		if (m_bMiddleOnlyMissing)
			conditions.push_back(_T("LeftExists and not MiddleExists and RightExists"));
		if (m_bRightOnlyMissing)
			conditions.push_back(_T("LeftExists and MiddleExists and not RightExists"));
	}
	else
	{
		if (m_bLeftOnly)
			conditions.push_back(_T("LeftExists and not RightExists"));
		if (m_bRightOnly)
			conditions.push_back(_T("not LeftExists and RightExists"));
	}

	if (conditions.empty())
		return _T("");

	String expression;
	bool isExclude = (m_nIncludeExclude == 1);

	if (conditions.size() == 1)
	{
		expression = conditions[0];
		if (isExclude)
			expression = _T("not (") + expression + _T(")");
	}
	else
	{
		expression = _T("(") + conditions[0] + _T(")");
		for (size_t i = 1; i < conditions.size(); ++i)
			expression += _T(" or (") + conditions[i] + _T(")");

		if (isExclude)
			expression = _T("not (") + expression + _T(")");
	}

	return expression;
}
