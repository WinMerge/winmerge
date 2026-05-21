/**
 * @file  LineFilterHelperMenu.cpp
 */
#include "StdAfx.h"
#include "LineFilterHelperMenu.h"
#include "LineFilterHelper.h"
#include "FilterConditionDlg.h"
#include "MatchInsideDlg.h"
#include "FilterMenuHelpers.h"
#include "ReplaceListHelper.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "Shell.h"
#include "paths.h"
#include "unicoder.h"
#include "resource.h"

using namespace FilterMenuHelpers;

namespace
{
	// Helper: Convert to line-contains expression, remove prefix, and split directives
	FilterExpression::DirectivesAndExpr NormalizeAndSplit(const String& expr)
	{
		return FilterExpression::SplitDirectivesAndExpr(
			LineFilterHelper::RemoveLePrefix(LineFilterHelper::ConvertToLineContainsExpression(expr)));
	}

	// Helper: Wrap expression with directives extracted from current filter
	String WrapWithFilterDirectives(const String& filterExpr, const String& wrapperFormat)
	{
		auto [directives, expr] = NormalizeAndSplit(filterExpr);
		String wrapped = strutils::format_string1(wrapperFormat, expr);
		return LineFilterHelper::BuildLeFilter(directives, wrapped);
	}

	// Helper: Check if character is valid for identifier continuation
	static bool IsIdentifierChar(TCHAR ch)
	{
		return (ch >= _T('A') && ch <= _T('Z')) ||
			   (ch >= _T('a') && ch <= _T('z')) ||
			   (ch >= _T('0') && ch <= _T('9')) ||
			   ch == _T('_');
	}

	// Helper: Check if an identifier represents a string-type attribute
	static bool IsLineOrColumnAttribute(const String& identifier)
	{
		// Check for Line attributes (with or without prefix)
		static const String prefixes[] = { _T(""), _T("Left"), _T("Right"), _T("Middle") };

		for (const auto& prefix : prefixes)
		{
			if (identifier == prefix + _T("Line"))
				return true;
		}

		// Check for ColumnN pattern (with or without prefix)
		size_t colPos = identifier.rfind(_T("Column"));
		if (colPos != String::npos)
		{
			// Check if "Column" is at the right position
			bool validPrefix = (colPos == 0) || // No prefix
							   (colPos == 4 && identifier.substr(0, 4) == _T("Left")) ||
							   (colPos == 5 && identifier.substr(0, 5) == _T("Right")) ||
							   (colPos == 6 && identifier.substr(0, 6) == _T("Middle"));

			if (validPrefix && colPos + 6 < identifier.length())
			{
				// Check if followed by digits only
				for (size_t i = colPos + 6; i < identifier.length(); ++i)
				{
					if (identifier[i] < _T('0') || identifier[i] > _T('9'))
						return false;
				}
				return true;
			}
		}

		return false;
	}

	// Helper: Check if a function returns a string type
	static bool IsLineOrColumnFunction(const String& funcName)
	{
		// String-returning line/column functions (with or without prefix)
		static const String baseFuncs[] = {
			_T("LineAt"), _T("LineOffsetAt"), _T("Column"), _T("ColumnAt"), _T("ColumnOffsetAt")
		};

		static const String prefixes[] = { _T(""), _T("Left"), _T("Right"), _T("Middle") };

		for (const auto& prefix : prefixes)
		{
			for (const auto& baseFunc : baseFuncs)
			{
				if (funcName == prefix + baseFunc)
					return true;
			}
		}

		return false;
	}

	// Helper: Extract an identifier or function call starting at pos
	static String ExtractIdentifierOrFunction(const String& expr, size_t& pos)
	{
		size_t start = pos;
		while (pos < expr.length() && IsIdentifierChar(expr[pos]))
			pos++;

		// Check if it's followed by '(' (function call)
		if (pos < expr.length() && expr[pos] == _T('('))
		{
			int parenDepth = 1;
			bool inQuotes = false;
			pos++; // skip '('
			while (pos < expr.length() && parenDepth > 0)
			{
				if (expr[pos] == _T('"'))
				{
					if (pos + 1 < expr.length() && expr[pos + 1] == _T('"'))
						pos += 2; // escaped quote
					else
						inQuotes = !inQuotes, pos++;
				}
				else if (!inQuotes && (expr[pos] == _T('(') || expr[pos] == _T(')')))
					parenDepth += (expr[pos++] == _T('(') ? 1 : -1);
				else
					pos++;
			}
		}

		return expr.substr(start, pos - start);
	}

	// Helper: Recursively wrap string-type attributes and function arguments with a transform function
	static String WrapExpressionRecursive(const String& expr, const String& funcTemplate);

	// Helper: Process function arguments recursively
	static String ProcessFunctionArguments(const String& args, const String& funcTemplate)
	{
		String result;
		size_t pos = 0;
		int parenDepth = 0;
		bool inQuotes = false;
		size_t argStart = 0;

		while (pos < args.length())
		{
			if (args[pos] == _T('"'))
			{
				if (pos + 1 < args.length() && args[pos + 1] == _T('"'))
					pos += 2; // escaped quote
				else
					inQuotes = !inQuotes, pos++;
			}
			else if (!inQuotes && args[pos] == _T('('))
				parenDepth++, pos++;
			else if (!inQuotes && args[pos] == _T(')'))
				parenDepth--, pos++;
			else if (!inQuotes && parenDepth == 0 && args[pos] == _T(','))
			{
				// Process argument before comma
				result += WrapExpressionRecursive(args.substr(argStart, pos - argStart), funcTemplate);
				result += _T(',');
				pos++;
				argStart = pos;
			}
			else
				pos++;
		}

		// Process last argument
		if (argStart < args.length())
			result += WrapExpressionRecursive(args.substr(argStart, pos - argStart), funcTemplate);

		return result;
	}

	// Helper: Recursively wrap string-type attributes and function arguments
	static String WrapExpressionRecursive(const String& expr, const String& funcTemplate)
	{
		String wrappedExpr;
		size_t pos = 0;

		while (pos < expr.length())
		{
			if (expr[pos] == _T('"'))
			{
				// Copy quoted string (handle escaped quotes "")
				wrappedExpr += expr[pos++];
				while (pos < expr.length())
				{
					wrappedExpr += expr[pos];
					if (expr[pos++] == _T('"'))
					{
						if (pos < expr.length() && expr[pos] == _T('"'))
							wrappedExpr += expr[pos++]; // escaped quote
						else
							break; // end of string
					}
				}
			}
			else if (IsIdentifierChar(expr[pos]) && (pos == 0 || !IsIdentifierChar(expr[pos - 1])))
			{
				// Extract identifier or function
				String identOrFunc = ExtractIdentifierOrFunction(expr, pos);
				size_t parenPos = identOrFunc.find(_T('('));

				if (parenPos != String::npos)
				{
					// It's a function call
					String funcName = identOrFunc.substr(0, parenPos);
					String funcArgs = identOrFunc.substr(parenPos + 1, identOrFunc.length() - parenPos - 2);

					if (IsLineOrColumnFunction(funcName))
					{
						// Wrap the entire function including its recursively processed arguments
						String processedArgs = ProcessFunctionArguments(funcArgs, funcTemplate);
						String processedFunc = funcName + _T("(") + processedArgs + _T(")");
						wrappedExpr += strutils::format_string1(funcTemplate, processedFunc);
					}
					else
					{
						// Not a line/column function, just process its arguments
						String processedArgs = ProcessFunctionArguments(funcArgs, funcTemplate);
						wrappedExpr += funcName + _T("(") + processedArgs + _T(")");
					}
				}
				else
				{
					// It's a simple identifier
					if (IsLineOrColumnAttribute(identOrFunc))
						wrappedExpr += strutils::format_string1(funcTemplate, identOrFunc);
					else
						wrappedExpr += identOrFunc;
				}
			}
			else
				wrappedExpr += expr[pos++];
		}

		return wrappedExpr;
	}

	// Helper: Wrap all string-type attributes and function calls with a transform function
	String WrapAttributesWithFunction(const String& filterExpr, const String& funcTemplate)
	{
		auto [directives, expr] = FilterExpression::SplitDirectivesAndExpr(
			LineFilterHelper::RemoveLePrefix(filterExpr));

		String wrappedExpr = WrapExpressionRecursive(expr, funcTemplate);
		if (expr == wrappedExpr)
			wrappedExpr = strutils::format_string1(funcTemplate, !expr.empty() ? expr : _T("Line"));

		return LineFilterHelper::BuildLeFilter(directives, wrappedExpr);
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
			// Populate replace lists dynamically
			FilterMenuHelpers::PopulateReplaceLists(pPopup,
				ID_FILTERMENU_STRING_REPLACE_LISTS_FIRST,
				ID_FILTERMENU_REGEX_REPLACE_LISTS_FIRST);

			CheckTargetSideMenuItems(pPopup, ID_FILTERMENU_CONDITION_ANY, ID_FILTERMENU_CONDITION_RIGHT);
			CheckDiffSideMenuItems(pPopup, ID_FILTERMENU_CONDITION_DIFF_LEFT_RIGHT, ID_FILTERMENU_CONDITION_DIFF_ALL);
			for (int i = ID_FILTERMENU_OPERATOR_AND; i <= ID_FILTERMENU_OPERATOR_OR; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_OPERATOR_AND + m_operator) == i ? MF_CHECKED : 0));
			for (int i = 0; i < 10; i++)
				CheckMenuItemRecursive(pPopup, ID_FILTERMENU_COLUMN_1 + i, m_columnIndex == i);

			// Check "Match case" if @cs directive is present
			bool hasMatchCase = FilterExpression::HasCaseSensitiveDirective(LineFilterHelper::RemoveLePrefix(filterExpr));
			pPopup->CheckMenuItem(ID_FILTERMENU_MATCHCASE, MF_BYCOMMAND | (hasMatchCase ? MF_CHECKED : MF_UNCHECKED));

			// Check "By Block" if enabled
			pPopup->CheckMenuItem(ID_FILTERMENU_LINE_OCCURRENCE_BY_BLOCK, MF_BYCOMMAND | (m_byBlock ? MF_CHECKED : MF_UNCHECKED));

			if (m_targetDiffSide == 3)
			{
				RemoveMenuItemsInRangeRecursive(pPopup, ID_FILTERMENU_DIFF_COLUMN_NUMBER_LESS, ID_FILTERMENU_DIFF_COLUMN_NUMBER_GREATER_EQUAL);
				RemoveMenuItemsInRangeRecursive(pPopup, ID_FILTERMENU_DIFF_COLUMN_DATETIME_LESS, ID_FILTERMENU_DIFF_COLUMN_DATETIME_GREATER_EQUAL);
			}

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
			else if (command >= ID_FILTERMENU_CONDITION_DIFF_LEFT_RIGHT && command <= ID_FILTERMENU_CONDITION_DIFF_ALL)
			{
				m_targetDiffSide = command - ID_FILTERMENU_CONDITION_DIFF_LEFT_RIGHT;
				continue;
			}
			else if (command >= ID_FILTERMENU_OPERATOR_AND && command <= ID_FILTERMENU_OPERATOR_OR)
			{
				m_operator = command - ID_FILTERMENU_OPERATOR_AND;
				continue;
			}
			else if (command >= ID_FILTERMENU_COLUMN_1 && command <= ID_FILTERMENU_COLUMN_10)
			{
				m_columnIndex = command - ID_FILTERMENU_COLUMN_1;
				continue;
			}
			else if (command == ID_FILTERMENU_LINE_OCCURRENCE_BY_BLOCK)
			{
				m_byBlock = !m_byBlock;
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
	const String Operators[] = { _T("AND"), _T("OR") };
	return Operators[m_operator];
}

String CLineFilterHelperMenu::defaultProp(const String& name) const
{
	return GetSidePrefix(m_targetSide) + name;
}

String CLineFilterHelperMenu::defaultDiffProp(const String& name, int i) const
{
	return GetDiffSidePrefix(m_targetDiffSide, i) + name;
}

String CLineFilterHelperMenu::defaultAllProp(const String& name, bool not) const
{
	return GenerateAllEqualCondition(name, not);
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
	else if (command == ID_FILTERMENU_LINE_LENGTH_RANGE)
	{
		CFilterConditionDlg dlg(false, m_targetSide, _T("LineLength"), _T(""), _("%1 = %2"), _T("%1"));
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command == ID_FILTERMENU_WORD_COUNT_RANGE)
	{
		CFilterConditionDlg dlg(false, m_targetSide, _T("Line"), _T(""), _("%1 = %2"), _T("regexCount(%1, \"\\S+\")"));
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command >= ID_FILTERMENU_COLUMN_TEXT && command <= ID_FILTERMENU_COLUMN_DATETIME)
	{
		static const tchar_t* Conversions[] = { _T("%1"), _T("toNumber(%1)"), _T("toDateTime(%1)") };
		static const tchar_t* Operators[] = { _T("%1 contains %2"), _T("%1 = %2"), _T("%1 < %2") };
		int dataType = (command - ID_FILTERMENU_COLUMN_TEXT) % std::size(Conversions);
		CFilterConditionDlg dlg(false, m_targetSide, 
			_T("Column") + strutils::to_str(m_columnIndex + 1),
			_T(""), Operators[dataType], Conversions[dataType]);
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command >= ID_FILTERMENU_LINE_ODD_LINES && command <= ID_FILTERMENU_LINE_EVEN_LINES)
	{
		static const tchar_t* LineNumberConditions[] = { _T("(%1 %% 2) = 1"), _T("(%1 %% 2) = 0"), };
		String expr = FormatCondition(LineNumberConditions[command - ID_FILTERMENU_LINE_ODD_LINES], defaultProp(_T("LineNumber")));
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
		static const tchar_t* Identifiers[] = { _T("Different"), _T("Identical"), _T("Trivial") };
		String expr = Identifiers[command - ID_FILTERMENU_LINE_DIFFERENT];
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_EXISTS && command <= ID_FILTERMENU_LINE_BOOKMARKED)
	{
		static const tchar_t* Identifiers[] = { _T("Exists"), _T("Missing"), _T("Moved"), _T("Bookmarked") };
		String expr = defaultProp(Identifiers[command - ID_FILTERMENU_LINE_EXISTS]);
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command >= ID_FILTERMENU_EOL_CRLF && command <= ID_FILTERMENU_EOL_MIXED)
	{
		static const tchar_t* EOLStrValues[] = { _T("\"CRLF\""), _T("\"LF\""), _T("\"CR\""), _T("\"None\""), _T("\"Mixed\"") };
		String expr = defaultProp(_T("EOLStr")) + _T(" = ") + EOLStrValues[command - ID_FILTERMENU_EOL_CRLF];
		result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
	}
	else if (command >= ID_FILTERMENU_LINE_CONTEXT_0 && command <= ID_FILTERMENU_LINE_CONTEXT_7)
	{
		static const String Contexts[] = { _T("0"), _T("1"), _T("3"), _T("5"), _T("7") };
		const int contextIndex = command - ID_FILTERMENU_LINE_CONTEXT_0;
		result = WrapWithFilterDirectives(filterExpr, _T("matchContext(%1, ") + Contexts[contextIndex] + _T(", ") + Contexts[contextIndex] + _T(")"));
	}
	else if (command >= ID_FILTERMENU_LINE_MATCHNUMBER_EQ_1 && command <= ID_FILTERMENU_LINE_MATCHNUMBER_GT_5)
	{
		const tchar_t* matchFuncName = m_byBlock ? _T("matchBlockNumber") : _T("matchNumber");
		const tchar_t* countFuncName = m_byBlock ? _T("blockCount") : _T("count");
		const tchar_t* Exprs[] = {
			_T("%s(%%1) = 1"), _T("%s(%%1) = %s(%%1)"),
			_T("%s(%%1) <= 5"), _T("%s(%%1) > 5"),
		};
		int index = command - ID_FILTERMENU_LINE_MATCHNUMBER_EQ_1;
		String expr;
		if (index == 1) // Last occurrence: matchNumber(%1) = count(%1)
			expr = strutils::format(Exprs[index], matchFuncName, countFuncName);
		else
			expr = strutils::format(Exprs[index], matchFuncName);
		result = WrapWithFilterDirectives(filterExpr, expr);
	}
	else if (command == ID_FILTERMENU_LINE_MATCHNUMBER_RANGE)
	{
		auto [filterDirectives, filterBody] = NormalizeAndSplit(filterExpr);
		const tchar_t* funcName = m_byBlock ? _T("matchBlockNumber") : _T("matchNumber");
		String propName = strutils::format(_T("%s(%s)"), funcName, filterBody.c_str());
		CFilterConditionDlg dlg(false, m_targetSide, filterBody, _T(""), _("%1 > %2"), propName);
		if (dlg.DoModal() == IDOK)
		{
			auto [dlgDirectives, expr] = FilterExpression::SplitDirectivesAndExpr(dlg.m_sExpression);
			String mergedDirectives = FilterExpression::MergeDirectives(filterDirectives, dlgDirectives);
			result = LineFilterHelper::BuildLeFilter(mergedDirectives, expr);
		}
	}
	else if (command == ID_FILTERMENU_LINE_MATCHINSIDE_WRAP || command == ID_FILTERMENU_LINE_MATCHOUTSIDE_WRAP)
	{
		CMatchInsideDlg dlg(filterExpr, filterExpr);
		if (dlg.DoModal() == IDOK)
		{
			auto [directives1, expr1] = NormalizeAndSplit(dlg.GetFilter1());
			auto [directives2, expr2] = NormalizeAndSplit(dlg.GetFilter2());
			if (expr1.empty())
				expr1 = _T("Line contains \"BEGIN\"");
			if (expr2.empty())
				expr2 = _T("Line contains \"END\"");
			String mergedDirectives = FilterExpression::MergeDirectives(directives1, directives2);
			String funcName = (command == ID_FILTERMENU_LINE_MATCHOUTSIDE_WRAP) ? _T("not matchInside(") : _T("matchInside(");
			result = LineFilterHelper::BuildLeFilter(mergedDirectives, funcName + expr1 + _T(", ") + expr2 + _T(")"));
		}
	}
	else if (command == ID_FILTERMENU_LINE_MATCHINSIDE || command == ID_FILTERMENU_LINE_MATCHOUTSIDE)
	{
		CMatchInsideDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			auto [directives1, expr1] = NormalizeAndSplit(dlg.GetFilter1());
			auto [directives2, expr2] = NormalizeAndSplit(dlg.GetFilter2());
			if (expr1.empty())
				expr1 = _T("Line contains \"BEGIN\"");
			if (expr2.empty())
				expr2 = _T("Line contains \"END\"");
			String mergedDirectives = FilterExpression::MergeDirectives(directives1, directives2);
			String funcName = (command == ID_FILTERMENU_LINE_MATCHOUTSIDE) ? _T("not matchInside(") : _T("matchInside(");
			String expr = (mergedDirectives.empty() ? _T("") : mergedDirectives + _T(" ")) + funcName + expr1 + _T(", ") + expr2 + _T(")");
			result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
		}
	}
	else if (command >= ID_FILTERMENU_FUNC_FIRST && command <= ID_FILTERMENU_FUNC_LAST)
	{
		static const tchar_t* TransformFunctions[] = {
			_T("trim(%1)"),
			_T("regexReplace(%1, \"[ \\t]+\", \" \")"),
			_T("replace(%1, \"from\", \"to\")"),
			_T("regexReplace(%1, \"pattern\", \"replacement\")"),
			_T("toLower(%1)"),
			_T("toUpper(%1)"),
			_T("toHalfWidth(%1)"),
			_T("toFullWidth(%1)"),
			_T("normalizeUnicode(%1, \"NFC\")"),
			_T("toSimplifiedChinese(%1)"),
			_T("toTraditionalChinese(%1)"),
			_T("toHiragana(%1)"),
			_T("toKatakana(%1)"),
		};
		int index = (command - ID_FILTERMENU_FUNC_FIRST);
		if (index >= 0 && index < static_cast<int>(std::size(TransformFunctions)))
		{
			result = WrapAttributesWithFunction(filterExpr, TransformFunctions[index]);
			if (result == filterExpr)
			{
				result = WrapAttributesWithFunction(
					LineFilterHelper::AddToExpression(filterExpr, _T("LeftLine = RightLine"), op()),
					TransformFunctions[index]);
			}
		}
	}
	else if (command >= ID_FILTERMENU_DIFF_LINE_EQUAL && command <= ID_FILTERMENU_DIFF_LINE_NOT_EQUAL)
	{
		if (m_targetDiffSide == 3)
		{
			result = LineFilterHelper::AddToExpression(filterExpr,
				defaultAllProp(_T("Line"), command == ID_FILTERMENU_DIFF_LINE_NOT_EQUAL), op());
		}
		else
		{
			static const tchar_t* DiffLineLengthConditions[] = {
				_T("%1 = %2"), _T("%1 != %2"),
			};
			const String identifier1 = defaultDiffProp(_T("Line"), 0);
			const String identifier2 = defaultDiffProp(_T("Line"), 1);
			result = LineFilterHelper::AddToExpression(filterExpr,
				FormatCondition(DiffLineLengthConditions[command - ID_FILTERMENU_DIFF_LINE_EQUAL],
				identifier1, identifier2), op());
		}
	}
	else if (command >= ID_FILTERMENU_DIFF_EOL_EQUAL && command <= ID_FILTERMENU_DIFF_EOL_NOT_EQUAL)
	{
		if (m_targetDiffSide == 3)
		{
			result = LineFilterHelper::AddToExpression(filterExpr,
				defaultAllProp(_T("EOL"), command == ID_FILTERMENU_DIFF_EOL_NOT_EQUAL), op());
		}
		else
		{
			static const tchar_t* DiffEOLConditions[] = {
				_T("%1 = %2"), _T("%1 != %2"),
			};
			const String identifier1 = defaultDiffProp(_T("EOL"), 0);
			const String identifier2 = defaultDiffProp(_T("EOL"), 1);
			result = LineFilterHelper::AddToExpression(filterExpr,
				FormatCondition(DiffEOLConditions[command - ID_FILTERMENU_DIFF_EOL_EQUAL],
				identifier1, identifier2), op());
		}
	}
	else if (command >= ID_FILTERMENU_DIFF_LINE_LENGTH_EQUAL && command <= ID_FILTERMENU_DIFF_LINE_LENGTH_GE_1000)
	{
		if (m_targetDiffSide == 3)
		{
			result = LineFilterHelper::AddToExpression(filterExpr,
				defaultAllProp(_T("LineLength"), command == ID_FILTERMENU_DIFF_LINE_LENGTH_NOT_EQUAL), op());
		}
		else
		{
			static const tchar_t* DiffLineLengthConditions[] = {
				_T("%1 = %2"), _T("%1 != %2"),
				_T("%1 < %2"), _T("%1 <= %2"),
				_T("%1 > %2"), _T("%1 >= %2"),
				_T("abs(%1 - %2) < 10"), _T("abs(%1 - %2) >= 10"),
				_T("abs(%1 - %2) < 100"), _T("abs(%1 - %2) >= 100"),
				_T("abs(%1 - %2) < 1000"), _T("abs(%1 - %2) >= 1000")
			};
			const String identifier1 = defaultDiffProp(_T("LineLength"), 0);
			const String identifier2 = defaultDiffProp(_T("LineLength"), 1);
			result = LineFilterHelper::AddToExpression(filterExpr,
				FormatCondition(DiffLineLengthConditions[command - ID_FILTERMENU_DIFF_LINE_LENGTH_EQUAL],
				identifier1, identifier2), op());
		}
	}
	else if (command == ID_FILTERMENU_DIFF_LINE_LENGTH_RANGE)
	{
		CFilterConditionDlg dlg(true, m_targetDiffSide, _T("LineLength"), _T(""), _("%1 = %2"), _T("abs(%1 - %2)"));
		if (dlg.DoModal() == IDOK)
			result = LineFilterHelper::AddToExpression(filterExpr, dlg.m_sExpression, op());
	}
	else if (command >= ID_FILTERMENU_DIFF_COLUMN_TEXT_EQUAL && command <= ID_FILTERMENU_DIFF_COLUMN_DATETIME_GREATER_EQUAL)
	{
		const String columnName = _T("Column") + strutils::to_str(m_columnIndex + 1);
		if (m_targetDiffSide == 3)
		{
			static const tchar_t* DiffColumnConditions[] = {
				_T("%1"), _T("%1"),
				_T("toNumber(%1)"), _T("toNumber(%1)"), _T(""), _T(""), _T(""), _T(""),
				_T("toDateTime(%1)"), _T("toDateTime(%1)"), _T(""), _T(""), _T(""), _T("")
			};
			static const bool NotConditions[] = {
				false, true,
				false, true, false, false, false, false,
				false, true, false, false, false, false,
			};
			const String func = FormatCondition(
				DiffColumnConditions[command - ID_FILTERMENU_DIFF_COLUMN_TEXT_EQUAL],
				columnName);
			const String expr = defaultAllProp(func, NotConditions[command - ID_FILTERMENU_DIFF_COLUMN_TEXT_EQUAL]);
			result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
		}
		else
		{
			static const String DiffColumnConditions[] = {
				_T("%1 = %2"), _T("%1 != %2"),
				_T("toNumber(%1) = toNumber(%2)"), _T("toNumber(%1) != toNumber(%2)"), _T("toNumber(%1) < toNumber(%2)"), _T("toNumber(%1) <= toNumber(%2)"), _T("toNumber(%1) > toNumber(%2)"), _T("toNumber(%1) >= toNumber(%2)"),
				_T("toDateTime(%1) = toDateTime(%2)"), _T("toDateTime(%1) != toDateTime(%2)"), _T("toDateTime(%1) < toDateTime(%2)"), _T("toDateTime(%1) <= toDateTime(%2)"), _T("toDateTime(%1) > toDateTime(%2)"), _T("toDateTime(%1) >= toDateTime(%2)"),
			};
			const String identifier1 = defaultDiffProp(columnName, 0);
			const String identifier2 = defaultDiffProp(columnName, 1);
			const String expr = FormatCondition(
				DiffColumnConditions[command - ID_FILTERMENU_DIFF_COLUMN_TEXT_EQUAL],
				identifier1, identifier2);
			result = LineFilterHelper::AddToExpression(filterExpr, expr, op());
		}
	}
	else if (command == ID_FILTERMENU_CREATE_REPLACELIST)
	{
		auto filepath = ReplaceListHelper::CreateAndSelectReplaceListFile(pParentWnd, false);
		if (filepath.has_value())
		{
			String filepath2 = ReplaceListHelper::ReplaceAppDataFolderOrUserProfileFolder(*filepath);
			result = WrapAttributesWithFunction(filterExpr, _T("replaceWithList(%1, \"") + filepath2 + _T("\")"));
		}
	}
	else if (command == ID_FILTERMENU_CREATE_REGEXREPLACELIST)
	{
		auto filepath = ReplaceListHelper::CreateAndSelectReplaceListFile(pParentWnd, true);
		if (filepath.has_value())
		{
			String filepath2 = ReplaceListHelper::ReplaceAppDataFolderOrUserProfileFolder(*filepath);
			result = WrapAttributesWithFunction(filterExpr, _T("regexReplaceWithList(%1, \"") + filepath2 + _T("\")"));
		}
	}
	else if (command == ID_FILTERMENU_STRING_REPLACE_LISTS_FOLDER)
	{
		int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
		String folder = ReplaceListHelper::GetReplaceListFolder(locationType, false);
		if (!folder.empty())
			shell::Open(folder.c_str());
	}
	else if (command == ID_FILTERMENU_REGEX_REPLACE_LISTS_FOLDER)
	{
		int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
		String folder = ReplaceListHelper::GetReplaceListFolder(locationType, true);
		if (!folder.empty())
			shell::Open(folder.c_str());
	}
	else if (command >= ID_FILTERMENU_STRING_REPLACE_LISTS_FIRST && 
			 command < ID_FILTERMENU_STRING_REPLACE_LISTS_FIRST + ReplaceListHelper::MaxReplaceListSize)
	{
		auto lists = ReplaceListHelper::GetReplaceLists(false);
		int index = command - ID_FILTERMENU_STRING_REPLACE_LISTS_FIRST;
		if (index < static_cast<int>(lists.size()))
		{
			String filepathEscaped = ReplaceListHelper::ReplaceAppDataFolderOrUserProfileFolder(lists[index]);
			strutils::replace(filepathEscaped, _T("%"), _T("%%"));
			result = WrapAttributesWithFunction(filterExpr, _T("replaceWithList(%1, \"") + filepathEscaped + _T("\")"));
		}
	}
	else if (command >= ID_FILTERMENU_REGEX_REPLACE_LISTS_FIRST && 
			 command < ID_FILTERMENU_REGEX_REPLACE_LISTS_FIRST + ReplaceListHelper::MaxReplaceListSize)
	{
		auto lists = ReplaceListHelper::GetReplaceLists(true);
		int index = command - ID_FILTERMENU_REGEX_REPLACE_LISTS_FIRST;
		if (index < static_cast<int>(lists.size()))
		{
			String filepathEscaped = ReplaceListHelper::ReplaceAppDataFolderOrUserProfileFolder(lists[index]);
			strutils::replace(filepathEscaped, _T("%"), _T("%%"));
			result = WrapAttributesWithFunction(filterExpr, _T("regexReplaceWithList(%1, \"") + filepathEscaped + _T("\")"));
		}
	}
	else if (command == ID_FILTERMENU_MATCHCASE)
	{
		auto [directives, filterBody] = NormalizeAndSplit(filterExpr);
		directives = FilterExpression::HasCaseSensitiveDirective(directives) ? 
			FilterExpression::RemoveCaseSensitiveDirective(directives) : FilterExpression::AddCaseSensitiveDirective(directives);
		result = LineFilterHelper::BuildLeFilter(directives, filterBody);
	}
	return result;
}

std::optional<String> CLineFilterHelperMenu::HandleMenuCommand(const String& filterExpr, int command, CWnd* pParentWnd)
{
	return OnCommand(filterExpr, command, pParentWnd);
}
