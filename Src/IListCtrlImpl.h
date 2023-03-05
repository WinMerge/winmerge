#pragma once

#include "IListCtrl.h"
#include <commctrl.h>
#include <atlimage.h>

struct ListViewOwnerDataItem
{
    LPARAM lParam;
    int iIndent;
    int iImage;
};

class IListCtrlImpl : public IListCtrl
{
public:
	explicit IListCtrlImpl(HWND hwndListCtrl, std::vector<ListViewOwnerDataItem>& listViewItems)
		: m_hwndListCtrl(hwndListCtrl)
		, m_listViewItems(listViewItems)
	{
	}

	~IListCtrlImpl() override {}

	int GetColumnCount() const override
	{
		return Header_GetItemCount(ListView_GetHeader(m_hwndListCtrl));
	}

	int GetRowCount() const override
	{
		return static_cast<int>(m_listViewItems.size());
	}

	String GetColumnName(int col) const override
	{
		LVCOLUMN lvc;
		tchar_t columnName[256]; // Assuming max col header will never be > 256
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = sizeof(columnName) / sizeof(tchar_t);
		ListView_GetColumn(m_hwndListCtrl, col, &lvc);
		return lvc.pszText;
	}

	String GetItemText(int row, int col) const override
	{
		tchar_t text[512]; // Assuming max col header will never be > 512
		text[0] = '\0';
		ListView_GetItemText(m_hwndListCtrl, row, col, text, sizeof(text)/sizeof(text[0]));
		return text;
	}

	void *GetItemData(int row) const override
	{
		if (row < 0 || row >= static_cast<int>(m_listViewItems.size()))
			return nullptr;
		return reinterpret_cast<void *>(m_listViewItems[row].lParam);
	}

	int GetTextColor(int row) const override
	{
		NMLVCUSTOMDRAW nmlvcd = {0};
		nmlvcd.nmcd.hdr.code = NM_CUSTOMDRAW;
		nmlvcd.nmcd.hdr.idFrom = GetDlgCtrlID(m_hwndListCtrl);
		nmlvcd.nmcd.hdr.hwndFrom = m_hwndListCtrl;
		nmlvcd.nmcd.dwDrawStage = CDDS_ITEMPREPAINT | CDDS_SUBITEM;
		nmlvcd.nmcd.dwItemSpec = row;
		SendMessage(GetParent(m_hwndListCtrl), WM_NOTIFY, (WPARAM)m_hwndListCtrl, (LPARAM)&nmlvcd);
		return nmlvcd.clrText;
	}

	int GetBackColor(int row) const override
	{
		NMLVCUSTOMDRAW nmlvcd = {0};
		nmlvcd.nmcd.hdr.code = NM_CUSTOMDRAW;
		nmlvcd.nmcd.hdr.idFrom = GetDlgCtrlID(m_hwndListCtrl);
		nmlvcd.nmcd.hdr.hwndFrom = m_hwndListCtrl;
		nmlvcd.nmcd.dwDrawStage = CDDS_ITEMPREPAINT | CDDS_SUBITEM;
		nmlvcd.nmcd.dwItemSpec = row;
		SendMessage(GetParent(m_hwndListCtrl), WM_NOTIFY, (WPARAM)m_hwndListCtrl, (LPARAM)&nmlvcd);
		return nmlvcd.clrTextBk;
	}

	bool IsSelectedItem(int sel) const override
	{
		return !!ListView_GetItemState(m_hwndListCtrl, sel, LVIS_SELECTED);
	}

	int GetNextItem(int sel, bool selected = false, bool reverse = false) const override
	{
		if (!reverse)
			return ListView_GetNextItem(m_hwndListCtrl, sel, (selected ? LVNI_SELECTED : 0));		
		if (!selected)
			return ListView_GetNextItem(m_hwndListCtrl, sel, LVNI_ABOVE);
		// It seems that when LVS_OWNERDATA is enabled and LVNI_SELECTED | LVNI_ABOVE is passed to ListView_GetNextItem(), the expected result is not obtained.
		for (int i = sel - 1; i >= 0; --i)
		{
			if (ListView_GetItemState(m_hwndListCtrl, i, LVIS_SELECTED))
				return i;
		}
		return -1;
	}

	int GetNextSelectedItem(int sel, bool reverse = false) const override
	{
		return GetNextItem(sel, true, reverse);
	}

	unsigned GetSelectedCount() const override
	{
		return ListView_GetSelectedCount(m_hwndListCtrl);
	}

	int GetIndent(int row) const override
	{
		LVITEM item = {0};
		item.mask = LVIF_INDENT;
		item.iItem = row;
		ListView_GetItem(m_hwndListCtrl, &item);
		return item.iIndent;
	}

	int GetIconIndex(int row) const override
	{
		LVITEM item = {0};
		item.mask = LVIF_IMAGE;
		item.iItem = row;
		ListView_GetItem(m_hwndListCtrl, &item);
		return item.iImage;
	}

	int GetIconCount() const override
	{
		HIMAGELIST hImageList = ListView_GetImageList(m_hwndListCtrl, LVSIL_SMALL);
		return ImageList_GetImageCount(hImageList);
	}

	std::string GetIconPNGData(int iconIndex) const override
	{
		HIMAGELIST hImageList = ListView_GetImageList(m_hwndListCtrl, LVSIL_SMALL);
		IMAGEINFO imageInfo;
		std::string ret;
		if (ImageList_GetImageInfo(hImageList, iconIndex, &imageInfo))
		{
			HDC hdcMem = CreateCompatibleDC(nullptr);
			BITMAPINFO bmpinfo = { 0 };
			bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			int w = imageInfo.rcImage.right - imageInfo.rcImage.left;
			int h = imageInfo.rcImage.bottom - imageInfo.rcImage.top;
			void *pBits;
			bmpinfo.bmiHeader.biWidth = w;
			bmpinfo.bmiHeader.biHeight = h;
			bmpinfo.bmiHeader.biPlanes = 1;
			bmpinfo.bmiHeader.biBitCount = 32;
			bmpinfo.bmiHeader.biCompression = BI_RGB;
			HBITMAP hbmpImage = CreateDIBSection(nullptr, &bmpinfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
			if (hbmpImage != nullptr)
			{
				HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpImage);
				ImageList_Draw(hImageList, iconIndex, hdcMem, 0, 0, ILD_TRANSPARENT);
				CImage image;
				image.Attach(hbmpImage);
				image.SetHasAlphaChannel(true);
				IStream *pStream = nullptr;
				if (SUCCEEDED(CreateStreamOnHGlobal(nullptr, TRUE, &pStream)))
				{
					image.Save(pStream, Gdiplus::ImageFormatPNG);
					STATSTG stat;
					if (SUCCEEDED(pStream->Stat(&stat, STATFLAG_NONAME)))
					{
						LARGE_INTEGER li = { 0 };
						ret.resize(stat.cbSize.LowPart, 0);
						pStream->Seek(li, STREAM_SEEK_SET, nullptr);
						pStream->Read(&ret[0], stat.cbSize.LowPart, nullptr);
					}
					pStream->Release();
				}
				SelectObject(hdcMem, hbmpOld);
				DeleteObject(hdcMem);
				DeleteObject(hbmpImage);
			}
		}
		return ret;
	}

protected:
	HWND m_hwndListCtrl;
	std::vector<ListViewOwnerDataItem>& m_listViewItems;
};
