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
	virtual bool HasLineInfo() const = 0;
	virtual void SetLineInfo(const tchar_t* szLine, int nChar, int nChars, int nColumn,
		int nColumns, int nSelectedLines, int nSelectedChars, const tchar_t* szEol, int nCodepage, bool bHasBom) = 0;
	/** @brief Tree-sitter language and enclosing-symbol breadcrumb shown in the encoding column; empty parser hides it */
	virtual void SetSyntaxParser(const tchar_t* szParser, const tchar_t* szSymbol) = 0;
	/** @brief Syntax type shown in the syntax column of the status bar */
	virtual void SetSyntaxName(const tchar_t* szName) = 0;
};
