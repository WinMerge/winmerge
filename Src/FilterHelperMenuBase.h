/**
 * @file  FilterHelperMenuBase.h
 *
 * @brief Base class for filter helper menu implementations
 */
#pragma once

#include "UnicodeString.h"
#include <afxwin.h>
#include <optional>

/**
 * @brief Base class providing common functionality for filter menu helpers
 * 
 * This class extracts common patterns from LineFilterHelperMenu and FileFilterHelperMenu,
 * including side selection logic, diff-side handling, and condition generation utilities.
 */
class CFilterHelperMenuBase : public CMenu
{
protected:
	CFilterHelperMenuBase() = default;
	virtual ~CFilterHelperMenuBase() = default;

	// Common state shared by derived classes
	int m_targetSide = 0;       // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_targetDiffSide = 0;   // 0 = left&right, 1 = left&middle, 2 = middle&right, 3 = all

	/**
	 * @brief Get the prefix string for a given side
	 * @param side The side index (0=any/empty, 1=Left, 2=Middle, 3=Right)
	 * @return The prefix string
	 */
	String GetSidePrefix(int side) const;

	/**
	 * @brief Get the prefix string for diff comparison side
	 * @param diffSide The diff side mode (0=left&right, 1=left&middle, 2=middle&right, 3=all)
	 * @param index Which side of the comparison (0=first, 1=second)
	 * @return The prefix string
	 */
	String GetDiffSidePrefix(int diffSide, int index) const;

	/**
	 * @brief Format a condition using a template and identifier
	 * @param conditionTemplate Template string with %1 placeholder
	 * @param identifier The identifier to substitute
	 * @return Formatted condition string
	 */
	String FormatCondition(const tchar_t* conditionTemplate, const String& identifier) const;

	/**
	 * @brief Format a condition using a template and two identifiers
	 * @param conditionTemplate Template string with %1 and %2 placeholders
	 * @param identifier1 The first identifier to substitute
	 * @param identifier2 The second identifier to substitute
	 * @return Formatted condition string
	 */
	String FormatCondition(const tchar_t* conditionTemplate, 
						   const String& identifier1, 
						   const String& identifier2) const;

	/**
	 * @brief Format a condition using a String template and two identifiers
	 * @param conditionTemplate Template string with %1 and %2 placeholders
	 * @param identifier1 The first identifier to substitute
	 * @param identifier2 The second identifier to substitute
	 * @return Formatted condition string
	 */
	String FormatCondition(const String& conditionTemplate, 
						   const String& identifier1, 
						   const String& identifier2) const;

	/**
	 * @brief Generate an allequal() condition
	 * @param content The content to wrap in allequal()
	 * @param negate Whether to negate the condition (not allequal())
	 * @return The generated condition string
	 */
	String GenerateAllEqualCondition(const String& content, bool negate) const;

	/**
	 * @brief Generate a condition from a template array based on command offset
	 * @param templates Array of condition templates
	 * @param templateCount Number of templates in array
	 * @param commandOffset Offset from base command ID
	 * @param identifier The identifier to use in the condition
	 * @return Formatted condition string
	 */
	String GenerateConditionFromArray(const tchar_t* templates[], 
									   size_t templateCount,
									   int commandOffset, 
									   const String& identifier) const;

	/**
	 * @brief Check menu items for target side selection
	 * @param pMenu The menu to modify
	 * @param startId Start of menu ID range
	 * @param endId End of menu ID range
	 */
	void CheckTargetSideMenuItems(CMenu* pMenu, int startId, int endId) const;

	/**
	 * @brief Check menu items for diff side selection
	 * @param pMenu The menu to modify
	 * @param startId Start of menu ID range
	 * @param endId End of menu ID range
	 */
	void CheckDiffSideMenuItems(CMenu* pMenu, int startId, int endId) const;
};
