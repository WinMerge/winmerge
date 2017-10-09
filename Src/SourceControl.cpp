/** 
 * @file  SourceControl.cpp
 *
 * @brief Implementation file for some source control-related functions.
 */

#include "StdAfx.h"
#include "SourceControl.h"
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
#include "MergeApp.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "RegKey.h"
#include "paths.h"
#include "VssPromptDlg.h"
#include "CCPromptDlg.h"
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

SourceControl::SourceControl() : 
  m_CheckOutMulti(false)
, m_bVCProjSync(false)
, m_bVssSuppressPathCheck(false)
, m_bCheckinVCS(false)
{
}

void
SourceControl::InitializeSourceControlMembers()
{
	m_vssHelper.SetProjectBase(GetOptionsMgr()->GetString(OPT_VSS_PROJECT));
	m_strVssUser = GetOptionsMgr()->GetString(OPT_VSS_USER);
	m_strVssDatabase = GetOptionsMgr()->GetString(OPT_VSS_DATABASE);
	m_strCCComment = _T("");
	m_bCheckinVCS = false;

	String vssPath = GetOptionsMgr()->GetString(OPT_VSS_PATH);
	if (vssPath.empty())
	{
		CRegKeyEx reg;
		if (reg.QueryRegMachine(_T("SOFTWARE\\Microsoft\\SourceSafe")))
		{
			TCHAR temp[_MAX_PATH] = {0};
			reg.ReadChars(_T("SCCServerPath"), temp, _MAX_PATH, _T(""));
			vssPath = paths::ConcatPath(paths::GetPathOnly(temp), _T("Ss.exe"));
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
bool SourceControl::SaveToVersionControl(const String& strSavePath)
{
	String spath, sname;
	paths::SplitFilename(strSavePath, &spath, &sname, NULL);
	int userChoice = 0;
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
		CVssPromptDlg dlg;
		dlg.m_strMessage = strutils::format_string1(_("Save changes to %1?"), strSavePath);
		dlg.m_strProject = m_vssHelper.GetProjectBase();
		dlg.m_strUser = m_strVssUser;          // BSP - Add VSS user name to dialog box
		dlg.m_strPassword = m_strVssPassword;

		// Dialog not suppressed - show it and allow user to select "checkout all"
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = false;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
		}
		else // Dialog already shown and user selected to "checkout all"
			userChoice = IDOK;

		// process versioning system specific action
		if (userChoice == IDOK)
		{
			CWaitCursor waitstatus;
			m_vssHelper.SetProjectBase(dlg.m_strProject);
			GetOptionsMgr()->SaveOption(OPT_VSS_PROJECT, m_vssHelper.GetProjectBase());
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
					AppErrorMessageBox(_("Versioning System returned an error while attempting to check out the file.  Unable to continue..."));
					return false;
				}
			}
			catch (...)
			{
				AppErrorMessageBox(_("Error executing versioning system command."));
				return false;
			}
		}
		else
			return false; // User selected cancel
	}
	break;
	case VCS_VSS5: // CVisual SourceSafe 5.0+ (COM)
	{
		// prompt for user choice
		CVssPromptDlg dlg;
		CRegKeyEx reg;

		dlg.m_strMessage = strutils::format_string1(_("Save changes to %1?"), strSavePath);
		dlg.m_strProject = m_vssHelper.GetProjectBase();
		dlg.m_strUser = m_strVssUser;          // BSP - Add VSS user name to dialog box
		dlg.m_strPassword = m_strVssPassword;
		dlg.m_strSelectedDatabase = m_strVssDatabase;
		dlg.m_bVCProjSync = true;

		// Dialog not suppressed - show it and allow user to select "checkout all"
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = false;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
			if (userChoice != IDOK)
				return false; // User selected cancel
		}
		// process versioning system specific action
		CWaitCursor waitstatus;
		m_vssHelper.SetProjectBase(dlg.m_strProject);
		m_strVssUser = dlg.m_strUser;
		m_strVssPassword = dlg.m_strPassword;
		m_strVssDatabase = dlg.m_strSelectedDatabase;
		m_bVCProjSync = dlg.m_bVCProjSync;					

		GetOptionsMgr()->SaveOption(OPT_VSS_DATABASE, m_strVssDatabase);
		GetOptionsMgr()->SaveOption(OPT_VSS_PROJECT, m_vssHelper.GetProjectBase());
		GetOptionsMgr()->SaveOption(OPT_VSS_USER, m_strVssUser);

		HRESULT hr;
		CMyComPtr<IVSSDatabase> vssdb;
		CMyComPtr<IVSSItems> vssis;
		CMyComPtr<IVSSItem> vssi;
			
		// BSP - Create the COM interface pointer to VSS
		if (FAILED(hr = vssdb.CoCreateInstance(CLSID_VSSDatabase, IID_IVSSDatabase)))
		{
			ShowVSSError(hr, _T(""));
			return false;
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

		_tcscpy_safe(buffer1, strSavePath.c_str());
		_tcscpy_safe(buffer2, m_vssHelper.GetProjectBase().c_str());
		_tcslwr_s(buffer1, nBufferSize);
		_tcslwr_s(buffer2, nBufferSize);

		//make sure they both have \\ instead of /
		_tcscpy_safe(buffer1, paths::ToWindowsPath(buffer1).c_str());

		m_vssHelper.SetProjectBase(buffer2);
		TCHAR * pbuf2 = &buffer2[2];//skip the $/
		TCHAR * pdest =  _tcsstr(buffer1, pbuf2);
		if (pdest)
		{
			size_t index  = pdest - buffer1 + 1;
			_tcscpy_safe(buffer, buffer1);
			TCHAR * fp = &buffer[index + _tcslen(pbuf2)];
			sname = fp;

			if (sname[0] == ':')
			{
				_tcscpy_safe(buffer2, sname.c_str());
				_tcscpy_safe(buffer, (TCHAR*)&buffer2[2]);
				sname = buffer;
			}
		}
		String strItem = m_vssHelper.GetProjectBase() + _T("\\") + sname;
			//  BSP - ...to get the specific source safe item to be checked out
		if (FAILED(hr = vssdb->get_VSSItem(
			CMyComBSTR(ucr::toUTF16(strItem).c_str()), VARIANT_FALSE, &vssi)))
		{
			ShowVSSError(hr, strItem);
			return false;
		}

		if (!m_bVssSuppressPathCheck)
		{
			// BSP - Get the working directory where VSS will put the file...
			CMyComBSTR bstrLocalSpec;
			vssi->get_LocalSpec(&bstrLocalSpec);
			// BSP - ...and compare it to the directory WinMerge is using.
			if (strutils::compare_nocase(ucr::toTString(bstrLocalSpec.m_str), strSavePath))
			{
				// BSP - if the directories are different, let the user confirm the CheckOut
				int iRes = AppMsgBox::warning(_("The VSS Working Folder and the location of the current file do not match. Continue?"), 
						AppMsgBox::YES | AppMsgBox::NO | AppMsgBox::YES_TO_ALL);

				if (iRes == AppMsgBox::NO)
				{
					m_bVssSuppressPathCheck = false;
					m_CheckOutMulti = false; // Reset, we don't want 100 of the same errors
					return false;   // No means user has to start from begin
				}
				else if (iRes == AppMsgBox::YES_TO_ALL)
					m_bVssSuppressPathCheck = true; // Don't ask again with selected files
			}
		}
			// BSP - Finally! Check out the file!
		if (FAILED(hr = vssi->Checkout(
			CMyComBSTR(L""),
			CMyComBSTR(ucr::toUTF16(strSavePath).c_str()), 0)))
		{
			ShowVSSError(hr, strSavePath);
			return false;
		}
	}
	break;
	case VCS_CLEARCASE:
	{
		// prompt for user choice
		CCCPromptDlg dlg;
		if (!m_CheckOutMulti)
		{
			dlg.m_bMultiCheckouts = false;
			dlg.m_comments = _T("");
			dlg.m_bCheckin = false;
			userChoice = dlg.DoModal();
			m_CheckOutMulti = dlg.m_bMultiCheckouts;
			m_strCCComment = dlg.m_comments;
			m_bCheckinVCS = dlg.m_bCheckin;
		}
		else // Dialog already shown and user selected to "checkout all"
			userChoice = IDOK;

		// process versioning system specific action
		if (userChoice == IDOK)
		{
			CWaitCursor waitstatus;
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
					AppErrorMessageBox(_("Versioning System returned an error while attempting to check out the file.  Unable to continue..."));
					return false;
				}
			}
			catch (...)
			{
				AppErrorMessageBox(_("Error executing versioning system command."));
				return false;
			}
		}
	}
	break;
	}	//switch(m_nVerSys)

	return true;
}

/**
 * @brief Checkin in file into ClearCase.
 */ 
void SourceControl::CheckinToClearCase(const String &strDestinationPath)
{
	String spath, sname;
	paths::SplitFilename(strDestinationPath, &spath, &sname, 0);
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
		ProcessHandle hVss1(Process::launch(vssPath, args));
		int code = Process::wait(hVss1);
		if (code != 0)
		{
			if (AppMsgBox::warning(_("Versioning System returned an error while attempting to check in the file.\n Please, check config spec of used view.\n Undo checkout operation?"),
				    AppMsgBox::YES | AppMsgBox::NO) == AppMsgBox::YES)
			{
				// undo checkout operation
				args.push_back("uncheckout");
				args.push_back("-rm");
				format(sname_utf8, "\"%s\"", ucr::toUTF8(sname));
				args.push_back(sname_utf8);
				ProcessHandle hVss2(Process::launch(vssPath, args));
				code = Process::wait(hVss2);
				if (code != 0)
				{
					AppErrorMessageBox(_("Versioning System returned an error while attempting to undo checkout the file.\n Please, check config spec of used view. "));
					return;
				}
			}
			return;
		}
	}
	catch (...)
	{
		AppErrorMessageBox(_("Error executing versioning system command."));
		return;
	}
}
