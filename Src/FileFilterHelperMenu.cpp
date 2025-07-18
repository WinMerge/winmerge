#include "StdAfx.h"
#include "FileFilterHelperMenu.h"
#include "FilterConditionDlg.h"
#include "Merge.h"
#include "resource.h"

std::optional<String> CFileFilterHelperMenu::ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd)
{
	std::optional<String> result;
	VERIFY(LoadMenu(IDR_POPUP_FILTERMENU));
	theApp.TranslateMenu(m_hMenu);
	for (;;)
	{
		CMenu* pPopup = GetSubMenu(0);
		if (pPopup)
		{
			for (int i = ID_FILTERMENU_CONDITION_ANY; i <= ID_FILTERMENU_CONDITION_RIGHT; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_CONDITION_ANY + m_targetSide) == i ? MF_CHECKED : 0));
			for (int i = ID_FILTERMENU_FILE_CONDITION_DIFF_LEFT_RIGHT; i <= ID_FILTERMENU_FILE_CONDITION_DIFF_MIDDLE_RIGHT; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_FILE_CONDITION_DIFF_LEFT_RIGHT + m_targetDiffSide) == i ? MF_CHECKED : 0));

			const int command = pPopup->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
			const String Sides[] = { _T(""), _T("Left"), _T("Middle"), _T("Right") };
			const String DiffSides1[] = { _T("Left"), _T("Left"), _T("Middle") };
			const String DiffSides2[] = { _T("Right"), _T("Middle"), _T("Right") };
			if (command == 0)
			{
				// User cancelled the menu
			}
			else if (command == ID_FILTERMENU_MASK_CLEAR)
			{
				result = _T("");
			}
			else if (command == ID_FILTERMENU_MASK_ALL)
			{
				result = _T("*.*");
			}
			else if (command == ID_FILTERMENU_FILE_SYS)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!pagefile.sys;!hiberfil.sys;!swapfile.sys;!Thumbs.db;!desktop.ini");
			}
			else if (command == ID_FILTERMENU_FILE_BACKUP)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!*.bak;!*.old;!*.orig;!*.swp;!*.swo;!*.tmp;!*.temp;!*.save;*.backup;!*.*~");
			}
			else if (command == ID_FILTERMENU_FILE_BIN)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!*.exe;!*.dll;!*.ocx;!*.sys;!*.drv;!*.cpl;!*.scr;!*.com;!*.jar;!*.war;!*.obj;!*.o;!*.lib;!*.so;!*.a;!*.class;!*.pyc;!*.pyo");
			}
			else if (command == ID_FILTERMENU_FILE_LOG)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!*.log;!*.out;!*.err;!*.trace");
			}
			else if (command == ID_FILTERMENU_FILE_TEMP)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!*.tmp;!*.temp;!*.cache;!*.dmp;!*.swp");
			}
			else if (command == ID_FILTERMENU_FOLDER_VCS)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!.git\\;!.svn\\;!.hg\\");
			}
			else if (command == ID_FILTERMENU_FOLDER_BUILD)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!build\\;!bin\\;!obj\\;!out\\;!dist\\;!release\\;!debug\\;!target\\;!temp\\;!cache\\");
			}
			else if (command == ID_FILTERMENU_FOLDER_IDE)
			{
				result = masks.empty() ? masks : masks + _T(";");
				*result += _T("!.vs\\;!.idea\\;!.vscode\\;!.metadata\\;!.settings\\");
			}
			else if (command >= ID_FILTERMENU_SIZE_LT_1KB && command <= ID_FILTERMENU_SIZE_GE_1GB)
			{
				static const String SizeConditions[] = {
					_T("%1 < 1KB"), _T("%1 >= 1KB"),
					_T("%1 < 10KB"), _T("%1 >= 10KB"),
					_T("%1 < 100KB"), _T("%1 >= 100KB"),
					_T("%1 < 1MB"), _T("%1 >= 1MB"),
					_T("%1 < 10MB"), _T("%1 >= 10MB"),
					_T("%1 < 100MB"), _T("%1 >= 100MB"),
					_T("%1 < 1GB"), _T("%1 >= 1GB")
				};
				const String identifier = Sides[m_targetSide] + _T("Size");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string1(SizeConditions[command - ID_FILTERMENU_SIZE_LT_1KB], identifier);
			}
			else if (command == ID_FILTERMENU_SIZE_RANGE)
			{
				CFilterConditionDlg dlg(false, m_targetSide, _T("Size"), _("%1 = %2"), _T("%1"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_DATE_HOUR_BEFORE_1 && command <= ID_FILTERMENU_DATE_YEAR_SINCE_LAST)
			{
				static const String DateConditions[] = {
					_T("%1 < now() - 1hour"), _T("%1 >= now() - 1hour"),
					_T("%1 < today()"), _T("%1 >= today()"),
					_T("%1 < today() - 1day"), _T("(%1 >= today() - 1day and %1 < today())"), _T("%1 >= today() - 1day"),
					_T("%1 < startOfWeek(now())"), _T("%1 >= startOfWeek(now())"),
					_T("%1 < startOfWeek(now()) - 7days"), _T("(%1 >= startOfWeek(now()) - 7days and %1 < startOfWeek(now()))"), _T("%1 >= startOfWeek(now()) - 7days"),
					_T("%1 < startOfMonth(now())"), _T("%1 >= startOfMonth(now())"),
					_T("%1 < startOfMonth(startOfMonth(now()) - 1day)"), _T("(%1 >= startOfMonth(startOfMonth(now()) - 1day) and %1 < startOfMonth(now()))"), _T("%1 >= startOfMonth(startOfMonth(now()) - 1day)"),
					_T("%1 < startOfYear(now())"), _T("%1 >= startOfYear(now())"),
					_T("%1 < startOfYear(startOfYear(now()) - 1day)"), _T("(%1 >= startOfYear(startOfYear(now()) - 1day) and %1 < startOfYear(now()))"), _T("%1 >= startOfYear(startOfYear(now()) - 1day)")
				};
				const String identifier = Sides[m_targetSide] + _T("Date");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string1(DateConditions[command - ID_FILTERMENU_DATE_HOUR_BEFORE_1], identifier);
			}
			else if (command == ID_FILTERMENU_DATE_RANGE)
			{
				CFilterConditionDlg dlg(false, m_targetSide, _T("DateStr"), _("%1 = %2"), _T("%1"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_ATTR_READONLY && command <= ID_FILTERMENU_ATTR_NOT_SYSTEM)
			{
				static const String AttrConditions[] = {
					_T("%1 contains \"R\""), _T("%1 not contains \"R\""),
					_T("%1 contains \"H\""), _T("%1 not contains \"H\""),
					_T("%1 contains \"S\""), _T("%1 not contains \"S\"")
				};
				const String identifier = Sides[m_targetSide] + _T("AttrStr");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string1(AttrConditions[command - ID_FILTERMENU_ATTR_READONLY], identifier);
			}
			else if (command >= ID_FILTERMENU_CONTENT_CONTAINS && command <= ID_FILTERMENU_CONTENT_LAST_LINE_NOT_CONTAINS)
			{
				const String LHSs[] = {
					_T("%1"), _T("%1"),
					_T("sublines(%1, 0, 1)"),  _T("sublines(%1, 0, 1)"),
					_T("sublines(%1, 0, 10)"), _T("sublines(%1, 0, 10)"),
					_T("sublines(%1, -10)"),   _T("sublines(%1, -10)"),
					_T("sublines(%1, -1)"),    _T("sublines(%1, -1)")
				};
				const String OPs[] = {
					_("%1 contains %2"), _("%1 not contains %2"),
					_("%1 contains %2"), _("%1 not contains %2"),
					_("%1 contains %2"), _("%1 not contains %2"),
					_("%1 contains %2"), _("%1 not contains %2"),
					_("%1 contains %2"), _("%1 not contains %2")
				};
				CFilterConditionDlg dlg(false, m_targetSide,
					_T("Content"), OPs[command - ID_FILTERMENU_CONTENT_CONTAINS],
					LHSs[command - ID_FILTERMENU_CONTENT_CONTAINS]);
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_LINES_LT_10 && command <= ID_FILTERMENU_LINES_GE_100000)
			{
				static const String LineCountConditions[] = {
					_T("lineCount(%1) < 10"), _T("lineCount(%1) >= 10"),
					_T("lineCount(%1) < 100"), _T("lineCount(%1) >= 100"),
					_T("lineCount(%1) < 1000"), _T("lineCount(%1) >= 1000"),
					_T("lineCount(%1) < 10000"), _T("lineCount(%1) >= 10000"),
					_T("lineCount(%1) < 100000"), _T("lineCount(%1) >= 100000")
				};
				const String identifier = Sides[m_targetSide] + _T("Content");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string1(LineCountConditions[command - ID_FILTERMENU_LINES_LT_10], identifier);
			}
			else if (command == ID_FILTERMENU_LINES_RANGE)
			{
				CFilterConditionDlg dlg(false, m_targetSide, _T("Content"), _("%1 > %2"), _T("lineCount(%1)"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_FOLDER_DATE_HOUR_BEFORE_1 && command <= ID_FILTERMENU_FOLDER_DATE_YEAR_SINCE_LAST)
			{
				static const String DateConditions[] = {
					_T("%1 < now() - 1hour"), _T("%1 >= now() - 1hour"),
					_T("%1 < today()"), _T("%1 >= today()"),
					_T("%1 < today() - 1day"), _T("(%1 >= today() - 1day and %1 < today())"), _T("%1 >= today() - 1day"),
					_T("%1 < startOfWeek(now())"), _T("%1 >= startOfWeek(now())"),
					_T("%1 < startOfWeek(now()) - 7days"), _T("(%1 >= startOfWeek(now()) - 7days and %1 < startOfWeek(now()))"), _T("%1 >= startOfWeek(now()) - 7days"),
					_T("%1 < startOfMonth(now())"), _T("%1 >= startOfMonth(now())"),
					_T("%1 < startOfMonth(startOfMonth(now()) - 1day)"), _T("(%1 >= startOfMonth(startOfMonth(now()) - 1day) and %1 < startOfMonth(now()))"), _T("%1 >= startOfMonth(startOfMonth(now()) - 1day)"),
					_T("%1 < startOfYear(now())"), _T("%1 >= startOfYear(now())"),
					_T("%1 < startOfYear(startOfYear(now()) - 1day)"), _T("(%1 >= startOfYear(startOfYear(now()) - 1day) and %1 < startOfYear(now()))"), _T("%1 >= startOfYear(startOfYear(now()) - 1day)")
				};
				const String identifier = Sides[m_targetSide] + _T("Date");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("de:") + strutils::format_string1(DateConditions[command - ID_FILTERMENU_FOLDER_DATE_HOUR_BEFORE_1], identifier);
			}
			else if (command == ID_FILTERMENU_FOLDER_DATE_RANGE)
			{
				CFilterConditionDlg dlg(false, m_targetSide, _T("DateStr"), _("%1 = %2"), _T("%1"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("de:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_CONDITION_ANY && command <= ID_FILTERMENU_CONDITION_RIGHT)
			{
				m_targetSide = command - ID_FILTERMENU_CONDITION_ANY;
				continue;
			}
			else if (command >= ID_FILTERMENU_DIFF_SIZE_EQUAL && command <= ID_FILTERMENU_DIFF_SIZE_GE_1KB)
			{
				static const String DiffSizeConditions[] = {
					_T("%1 = %2"), _T("%1 != %2"),
					_T("abs(%1 - %2) < 10"), _T("abs(%1 - %2) >= 10"),
					_T("abs(%1 - %2) < 100"), _T("abs(%1 - %2) >= 100"),
					_T("abs(%1 - %2) < 1KB"), _T("abs(%1 - %2) >= 1KB")
				};
				const String identifier1 = DiffSides1[m_targetDiffSide] + _T("Size");
				const String identifier2 = DiffSides2[m_targetDiffSide] + _T("Size");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string2(DiffSizeConditions[command - ID_FILTERMENU_DIFF_SIZE_EQUAL],
					identifier1, identifier2);
			}
			else if (command == ID_FILTERMENU_DIFF_SIZE_RANGE)
			{
				CFilterConditionDlg dlg(true, m_targetDiffSide, _T("Size"), _("%1 = %2"), _T("abs(%1 - %2)"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_DIFF_DATE_EQUAL && command <= ID_FILTERMENU_DIFF_DATE_GE_1WEEK)
			{
				static const String DiffDateConditions[] = {
					_T("%1 = %2"), _T("%1 != %2"),
					_T("abs(%1 - %2) < 1second"), _T("abs(%1 - %2) >= 1second"),
					_T("abs(%1 - %2) < 1minute"), _T("abs(%1 - %2) >= 1minute"),
					_T("abs(%1 - %2) < 1hour"), _T("abs(%1 - %2) >= 1hour"),
					_T("abs(%1 - %2) < 1day"), _T("abs(%1 - %2) >= 1day"),
					_T("abs(%1 - %2) < 1week"), _T("abs(%1 - %2) >= 1week")
				};
				const String identifier1 = DiffSides1[m_targetDiffSide] + _T("Date");
				const String identifier2 = DiffSides2[m_targetDiffSide] + _T("Date");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string2(DiffDateConditions[command - ID_FILTERMENU_DIFF_DATE_EQUAL],
					identifier1, identifier2);
			}
			else if (command == ID_FILTERMENU_DIFF_DATE_RANGE)
			{
				CFilterConditionDlg dlg(true, m_targetDiffSide, _T("Date"), _("%1 = %2"), _T("abs(%1 - %2)"));
				if (dlg.DoModal() == IDOK)
					result = (masks.empty() ? masks : masks + _T("|")) + _T("fe:") + dlg.m_sExpression;
			}
			else if (command >= ID_FILTERMENU_DIFF_ATTR_EQUAL && command <= ID_FILTERMENU_DIFF_ATTR_NOT_EQUAL)
			{
				static const String DiffAttrConditions[] = {
					_T("%1 = %2"), _T("%1 != %2")
				};
				const String identifier1 = DiffSides1[m_targetDiffSide] + _T("AttrStr");
				const String identifier2 = DiffSides2[m_targetDiffSide] + _T("AttrStr");
				result = masks.empty() ? masks : masks + _T("|");
				*result += _T("fe:") + strutils::format_string2(DiffAttrConditions[command - ID_FILTERMENU_DIFF_ATTR_EQUAL],
					identifier1, identifier2);
			}
			else if (command >= ID_FILTERMENU_FILE_CONDITION_DIFF_LEFT_RIGHT && command <= ID_FILTERMENU_FILE_CONDITION_DIFF_MIDDLE_RIGHT)
			{
				m_targetDiffSide = command - ID_FILTERMENU_FILE_CONDITION_DIFF_LEFT_RIGHT;
				continue;
			}
		}
		break;
	}
	DestroyMenu();

	return result;
}
