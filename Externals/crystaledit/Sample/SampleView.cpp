// SampleView.cpp : implementation of the CSampleView class
//

#include "stdafx.h"
#include "Sample.h"

#include "SampleDoc.h"
#include "SampleView.h"
#include "editcmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleView

IMPLEMENT_DYNCREATE(CSampleView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CSampleView, CCrystalEditView)
	//{{AFX_MSG_MAP(CSampleView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_CONVERT_TO_HTML, OnFileConvertToHTML)
	ON_COMMAND(ID_VIEW_SELMARGIN, OnSelMargin)
	ON_COMMAND(ID_VIEW_TOPMARGIN, OnTopMargin)
	ON_COMMAND(ID_VIEW_WORDWRAP, OnWordWrap)
	ON_COMMAND(ID_VIEW_WHITESPACE, OnViewWhitespace)
	ON_COMMAND(ID_VIEW_LINENUMBERS, OnViewLineNumbers)
	ON_COMMAND(ID_VIEW_SELECT_FONT, OnViewSelectFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, OnUpdateViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINENUMBERS, OnUpdateViewLineNumbers)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleView construction/destruction

CSampleView::CSampleView()
{
	// TODO: add construction code here
	SetParser(&m_xParser);
}

CSampleView::~CSampleView()
{
}

BOOL CSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CSampleView diagnostics

#ifdef _DEBUG
void CSampleView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CSampleView::Dump(CDumpContext& dc) const
{
	CCrystalEditViewEx::Dump(dc);
}

CSampleDoc* CSampleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleDoc)));
	return static_cast<CSampleDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleView message handlers

CCrystalTextBuffer *CSampleView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void CSampleView::OnInitialUpdate() 
{
	CCrystalEditViewEx::OnInitialUpdate();

	SetFont(GetDocument()->m_lf);
	SetColorContext(GetDocument()->m_pSyntaxColors);
	SetMarkersContext(GetDocument()->m_pMarkers);
	if (GetDocument()->m_xTextBuffer.GetTableEditing())
	{
		SetTopMargin(true);
		AutoFitColumn();
	}
}

void CSampleView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AfxMessageBox(_T("Build your own context menu!"));
}

void CSampleView::OnSelMargin()
{
	GetDocument()->ForEachView([](CSampleView* pView) {
		pView->SetSelectionMargin(!pView->GetSelectionMargin());
	});
}

void CSampleView::OnTopMargin()
{
	GetDocument()->ForEachView([](CSampleView* pView) {
		pView->SetTopMargin(!pView->GetTopMargin());
	});
}

void CSampleView::OnWordWrap()
{
	GetDocument()->ForEachView([](CSampleView* pView) {
		pView->SetWordWrapping(!pView->GetWordWrapping());
	});
}

void CSampleView::OnViewWhitespace()
{
	GetDocument()->ForEachView([](CSampleView* pView) {
		bool bViewTabs = pView->GetViewTabs();
		pView->SetViewTabs(!bViewTabs);
		pView->SetViewEols(!bViewTabs, true);
	});
}

void CSampleView::OnUpdateViewWhitespace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetViewTabs());
}

void CSampleView::OnViewLineNumbers()
{
	GetDocument()->ForEachView([](CSampleView* pView) {
		pView->SetViewLineNumbers(!pView->GetViewLineNumbers());
	});
}

void CSampleView::OnUpdateViewLineNumbers(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetViewLineNumbers());
}

void CSampleView::OnViewSelectFont()
{
	CFontDialog dlg;
	dlg.m_cf.Flags |= CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_SCREENFONTS;
	GetFont(*dlg.m_cf.lpLogFont);
	if (dlg.DoModal() == IDOK)
	{
		SetFont(dlg.m_lf);
	}
}

bool CSampleView::ConvertToHTML(const CString& filename)
{
	LOGFONT lf;
	CClientDC dc (CWnd::GetDesktopWindow());
	GetFont(lf);
	int nFontSize = -MulDiv (lf.lfHeight, 72, dc.GetDeviceCaps (LOGPIXELSY));
	CString linemax;
	linemax.Format(_T("%d"),  GetLineCount());
	double marginWidth = GetViewLineNumbers() ? linemax.GetLength() / 1.5 + 0.5 : 0.5;
	const int nColumnCountMax = m_pTextBuffer->GetColumnCountMax();
	CString tableStyle;
	CString tdthStyle;
	CString colgroup;
	switch (GetTextLayoutMode())
	{
	case TEXTLAYOUT_TABLE_NOWORDWRAP:
	case TEXTLAYOUT_TABLE_WORDWRAP:
		tableStyle.Format(
			_T("table { table-layout: fixed; width: max-content; height: 100%%; border-collapse: collapse; font-size: %dpt;}\n"), nFontSize);
		tdthStyle = _T("td,th {word-break: break-all; padding: 0 3px; border: 1px solid #a0a0a0; }\n");
		break;
	default:
		tableStyle.Format(
			_T("table { table-layout: fixed; width: 100%%; height: 100%%; border-collapse: collapse; font-size: %dpt;}\n"), nFontSize);
		tdthStyle = _T("td,th {word-break: break-all; padding: 0 3px; }\n");
		colgroup.Format(
			_T("<colgroup>\n")
			_T("<col style=\"width: %.1fem;\" />\n")
			_T("<col style=\"width: calc(100%% - %.1fem);\" />\n")
			_T("</colgroup>\n"), marginWidth, marginWidth);
		break;
	}

#pragma warning(disable: 4996)
	try
	{
		CStdioFile file(_tfopen(filename, _T("w, ccs=UTF-8")));
		file.WriteString(
			_T("<!DOCTYPE html>\n")
			_T("<html>\n")
			_T("<head>\n")
			_T("<meta charset=\"UTF-8\">\n")
			_T("<title>") + GetDocument()->GetPathName() + _T("</title>\n")
			_T("<style>\n")
			+ tableStyle + tdthStyle +
			_T(".ln { text-align: right; word-break: normal; color: #000000; background-color: #f0f0f0; }\n")
			+ GetHTMLStyles() +
			_T("</style>\n")
			_T("</head>\n")
			_T("<body>\n")
			_T("<table>\n")
			+ colgroup
		);
		if (m_pTextBuffer->GetTableEditing())
		{
			CString columnHeader = _T("<th class=\"cn\"></th>");
			for (int nColumn = 0; nColumn < nColumnCountMax; nColumn++)
				columnHeader += _T("<th class=\"cn\">") + GetColumnName(nColumn) + _T("</th>");
			file.WriteString(columnHeader);
			file.WriteString(_T("</tr>"));
		}
		for (int line = 0; line < GetLineCount(); ++line)
		{
			CString ln;
			if (GetViewLineNumbers())
				ln.Format(_T("<td class=\"ln\">%d</td>"), line + 1);
			else
				ln.Format(_T("<td class=\"ln\"></td>"));
			file.WriteString(_T("<tr>"));
			file.WriteString(ln);
			file.WriteString(GetHTMLLine(line, _T("td"), nColumnCountMax));
			file.WriteString(_T("</tr>\n"));
		}
		file.WriteString(
			_T("</table>\n")
			_T("</body>\n")
			_T("</html>\n"));
		file.Close();
		return true;
	}
	catch (CFileException* pe)
	{
		TRACE(_T("File could not be opened, cause = %d\n"), pe->m_cause);
		pe->Delete();
	}
	return false;
}

void CSampleView::OnFileConvertToHTML()
{
	CFileDialog dlg(FALSE, _T("htm"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		_T("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"));
	if (dlg.DoModal() != IDOK)
		return;
	ConvertToHTML(dlg.GetPathName());
}

