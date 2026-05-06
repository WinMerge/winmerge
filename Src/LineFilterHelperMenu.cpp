/**
 * @file  LineFilterHelperMenu.cpp
 */
#include "StdAfx.h"
#include "LineFilterHelperMenu.h"
#include "LineFilterHelper.h"
#include "FilterConditionDlg.h"
#include "resource.h"

std::optional<String> CLineFilterHelperMenu::ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd)
{
	std::optional<String> result;
	VERIFY(LoadMenu(IDR_POPUP_LINEFILTERMENU));
	I18n::TranslateMenu(m_hMenu);
	for (;;)
	{
		CMenu* pPopup = GetSubMenu(0);
		if (pPopup)
		{
			for (int i = ID_FILTERMENU_CONDITION_ANY; i <= ID_FILTERMENU_CONDITION_RIGHT; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_CONDITION_ANY + m_targetSide) == i ? MF_CHECKED : 0));
			for (int i = ID_FILTERMENU_OPERATOR_OR; i <= ID_FILTERMENU_OPERATOR_AND; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_OPERATOR_OR + m_operator) == i ? MF_CHECKED : 0));
			for (int i = ID_FILTERMENU_LINE_CONTEXT_0; i <= ID_FILTERMENU_LINE_CONTEXT_7; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_LINE_CONTEXT_0 + m_context) == i ? MF_CHECKED : 0));

			const int command = pPopup->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
			if (command == 0)
			{
				// User cancelled the menu
			}
			else if (command >= ID_FILTERMENU_CONDITION_ANY && command <= ID_FILTERMENU_CONDITION_RIGHT)
			{
				m_targetSide = command - ID_FILTERMENU_CONDITION_ANY;
				continue;
			}
			else if (command >= ID_FILTERMENU_OPERATOR_OR && command <= ID_FILTERMENU_OPERATOR_AND)
			{
				m_operator = command - ID_FILTERMENU_OPERATOR_OR;
				continue;
			}
			else if (command >= ID_FILTERMENU_LINE_CONTEXT_0 && command <= ID_FILTERMENU_LINE_CONTEXT_7)
			{
				m_context = command - ID_FILTERMENU_LINE_CONTEXT_0;
				continue;
			}
			else
			{
				result = OnCommand(masks, command, pParentWnd);
			}
		}
		break;
	}
	DestroyMenu();

	return result;
}

String CLineFilterHelperMenu::op() const
{
	const String Operators[] = { _T("OR"), _T("AND") };
	return Operators[m_operator];
}

String CLineFilterHelperMenu::defaultProp(const String& name) const
{
	const String Sides[] = { _T(""), _T("Left"), _T("Middle"), _T("Right") };
	return Sides[m_targetSide] + name;
}

String CLineFilterHelperMenu::matchContext(const String& expr) const
{
	if (m_context == 0)
		return expr;
	static const String Contexts[] = {_T("0"), _T("1"), _T("3"), _T("5"), _T("7") };
	return _T("MatchContext(") + expr + _T(", ") + Contexts[m_context] + _T(", ") + Contexts[m_context] + _T(")");
}

std::optional<String> CLineFilterHelperMenu::OnCommand(const String& masks, int command, CWnd* pParentWnd)
{
	std::optional<String> result;
	if (command == ID_FILTERMENU_MASK_CLEAR)
	{
		result = _T("");
	}
	else if (command == ID_FILTERMENU_MASK_REMOVE_LAST)
	{
	}
	else if (command == ID_FILTERMENU_LINE_RANGE)
	{
		CFilterConditionDlg dlg(false, m_targetSide, _T("Line"), _T(""), _("%1 contains %2"), _T("%1"));
		if (dlg.DoModal() == IDOK)
		{
			String directives = FilterExpression::ExtractDirectivesPrefix(dlg.m_sExpression);
			String exprWithoutDirective = FilterExpression::RemoveAllDirectives(dlg.m_sExpression);
			String expr = matchContext(exprWithoutDirective);
			if (!directives.empty())
				expr = directives + _T(" ") + expr;
			result = LineFilterHelper::AddToExpression(masks, expr, op());
		}
	}
	else if (command >= ID_FILTERMENU_LINE_ODD_LINES && command <= ID_FILTERMENU_LINE_EVEN_LINES)
	{
		static const String LineNumberConditions[] = { _T("(%1 %% 2) = 1"), _T("(%1 %% 2) = 0"), };
		String expr = matchContext(
			strutils::format_string1(LineNumberConditions[command - ID_FILTERMENU_LINE_ODD_LINES],
			defaultProp(_T("LineNumber"))));
		result = LineFilterHelper::AddToExpression(masks, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_DIFFERENT && command <= ID_FILTERMENU_LINE_TRIVIAL)
	{
		static const String Identifiers[] = { _T("Different"), _T("Identical"), _T("Trivial") };
		String expr = matchContext(Identifiers[command - ID_FILTERMENU_LINE_DIFFERENT]);
		result = LineFilterHelper::AddToExpression(masks, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_EXISTS && command <= ID_FILTERMENU_LINE_BOOKMARKED)
	{
		static const String Identifiers[] = { _T("Exists"), _T("Missing"), _T("Moved"), _T("Bookmarked") };
		String expr = matchContext(defaultProp(Identifiers[command - ID_FILTERMENU_LINE_EXISTS]));
		result = LineFilterHelper::AddToExpression(masks, expr, op());
	}
	return result;
}

std::optional<String> CLineFilterHelperMenu::HandleMenuCommand(const String& masks, int command, CWnd* pParentWnd)
{
	return OnCommand(masks, command, pParentWnd);
}
