/**
 * @file WindowListCtrl.cpp
 * @brief Implementation of CWindowListCtrl class for displaying a list of windows with checkboxes.
 */
#include "stdafx.h"
#include "WindowListCtrl.h"

IMPLEMENT_DYNAMIC(CWindowListCtrl, CListCtrl)

CWindowListCtrl::CWindowListCtrl()
{}

CWindowListCtrl::~CWindowListCtrl()
{}

BEGIN_MESSAGE_MAP(CWindowListCtrl, CListCtrl)
END_MESSAGE_MAP()

BOOL CWindowListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 'A' &&
			(::GetKeyState(VK_CONTROL) & 0x8000))
		{
			for (int i = 0; i < GetItemCount(); ++i)
				SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

			return TRUE;
		}
		if (pMsg->wParam == VK_SPACE)
		{
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos == nullptr)
				return TRUE;

			int first = GetNextSelectedItem(pos);
			bool check = !GetCheck(first);
			SetCheck(first, check);

			while (pos != nullptr)
			{
				int i = GetNextSelectedItem(pos);
				SetCheck(i, check);
			}

			return TRUE;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CWindowListCtrl::Initialize()
{
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	CRect rc;
	GetClientRect(&rc);

	InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());

	m_imageList.Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 4, 4);

	SetImageList(&m_imageList, LVSIL_SMALL);
}

void CWindowListCtrl::SetWindows(const std::vector<WindowItem>& windows)
{
	DeleteAllItems();
	m_imageList.DeleteImageList();

	m_imageList.Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 4, 4);

	SetImageList(&m_imageList, LVSIL_SMALL);

	for (const auto& window : windows)
	{
		HWND hwnd = window.pFrame->GetSafeHwnd();

		HICON hIcon = (HICON)::SendMessage(hwnd, WM_GETICON, ICON_SMALL2, 0);
		if (hIcon == nullptr)
			hIcon = (HICON)::GetClassLongPtr(hwnd, GCLP_HICONSM);

		int image = m_imageList.Add(hIcon);

		CString title;
		window.pFrame->GetWindowText(title);

		LVITEM item = {};
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.iItem = GetItemCount();
		item.iImage = image;
		item.pszText = title.GetBuffer();
		item.lParam = window.data;

		int index = InsertItem(&item);
		SetCheck(index, window.checked);
	}
}

void CWindowListCtrl::GetCheckedData(std::vector<uintptr_t>& data) const
{
	data.clear();

	LVITEM item{};
	item.mask = LVIF_PARAM;

	for (int i = 0; i < GetItemCount(); ++i)
	{
		if (!GetCheck(i))
			continue;

		item.iItem = i;
		const_cast<CWindowListCtrl*>(this)->GetItem(&item);

		data.push_back(static_cast<uintptr_t>(item.lParam));
	}
}