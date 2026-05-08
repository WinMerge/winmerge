/**
 * @file  LineFilterHelperMenu.cpp
 */
#include "StdAfx.h"
#include "LineFilterHelperMenu.h"
#include "LineFilterHelper.h"
#include "FilterConditionDlg.h"
#include "MatchInsideDlg.h"
#include "resource.h"

namespace
{
	// Helper: Convert to line-contains expression, remove prefix, and split directives
	FilterExpression::DirectivesAndExpr NormalizeAndSplit(const String& expr)
	{
		return FilterExpression::SplitDirectivesAndExpr(
			LineFilterHelper::RemovePrefix(LineFilterHelper::ConvertToLineContainsExpression(expr)));
	}

	// Helper: Wrap expression with directives extracted from current filter
	String WrapWithFilterDirectives(const String& filterExpr, const String& wrapperFormat)
	{
		auto [directives, expr] = NormalizeAndSplit(filterExpr);
		String wrapped = strutils::format_string1(wrapperFormat, expr);
		return LineFilterHelper::BuildFilter(directives, wrapped);
	}
}

std::optional<String> CLineFilterHelperMenu::ShowMenu(const String& filterExpr, int x, int y, CWnd* pParentWnd)
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
			else
			{
				result = OnCommand(filterExpr, command, pParentWnd);
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

std::optional<String> CLineFilterHelperMenu::OnCommand(const String& filterExpr, int command, CWnd* pParentWnd)
{
	std::optional<String> result;
	if (command == ID_FILTERMENU_MASK_CLEAR)
	{
		result = _T("");
	}
	else if (command == ID_FILTERMENU_LINE_RANGE)
	{
		CFilterConditionDlg dlg(false, m_targetSide, _T("Line"), _T(""), _("%1 contains %2"), _T("%1"));
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command >= ID_FILTERMENU_LINE_ODD_LINES && command <= ID_FILTERMENU_LINE_EVEN_LINES)
	{
		static const String LineNumberConditions[] = { _T("(%1 %% 2) = 1"), _T("(%1 %% 2) = 0"), };
		String expr = strutils::format_string1(LineNumberConditions[command - ID_FILTERMENU_LINE_ODD_LINES], defaultProp(_T("LineNumber")));
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command == ID_FILTERMENU_LINE_NUMBER_RANGE)
	{
		CFilterConditionDlg dlg(false, m_targetSide, _T("LineNumber"), _T(""), _("%1 > %2"), _T("%1"));
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command >= ID_FILTERMENU_LINE_DIFFERENT && command <= ID_FILTERMENU_LINE_TRIVIAL)
	{
		static const String Identifiers[] = { _T("Different"), _T("Identical"), _T("Trivial") };
		String expr = Identifiers[command - ID_FILTERMENU_LINE_DIFFERENT];
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_EXISTS && command <= ID_FILTERMENU_LINE_BOOKMARKED)
	{
		static const String Identifiers[] = { _T("Exists"), _T("Missing"), _T("Moved"), _T("Bookmarked") };
		String expr = defaultProp(Identifiers[command - ID_FILTERMENU_LINE_EXISTS]);
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_CONTEXT_0 && command <= ID_FILTERMENU_LINE_CONTEXT_7)
	{
		static const String Contexts[] = { _T("0"), _T("1"), _T("3"), _T("5"), _T("7") };
		const int contextIndex = command - ID_FILTERMENU_LINE_CONTEXT_0;
		result = WrapWithFilterDirectives(filterExpr, _T("MatchContext(%1, ") + Contexts[contextIndex] + _T(", ") + Contexts[contextIndex] + _T(")"));
	}
	else if (command >= ID_FILTERMENU_LINE_MATCHNUMBER_EQ_1 && command <= ID_FILTERMENU_LINE_MATCHNUMBER_GT_5)
	{
		static const String Exprs[] = {
			_T("MatchNumber(%1) = 1"), _T("MatchNumber(%1) = MatchCount(%1)"),
			_T("MatchNumber(%1) <= 5"), _T("MatchNumber(%1) > 5"),
		};
		result = WrapWithFilterDirectives(filterExpr, Exprs[command - ID_FILTERMENU_LINE_MATCHNUMBER_EQ_1]);
	}
	else if (command == ID_FILTERMENU_LINE_MATCHNUMBER_RANGE)
	{
		auto [filterDirectives, filterBody] = NormalizeAndSplit(filterExpr);
		CFilterConditionDlg dlg(false, m_targetSide, filterBody, _T(""), _("%1 > %2"), _T("MatchNumber(") + filterBody + _T(")"));
		if (dlg.DoModal() == IDOK)
		{
			auto [dlgDirectives, expr] = FilterExpression::SplitDirectivesAndExpr(dlg.m_sExpression);
			String mergedDirectives = FilterExpression::MergeDirectives(filterDirectives, dlgDirectives);
			result = LineFilterHelper::BuildFilter(mergedDirectives, expr);
		}
	}
	else if (command == ID_FILTERMENU_LINE_MATCHINSIDE_WRAP || command == ID_FILTERMENU_LINE_MATCHOUTSIDE_WRAP)
	{
		CMatchInsideDlg dlg(LineFilterHelper::RemovePrefix(filterExpr), _T(""));
		if (dlg.DoModal() == IDOK)
		{
			auto [directives1, expr1] = NormalizeAndSplit(dlg.GetFilter1());
			auto [directives2, expr2] = NormalizeAndSplit(dlg.GetFilter2());
			String mergedDirectives = FilterExpression::MergeDirectives(directives1, directives2);
			String funcName = (command == ID_FILTERMENU_LINE_MATCHOUTSIDE_WRAP) ? _T("not MatchInside(") : _T("MatchInside(");
			result = LineFilterHelper::BuildFilter(mergedDirectives, funcName + expr1 + _T(", ") + expr2 + _T(")"));
		}
	}
	else if (command == ID_FILTERMENU_LINE_MATCHINSIDE || command == ID_FILTERMENU_LINE_MATCHOUTSIDE)
	{
		CMatchInsideDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			auto [directives1, expr1] = NormalizeAndSplit(dlg.GetFilter1());
			auto [directives2, expr2] = NormalizeAndSplit(dlg.GetFilter2());
			String mergedDirectives = FilterExpression::MergeDirectives(directives1, directives2);
			String funcName = (command == ID_FILTERMENU_LINE_MATCHOUTSIDE) ? _T("not MatchInside(") : _T("MatchInside(");
			String expr = (mergedDirectives.empty() ? _T("") : mergedDirectives + _T(" ")) + funcName + expr1 + _T(", ") + expr2 + _T(")");
			result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
		}
	}
	return result;
}

std::optional<String> CLineFilterHelperMenu::HandleMenuCommand(const String& filterExpr, int command, CWnd* pParentWnd)
{
	return OnCommand(filterExpr, command, pParentWnd);
}
