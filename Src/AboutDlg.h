// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  AboutDlg.h
 *
 * @brief Declaration file for CAboutDlg.
 *
 */
#pragma once

#include <memory>
#include <Poco/BasicEvent.h>
#include "MergeApp.h"

class CAboutDlg
{
// Construction
public:
	CAboutDlg();
	~CAboutDlg();
	int DoModal();

	AboutInfo m_info;
	Poco::BasicEvent<int> m_onclick_contributers;
	Poco::BasicEvent<int> m_onclick_url;

private:
	CAboutDlg(const CAboutDlg &) = delete;
	CAboutDlg & operator=(const CAboutDlg &) = delete;

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
