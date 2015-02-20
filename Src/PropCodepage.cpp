/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PropCodepage.h
 *
 * @brief Implementation file for PropCodepage propertyheet
 *
 */

#include "stdafx.h"
#include "PropCodepage.h"
#include "common/unicoder.h"
#include "common/ExConverter.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "charsets.h"
#include "DDXHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PropCodepage::PropCodepage(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropCodepage::IDD)
, m_nCodepageSystem(-1)
, m_nCustomCodepageValue(0)
, m_bDetectCodepage(false)
, m_bDetectCodepage2(false)
, m_nAutodetectType(50001)
{
}

void PropCodepage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCodepage)
	DDX_Radio(pDX, IDC_CP_SYSTEM, m_nCodepageSystem);
	DDX_Text(pDX, IDC_CUSTOM_CP_NUMBER, m_cCustomCodepageValue);
	DDX_Check(pDX, IDC_DETECT_CODEPAGE, m_bDetectCodepage);
	DDX_Check(pDX, IDC_DETECT_CODEPAGE2, m_bDetectCodepage2);
	DDX_Control(pDX, IDC_CUSTOM_CP_NUMBER, m_comboCustomCodepageValue);
	DDX_Control(pDX, IDC_DETECT_AUTODETECTTYPE, m_comboAutodetectType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCodepage, CPropertyPage)
	//{{AFX_MSG_MAP(PropCodepage)
	ON_BN_CLICKED(IDC_CP_SYSTEM, OnCpSystem)
	ON_BN_CLICKED(IDC_CP_CUSTOM, OnCpCustom)
	ON_BN_CLICKED(IDC_DETECT_CODEPAGE2, OnDetectCodepage2)
	ON_CBN_SELCHANGE(IDC_DETECT_AUTODETECTTYPE, OnDetectAutodetecttype)
	ON_BN_CLICKED(IDC_CP_UI, OnCpUi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropCodepage::ReadOptions()
{
	m_nCodepageSystem = GetOptionsMgr()->GetInt(OPT_CP_DEFAULT_MODE);
	m_nCustomCodepageValue = GetOptionsMgr()->GetInt(OPT_CP_DEFAULT_CUSTOM);
	m_cCustomCodepageValue = string_to_str(m_nCustomCodepageValue);
	m_bDetectCodepage = GetOptionsMgr()->GetInt(OPT_CP_DETECT) & 1;
	m_bDetectCodepage2 = (GetOptionsMgr()->GetInt(OPT_CP_DETECT) & 2) != 0;
	m_nAutodetectType = ((unsigned)GetOptionsMgr()->GetInt(OPT_CP_DETECT) >> 16);
	if (m_nAutodetectType == 0)
		m_nAutodetectType = 50001;
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropCodepage::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CP_DEFAULT_MODE, (int)m_nCodepageSystem);
	GetEncodingCodePageFromNameString();
	GetOptionsMgr()->SaveOption(OPT_CP_DEFAULT_CUSTOM, (int)m_nCustomCodepageValue);
	GetOptionsMgr()->SaveOption(OPT_CP_DETECT, (m_bDetectCodepage ? 1 : 0) | (m_bDetectCodepage2 << 1) | (m_nAutodetectType << 16));
}

BOOL PropCodepage::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();
	
	CEdit * pEdit = (CEdit *) GetDlgItem(IDC_CUSTOM_CP_NUMBER);

	// Enable/disable "Custom codepage" edit field
	if (IsDlgButtonChecked(IDC_CP_CUSTOM))
		pEdit->EnableWindow(TRUE);
	else
		pEdit->EnableWindow(FALSE);

	if (IsDlgButtonChecked(IDC_DETECT_CODEPAGE2))
		m_comboAutodetectType.EnableWindow(TRUE);
	else
		m_comboAutodetectType.EnableWindow(FALSE);

	m_comboCustomCodepageValue.SetWindowText(string_to_str(m_nCustomCodepageValue).c_str());

	IExconverter *pexconv = Exconverter::getInstance();
	if (pexconv != NULL)
	{
		CodePageInfo cpi[256];
		int count = pexconv->enumCodePages(cpi, sizeof(cpi)/sizeof(CodePageInfo));
		int i, j;
		for (i = 0, j = 0; i < count; i++)
		{
			if (cpi[i].codepage == 1200 /* UNICODE */)
				continue;
			TCHAR desc[256];
			wsprintf(desc, _T("%05d - %ls"), cpi[i].codepage, cpi[i].desc);
			m_comboCustomCodepageValue.AddString(desc);
			m_comboCustomCodepageValue.SetItemData(j, cpi[i].codepage);
			if (cpi[i].codepage == m_nCustomCodepageValue)
				m_comboCustomCodepageValue.SetCurSel(j);
			j++;
		}

		static int autodetectTypeList[] = {50001, 50936, 50950, 50932, 50949};
		for (i = 0; i < sizeof(autodetectTypeList)/sizeof(int); i++)
		{
			String desc;
			pexconv->getCodepageDescription(autodetectTypeList[i], desc);
			m_comboAutodetectType.AddString(desc.c_str());
			m_comboAutodetectType.SetItemData(i, autodetectTypeList[i]);
			if (m_nAutodetectType == autodetectTypeList[i])
				m_comboAutodetectType.SetCurSel(i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PropCodepage::OnCpSystem() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(FALSE);	
}

void PropCodepage::OnCpCustom() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(TRUE);	
}

void PropCodepage::OnDetectCodepage2() 
{
	if (IsDlgButtonChecked(IDC_DETECT_CODEPAGE2))
		m_comboAutodetectType.EnableWindow(TRUE);
	else
		m_comboAutodetectType.EnableWindow(FALSE);
}

void PropCodepage::OnDetectAutodetecttype()
{
	m_nAutodetectType = static_cast<int>(m_comboAutodetectType.GetItemData(m_comboAutodetectType.GetCurSel()));	
}

void PropCodepage::OnCpUi() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(FALSE);	
}

void PropCodepage::GetEncodingCodePageFromNameString()
{
	int nCustomCodepageValue = _ttol(m_cCustomCodepageValue.c_str());
	if (nCustomCodepageValue == 0)
		nCustomCodepageValue = GetEncodingCodePageFromName(ucr::toSystemCP(m_cCustomCodepageValue).c_str());
	//if found a new codepage valid
	if (nCustomCodepageValue)
		m_nCustomCodepageValue = nCustomCodepageValue;
}
