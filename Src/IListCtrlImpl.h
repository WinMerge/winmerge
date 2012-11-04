#include "IListCtrl.h"
#include <commctrl.h>

class IListCtrlImpl : public IListCtrl
{
public:
	IListCtrlImpl(HWND hwndListCtrl) : m_hwndListCtrl(hwndListCtrl)
	{
	}

	int GetColumnCount() const
	{
		return Header_GetItemCount(ListView_GetHeader(m_hwndListCtrl));
	}

	int GetRowCount() const
	{
		return ListView_GetItemCount(m_hwndListCtrl);
	}

	String GetColumnName(int col) const
	{
		LVCOLUMN lvc;
		TCHAR columnName[256]; // Assuming max col header will never be > 256
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = sizeof(columnName) / sizeof(TCHAR);
		ListView_GetColumn(m_hwndListCtrl, col, &lvc);
		return lvc.pszText;
	}

	String GetItemText(int row, int col) const
	{
		TCHAR text[512]; // Assuming max col header will never be > 512
		ListView_GetItemText(m_hwndListCtrl, row, col, text, sizeof(text)/sizeof(text[0]));
		return text;
	}

protected:
	HWND m_hwndListCtrl;
};