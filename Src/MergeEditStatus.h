/** 
 * @file  MergeEditStatus.h
 *
 * @brief Interface for merge edit view to display status bar info
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

/**
 * @brief Interface for merge edit view to display status bar info
 */
class IMergeEditStatus
{
public:
	virtual void SetLineInfo(LPCTSTR szLine, int nChar, int nChars, int nColumn,
		int nColumns, LPCTSTR szEol) = 0;
};
