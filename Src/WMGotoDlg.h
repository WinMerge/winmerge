// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  WMGotoDlg.h
 *
 * @brief Declaration file for WMGotoDlg dialog.
 *
 */
#pragma once

#include <memory>
#include "UnicodeString.h"

class WMGotoDlg
{
// Construction
public:
	WMGotoDlg();
	~WMGotoDlg();
	int DoModal();

	String m_strParam;   /**< Line/difference number. */
	int m_nFile;         /**< Target file number. */
	int m_nGotoWhat;     /**< Goto line or difference? */
	int m_nFiles;        /**< Number of files being compared. */
private:
	WMGotoDlg(const WMGotoDlg &) = delete;
	WMGotoDlg & operator=(const WMGotoDlg &) = delete;

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
