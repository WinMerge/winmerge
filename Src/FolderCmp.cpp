/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */

#include "pch.h"
#include "diff.h"
#include "FolderCmp.h"
#include "paths.h"
#include "DiffWrapper.h"
#include "DiffContext.h"
#include "FullQuickCompare.h"
#include "BinaryCompare.h"
#include "TimeSizeCompare.h"
#include "ExistenceCompare.h"
#include "FileFilterHelper.h"
#include "FilterEngine/FilterExpression.h"
#include "Logger.h"
#include "I18n.h"

using CompareEngines::FullQuickCompare;
using CompareEngines::BinaryCompare;
using CompareEngines::TimeSizeCompare;
using CompareEngines::ExistenceCompare;
using CompareEngines::ImageCompare;

FolderCmp::FolderCmp(CDiffContext *pCtxt)
: m_pCtxt(pCtxt)
{
}

FolderCmp::~FolderCmp() = default;

void FolderCmp::LogError(const DIFFITEM& di)
{
	PathContext paths;
	m_pCtxt->GetComparePaths(di, paths);
	const String s = (m_pCtxt->GetCompareDirs() < 3 ?
			strutils::format_string2(_("Failed to compare %1 with %2: "), paths[0], paths[1]) : 
			strutils::format_string3(_("Failed to compare %1 with %2 and %3: "), paths[0], paths[1], paths[2]))
			+ strutils::format(
			_("(errno=%d: %s)"), errno, ucr::toTStringFromACP(std::error_code(errno, std::generic_category()).message()));
	RootLogger::Error(s);
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
int FolderCmp::prepAndCompareFiles(DIFFITEM &di)
{
	int nCompMethod = m_pCtxt->GetCompareMethod();
	int nDirs = m_pCtxt->GetCompareDirs();

	unsigned code = DIFFCODE::FILE | DIFFCODE::CMPERR;

	if (nCompMethod == CMP_CONTENT || nCompMethod == CMP_QUICK_CONTENT)
	{
		if ((di.diffFileInfo[0].size > m_pCtxt->m_nBinaryCompareLimit && di.diffFileInfo[0].size != DirItem::FILE_SIZE_NONE) ||
			(di.diffFileInfo[1].size > m_pCtxt->m_nBinaryCompareLimit && di.diffFileInfo[1].size != DirItem::FILE_SIZE_NONE) ||
			(nDirs > 2 && di.diffFileInfo[2].size > m_pCtxt->m_nBinaryCompareLimit && di.diffFileInfo[2].size != DirItem::FILE_SIZE_NONE))
		{
			nCompMethod = CMP_BINARY_CONTENT;
		}
		else if (m_pCtxt->m_bEnableImageCompare && (
			di.diffFileInfo[0].size != DirItem::FILE_SIZE_NONE && m_pCtxt->m_pImgfileFilter->includeFile(
				paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename)) ||
			di.diffFileInfo[1].size != DirItem::FILE_SIZE_NONE && m_pCtxt->m_pImgfileFilter->includeFile(
				paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename)) ||
			nDirs > 2 && di.diffFileInfo[2].size != DirItem::FILE_SIZE_NONE && m_pCtxt->m_pImgfileFilter->includeFile(
				paths::ConcatPath(di.diffFileInfo[2].path, di.diffFileInfo[2].filename))))
		{
			nCompMethod = CMP_IMAGE_CONTENT;
		}
	}

	if (nCompMethod == CMP_CONTENT)
	{
		// If either file is larger than limit compare files by quick contents
		// This allows us to (faster) compare big binary files
		if ((di.diffFileInfo[0].size > m_pCtxt->m_nQuickCompareLimit ||
		     di.diffFileInfo[1].size > m_pCtxt->m_nQuickCompareLimit ||
		     (nDirs > 2 && di.diffFileInfo[2].size > m_pCtxt->m_nQuickCompareLimit)))
		{
			nCompMethod = CMP_QUICK_CONTENT;
		}
	}

	if (nCompMethod == CMP_CONTENT)
	{
		if (m_pFullCompare == nullptr)
			m_pFullCompare.reset(new FullQuickCompare(*m_pCtxt, nCompMethod));
		code = m_pFullCompare->CompareFiles(di);
	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		if (m_pQuickCompare == nullptr)
			m_pQuickCompare.reset(new FullQuickCompare(*m_pCtxt, nCompMethod));
		code = m_pQuickCompare->CompareFiles(di);
	}
	else if (nCompMethod == CMP_BINARY_CONTENT)
	{
		if (m_pBinaryCompare == nullptr)
			m_pBinaryCompare.reset(new BinaryCompare(*m_pCtxt));
		code = m_pBinaryCompare->CompareFiles(di);
	}
	else if (nCompMethod == CMP_DATE || nCompMethod == CMP_DATE_SIZE || nCompMethod == CMP_SIZE)
	{
		if (m_pTimeSizeCompare == nullptr)
			m_pTimeSizeCompare.reset(new TimeSizeCompare(*m_pCtxt));
		code = m_pTimeSizeCompare->CompareFiles(di);
	}
	else if (nCompMethod == CMP_EXISTENCE)
	{
		if (m_pExistenceCompare == nullptr)
			m_pExistenceCompare.reset(new ExistenceCompare(*m_pCtxt));
		code = m_pExistenceCompare->CompareFiles(di);
	}
	else if (nCompMethod == CMP_IMAGE_CONTENT)
	{
		if (!m_pImageCompare)
			m_pImageCompare.reset(new ImageCompare(*m_pCtxt));
		code = DIFFCODE::IMAGE | m_pImageCompare->CompareFiles(di);
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		throw "Invalid compare type, DiffFileData can't handle it";
	}
	if (DIFFCODE::isResultError(code))
		LogError(di);

	if ((code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME && m_pCtxt->m_pAdditionalCompareExpression)
	{
		m_pCtxt->m_pAdditionalCompareExpression->errorCode = FilterErrorCode::FILTER_ERROR_NO_ERROR;
		if (!m_pCtxt->m_pAdditionalCompareExpression->Evaluate(di))
		{
			if (m_pCtxt->m_pAdditionalCompareExpression->errorCode != FilterErrorCode::FILTER_ERROR_NO_ERROR)
			{
				code &= ~DIFFCODE::COMPAREFLAGS;
				code |= DIFFCODE::CMPERR;
			}
			else
			{
				code &= ~(DIFFCODE::COMPAREFLAGS | DIFFCODE::EXPRFLAGS);
				code |= DIFFCODE::DIFF | DIFFCODE::EXPRDIFF;
			}
		}
	}

	return code;
}

