// SubeditList.cpp : implementation file
//

#include "stdafx.h"
#include "SubeditList.h"
#include "Win_VersionHelper.h"
#include "WildcardDropList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr UINT IDC_IPEDIT = 1000;

/// Some stuff is from https://www.codeguru.com/cpp/controls/listview/editingitemsandsubitem/article.php/c923/Editable-subitems.htm

/////////////////////////////////////////////////////////////////////////////
// CSubeditList

CSubeditList::CSubeditList()
{
}

CSubeditList::~CSubeditList()
{
}

void CSubeditList::SetColumnAttribute(int nCol, int limit, int attribute)
{
	if (!IsValidCol(nCol))
		return;

	if (m_columnsAttributes.size() <= static_cast<size_t>(nCol))
		m_columnsAttributes.resize(static_cast<size_t>(nCol) + 1);

	std::pair<int, int>& r = m_columnsAttributes[nCol];
	if (limit) r.first = limit;
	r.second |= attribute;
}

void CSubeditList::SetItemBooleanValue(int nItem, int nSubItem, bool value)
{
	if (IsWin7_OrGreater())
		SetItemText(nItem, nSubItem, value ? _T("\u2611") : _T("\u2610"));
	else
		SetItemText(nItem, nSubItem, value ? _T("true") : _T("false"));
}

bool CSubeditList::GetItemBooleanValue(int nItem, int nSubItem) const
{
	CString text = GetItemText(nItem, nSubItem);
	return (text.Compare(_T("true")) == 0 || text.Compare(_T("\u2611")) == 0);
}

bool CSubeditList::IsValidCol(int nSubItem) const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	unsigned nColumnCount = static_cast<unsigned>(pHeader->GetItemCount());
	return static_cast<unsigned>(nSubItem) < nColumnCount;
}

bool CSubeditList::IsValidRowCol(int nItem, int nSubItem) const
{
	unsigned nItemCount = static_cast<unsigned>(GetItemCount());
	if (static_cast<unsigned>(nItem) >= nItemCount)
		return false;

	return IsValidCol(nSubItem);
}

/**
 * @brief Get the edit style for the specified column.
 * @param [in] nCol Column to get edit style
 * @return Edit style for the specified column
 * @remarks Returns EditStyle::EDIT_BOX; as default if a column with no edit style is specified.
 */
CSubeditList::EditStyle CSubeditList::GetEditStyle(int nCol) const
{
	if (static_cast<size_t>(nCol) >= m_columnsAttributes.size() || !IsValidCol(nCol))
		return EditStyle::EDIT_BOX;

	return static_cast<EditStyle>(m_columnsAttributes[nCol].second & EDIT_STYLES_ALL);
}

/**
 * @brief Set the edit style for the specified column.
 * @param [in] nCol Column to set the edit style
 * @param [in] style Edit style to set
 */
void CSubeditList::SetEditStyle(int nCol, EditStyle style)
{
	if (!IsValidCol(nCol))
		return;

	static_assert(static_cast<int>(EditStyle::EDIT_BOX) == 0, "assume 0-value by default");

	if (m_columnsAttributes.size() <= static_cast<size_t>(nCol))
		m_columnsAttributes.resize(static_cast<size_t>(nCol) + 1);

	auto& r = m_columnsAttributes[nCol];
	r.second = (r.second & ~EDIT_STYLES_ALL) | static_cast<int>(style);
}

/**
 * @brief Get the character limit for the specified column.
 * @param [in] nCol Column to get character limit
 * @return Character limit for the specified column
 * @remarks Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
 */
int CSubeditList::GetLimitTextSize(int nCol) const
{
	// Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (!IsValidCol(nCol) || GetEditStyle(nCol) != EditStyle::WILDCARD_DROP_LIST)
		return 0;

	return m_columnsAttributes[nCol].first;
}

/**
 * @brief Set the character limit for the specified column.
 * @param [in] nCol Column to set the character limit
 * @param [in] nLimitTextSize Character limit to set
 * @remarks Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
 */
void CSubeditList::SetLimitTextSize(int nCol, int nLimitTextSize)
{
	if (!IsValidCol(nCol) || GetEditStyle(nCol) != EditStyle::WILDCARD_DROP_LIST)
		return;

	if (m_columnsAttributes.size() <= static_cast<size_t>(nCol))
		m_columnsAttributes.resize(static_cast<size_t>(nCol) + 1);

	m_columnsAttributes[nCol].first = nLimitTextSize;
}

/**
 * @brief Get the wildcard drop list fixed pattern for the specified cell.
 * @param [in] nItem Th row index to get wildcard drop list fixed pattern
 * @param [in] nSubItem The column to get wildcard drop list fixed pattern
 * @return Wildcard drop list fixed pattern for the specified cell
 */
String CSubeditList::GetDropListFixedPattern(int nItem, int nSubItem) const
{
	// This setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (!IsValidRowCol(nItem, nSubItem) || GetEditStyle(nSubItem) != EditStyle::WILDCARD_DROP_LIST)
		return _T("");

	if (static_cast<size_t>(nItem) < m_dropListFixedPattern.size())
		if (static_cast<size_t>(nSubItem) < m_dropListFixedPattern[nItem].size())
			return m_dropListFixedPattern[nItem][nSubItem];

	return _T("");
}

/**
 * @brief Set the wildcard drop list fixed pattern for the specified cell.
 * @param [in] nItem The row index to set wildcard drop list fixed pattern
 * @param [in] nSubItem The column to set wildcard drop list fixed pattern
 * @param [in] fixedPattern Wildcard drop list fixed pattern to set
 */
void CSubeditList::SetDropListFixedPattern(int nItem, int nSubItem, const String& fixedPattern)
{
	// This setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (!IsValidRowCol(nItem, nSubItem) || GetEditStyle(nSubItem) != EditStyle::WILDCARD_DROP_LIST)
		return;

	if (m_dropListFixedPattern.size() <= static_cast<size_t>(nItem))
		m_dropListFixedPattern.resize(static_cast<size_t>(nItem) + 1);

	auto& sub = m_dropListFixedPattern[nItem];
	if (sub.size() <= static_cast<size_t>(nSubItem))
		sub.resize(static_cast<size_t>(nSubItem) + 1, _T(""));

	sub[nSubItem] = fixedPattern;
}

/**
 * @brief Get the dropdown list data for the specified cell.
 * @param [in] nItem The row index to get dropdown list data
 * @param [in] nSubItem The column to get dropdown list data
 * @return dropdown list data for the specified cell
 */
std::vector<String> CSubeditList::GetDropdownList(int nItem, int nSubItem) const
{
	// This setting is valid only for columns whose edit style is EditStyle::DROPDOWN_LIST.
	if (!IsValidRowCol(nItem, nSubItem) || GetEditStyle(nSubItem) != EditStyle::DROPDOWN_LIST)
		return {};

	if (static_cast<size_t>(nItem) < m_dropList.size())
		if (static_cast<size_t>(nSubItem) < m_dropList[nItem].size())
			return m_dropList[nItem][nSubItem];

	return {};
}

/**
 * @brief Set the drop list data for the specified cell.
 * @param [in] nItem The row index to set dropdown list data
 * @param [in] nSubItem The column to set dropdown list data
 * @param [in] list dropdown list data to set
 */
void CSubeditList::SetDropdownList(int nItem, int nSubItem, const std::vector<String>& list)
{
	// This setting is valid only for columns whose edit style is EditStyle::DROPDOWN_LIST.
	if (!IsValidRowCol(nItem, nSubItem) || GetEditStyle(nSubItem) != EditStyle::DROPDOWN_LIST)
		return;

	if (m_dropList.size() <= static_cast<size_t>(nItem))
		m_dropList.resize(static_cast<size_t>(nItem) + 1);

	auto& sub = m_dropList[nItem];
	if (sub.size() <= static_cast<size_t>(nSubItem))
		sub.resize(static_cast<size_t>(nSubItem) + 1);

	sub[nSubItem] = list;
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CSubeditList::HitTestEx(CPoint &point, int *col) const
{
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;
 
	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;
 
	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	int itemCount = GetItemCount();
	if( bottom > itemCount )
		bottom = itemCount;
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
 
	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( int colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

BEGIN_MESSAGE_MAP(CSubeditList, CListCtrl)
	//{{AFX_MSG_MAP(CSubeditList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubeditList message handlers


// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
//CEdit* CSubeditList::EditSubLabel(int nItem, int nCol)
CInPlaceEdit* CSubeditList::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved
	 
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return NULL;
	 
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	if (GetEditStyle(nCol) != EditStyle::EDIT_BOX)
		return NULL;
	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );
	 
	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );
	 
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	int dx = offset + rect.left;
	if( dx < 0 || dx > rcClient.right )
	{
		Scroll(CSize(dx, 0));
		rect.left -= dx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CInPlaceEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create( dwStyle, rect, this, IDC_IPEDIT );

	return pEdit;
}

/**
 * @brief Start edit of a sub item label with dropdown list.
 * @param [in] nItem The row index of the item to edit
 * @param [in] nCol The column of the sub item
 */
void CSubeditList::EditSubLabelDropdownList( int nItem, int nCol )
{
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return;

	if (GetEditStyle(nCol) != EditStyle::DROPDOWN_LIST)
		return;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );
	 
	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );
	 
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	int dx = offset + rect.left;
	if( dx < 0 || dx > rcClient.right )
	{
		Scroll(CSize(dx, 0));
		rect.left -= dx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle = 0;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_AUTOHSCROLL;
	CInPlaceComboBox *pComboBox = new CInPlaceComboBox(nItem, nCol, GetItemText(nItem, nCol), GetDropdownList(nItem, nCol));
	pComboBox->Create( dwStyle, rect, this, IDC_IPEDIT );

	SetItemText(nItem, nCol, _T(""));
}

/**
 * @brief Start edit of a sub item label with wilcard drop list.
 * @param [in] nItem The row index of the item to edit
 * @param [in] nCol The column of the sub item
 */
void CSubeditList::EditSubLabelWildcardDropList( int nItem, int nCol )
{
	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return;

	if (GetEditStyle(nCol) != EditStyle::WILDCARD_DROP_LIST)
		return;

	CString pattern = GetDropListFixedPattern(nItem, nCol).c_str();
	int nLimitTextSize = GetLimitTextSize(nCol);
	WildcardDropList::OnItemActivate(m_hWnd, nItem, nCol, 4, pattern, true, nLimitTextSize);
}

void CSubeditList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSubeditList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSubeditList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( GetFocus() != this ) SetFocus();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CSubeditList::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnMouseHWheel(nFlags, zDelta, pt);
}

void CSubeditList::OnLButtonDown(UINT nFlags, CPoint point)
{
	int colnum;
	CListCtrl::OnLButtonDown(nFlags, point);
	int index = HitTestEx(point, &colnum);
	if( index != -1 )
	{
		if ((size_t)colnum >= m_columnsAttributes.size())
			return;

		UINT flag = LVIS_FOCUSED;
		//if ((GetItemState(index, flag) & flag) == flag && colnum > 0)
		if ((GetItemState(index, flag) & flag) == flag)
		{
			auto pr = m_columnsAttributes[(size_t)colnum];
			if (!(pr.second & READ_ONLY))
			{
				if (pr.second & BOOLEAN_VALUE)
				{
					CString text = GetItemText(index, colnum);
					if (IsWin7_OrGreater())
					{
						SetItemText(index, colnum, text.Compare(_T("\u2611")) == 0 ?
							_T("\u2610") : _T("\u2611"));
					}
					else
					{
						SetItemText(index, colnum, text.Compare(_T("true")) == 0 ?
							_T("false") : _T("true"));
					}
				}
				else
				{
					switch (GetEditStyle(colnum))
					{
					case EditStyle::EDIT_BOX:
						EditSubLabel(index, colnum);
						break;
					case EditStyle::DROPDOWN_LIST:
						EditSubLabelDropdownList(index, colnum);
						break;
					case EditStyle::WILDCARD_DROP_LIST:
						EditSubLabelWildcardDropList(index, colnum);
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED,
					LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit::CInPlaceEdit(int iItem, int iSubItem, CString sInitText)
: m_sInitText( sInitText )
, m_iItem(iItem)
, m_iSubItem(iSubItem)
, m_bESC(false)
{
}

CInPlaceEdit::~CInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers
	 
BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| GetKeyState( VK_CONTROL)
				)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}


void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	if (!m_bESC)
	{
		CString str;
		GetWindowText(str);
		static_cast<CListCtrl*>(GetParent())->SetItemText(m_iItem, m_iSubItem, str);
	}

	DestroyWindow();
}
	 
void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}


void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = true;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;

	GetWindowText( str );
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	LONG width = dc.GetTextExtent( str ).cx + 5; // add some extra buffer
	dc.SelectObject( pFontDC );

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( width > rect.Width() )
	{
		width += rect.left;
		rect.right = (std::min)(width, parentrect.right);
		MoveWindow( &rect );
	}
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	 
	SetWindowText( m_sInitText );
	SetFocus();
	SetSel( 0, -1 );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBox

CInPlaceComboBox::CInPlaceComboBox(int iItem, int iSubItem, CString sInitText, const std::vector<String>& list)
: m_sInitText( sInitText )
, m_list(list)
, m_iItem(iItem)
, m_iSubItem(iSubItem)
, m_bESC(false)
{
}

CInPlaceComboBox::~CInPlaceComboBox()
{
}


BEGIN_MESSAGE_MAP(CInPlaceComboBox, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceComboBox)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBox message handlers
	 
BOOL CInPlaceComboBox::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| GetKeyState( VK_CONTROL)
				)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CInPlaceComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	if (!m_bESC)
	{
		CString str;
		GetWindowText(str);
		static_cast<CListCtrl*>(GetParent())->SetItemText(m_iItem, m_iSubItem, str);
	}

	DestroyWindow();
}
	 
void CInPlaceComboBox::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	delete this;
}

void CInPlaceComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			m_bESC = true;
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

int CInPlaceComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	 
	int sel = -1;
	for (int i = 0; i < static_cast<int>(m_list.size()); ++i)
	{
		AddString(m_list[i].c_str());
		if (m_sInitText == m_list[i].c_str())
			sel = i;
	}
	if (sel >= 0)
		SetCurSel(sel);
	SetFocus();
	return 0;
}
