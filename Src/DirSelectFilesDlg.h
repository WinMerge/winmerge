/**
 * @file  DirSelectFilesDlg.h
 *
 * @brief Declaration file for DirSelectFilesDlg dialog.
 *
 */
#pragma once

#include <memory>
#include "UnicodeString.h"
#include "DiffItem.h"
#include "PathContext.h"

class DirSelectFilesDlg
{
public:
	DirSelectFilesDlg();
	~DirSelectFilesDlg();
	int DoModal();

	std::array<DIFFITEM *, 3> m_pdi;
	PathContext m_selectedFiles;
	std::vector<int> m_selectedButtons;
private:
	DirSelectFilesDlg(const DirSelectFilesDlg &) = delete;
	DirSelectFilesDlg & operator=(const DirSelectFilesDlg &) = delete;

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
