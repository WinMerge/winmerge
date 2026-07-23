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
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CWindowListCtrl::OnItemChanged)
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
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

BOOL CWindowListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;

	// Check if the state image (checkbox) has changed
	if ((pNMLV->uChanged & LVIF_STATE) &&
		((pNMLV->uOldState & LVIS_STATEIMAGEMASK) != (pNMLV->uNewState & LVIS_STATEIMAGEMASK)))
	{
		int changedItem = pNMLV->iItem;
		if (changedItem < 0 || changedItem >= GetItemCount())
		{
			*pResult = 0;
			return FALSE;
		}

		// Check if the clicked item is currently selected
		bool isClickedItemSelected = (GetItemState(changedItem, LVIS_SELECTED) & LVIS_SELECTED) != 0;

		if (isClickedItemSelected)
		{
			// Get the new state of the clicked item
			bool newState = GetCheck(changedItem);

			// Apply the same state to all selected items
			POSITION pos = GetFirstSelectedItemPosition();
			while (pos != nullptr)
			{
				int i = GetNextSelectedItem(pos);
				SetCheck(i, newState);
			}
		}
		// If the clicked item is not selected, only its state is changed (no additional action needed)
	}

	*pResult = 0;
	return FALSE;
}

void CWindowListCtrl::Initialize()
{
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	CRect rc;
	GetClientRect(&rc);

	InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());
}

void CWindowListCtrl::SetItems(const std::vector<Item>& items)
{
	DeleteAllItems();

	for (const auto& itemData: items)
	{
		LVITEM item = {};
		item.mask = LVIF_TEXT | LVIF_PARAM;

		if (itemData.iImage >= 0)
		{
			item.mask |= LVIF_IMAGE;
			item.iImage = itemData.iImage;
		}

		item.iItem = GetItemCount();
		item.pszText = const_cast<LPTSTR>(itemData.title.c_str());
		item.lParam = itemData.data;

		int index = InsertItem(&item);
		SetCheck(index, itemData.checked);
	}
}

std::vector<uintptr_t> CWindowListCtrl::GetCheckedData() const
{
	std::vector<uintptr_t> data;

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
	return data;
}

std::vector<bool> CWindowListCtrl::GetChecked() const
{
	std::vector<bool> checkedList;
	for (int i = 0; i < GetItemCount(); ++i)
		checkedList.push_back(GetCheck(i));
	return checkedList;
}