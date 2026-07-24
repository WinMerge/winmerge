/**
 * @file  DiffImageListUtils.cpp
 *
 * @brief Utilities for initializing list control image lists.
 */

#include "StdAfx.h"
#include "DiffImageListUtils.h"
#include "IMergeDoc.h"
#include "DirActions.h"
#include "resource.h"

namespace DiffImageListUtils
{
	void InitializeDiffImageList(CImageList& imageList)
	{
		if (imageList.GetSafeHandle() != nullptr)
			imageList.DeleteImageList();

		const int iconCX = []() {
			const int cx = GetSystemMetrics(SM_CXSMICON);
			if (cx < 24)
				return 16;
			if (cx < 32)
				return 24;
			if (cx < 48)
				return 32;
			return 48;
		}();
		imageList.Create(iconCX, iconCX, ILC_COLOR32 | ILC_MASK, 15, 1);

		const int iconIds[] = {
			IDI_LFILE, IDI_MFILE, IDI_RFILE,
			IDI_MRFILE, IDI_LRFILE, IDI_LMFILE,
			IDI_NOTEQUALFILE, IDI_EQUALFILE, IDI_FILE,
			IDI_EQUALBINARY, IDI_BINARYDIFF,
			IDI_LFOLDER, IDI_MFOLDER, IDI_RFOLDER,
			IDI_MRFOLDER, IDI_LRFOLDER, IDI_LMFOLDER,
			IDI_FILESKIP, IDI_FOLDERSKIP,
			IDI_NOTEQUALFOLDER, IDI_EQUALFOLDER, IDI_FOLDER,
			IDI_COMPARE_ERROR,
			IDI_FOLDERUP, IDI_FOLDERUP_DISABLE,
			IDI_COMPARE_ABORTED,
			IDI_NOTEQUALTEXTFILE, IDI_EQUALTEXTFILE,
			IDI_NOTEQUALIMAGE, IDI_EQUALIMAGE,
			IDI_NOTEQUALWEBPAGE, IDI_EQUALWEBPAGE,
		};
		const size_t iconCount = std::size(iconIds);
		for (size_t i = 0; i < iconCount; ++i)
		{
			HICON icon = static_cast<HICON>(::LoadImage(
				AfxGetInstanceHandle(), MAKEINTRESOURCE(iconIds[i]), IMAGE_ICON,
				iconCX, iconCX, LR_DEFAULTCOLOR));
			if (icon != nullptr)
			{
				imageList.Add(icon);
				::DestroyIcon(icon);
			}
		}
	}

	int GetDiffImageIndex(IMergeDoc* pDoc)
	{
		const bool identical = (pDoc->GetDiffCount() == 0);
		switch (pDoc->GetDocumentType())
		{
		case IMergeDoc::DocumentType::Text:
		case IMergeDoc::DocumentType::Table:
			return identical ? DIFFIMG_TEXTSAME : DIFFIMG_TEXTDIFF;
		case IMergeDoc::DocumentType::Binary:
			return identical ? DIFFIMG_BINSAME : DIFFIMG_BINDIFF;
		case IMergeDoc::DocumentType::Image:
			return identical ? DIFFIMG_IMAGESAME : DIFFIMG_IMAGEDIFF;
		case IMergeDoc::DocumentType::WebPage:
			return identical ? DIFFIMG_WEBPAGESAME : DIFFIMG_WEBPAGEDIFF;
		default:
			return identical ? DIFFIMG_SAME : DIFFIMG_DIFF;
		}
	}
}
