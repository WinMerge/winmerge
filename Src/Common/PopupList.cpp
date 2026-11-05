#include "StdAfx.h"
#include "PopupList.h"
#include "DarkModeLib.h"

namespace PopupList
{
	namespace
	{
		/** @brief Index of the item at ptClient (client coordinates of list), -1 if none */
		int HitItem(CListBox& list, CPoint ptClient)
		{
			CRect rcClient;
			list.GetClientRect(&rcClient);
			BOOL bOutside = TRUE;
			const int nItem = static_cast<int>(list.ItemFromPoint(ptClient, bOutside));
			return (bOutside || !rcClient.PtInRect(ptClient)) ? -1 : nItem;
		}

		/** @brief List box that highlights the item under the mouse (like a combo box drop-down) and chooses it on click */
		class CHoverListBox : public CListBox
		{
		protected:
			afx_msg void OnMouseMove(UINT nFlags, CPoint point)
			{
				const int nItem = HitItem(*this, point);
				if (nItem >= 0 && nItem != GetCurSel())
					SetCurSel(nItem);
				CListBox::OnMouseMove(nFlags, point);
			}
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point)
			{
				CListBox::OnLButtonUp(nFlags, point);
				if (HitItem(*this, point) >= 0)
					GetParent()->PostMessage(WM_COMMAND, IDOK);
			}
			DECLARE_MESSAGE_MAP()
		};

		BEGIN_MESSAGE_MAP(CHoverListBox, CListBox)
			ON_WM_MOUSEMOVE()
			ON_WM_LBUTTONUP()
		END_MESSAGE_MAP()

		/**
		 * @brief Borderless modal popup with a list box and an optional checkbox.
		 * Esc, Enter, Tab, arrow keys and the mouse wheel are handled by the dialog manager and the list box;
		 * the popup closes when another window becomes active.
		 */
		class CPopupListDlg : public CDialog
		{
		public:
			CPopupListDlg(CWnd* pOwner, CFont* pFont, const std::vector<String>& items, int nSel, bool bEnabled,
				const String& checkBoxText, bool bChecked, CPoint ptScreen)
				: m_pOwner(pOwner), m_pFont(pFont), m_items(items), m_nSel(nSel), m_bEnabled(bEnabled)
				, m_checkBoxText(checkBoxText), m_bChecked(bChecked), m_ptScreen(ptScreen)
			{
				static const struct { DLGTEMPLATE dt; WORD menu, wndClass, title; } tmpl =
				{ { WS_POPUP | WS_BORDER, 0, 0, 0, 0, 0, 0 }, 0, 0, 0 };
				InitModalIndirect(reinterpret_cast<LPCDLGTEMPLATE>(&tmpl), pOwner);
			}

			int m_nResult = -1;
			bool m_bChecked;

		protected:
			BOOL OnInitDialog() override;
			void OnOK() override;
			afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
			DECLARE_MESSAGE_MAP()

		private:
			CWnd* m_pOwner;
			CFont* m_pFont;
			const std::vector<String>& m_items;
			int m_nSel;
			bool m_bEnabled;
			String m_checkBoxText;
			CPoint m_ptScreen;
			bool m_bClosing = false;
			CHoverListBox m_list;
			CButton m_check;
		};

		BEGIN_MESSAGE_MAP(CPopupListDlg, CDialog)
			ON_WM_ACTIVATE()
		END_MESSAGE_MAP()

		BOOL CPopupListDlg::OnInitDialog()
		{
			CDialog::OnInitDialog();
			ModifyStyleEx(0, WS_EX_TOOLWINDOW);

			// DoModal disabled the top-level owner. Enable it again, so that a click on it activates it,
			// which deactivates this popup and closes it (OnActivate)
			if (m_pOwner != nullptr)
				::EnableWindow(::GetAncestor(m_pOwner->GetSafeHwnd(), GA_ROOT), TRUE);

			const bool hasCheck = !m_checkBoxText.empty();
			// Control IDs must not be IDOK (1) / IDCANCEL (2): their notifications would close the dialog
			constexpr UINT IDC_POPUP_LIST = 100;
			constexpr UINT IDC_POPUP_CHECK = 101;
			m_list.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
				CRect(0, 0, 0, 0), this, IDC_POPUP_LIST);
			if (hasCheck)
				m_check.Create(m_checkBoxText.c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
					CRect(0, 0, 0, 0), this, IDC_POPUP_CHECK);
			if (m_pFont != nullptr)
			{
				m_list.SetFont(m_pFont);
				if (hasCheck)
					m_check.SetFont(m_pFont);
			}
			if (DarkMode::isEnabled())
			{
				DarkMode::setWindowCtlColorSubclass(m_hWnd);
				DarkMode::setChildCtrlsSubclassAndThemeEx(m_hWnd, true, true);
			}

			// Measure: widest item, rows up to half the work area of the monitor, a scroll bar if they do not fit
			MONITORINFO monitorInfo{ sizeof(monitorInfo) };
			GetMonitorInfo(MonitorFromPoint(m_ptScreen, MONITOR_DEFAULTTONEAREST), &monitorInfo);
			const CRect rcWork(monitorInfo.rcWork);

			int cxText = 0, cxSpace = 0, cxCheck = 0;
			{
				CClientDC dc(&m_list);
				CFont* pOldFont = dc.SelectObject(m_list.GetFont());
				for (const auto& item : m_items)
				{
					m_list.AddString(item.c_str());
					cxText = (std::max)(cxText, static_cast<int>(dc.GetTextExtent(item.c_str()).cx));
				}
				cxSpace = dc.GetTextExtent(_T(" ")).cx;	// scales with font and DPI
				if (hasCheck)
					cxCheck = dc.GetTextExtent(m_checkBoxText.c_str()).cx + GetSystemMetrics(SM_CXMENUCHECK) + 4 * cxSpace;
				dc.SelectObject(pOldFont);
			}
			const int nItems = static_cast<int>(m_items.size());
			const int cyItem = (std::max)(1, m_list.GetItemHeight(0));
			const int nRows = (std::max)(1, (std::min)(nItems, rcWork.Height() / 2 / cyItem));
			const int cxScroll = nRows < nItems ? GetSystemMetrics(SM_CXVSCROLL) : 0;
			const int cxList = (std::max)(cxText + 4 * cxSpace + cxScroll, cxCheck);
			const int cyCheck = hasCheck ? cyItem + cxSpace : 0;
			const CSize sizeClient(cxList, cyCheck + nRows * cyItem);

			if (hasCheck)
				m_check.MoveWindow(cxSpace, 0, cxList - cxSpace, cyCheck);
			m_list.MoveWindow(0, cyCheck, cxList, nRows * cyItem);
			m_list.SetCurSel(m_nSel);
			m_list.EnableWindow(m_bEnabled);
			if (hasCheck)
				m_check.SetCheck(m_bChecked ? BST_CHECKED : BST_UNCHECKED);

			// Place it like a popup menu: above the point, below it when there is no room above, inside the work area
			CRect rcPopup(CPoint(0, 0), sizeClient);
			::AdjustWindowRectEx(&rcPopup, GetStyle(), FALSE, GetExStyle());
			const CSize sizePopup = rcPopup.Size();
			CPoint pt(m_ptScreen.x, m_ptScreen.y - sizePopup.cy);
			if (pt.y < rcWork.top)
				pt.y = m_ptScreen.y;
			pt.x = (std::max)(rcWork.left, (std::min)(pt.x, rcWork.right - sizePopup.cx));
			pt.y = (std::max)(rcWork.top, (std::min)(pt.y, rcWork.bottom - sizePopup.cy));
			SetWindowPos(&wndTop, pt.x, pt.y, sizePopup.cx, sizePopup.cy, SWP_SHOWWINDOW);

			if (m_bEnabled)
				m_list.SetFocus();
			return FALSE;	// focus set above
		}

		void CPopupListDlg::OnOK()
		{
			if (!m_bEnabled || m_list.GetCurSel() < 0)
			{
				OnCancel();
				return;
			}
			m_bClosing = true;
			m_nResult = m_list.GetCurSel();
			m_bChecked = m_check.m_hWnd != nullptr && m_check.GetCheck() == BST_CHECKED;
			EndDialog(IDOK);
		}

		void CPopupListDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
		{
			CDialog::OnActivate(nState, pWndOther, bMinimized);
			if (nState == WA_INACTIVE && !m_bClosing)
				EndDialog(IDCANCEL);
		}
	}

	Selection Show(CWnd* pOwner, CFont* pFont, const std::vector<String>& items, int selectedIndex,
		bool enabled, const String& checkBoxText, bool checkBoxChecked, CPoint ptScreen)
	{
		CPopupListDlg dlg(pOwner, pFont, items, selectedIndex, enabled, checkBoxText, checkBoxChecked, ptScreen);
		Selection result;
		result.checkBoxChecked = checkBoxChecked;
		if (dlg.DoModal() == IDOK)
		{
			result.index = dlg.m_nResult;
			result.checkBoxChecked = dlg.m_bChecked;
		}
		return result;
	}
}