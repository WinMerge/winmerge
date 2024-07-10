/** 
 * @file  MyStatusBar.h
 *
 * @brief Declaration file for CMyStatusBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>

class CMyStatusBar : public CStatusBar
{
public:
    CMyStatusBar();

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};