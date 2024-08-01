/** 
 * @file  CommandBar.h
 *
 * @brief Declaration file for CCommandBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <vector>

class CCommandBar : public CToolBar
{
	DECLARE_DYNAMIC(CCommandBar)
public:
	constexpr static int FIRST_MENUID = 10000;

	CCommandBar();

	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, UINT nID = AFX_IDW_TOOLBAR);
	bool AttachMenu(CMenu* pMenu);
	void OnCommandBarMenuItem(UINT nID);
	void OnUpdateCommandBarMenuItem(CCmdUI* pCmdUI);

protected:
	DECLARE_MESSAGE_MAP()

	HMENU m_hMenu;
	CFont m_hToolbarFont;
};