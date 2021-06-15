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

#define IDC_IPEDIT 1000

/// Some stuff is from https://www.codeguru.com/cpp/controls/listview/editingitemsandsubitem/article.php/c923/Editable-subitems.htm

/////////////////////////////////////////////////////////////////////////////
// CSubeditList

CSubeditList::CSubeditList()
{
}

CSubeditList::~CSubeditList()
{
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

/**
 * @brief Get the edit style for the specified column.
 * @param [in] nCol Column to get edit style
 * @return@Edit style for the specified column
 * @remarks Returns EditStyle::EDIT_BOX; as default if a column with no edit style is specified.
 */
CSubeditList::EditStyle CSubeditList::GetEditStyle(int nCol) const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	if (nCol < 0 || nCol >= nColumnCount)
		return EditStyle::EDIT_BOX;

	if (static_cast<size_t>(nCol) >= m_editStyle.size())
		return EditStyle::EDIT_BOX;

	return m_editStyle.at(nCol);
}

/**
 * @brief Set the edit style for the specified column.
 * @param [in] nCol Column to set the edit style
 * @param [in] style Edit style to set
 */
void CSubeditList::SetEditStyle(int nCol, EditStyle style)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol < 0 || nCol >= nColumnCount)
		return;

	for (size_t i = m_editStyle.size(); i <= static_cast<size_t>(nCol); i++)
		m_editStyle.push_back(EditStyle::EDIT_BOX);

	m_editStyle[nCol] = style;
}

/**
 * @brief Get the character limit for the specified column.
 * @param [in] nCol Column to get character limit
 * @return@Character limit for the specified column
 * @remarks Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
 */
int CSubeditList::GetLimitTextSize(int nCol) const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	if (nCol < 0 || nCol >= nColumnCount)
		return 0;

	// Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (GetEditStyle(nCol) != EditStyle::WILDCARD_DROP_LIST)
		return 0;

	if (static_cast<size_t>(nCol) >= m_limitTextSize.size())
		return 0;

	return m_limitTextSize.at(nCol);
}

/**
 * @brief Set the character limit for the specified column.
 * @param [in] nCol Column to set the character limit
 * @param [in] nLimitTextSize Character limit to set
 * @remarks Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
 */
void CSubeditList::SetLimitTextSize(int nCol, int nLimitTextSize)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol < 0 || nCol >= nColumnCount)
		return;

	// Currently, this setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (GetEditStyle(nCol) != EditStyle::WILDCARD_DROP_LIST)
		return;

	for (size_t i = m_limitTextSize.size(); i <= static_cast<size_t>(nCol); i++)
		m_limitTextSize.push_back(0);

	m_limitTextSize[nCol] = nLimitTextSize;
}

/**
 * @brief Get the wildcard drop list fixed pattern for the specified cell.
 * @param [in] nItem Th row index to get wildcard drop list fixed pattern
 * @param [in] nSubItem The column to get wildcard drop list fixed pattern
 * @return Wildcard drop list fixed pattern for the specified cell
 */
String CSubeditList::GetDropListFixedPattern(int nItem, int nSubItem) const
{
	int nItemCount = GetItemCount();
	if (nItem < 0 || nItem >= nItemCount)
		return _T("");

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nSubItem < 0 || nSubItem >= nColumnCount)
		return _T("");

	// This setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (GetEditStyle(nSubItem) != EditStyle::WILDCARD_DROP_LIST)
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
	int nItemCount = GetItemCount();
	if (nItem < 0 || nItem >= nItemCount)
		return;

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nSubItem < 0 || nSubItem >= nColumnCount)
		return;

	// This setting is valid only for columns whose edit style is EditStyle::WILDCARD_DROPLIST.
	if (GetEditStyle(nSubItem) != EditStyle::WILDCARD_DROP_LIST)
		return;

	for (size_t i = m_dropListFixedPattern.size(); i <= static_cast<size_t>(nItem); i++)
		m_dropListFixedPattern.push_back(std::vector<String>());

	for (size_t i = m_dropListFixedPattern[nItem].size(); i <= static_cast<size_t>(nSubItem); i++)
		m_dropListFixedPattern[nItem].push_back(_T(""));

	m_dropListFixedPattern[nItem][nSubItem] = fixedPattern;
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CSubeditList::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;
 
	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;
 
	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
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
			for( colnum = 0; colnum < nColumnCount; colnum++ )
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


BEGIN_MESSAGE_MAP(CSubeditList, CListCtrl)
	//{{AFX_MSG_MAP(CSubeditList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SUBSTITUTION_FILTERS, OnEndLabelEdit)
	ON_WM_LBUTTONDOWN()
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
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
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
	CInPlaceEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	pEdit->Create( dwStyle, rect, this, IDC_IPEDIT );

	return pEdit;
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

void CSubeditList::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM	*plvItem = &plvDispInfo->item;
	 
	if (plvItem->pszText != NULL)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;
}

void CSubeditList::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	plvItem->iSubItem = 1;

// 	if (plvItem->pszText != NULL)
// 	{
// 		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
// 	}
	*pResult = FALSE;
}

void CSubeditList::OnLButtonDown(UINT nFlags, CPoint point)
{
	int index;
	int colnum;
	CListCtrl::OnLButtonDown(nFlags, point);
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		UINT flag = LVIS_FOCUSED;
		//if ((GetItemState(index, flag) & flag) == flag && colnum > 0)
		if ((GetItemState(index, flag) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				if (m_readOnlyColumns.find(colnum) == m_readOnlyColumns.end())
					if (m_binaryValueColumns.find(colnum) != m_binaryValueColumns.end())
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
:m_sInitText( sInitText )
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;
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

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
					(LPARAM)&dispinfo );

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
			m_bESC = TRUE;
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
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );
	size.cx += 5;			   	// add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < parentrect.right )
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
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
