/** 
 * @file  SourceControl.cpp
 *
 * @brief Implementation file for some source control-related functions.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#define POCO_NO_UNWINDOWS 1
#include <Poco/Process.h>
#include <Poco/Format.h>
#include <direct.h>
#include <Shlwapi.h>
#include <initguid.h>
#include "ssauto.h"
#include "MyCom.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include "MainFrm.h"
#include "Merge.h"
#include "MergeApp.h"
#include "OptionsDef.h"
#include "RegKey.h"
#include "paths.h"
#include "VssPrompt.h"
#include "WaitStatusCursor.h"
#include "CCPrompt.h"
#include "coretools.h"

using Poco::format;
using Poco::Process;
using Poco::ProcessHandle;

/**
 * @brief Shows VSS error from exception and writes log.
 */
static void ShowVSSError(HRESULT hr, const String& strItem)
{
	String errStr = GetSysError(hr);
	if (errStr == _T("?"))
	{
		CMyComPtr<IErrorInfo> perrinfo;
		if (SUCCEEDED(GetErrorInfo(0, &perrinfo)) && perrinfo)
		{
			CMyComBSTR bstrSource, bstrDesc;
			if (SUCCEEDED(perrinfo->GetSource(&bstrSource)) && bstrSource.m_str)
				errStr = _("\n") + ucr::toTString(bstrSource.m_str) + _T(" - ");
			if (SUCCEEDED(perrinfo->GetDescription(&bstrDesc)) && bstrDesc.m_str)
				errStr += ucr::toTString(bstrDesc.m_str);
		}
	}
	if (!errStr.empty())
	{
		String errMsg = _("Error from VSS:");
		String logMsg = errMsg;
		errMsg += _T("\n");
		errMsg += errStr;
		logMsg += _T(" ");
		logMsg += errStr;
		if (!strItem.empty())
		{
			errMsg += _T("\n\n");
			errMsg += strItem;
			logMsg += _T(": ");
			logMsg += strItem;
		}
		LogErrorString(logMsg);
		AppErrorMessageBox(errMsg);
	}
	else
	{
		LogErrorString(_T("VSSError (unable to GetErrorMessage)"));
		AppErrorMessageBox(_("Error executing versioning system command."));
	}
}

void
CMainFrame::InitializeSourceControlMembers()
{
	m_vssHelper.SetProjectBase((const TCHAR *)theApp.GetProfileString(_T("Settings"), _T("VssProject"), _T("")));
	m_strVssUser = theApp.GetProfileString(_T("Settings"), _T("VssUser"), _T(""));
//	m_strVssPassword = theApp.GetProfileString(_T("Settings"), _T("VssPassword"), _T(""));
	theApp.WriteProfileString(_T("Settings"), _T("VssPassword"), _T(""));
	m_strVssDatabase = theApp.GetProfileString(_T("Settings"), _T("VssDatabase"),_T(""));
	m_strCCComment = _T("");
	m_bCheckinVCS = FALSE;

	String vssPath = GetOptionsMgr()->GetString(OPT_VSS_PATH);
	if (vssPath.empty())
	{
		CRegKeyEx reg;
		if (reg.QueryRegMachine(_T("SOFTWARE\\Microsoft\\SourceSafe")))
		{
			TCHAR temp[_MAX_PATH] = {0};
			reg.ReadChars(_T("SCCServerPath"), temp, _MAX_PATH, _T(""));
			vssPath = paths_ConcatPath(paths_GetPathOnly(temp), _T("Ss.exe"));
			GetOptionsMgr()->SaveOption(OPT_VSS_PATH, vssPath);
		}
	}
}

/**
* @brief Saves file to selected version control system
* @param strSavePath Path where to save including filename
* @return Tells if caller can continue (no errors happened)
* @sa CheckSavePath()
*/
BOOL CMainFrame::SaveToVersionControl(const String& strSavePath)
{
	String spath, sname;
	paths_SplitFilename(strSavePath, &spath, &sname, NULL);
	CFileStatus status;
	UINT userChoice = 0;
	int nVerSys = 0;

	nVerSys = GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM);

	switch(nVerSys)
	{
	case VCS_NONE:	//no versioning system
		// Already handled in CheckSavePath()
		break;
	case VCS_VSS4:	// Visual Source Safe
	{
		// Prompt for user choice
		CVssPrompt dlg;
		dlg.m_strMessage = string_format_string1(_("Save changes to %1?"), strSavePath).c_str();
		dlg.m_strProject = m_vssHelper.GetProjectBase().c_str();
		dlg.m_strUser = m_strVssUser.c_str();          // BSP - Add VSS user name to dialog box
		dlg.m_strPassword = m_strVssPassword.c_str();

		// Dialog not suppressed - show it and allow user to select "checkout all"
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = FALSE;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
		}
		else // Dialog already shown and user selected to "checkout all"
			userChoice = IDOK;

		// process versioning system specific action
		if (userChoice == IDOK)
		{
			WaitStatusCursor waitstatus(_("Checkout files from VSS..."));
			m_vssHelper.SetProjectBase((const TCHAR *)dlg.m_strProject);
			theApp.WriteProfileString(_T("Settings"), _T("VssProject"), m_vssHelper.GetProjectBase().c_str());
			if (!spath.empty())
			{
				_chdrive(_totupper(spath[0]) - 'A' + 1);
				_tchdir(spath.c_str());
			}
			try
			{
				std::string vssPath = ucr::toUTF8(GetOptionsMgr()->GetString(OPT_VSS_PATH));
				std::string sn;
				std::vector<std::string> args;
				args.push_back("checkout");
				format(sn, "\"%s/%s\"", ucr::toUTF8(m_vssHelper.GetProjectBase()), ucr::toUTF8(sname));
				args.push_back(sn);
				ProcessHandle hVss(Process::launch(vssPath, args));
				int code = Process::wait(hVss);
				if (code != 0)
				{
					LangMessageBox(IDS_VSSERROR, MB_ICONSTOP);
					return FALSE;
				}
			}
			catch (...)
			{
				LangMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
				return FALSE;
			}
		}
		else
			return FALSE; // User selected cancel
	}
	break;
	case VCS_VSS5: // CVisual SourceSafe 5.0+ (COM)
	{
		// prompt for user choice
		CVssPrompt dlg;
		CRegKeyEx reg;

		dlg.m_strMessage = string_format_string1(_("Save changes to %1?"), strSavePath).c_str();
		dlg.m_strProject = m_vssHelper.GetProjectBase().c_str();
		dlg.m_strUser = m_strVssUser.c_str();          // BSP - Add VSS user name to dialog box
		dlg.m_strPassword = m_strVssPassword.c_str();
		dlg.m_strSelectedDatabase = m_strVssDatabase.c_str();
		dlg.m_bVCProjSync = TRUE;

		// Dialog not suppressed - show it and allow user to select "checkout all"
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = FALSE;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
			if (userChoice != IDOK)
				return FALSE; // User selected cancel
		}
		// process versioning system specific action
		WaitStatusCursor waitstatus(_("Checkout files from VSS..."));
		BOOL bOpened = FALSE;
		m_vssHelper.SetProjectBase((const TCHAR *)dlg.m_strProject);
		m_strVssUser = dlg.m_strUser;
		m_strVssPassword = dlg.m_strPassword;
		m_strVssDatabase = dlg.m_strSelectedDatabase;
		m_bVCProjSync = dlg.m_bVCProjSync;					

		theApp.WriteProfileString(_T("Settings"), _T("VssDatabase"), m_strVssDatabase.c_str());
		theApp.WriteProfileString(_T("Settings"), _T("VssProject"), m_vssHelper.GetProjectBase().c_str());
		theApp.WriteProfileString(_T("Settings"), _T("VssUser"), m_strVssUser.c_str());
//		theApp.WriteProfileString(_T("Settings"), _T("VssPassword"), m_strVssPassword.c_str());

		HRESULT hr;
		CMyComPtr<IVSSDatabase> vssdb;
		CMyComPtr<IVSSItems> vssis;
		CMyComPtr<IVSSItem> vssi;
			
		// BSP - Create the COM interface pointer to VSS
		if (FAILED(hr = vssdb.CoCreateInstance(CLSID_VSSDatabase, IID_IVSSDatabase)))
		{
			ShowVSSError(hr, _T(""));
			return FALSE;
		}
				// BSP - Open the specific VSS data file  using info from VSS dialog box
		// let vss try to find one if not specified
		if (FAILED(hr = vssdb->Open(
			CMyComBSTR(!m_strVssDatabase.empty() ?
				(ucr::toUTF16(m_strVssDatabase) + L"\\srcsafe.ini").c_str() : NULL),
			CMyComBSTR(ucr::toUTF16(m_strVssUser).c_str()),
			CMyComBSTR(ucr::toUTF16(m_strVssPassword).c_str()))))
		{
			ShowVSSError(hr, _T(""));
		}

		// BSP - Combine the project entered on the dialog box with the file name...
		const UINT nBufferSize = 1024;
		static TCHAR buffer[nBufferSize];
		static TCHAR buffer1[nBufferSize];
		static TCHAR buffer2[nBufferSize];

		_tcscpy(buffer1, strSavePath.c_str());
		_tcscpy(buffer2, m_vssHelper.GetProjectBase().c_str());
		_tcslwr(buffer1);
		_tcslwr(buffer2);

		//make sure they both have \\ instead of /
		replace_char(buffer1, '/', '\\');

		m_vssHelper.SetProjectBase(buffer2);
		TCHAR * pbuf2 = &buffer2[2];//skip the $/
		TCHAR * pdest =  _tcsstr(buffer1, pbuf2);
		if (pdest)
		{
			size_t index  = pdest - buffer1 + 1;
			_tcscpy(buffer, buffer1);
			TCHAR * fp = &buffer[index + _tcslen(pbuf2)];
			sname = fp;

			if (sname[0] == ':')
			{
				_tcscpy(buffer2, sname.c_str());
				_tcscpy(buffer, (TCHAR*)&buffer2[2]);
				sname = buffer;
			}
		}
		String strItem = m_vssHelper.GetProjectBase() + _T("\\") + sname;
			//  BSP - ...to get the specific source safe item to be checked out
		if (FAILED(hr = vssdb->get_VSSItem(
			CMyComBSTR(ucr::toUTF16(strItem).c_str()), VARIANT_FALSE, &vssi)))
		{
			ShowVSSError(hr, strItem);
			return FALSE;
		}

		if (!m_bVssSuppressPathCheck)
		{
			// BSP - Get the working directory where VSS will put the file...
			CMyComBSTR bstrLocalSpec;
			vssi->get_LocalSpec(&bstrLocalSpec);
			// BSP - ...and compare it to the directory WinMerge is using.
			if (string_compare_nocase(ucr::toTString(bstrLocalSpec.m_str), strSavePath))
			{
				// BSP - if the directories are different, let the user confirm the CheckOut
				int iRes = LangMessageBox(IDS_VSSFOLDER_AND_FILE_NOMATCH, 
						MB_YESNO | MB_YES_TO_ALL | MB_ICONWARNING);

				if (iRes == IDNO)
				{
					m_bVssSuppressPathCheck = FALSE;
					m_CheckOutMulti = FALSE; // Reset, we don't want 100 of the same errors
					return FALSE;   // No means user has to start from begin
				}
				else if (iRes == IDYESTOALL)
					m_bVssSuppressPathCheck = TRUE; // Don't ask again with selected files
			}
		}
			// BSP - Finally! Check out the file!
		if (FAILED(hr = vssi->Checkout(
			CMyComBSTR(L""),
			CMyComBSTR(ucr::toUTF16(strSavePath).c_str()), 0)))
		{
			ShowVSSError(hr, strSavePath);
			return FALSE;
		}
	}
	break;
	case VCS_CLEARCASE:
	{
		// prompt for user choice
		CCCPrompt dlg;
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = FALSE;
			dlg.m_comments = _T("");
			dlg.m_bCheckin = FALSE;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
			m_strCCComment = static_cast<const TCHAR *>(dlg.m_comments);
			m_bCheckinVCS = dlg.m_bCheckin;
		}
		else // Dialog already shown and user selected to "checkout all"
			userChoice = IDOK;

		// process versioning system specific action
		if (userChoice == IDOK)
		{
			WaitStatusCursor waitstatus(_T(""));
			if (!spath.empty())
			{
				_chdrive(_totupper(spath[0])-'A'+1);
				_tchdir(spath.c_str());
			}

			// checkout operation
			std::string vssPath = ucr::toUTF8(GetOptionsMgr()->GetString(OPT_VSS_PATH));
			std::string sn, sn2;
			std::vector<std::string> args;
			args.push_back("checkout");
			args.push_back("-c");
			format(sn, "\"%s\"", ucr::toUTF8(m_strCCComment));
			args.push_back(sn);
			format(sn2, "\"%s\"", ucr::toUTF8(sname));
			args.push_back(sn2);
			try
			{
				ProcessHandle hVss(Process::launch(vssPath, args));
				int code = Process::wait(hVss);
				if (code != 0)
				{
					LangMessageBox(IDS_VSSERROR, MB_ICONSTOP);
					return FALSE;
				}
			}
			catch (...)
			{
				LangMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
				return FALSE;
			}
		}
	}
	break;
	}	//switch(m_nVerSys)

	return TRUE;
}

/**
 * @brief Checkin in file into ClearCase.
 */ 
void CMainFrame::CheckinToClearCase(const String &strDestinationPath)
{
	String spath, sname;
	paths_SplitFilename(strDestinationPath, &spath, &sname, 0);
	int code;
	std::vector<std::string> args;
	std::string sname_utf8;
	
	// checkin operation
	args.push_back("checkin");
	args.push_back("-nc");
	format(sname_utf8, "\"%s\"", ucr::toUTF8(sname));
	args.push_back(sname_utf8);
	std::string vssPath = ucr::toUTF8(GetOptionsMgr()->GetString(OPT_VSS_PATH));
	try
	{
		ProcessHandle hVss(Process::launch(vssPath, args));
		code = Process::wait(hVss);
		if (code != 0)
		{
			if (LangMessageBox(IDS_VSS_CHECKINERROR, MB_ICONWARNING | MB_YESNO) == IDYES)
			{
				// undo checkout operation
				args.push_back("uncheckout");
				args.push_back("-rm");
				format(sname_utf8, "\"%s\"", ucr::toUTF8(sname));
				args.push_back(sname_utf8);
				ProcessHandle hVss(Process::launch(vssPath, args));
				code = Process::wait(hVss);
				if (code != 0)
				{
					LangMessageBox(IDS_VSS_UNCOERROR, MB_ICONSTOP);
					return;
				}
			}
			return;
		}
	}
	catch (...)
	{
		LangMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
		return;
	}
}
