// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  ProjectFile.h
 *
 * @brief Declaration file ProjectFile class
 */
#pragma once

#include "UnicodeString.h"
#include "PathContext.h"
#include <optional>

class ProjectFileItem
{
	friend class ProjectFile;
	friend class ProjectFileHandler;
public:
	ProjectFileItem();
	bool HasLeft() const;
	bool HasMiddle() const;
	bool HasRight() const;
	bool HasLeftDesc() const;
	bool HasMiddleDesc() const;
	bool HasRightDesc() const;
	bool HasFilter() const;
	bool HasSubfolders() const;
	bool HasUnpacker() const;
	bool HasPrediffer() const;
	bool HasWindowType() const;
	bool HasTableDelimiter() const;
	bool HasTableQuote() const;
	bool HasTableAllowNewLinesInQuotes() const;
	bool HasIgnoreWhite() const;
	bool HasIgnoreBlankLines() const;
	bool HasIgnoreCase() const;
	bool HasIgnoreEol() const;
	bool HasIgnoreNumbers() const;
	bool HasIgnoreCodepage() const;
	bool HasIgnoreMissingTrailingEol() const;
	bool HasIgnoreLineBreaks() const;
	bool HasFilterCommentsLines() const;
	bool HasCompareMethod() const;
	bool HasHiddenItems() const;

	String GetLeft(bool * pReadOnly = nullptr) const;
	String GetLeftDesc() const;
	bool GetLeftReadOnly() const;
	String GetMiddle(bool * pReadOnly = nullptr) const;
	String GetMiddleDesc() const;
	bool GetMiddleReadOnly() const;
	String GetRight(bool * pReadOnly = nullptr) const;
	String GetRightDesc() const;
	bool GetRightReadOnly() const;
	String GetFilter() const;
	int GetSubfolders() const;
	String GetUnpacker() const;
	String GetPrediffer() const;
	int GetWindowType() const;
	tchar_t GetTableDelimiter() const;
	tchar_t GetTableQuote() const;
	bool GetTableAllowNewLinesInQuotes() const;
	int GetIgnoreWhite() const;
	bool GetIgnoreBlankLines() const;
	bool GetIgnoreCase() const;
	bool GetIgnoreEol() const;
	bool GetIgnoreNumbers() const;
	bool GetIgnoreCodepage() const;
	bool GetIgnoreMissingTrailingEol() const;
	bool GetIgnoreLineBreaks() const;
	bool GetFilterCommentsLines() const;
	int GetCompareMethod() const;
	const std::vector<String>& GetHiddenItems() const;

	void SetLeft(const String& sLeft, const bool * pReadOnly = nullptr);
	void SetMiddle(const String& sMiddle, const bool * pReadOnly = nullptr);
	void SetRight(const String& sRight, const bool * pReadOnly = nullptr);
	void SetLeftDesc(const String& sLeftDesc);
	void SetMiddleDesc(const String& sMiddleDesc);
	void SetRightDesc(const String& sRightDesc);
	void SetFilter(const String& sFilter);
	void SetSubfolders(bool bSubfolder);
	void SetUnpacker(const String& sUnpacker);
	void SetPrediffer(const String& sPrediffer);
	void SetWindowType(int nWindowType);
	void SetTableDelimiter(tchar_t cTableDelimiter);
	void SetTableQuote(tchar_t cTableQuote);
	void SetTableAllowNewLinesInQuotes(bool bAllowNewLinesInQuotes);
	void SetIgnoreWhite(int nIgnoreWhite);
	void SetIgnoreBlankLines(bool bIgnoreBlankLines);
	void SetIgnoreCase(bool bIgnoreCase);
	void SetIgnoreEol(bool bIgnoreEol);
	void SetIgnoreNumbers(bool bIgnoreNumbers);
	void SetIgnoreCodepage(bool bIgnoreCodepage);
	void SetIgnoreMissingTrailingEol(bool bIgnoreMissingTrailingEol);
	void SetIgnoreLineBreaks(bool bIgnoreLineBreaks);
	void SetFilterCommentsLines(bool bFilterCommentsLines);
	void SetCompareMethod(int nCompareMethod);
	void SetHiddenItems(const std::vector<String>& hiddenItems);

	void GetPaths(PathContext& files, bool & bSubFolders) const;
	void SetPaths(const PathContext& files, bool bSubFolders = false);

	void SetSaveFilter(bool bSaveFilter);
	void SetSaveSubfolders(bool bSaveSubfolders);
	void SetSaveUnpacker(bool bSaveUnpacker);
	void SetSavePrediffer(bool bSavePrediffer);
	void SetSaveIgnoreWhite(bool bSaveIgnoreWhite);
	void SetSaveIgnoreBlankLines(bool bSaveIgnoreBlankLines);
	void SetSaveIgnoreCase(bool bSaveIgnoreCase);
	void SetSaveIgnoreEol(bool bSaveIgnoreEol);
	void SetSaveIgnoreNumbers(bool bSaveIgnoreNumbers);
	void SetSaveIgnoreCodepage(bool bSaveIgnoreCodepage);
	void SetSaveIgnoreMissingTrailingEol(bool bSaveIgnoreMissingTrailingEol);
	void SetSaveIgnoreBreakLines(bool bSaveIgnoreBreakLines);
	void SetSaveFilterCommentsLines(bool bSaveFilterCommentsLines);
	void SetSaveCompareMethod(bool bSaveCompareMethod);
	void SetSaveHiddenItems(bool bSaveHiddenItems);

private:
	PathContext m_paths;
	bool m_bHasLeft; /**< Has left path? */
	bool m_bHasMiddle; /**< Has middle path? */
	bool m_bHasRight; /**< Has right path? */
	std::optional<String> m_leftDesc;
	std::optional<String> m_middleDesc;
	std::optional<String> m_rightDesc;
	std::optional<String> m_filter; /**< Filter name or mask */
	std::optional<int> m_subfolders; /**< Are subfolders included (recursive scan) */
	bool m_bLeftReadOnly; /**< Is left path opened as read-only */
	bool m_bMiddleReadOnly; /**< Is middle path opened as read-only */
	bool m_bRightReadOnly; /**< Is right path opened as read-only */
	std::optional<String> m_unpacker; /**< Unpacker name or pipeline */
	std::optional<String> m_prediffer; /**< Prediffer name or pipeline */
	std::optional<int> m_nWindowType; /**< The value of the window type */
	std::optional<tchar_t> m_cTableDelimiter;
	std::optional<tchar_t> m_cTableQuote;
	std::optional<bool> m_bTableAllowNewLinesInQuotes;
	std::optional<int> m_nIgnoreWhite; /**< The value of the "Whitespaces" setting */
	std::optional<bool> m_bIgnoreBlankLines; /**< The value of the "Ignore blank lines" setting */
	std::optional<bool> m_bIgnoreCase; /**< The value of the "Ignore case" setting */
	std::optional<bool> m_bIgnoreEol; /**< The value of the "Ignore carriage return differences" setting */
	std::optional<bool> m_bIgnoreNumbers; /**< The value of the "Ignore numbers" setting */
	std::optional<bool> m_bIgnoreCodepage; /**< The value of the "Ignore codepage differences" setting */
	std::optional<bool> m_bIgnoreMissingTrailingEol; /**< The value of the "Ignore missing trailing EOL" setting */
	std::optional<bool> m_bIgnoreLineBreaks; /**< The value of the "Ignore line breaks (treat as spaces)" setting */
	std::optional<bool> m_bFilterCommentsLines; /**< The value of the "Ignore comment differences" setting */
	std::optional<int> m_nCompareMethod; /**< The value of the "Compare method" setting */
	std::vector<String> m_vSavedHiddenItems; /**< The list of hidden items saved */
	bool m_bSaveFilter; /**< Save filter? */
	bool m_bSaveSubfolders; /**< Save subfolders? */
	bool m_bSaveUnpacker; /**< Save unpacker? */
	bool m_bSavePrediffer; /**< Save prediffer? */
	bool m_bSaveIgnoreWhite; /**< Save "Whitespaces" setting? */
	bool m_bSaveIgnoreBlankLines; /**< Save "Ignore blank lines" setting? */
	bool m_bSaveIgnoreCase; /**< Save "Ignore case" setting? */
	bool m_bSaveIgnoreEol; /**< Save "Ignore carriage return differences" setting? */
	bool m_bSaveIgnoreNumbers; /**< Save "Ignore numbers" setting? */
	bool m_bSaveIgnoreCodepage; /**< Save "Ignore codepage differences" setting? */
	bool m_bSaveIgnoreMissingTrailingEol; /**< Save "Ignore missing trailing EOL" setting? */
	bool m_bSaveIgnoreBreakLines; /**< Save "Ignore line breaks (treat as spaces)" setting? */
	bool m_bSaveFilterCommentsLines; /**< Save "Ignore comment differences" setting? */
	bool m_bSaveCompareMethod; /**< Save "Compare method" setting? */
	bool m_bSaveHiddenItems;/* Save "Hidden item" settings?*/
};

/**
 * @brief Class for handling project files.
 *
 * This class loads and saves project files. Expat parser and SCEW wrapper for
 * expat are used for XML parsing. We use UTF-8 encoding so Unicode paths are
 * supported.
 */
class ProjectFile
{
public:
	bool Read(const String& path);
	bool Save(const String& path) const;
	const std::list<ProjectFileItem>& Items() const { return m_items; };
	std::list<ProjectFileItem>& Items() { return m_items; }
	static const String PROJECTFILE_EXT;
private:
	std::list<ProjectFileItem> m_items;
};

/** 
 * @brief Returns if left path is defined in project file.
 * @return true if project file has left path.
 */
inline bool ProjectFileItem::HasLeft() const
{
	return m_bHasLeft;
}

/** 
 * @brief Returns if middle path is defined.
 */
inline bool ProjectFileItem::HasMiddle() const
{
	return m_bHasMiddle;
}

/** 
 * @brief Returns if right path is defined in project file.
 * @return true if project file has right path.
 */
inline bool ProjectFileItem::HasRight() const
{
	return m_bHasRight;
}

/** 
 * @brief Returns if left description is defined in project file.
 * @return true if project file has left description.
 */
inline bool ProjectFileItem::HasLeftDesc() const
{
	return m_leftDesc.has_value();
}

/** 
 * @brief Returns if middle description is defined.
 */
inline bool ProjectFileItem::HasMiddleDesc() const
{
	return m_middleDesc.has_value();
}

/** 
 * @brief Returns if right description is defined in project file.
 * @return true if project file has right description.
 */
inline bool ProjectFileItem::HasRightDesc() const
{
	return m_rightDesc.has_value();
}

/** 
 * @brief Returns left description.
 * @return left description.
 */
inline String ProjectFileItem::GetLeftDesc() const
{
	return m_leftDesc.value_or(_T(""));
}

/** 
 * @brief Returns middle description.
 * @return middle description.
 */
inline String ProjectFileItem::GetMiddleDesc() const
{
	return m_middleDesc.value_or(_T(""));
}

/** 
 * @brief Returns right description.
 * @return right description.
 */
inline String ProjectFileItem::GetRightDesc() const
{
	return m_rightDesc.value_or(_T(""));
}

/** 
 * @brief Set left description.
 * @param [in] sDesc New left description.
 */
inline void ProjectFileItem::SetLeftDesc(const String& sDesc)
{
	m_leftDesc = sDesc;
}

/** 
 * @brief Set middle description.
 * @param [in] sDesc New middle description.
 */
inline void ProjectFileItem::SetMiddleDesc(const String& sDesc)
{
	m_middleDesc = sDesc;
}

/** 
 * @brief Set right description.
 * @param [in] sDesc New right description.
 */
inline void ProjectFileItem::SetRightDesc(const String& sDesc)
{
	m_rightDesc = sDesc;
}

/** 
 * @brief Returns if filter is defined in project file.
 * @return true if project file has filter.
 */
inline bool ProjectFileItem::HasFilter() const
{
	return m_filter.has_value();
}

/** 
 * @brief Returns if subfolder is defined in projectfile.
 * @return true if project file has subfolder definition.
 */
inline bool ProjectFileItem::HasSubfolders() const
{
	return m_subfolders.has_value();
}

/** 
 * @brief Returns if unpacker is defined in projectfile.
 * @return true if project file has unpacker definition.
 */
inline bool ProjectFileItem::HasUnpacker() const
{
	return m_unpacker.has_value();
}

/** 
 * @brief Returns if prediffer is defined in projectfile.
 * @return true if project file has prediffer definition.
 */
inline bool ProjectFileItem::HasPrediffer() const
{
	return m_prediffer.has_value();
}

/** 
 * @brief Returns if window type is defined in projectfile.
 * @return true if project file has window type definition.
 */
inline bool ProjectFileItem::HasWindowType() const
{
	return m_nWindowType.has_value();
}

/** 
 * @brief Returns if table delimiter is defined in projectfile.
 * @return true if project file has table delimiter definition.
 */
inline bool ProjectFileItem::HasTableDelimiter() const
{
	return m_cTableDelimiter.has_value();
}

/** 
 * @brief Returns if table quote is defined in projectfile.
 * @return true if project file has table quote definition.
 */
inline bool ProjectFileItem::HasTableQuote() const
{
	return m_cTableQuote.has_value();
}

/** 
 * @brief Returns if table-allownewlinesinquotes is defined in projectfile.
 * @return true if project file has table-allownewlinesinquotes definition.
 */
inline bool ProjectFileItem::HasTableAllowNewLinesInQuotes() const
{
	return m_bTableAllowNewLinesInQuotes.has_value();
}

/** 
 * @brief Returns if "Whitespaces" setting is defined in projectfile.
 * @return true if project file has "Whitespaces" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreWhite() const
{
	return m_nIgnoreWhite.has_value();
}

/** 
 * @brief Returns if "Ignore blank lines" setting is defined in projectfile.
 * @return true if project file has "Ignore blank lines" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreBlankLines() const
{
	return m_bIgnoreBlankLines.has_value();
}

/** 
 * @brief Returns if "Ignore case" setting is defined in projectfile.
 * @return true if project file has "Ignore case" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreCase() const
{
	return m_bIgnoreCase.has_value();
}

/** 
 * @brief Returns if "Ignore carriage return differences" setting is defined in projectfile.
 * @return true if project file has "Ignore carriage return differences" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreEol() const
{
	return m_bIgnoreEol.has_value();
}

/**
 * @brief Returns if "Ignore numbers" setting is defined in projectfile.
 * @return true if project file has "Ignore numbers" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreNumbers() const
{
	return m_bIgnoreNumbers.has_value();
}

/** 
 * @brief Returns if "Ignore codepage differences" setting is defined in projectfile.
 * @return true if project file has "Ignore codepage differences" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreCodepage() const
{
	return m_bIgnoreCodepage.has_value();
}

/** 
 * @brief Returns if "Ignore missing trailing EOL" setting is defined in projectfile.
 * @return true if project file has "Ignore missing trailing EOL" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreMissingTrailingEol() const
{
	return m_bIgnoreMissingTrailingEol.has_value();
}

/** 
 * @brief Returns if "Ignore line breaks (treat as spaces)" setting is defined in projectfile.
 * @return true if project file has "Ignore line breaks (treat as spaces)" setting definition.
 */
inline bool ProjectFileItem::HasIgnoreLineBreaks() const
{
	return m_bIgnoreLineBreaks.has_value();
}

/** 
 * @brief Returns if "Ignore comment differences" is defined in projectfile.
 * @return true if project file has "Ignore comment differences" definition.
 */
inline bool ProjectFileItem::HasFilterCommentsLines() const
{
	return m_bFilterCommentsLines.has_value();
}

/** 
 * @brief Returns if "Compare method" setting is defined in projectfile.
 * @return true if project file has "Compare method" setting definition.
 */
inline bool ProjectFileItem::HasCompareMethod() const
{
	return m_nCompareMethod.has_value();
}

/** 
 * @brief Returns if "Hidden items" setting is defined in projectfile.
 * @return true if project file has "Hidden items" setting definition.
 */
inline bool ProjectFileItem::HasHiddenItems() const
{
	return !m_vSavedHiddenItems.empty();
}

/** 
 * @brief Returns if left path is specified read-only.
 * @return true if left path is read-only, false otherwise.
 */
inline bool ProjectFileItem::GetLeftReadOnly() const
{
	return m_bLeftReadOnly;
}

/** 
 * @brief Returns if middle path is specified read-only.
 */
inline bool ProjectFileItem::GetMiddleReadOnly() const
{
	return m_bMiddleReadOnly;
}

/** 
 * @brief Returns if right path is specified read-only.
 * @return true if right path is read-only, false otherwise.
 */
inline bool ProjectFileItem::GetRightReadOnly() const
{
	return m_bRightReadOnly;
}

/** 
 * @brief Returns filter.
 * @return Filter string.
 */
inline String ProjectFileItem::GetFilter() const
{
	return m_filter.value_or(_T(""));
}

/** 
 * @brief Set filter.
 * @param [in] sFilter New filter string to set.
 */
inline void ProjectFileItem::SetFilter(const String& sFilter)
{
	m_filter = sFilter;
}

/** 
 * @brief Returns subfolder included -setting.
 * @return != 0 if subfolders are included.
 */
inline int ProjectFileItem::GetSubfolders() const
{
	return m_subfolders.value_or(-1);
}

/** 
 * @brief set subfolder.
 * @param [in] iSubfolder New value for subfolder inclusion.
 */
inline void ProjectFileItem::SetSubfolders(bool bSubfolder)
{
	m_subfolders = bSubfolder ? 1 : 0;
}

/** 
 * @brief Returns unpacker name or pipeline
 * @return Unpacker name or pipeline
 */
inline String ProjectFileItem::GetUnpacker() const
{
	return m_unpacker.value_or(_T(""));
}

/** 
 * @brief Set unpacker name or pipeline.
 * @param [in] sUnpacker New unpacker name or pipeline to set.
 */
inline void ProjectFileItem::SetUnpacker(const String& sUnpacker)
{
	m_unpacker = sUnpacker;
}

/** 
 * @brief Returns prediffer name or pipeline
 * @return Prediffer name or pipeline
 */
inline String ProjectFileItem::GetPrediffer() const
{
	return m_prediffer.value_or(_T(""));
}

/** 
 * @brief Set prediffer name or pipeline.
 * @param [in] sPrediffer New prediffer name or pipeline to set.
 */
inline void ProjectFileItem::SetPrediffer(const String& sPrediffer)
{
	m_prediffer = sPrediffer;
}

/** 
 * @brief Returns window type
 * @return Window type
 */
inline int ProjectFileItem::GetWindowType() const
{
	return m_nWindowType.value_or(-1);
}

/** 
 * @brief Set window type
 * @param [in] nWindowType New window type to set.
 */
inline void ProjectFileItem::SetWindowType(int nWindowType)
{
	m_nWindowType = nWindowType;
}

/** 
 * @brief Returns table delimiter
 * @return Table delimiter
 */
inline tchar_t ProjectFileItem::GetTableDelimiter() const
{
	return m_cTableDelimiter.value_or(',');
}

/** 
 * @brief Set table delimiter
 * @param [in] cTableDelimiter New table delimiter to set.
 */
inline void ProjectFileItem::SetTableDelimiter(tchar_t cTableDelimiter)
{
	m_cTableDelimiter = cTableDelimiter;
}

/** 
 * @brief Returns table quote
 * @return Table quote
 */
inline tchar_t ProjectFileItem::GetTableQuote() const
{
	return m_cTableQuote.value_or('\"');
}

/** 
 * @brief Set table quote
 * @param [in] cTableQuote New table quote to set.
 */
inline void ProjectFileItem::SetTableQuote(tchar_t cTableQuote)
{
	m_cTableQuote = cTableQuote;
}

/** 
 * @brief Returns table-allownewlinesinquotes
 * @return Table-allownewlinesinquotes
 */
inline bool ProjectFileItem::GetTableAllowNewLinesInQuotes() const
{
	return m_bTableAllowNewLinesInQuotes.value_or(true);
}

/** 
 * @brief Set table-allownewlinesinquotes
 * @param [in] cTableAllowNewLinesInQuotes New table-allownewlinesinquotes to set.
 */
inline void ProjectFileItem::SetTableAllowNewLinesInQuotes(bool bTableAllowNewLinesInQuotes)
{
	m_bTableAllowNewLinesInQuotes = bTableAllowNewLinesInQuotes;
}

/** 
 * @brief Returns the value of the "Whitespaces" setting.
 * @return The value of the "Whitespaces" setting
 */
inline int ProjectFileItem::GetIgnoreWhite() const
{
	return m_nIgnoreWhite.value_or(0);
}

/** 
 * @brief Set the value of the "Whitespaces" setting.
 * @param [in] nIgnoreWhite New value of the "Whitespaces" setting to set.
 */
inline void ProjectFileItem::SetIgnoreWhite(int nIgnoreWhite)
{
	m_nIgnoreWhite = nIgnoreWhite;
}

/** 
 * @brief Returns the value of the "Ignore blank lines" setting.
 * @return The value of the "Ignore blank lines" setting
 */
inline bool ProjectFileItem::GetIgnoreBlankLines() const
{
	return m_bIgnoreBlankLines.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore blank lines" setting.
 * @param [in] bIgnoreBlankLines New value of the "Ignore blank lines" setting to set.
 */
inline void ProjectFileItem::SetIgnoreBlankLines(bool bIgnoreBlankLines)
{
	m_bIgnoreBlankLines = bIgnoreBlankLines;
}

/** 
 * @brief Returns the value of the "Ignore case" setting.
 * @return Unpacker name or pipelineThe value of the "Ignore case" setting
 */
inline bool ProjectFileItem::GetIgnoreCase() const
{
	return m_bIgnoreCase.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore case" setting.
 * @param [in] bIgnoreCase New value of the "Ignore case" setting to set.
 */
inline void ProjectFileItem::SetIgnoreCase(bool bIgnoreCase)
{
	m_bIgnoreCase = bIgnoreCase;
}

/** 
 * @brief Returns the value of the "Ignore carriage return differences" setting.
 * @return The value of the "Ignore carriage return differences" setting
 */
inline bool ProjectFileItem::GetIgnoreEol() const
{
	return m_bIgnoreEol.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore carriage return differences" setting.
 * @param [in] bIgnoreEol New value of the "Ignore carriage return differences" setting to set.
 */
inline void ProjectFileItem::SetIgnoreEol(bool bIgnoreEol)
{
	m_bIgnoreEol = bIgnoreEol;
}

/**
 * @brief Returns the value of the "Ignore numbers" setting.
 * @return The value of the "Ignore numbers" setting
 */
inline bool ProjectFileItem::GetIgnoreNumbers() const
{
	return m_bIgnoreNumbers.value_or(false);
}

/**
 * @brief Set the value of the "Ignore numbers" setting.
 * @param [in] bIgnoreNumbers New value of the "Ignore numbers" setting to set.
 */
inline void ProjectFileItem::SetIgnoreNumbers(bool bIgnoreNumbers)
{
	m_bIgnoreNumbers = bIgnoreNumbers;
}

/** 
 * @brief Returns the value of the "Ignore codepage differences" setting.
 * @return The value of the "Ignore codepage differences" setting
 */
inline bool ProjectFileItem::GetIgnoreCodepage() const
{
	return m_bIgnoreCodepage.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore codepage differences" setting.
 * @param [in] bIgnoreCodepage New value of the "Ignore codepage differences" setting to set.
 */
inline void ProjectFileItem::SetIgnoreCodepage(bool bIgnoreCodepage)
{
	m_bIgnoreCodepage = bIgnoreCodepage;
}

/** 
 * @brief Returns the value of the "Ignore missing trailing EOL" setting.
 * @return The value of the "Ignore missing trailing EOL" setting
 */
inline bool ProjectFileItem::GetIgnoreMissingTrailingEol() const
{
	return m_bIgnoreMissingTrailingEol.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore missing trailing EOL" setting.
 * @param [in] bIgnoreMissingTrailingEol New value of the "Ignore missing trailing EOL" setting to set.
 */
inline void ProjectFileItem::SetIgnoreMissingTrailingEol(bool bIgnoreMissingTrailingEol)
{
	m_bIgnoreMissingTrailingEol = bIgnoreMissingTrailingEol;
}

/** 
 * @brief Returns the value of the "Ignore line breaks (treat as spaces)" setting.
 * @return The value of the "Ignore line breaks (treat as spaces)" setting
 */
inline bool ProjectFileItem::GetIgnoreLineBreaks() const
{
	return m_bIgnoreLineBreaks.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore line breaks (treat as spaces)" setting.
 * @param [in] bIgnoreLineBreaks New value of the "Ignore line breaks (treat as spaces)" setting to set.
 */
inline void ProjectFileItem::SetIgnoreLineBreaks(bool bIgnoreLineBreaks)
{
	m_bIgnoreLineBreaks = bIgnoreLineBreaks;
}

/** 
 * @brief Returns the value of the "Ignore comment differences" setting.
 * @return The value of the "Ignore comment differences" setting
 */
inline bool ProjectFileItem::GetFilterCommentsLines() const
{
	return m_bFilterCommentsLines.value_or(false);
}

/** 
 * @brief Set the value of the "Ignore comment differences" setting.
 * @param [in] bFilterCommentsLines New value of the "Ignore comment differences" setting to set.
 */
inline void ProjectFileItem::SetFilterCommentsLines(bool bFilterCommentsLines)
{
	m_bFilterCommentsLines = bFilterCommentsLines;
}

/** 
 * @brief Returns the value of the "Compare method" setting.
 * @return The value of the "Compare method" setting
 */
inline int ProjectFileItem::GetCompareMethod() const
{
	return m_nCompareMethod.value_or(0);
}

/** 
 * @brief Set the value of the "Compare method" setting.
 * @param [in] nCompareMethod New value of the "Compare method" setting to set.
 */
inline void ProjectFileItem::SetCompareMethod(int nCompareMethod)
{
	m_nCompareMethod = nCompareMethod;
}

/** 
 * @brief Returns the value of the "Hidden items" setting.
 * @return The value of the "Hidden items" setting
 */
inline const std::vector<String>& ProjectFileItem::GetHiddenItems() const
{
	return m_vSavedHiddenItems;
}

/** 
 * @brief Set the value of the "Hidden items" setting.
 * @param [in] hiddenItems New value of the "Hidden items" setting to set.
 */
inline void ProjectFileItem::SetHiddenItems(const std::vector<String>& hiddenItems)
{
	m_vSavedHiddenItems = hiddenItems;
}

/** 
 * @brief 
 *
 * @param [in] paths Files in project
 * @param [in] bSubFolders If true subfolders included (recursive compare)
 */
inline void ProjectFileItem::SetPaths(const PathContext& paths, bool bSubfolders)
{
	m_paths = paths;
	m_subfolders = bSubfolders;
}

/** 
 * @brief Set whether to save filter.
 * @param [in] bSaveFilter Whether to save filter.
 */
inline void ProjectFileItem::SetSaveFilter(bool bSaveFilter)
{
	m_bSaveFilter = bSaveFilter;
}

/** 
 * @brief Set whether to save subfolders.
 * @param [in] bSaveSubfolders Whether to save subfolders.
 */
inline void ProjectFileItem::SetSaveSubfolders(bool bSaveSubfolders)
{
	m_bSaveSubfolders = bSaveSubfolders;
}

/** 
 * @brief Set whether to save unpacker.
 * @param [in] bSaveUnpacker Whether to save unpacker.
 */
inline void ProjectFileItem::SetSaveUnpacker(bool bSaveUnpacker)
{
	m_bSaveUnpacker = bSaveUnpacker;
}

/** 
 * @brief Set whether to save unpacker.
 * @param [in] bSavePrediffer Whether to save unpacker.
 */
inline void ProjectFileItem::SetSavePrediffer(bool bSavePrediffer)
{
	m_bSavePrediffer = bSavePrediffer;
}

/** 
 * @brief Set whether to save "Whitespaces" setting.
 * @param [in] bSaveIgnoreWhite Whether to save "Whitespaces" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreWhite(bool bSaveIgnoreWhite)
{
	m_bSaveIgnoreWhite = bSaveIgnoreWhite;
}

/** 
 * @brief Set whether to save "Ignore blank lines" setting.
 * @param [in] bSaveIgnoreBlankLines Whether to save "Ignore blank lines" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreBlankLines(bool bSaveIgnoreBlankLines)
{
	m_bSaveIgnoreBlankLines = bSaveIgnoreBlankLines;
}

/** 
 * @brief Set whether to save "Ignore case" setting.
 * @param [in] bSaveIgnoreCase Whether to save "Ignore case" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreCase(bool bSaveIgnoreCase)
{
	m_bSaveIgnoreCase = bSaveIgnoreCase;
}

/** 
 * @brief Set whether to save "Ignore carriage return differences" setting.
 * @param [in] bSaveIgnoreEol Whether to save "Ignore carriage return differences" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreEol(bool bSaveIgnoreEol)
{
	m_bSaveIgnoreEol = bSaveIgnoreEol;
}

/**
 * @brief Set whether to save "Ignore numbers" setting.
 * @param [in] bSaveIgnoreNumbers Whether to save "Ignore numbers" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreNumbers(bool bSaveIgnoreNumbers)
{
	m_bSaveIgnoreNumbers = bSaveIgnoreNumbers;
}

/** 
 * @brief Set whether to save "Ignore codepage differences" setting.
 * @param [in] bSaveIgnoreCodepage Whether to save "Ignore codepage differences" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreCodepage(bool bSaveIgnoreCodepage)
{
	m_bSaveIgnoreCodepage = bSaveIgnoreCodepage;
}

/** 
 * @brief Set whether to save "Ignore missing trailing EOL" setting.
 * @param [in] bSaveIgnoreMissingTrailingEol Whether to save "Ignore missing trailing EOL" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreMissingTrailingEol(bool bSaveIgnoreMissingTrailingEol)
{
	m_bSaveIgnoreMissingTrailingEol = bSaveIgnoreMissingTrailingEol;
}

/** 
 * @brief Set whether to save "Ignore line breaks (treat as spaces)" setting.
 * @param [in] bSaveIgnoreBreakLines Whether to save "Ignore missing trailing EOL" setting.
 */
inline void ProjectFileItem::SetSaveIgnoreBreakLines(bool bSaveIgnoreBreakLines)
{
	m_bSaveIgnoreBreakLines = bSaveIgnoreBreakLines;
}

/** 
 * @brief Set whether to save "Ignore comment differences" setting.
 * @param [in] bSaveFilterCommentsLines Whether to save "Ignore comment differences" setting.
 */
inline void ProjectFileItem::SetSaveFilterCommentsLines(bool bSaveFilterCommentsLines)
{
	m_bSaveFilterCommentsLines = bSaveFilterCommentsLines;
}

/** 
 * @brief Set whether to save "Compare method" setting.
 * @param [in] bSaveCompareMethod Whether to save "Compare method" setting.
 */
inline void ProjectFileItem::SetSaveCompareMethod(bool bSaveCompareMethod)
{
	m_bSaveCompareMethod = bSaveCompareMethod;
}

inline void ProjectFileItem::SetSaveHiddenItems(bool bSaveHiddenItems)
{
	m_bSaveHiddenItems = bSaveHiddenItems;
}