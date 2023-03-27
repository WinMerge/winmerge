/** 
 * @file  MergeEditStatus.h
 *
 * @brief Interface for merge edit view to display status bar info
 *
 */
#pragma once

/**
 * @brief Interface for merge edit view to display status bar info
 */
class IMergeEditStatus
{
public:
	virtual void SetLineInfo(const tchar_t* szLine, int nChar, int nChars, int nColumn,
		int nColumns, int nSelectedLines, int nSelectedChars, const tchar_t* szEol, int nCodepage, bool bHasBom) = 0;
};
