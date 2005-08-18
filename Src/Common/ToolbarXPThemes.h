/** 
 * @file  ToolBarXPThemes.h
 *
 * @brief Declaration file for ToolBarXPThemes
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _TOOLBAR_XPTHEMES_H_
#define _TOOLBAR_XPTHEMES_H_

/**
 * @brief Toolbar class supporting XP themes.
 *
 * When XP themes are enabled using manifest files there seems to be
 * two problems:
 * - toolbar grippers don't get drawn correctly
 * - if both mousebuttons are clicked simultanously, application hangs
 * This toolbar class overrides toolbar drawing to fix drawing issue.
 * Also overrides right-mousebutton up- and down-events with empty functions
 * so they don't get delivered to actual toolbar class.
 *
 * Drawing issue is described in MSDN KB article 843490:
 * http://support.microsoft.com/default.aspx?scid=kb;en-us;843490
 */
class ToolBarXPThemes : public CToolBar
{
public:
	void DrawGripper(CDC* pDC, const CRect& rect);
	void EraseNonClient();
	virtual void DoPaint(CDC* pDC);

protected:

	//{{AFX_MSG(ToolBarXPThemes)
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _TOOLBAR_XPTHEMES_H_
