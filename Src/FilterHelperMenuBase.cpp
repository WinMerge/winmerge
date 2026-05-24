/**
 * @file  FilterHelperMenuBase.cpp
 *
 * @brief Implementation of base class for filter helper menus
 */
#include "StdAfx.h"
#include "FilterHelperMenuBase.h"
#include "LanguageSelect.h"

/**
 * @brief Get the prefix string for a given side
 */
String CFilterHelperMenuBase::GetSidePrefix(int side) const
{
	static const tchar_t* Sides[] = { _T(""), _T("Left"), _T("Middle"), _T("Right") };
	if (side >= 0 && side < static_cast<int>(std::size(Sides)))
		return Sides[side];
	return _T("");
}

/**
 * @brief Get the prefix string for diff comparison side
 */
String CFilterHelperMenuBase::GetDiffSidePrefix(int diffSide, int index) const
{
	static const tchar_t* DiffSides1[] = { _T("Left"), _T("Left"), _T("Middle") };
	static const tchar_t* DiffSides2[] = { _T("Right"), _T("Middle"), _T("Right") };

	if (diffSide < 0 || diffSide >= static_cast<int>(std::size(DiffSides1)))
		return _T("");

	return (index == 0) ? DiffSides1[diffSide] : DiffSides2[diffSide];
}

/**
 * @brief Format a condition using a template and identifier
 */
String CFilterHelperMenuBase::FormatCondition(const tchar_t* conditionTemplate, const String& identifier) const
{
	return strutils::format_string1(conditionTemplate, identifier);
}

/**
 * @brief Format a condition using a template and two identifiers
 */
String CFilterHelperMenuBase::FormatCondition(const tchar_t* conditionTemplate, 
											   const String& identifier1, 
											   const String& identifier2) const
{
	return strutils::format_string2(conditionTemplate, identifier1, identifier2);
}

/**
 * @brief Format a condition using a String template and two identifiers
 */
String CFilterHelperMenuBase::FormatCondition(const String& conditionTemplate, 
											   const String& identifier1, 
											   const String& identifier2) const
{
	return strutils::format_string2(conditionTemplate, identifier1, identifier2);
}

/**
 * @brief Generate an allequal() condition
 */
String CFilterHelperMenuBase::GenerateAllEqualCondition(const String& content, bool negate) const
{
	return (negate ? _T("not allequal(") : _T("allequal(")) + content + _T(")");
}

/**
 * @brief Generate a condition from a template array based on command offset
 */
String CFilterHelperMenuBase::GenerateConditionFromArray(const tchar_t* templates[], 
														  size_t templateCount,
														  int commandOffset, 
														  const String& identifier) const
{
	if (commandOffset < 0 || static_cast<size_t>(commandOffset) >= templateCount)
		return _T("");

	return FormatCondition(templates[commandOffset], identifier);
}

/**
 * @brief Check menu items for target side selection
 */
void CFilterHelperMenuBase::CheckTargetSideMenuItems(CMenu* pMenu, int startId, int endId) const
{
	if (!pMenu)
		return;

	for (int i = startId; i <= endId; i++)
	{
		pMenu->CheckMenuItem(i,
			MF_BYCOMMAND | ((startId + m_targetSide) == i ? MF_CHECKED : 0));
	}
}

/**
 * @brief Check menu items for diff side selection
 */
void CFilterHelperMenuBase::CheckDiffSideMenuItems(CMenu* pMenu, int startId, int endId) const
{
	if (!pMenu)
		return;

	for (int i = startId; i <= endId; i++)
	{
		pMenu->CheckMenuItem(i,
			MF_BYCOMMAND | ((startId + m_targetDiffSide) == i ? MF_CHECKED : 0));
	}
}
