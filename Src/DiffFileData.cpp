/** 
 * @file DiffFileData.cpp
 *
 * @brief Code for DiffFileData class
 *
 * @date  Created: 2003-08-22
 */

#include "pch.h"
#include "DiffFileData.h"
#include "cio.h"
#include <memory>
#include "DiffItem.h"
#include "FileLocation.h"
#include "diff.h"
#include "TFile.h"
#include "FileTransform.h"
#include "unicoder.h"
#include "DebugNew.h"

/**
 * @brief Simple initialization of DiffFileData
 * @note Diffcounts are initialized to invalid values, not zeros.
 */
DiffFileData::DiffFileData()
: m_inf(new file_data[2]{})
, m_used(false)
{
	//Reset(); //this call not needed because memset implicitly used in line 26
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles(const String& szFilepath1, const String& szFilepath2)
{
	m_FileLocation[0].setPath(szFilepath1);
	m_FileLocation[1].setPath(szFilepath2);
	bool b = DoOpenFiles();
	if (!b)
		Reset();
	return b;
}

/** @brief stash away true names for display, before opening files */
void DiffFileData::SetDisplayFilepaths(const String& szTrueFilepath1, const String& szTrueFilepath2)
{
	m_sDisplayFilepath[0] = szTrueFilepath1;
	m_sDisplayFilepath[1] = szTrueFilepath2;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::DoOpenFiles()
{
	Reset();

	const bool same_file = !strutils::compare_nocase(m_FileLocation[0].filepath, m_FileLocation[1].filepath);

	for (int i = 0; i < 2; ++i)
	{
		// Fill in 8-bit versions of names for diffutils (WinMerge doesn't use these)
		// Actual paths are m_FileLocation[i].filepath
		// but these are often temporary files
		// Displayable (original) paths are m_sDisplayFilepath[i]
		m_inf[i].name = strdup(ucr::toSystemCP(m_sDisplayFilepath[i]).c_str());
		if (m_inf[i].name == nullptr)
			return false;

		// Open up file descriptors
		// Always use O_BINARY mode, to avoid terminating file read on ctrl-Z (DOS EOF)
		// Also, WinMerge-modified diffutils handles all three major eol styles
		if (m_inf[i].desc == 0)
		{
			cio::tsopen_s(&m_inf[i].desc, m_FileLocation[i].filepath, O_RDONLY | O_BINARY, _SH_DENYNO, _S_IREAD);
			if (m_inf[i].desc < 0)
				return false;

			// Get file stats (diffutils uses these)
			if (cio::fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
				return false;
		}
		
		if (same_file)
		{
			m_inf[1].desc = m_inf[0].desc;
			m_inf[1].stat = m_inf[0].stat;
		}
	}

	m_used = true;
	return true;
}

/** @brief Clear inf structure to pristine */
void DiffFileData::Reset()
{
	assert(m_inf != nullptr);
	// If diffutils put data in, have it cleanup
	if (m_used)
	{
		cleanup_file_buffers(m_inf);
		m_used = false;
	}
	// clean up any open file handles, and zero stuff out
	// open file handles might be leftover from a failure in DiffFileData::OpenFiles
	if (m_inf[1].desc == m_inf[0].desc)
	{
		m_inf[1].desc = 0;
	}
	for (int i = 0; i < 2; ++i)
	{
		free((void *)m_inf[i].name);

		if (m_inf[i].desc > 0)
		{
			cio::close(m_inf[i].desc);
		}
		m_inf[i] = {};
	}
}

/**
 * @brief Invoke appropriate plugins for prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
bool DiffFileData::Filepath_Transform(bool bForceUTF8,
	const FileTextEncoding & encoding, const String & filepath, String & filepathTransformed,
	const String& filteredFilenames, PrediffingInfo& infoPrediffer)
{
	// third step : prediff (plugins)
	bool bMayOverwrite =  // temp variable set each time it is used
		(filepathTransformed != filepath); // may overwrite if we've already copied to temp file

	// FileTransform_Prediffing tries each prediffer for the pointed out filteredFilenames
	// if a prediffer fails, we consider it is not the good one, that's all
	// FileTransform_Prediffing returns `false` only if the prediffer works, 
	// but the data can not be saved to disk (no more place ??)
	if (!infoPrediffer.Prediffing(filepathTransformed, filteredFilenames, bMayOverwrite, { filepath }))
		return false;

	if ((encoding.m_unicoding && encoding.m_unicoding != ucr::UTF8) || bForceUTF8)
	{
		// fourth step : prepare for diffing
		// may overwrite if we've already copied to temp file
		bool bMayOverwrite1 = 0 != strutils::compare_nocase(filepathTransformed, filepath);
		if (!FileTransform::AnyCodepageToUTF8(encoding.m_codepage, filepathTransformed, bMayOverwrite1))
			return false;
	}
	return true;
}
