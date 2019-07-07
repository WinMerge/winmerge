/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PatchDlg.h
 *
 * @brief Declaration file for patch creation dialog
 */
#pragma once

#include "resource.h"
#include "TrDialogs.h"
#include "SuperComboBox.h"
#include "UnicodeString.h"

struct PATCHFILES;

/////////////////////////////////////////////////////////////////////////////
// PatchDlg dialog

/** 
 * @brief Dialog class for Generate Patch -dialog.
 * This dialog allows user to select files from which to create a patch,
 * patch file's filename and several options related to patch.
 */
class CPatchDlg : public CTrDialog
{
// Construction
public:
	explicit CPatchDlg(CWnd* pParent = nullptr);   // standard constructor

	// Functions to add and get selected files (as PATCHFILEs)
	void AddItem(const PATCHFILES& pf);
	size_t GetItemCount();
	const PATCHFILES& GetItemAt(size_t position);
	void ClearItems();

// Dialog Data
	//{{AFX_DATA(CPatchDlg)
	enum { IDD = IDD_GENERATE_PATCH };
	CComboBox m_comboStyle;
	CSuperComboBox m_comboContext;
	bool m_caseSensitive;
	CSuperComboBox m_ctlFile1;
	CSuperComboBox m_ctlFile2;
	CSuperComboBox m_ctlResult;
	String	m_file1;
	String	m_file2;
	String	m_fileResult;
	bool m_ignoreBlanks;
	bool m_ignoreEOLDifference;
	int m_whitespaceCompare;
	bool m_appendFile;
	bool m_openToEditor;
	bool m_includeCmdLine;
	//}}AFX_DATA

	enum output_style m_outputStyle; /**< Patch style (context, unified etc.) */
	int m_contextLines; /**< How many context lines are added. */
	enum DiffAlgorithm m_diffAlgorithm;
	bool m_indentHeuristic;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	std::vector<PATCHFILES> m_fileList; /**< Source files to create patch from */

	void UpdateSettings();
	void LoadSettings();
	void SaveSettings();
	void Swap();

	// Generated message map functions
	//{{AFX_MSG(CPatchDlg)
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDiffBrowseFile1();
	afx_msg void OnDiffBrowseFile2();
	afx_msg void OnDiffBrowseResult();
	afx_msg void OnSelchangeDiffStyle();
	afx_msg void OnDiffSwapFiles();
	afx_msg void OnDefaultSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/** 
 * @brief Add patch item to internal list.
 * @param [in] pf Patch item to add.
 */
inline void CPatchDlg::AddItem(const PATCHFILES& pf)
{
	m_fileList.push_back(pf);
}

/** 
 * @brief Returns amount of patch items in the internal list.
 * @return Count of patch items in the list.
 */
inline size_t CPatchDlg::GetItemCount()
{
	return m_fileList.size();
}

/** 
 * @brief Return item in the internal list at given position
 * @param [in] position Zero-based index of item to get
 * @return PATCHFILES from given position.
 */
inline const PATCHFILES& CPatchDlg::GetItemAt(size_t position)
{
	return m_fileList.at(position);
}

/** 
 * @brief Empties internal item list.
 */
inline void CPatchDlg::ClearItems()
{
	m_fileList.clear();
}

