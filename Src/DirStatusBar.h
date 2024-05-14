/** 
 * @file  DirStatusBar.h
 *
 * @brief Interface of the CDirStatusBar class.
 *
 */
#pragma once

class CDirStatusBar : public CStatusBar
{
protected:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP();
};
