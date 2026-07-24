/** 
 * @file WindowListCtrl.h
 * @brief Declaration of CWindowListCtrl class for displaying a list of windows with checkboxes.
 */
#pragma once

#include <vector>

class CWindowListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CWindowListCtrl)

public:
	struct Item
	{
		String title;
		uintptr_t data = 0;
		bool checked = false;
		int iImage = -1;
	};

	CWindowListCtrl();
	virtual ~CWindowListCtrl();

	void Initialize();
	CImageList& GetImageList() { return m_imageList; }
	void SetItems(const std::vector<Item>& items);
	std::vector<uintptr_t> GetCheckedData() const;
	std::vector<bool> GetChecked() const;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP();

private:
	CImageList m_imageList;
};