/**
 * @file  ImageCompare.cpp
 *
 * @brief Implementation file for ImageCompare
 */

#include "pch.h"
#include "ImageCompare.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "WinIMergeLib.h"
#include "IAbortable.h"
#include <windows.h>

namespace CompareEngines
{

ImageCompare::ImageCompare(CDiffContext& ctxt)
	: m_ctxt(ctxt)
	, m_colorDistanceThreshold(ctxt.m_dColorDistanceThreshold)
	, m_pImgMergeWindow(nullptr)
	, m_hModule(nullptr)
{
	m_hModule = LoadLibraryW(L"WinIMerge\\WinIMergeLib.dll");
	if (m_hModule == nullptr)
		return;
	IImgMergeWindow* (*pfnWinIMerge_CreateWindowless)() =
		(IImgMergeWindow * (*)())GetProcAddress(m_hModule, "WinIMerge_CreateWindowless");
	if (pfnWinIMerge_CreateWindowless == nullptr)
		return;
	m_pImgMergeWindow = pfnWinIMerge_CreateWindowless();
}

ImageCompare::~ImageCompare()
{
	if (m_pImgMergeWindow)
	{
		bool(*pfnWinIMerge_DestroyWindow)(IImgMergeWindow *) =
			(bool(*)(IImgMergeWindow *))GetProcAddress(m_hModule, "WinIMerge_DestroyWindow");
		if (pfnWinIMerge_DestroyWindow != nullptr)
			pfnWinIMerge_DestroyWindow(m_pImgMergeWindow);
	}
	if (m_hModule)
		FreeLibrary(m_hModule);
}

int ImageCompare::compare_files(const String& file1, const String& file2, const IAbortable *piAbortable) const
{
	if (!m_pImgMergeWindow)
		return DIFFCODE::CMPERR;
	m_pImgMergeWindow->SetColorDistanceThreshold(m_colorDistanceThreshold);
	if (!m_pImgMergeWindow->OpenImages(file1.c_str(), file2.c_str()))
		return DIFFCODE::CMPERR;
	std::optional<bool> bImgDiff = false;
	if (m_pImgMergeWindow->GetPageCount(0) == m_pImgMergeWindow->GetPageCount(1))
	{
		for (int page = 0; page < m_pImgMergeWindow->GetPageCount(0); ++page)
		{
			if (piAbortable && piAbortable->ShouldAbort())
			{
				bImgDiff.reset();
				break;
			}
			m_pImgMergeWindow->SetCurrentPageAll(page);
			if (m_pImgMergeWindow->GetDiffCount() > 0)
			{
				bImgDiff = true;
				break;
			}
		}
	}
	else
	{
		bImgDiff = true;
	}
	m_pImgMergeWindow->CloseImages();
	if (!bImgDiff.has_value())
		return DIFFCODE::CMPABORT;
	return *bImgDiff ? DIFFCODE::DIFF : DIFFCODE::SAME;
}

/**
 * @brief Compare two specified files
 * @param [in] di Diffitem info.
 * @return DIFFCODE
 */
int ImageCompare::CompareFiles(const DIFFITEM &di) const
{
	PathContext files;
	m_ctxt.GetComparePaths(di, files);

	switch (files.GetSize())
	{
	case 2:
		return (!di.diffcode.exists(0) || !di.diffcode.exists(1)) ?
			DIFFCODE::DIFF : compare_files(files[0], files[1], m_ctxt.GetAbortable());
	case 3:
		unsigned code10 = (!di.diffcode.exists(1) || !di.diffcode.exists(0)) ?
			DIFFCODE::DIFF : compare_files(files[1], files[0], m_ctxt.GetAbortable());
		unsigned code12 = (!di.diffcode.exists(1) || !di.diffcode.exists(2)) ?
			DIFFCODE::DIFF : compare_files(files[1], files[2], m_ctxt.GetAbortable());
		unsigned code02 = DIFFCODE::SAME;
		if (code10 == DIFFCODE::SAME && code12 == DIFFCODE::SAME)
			return DIFFCODE::SAME;
		else if (code10 == DIFFCODE::SAME && code12 == DIFFCODE::DIFF)
			return DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY;
		else if (code10 == DIFFCODE::DIFF && code12 == DIFFCODE::SAME)
			return DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY;
		else if (code10 == DIFFCODE::DIFF && code12 == DIFFCODE::DIFF)
		{
			code02 = (!di.diffcode.exists(0) || !di.diffcode.exists(2)) ?
				DIFFCODE::DIFF : compare_files(files[0], files[2], m_ctxt.GetAbortable());
			if (code02 == DIFFCODE::SAME)
				return DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY;
		}
		if (code10 == DIFFCODE::CMPERR || code12 == DIFFCODE::CMPERR || code02 == DIFFCODE::CMPERR)
			return DIFFCODE::CMPERR;
		return DIFFCODE::DIFF;
	}
	return DIFFCODE::CMPERR;
}

} // namespace CompareEngines
