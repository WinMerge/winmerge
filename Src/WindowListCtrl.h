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
	struct WindowItem
	{
		CFrameWnd* pFrame = nullptr;
		uintptr_t data = 0;
		bool checked = false;
	};

	CWindowListCtrl();
	virtual ~CWindowListCtrl();

	void Initialize();
	void SetWindows(const std::vector<WindowItem>& windows);
	void GetCheckedData(std::vector<uintptr_t>& data) const;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP();

private:
	CImageList m_imageList;
};