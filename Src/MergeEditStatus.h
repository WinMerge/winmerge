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
	virtual void SetLineInfo(LPCTSTR szLine, int nChar, int nChars, int nColumn,
		int nColumns, LPCTSTR szEol, int nCodepage, bool bHasBom) = 0;
};
