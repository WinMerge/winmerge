////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  VSSHelper.h
 *
 * @brief Declaration file for VSSHelper
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _VSSHELPER_H_
#define _VSSHELPER_H_

/**
 * @brief Helper class for using VSS integration.
 */
class VSSHelper
{
public:
	CString GetProjectBase();
	BOOL SetProjectBase(CString strPath);

	BOOL ReLinkVCProj(CString strSavePath,CString * psError);
	void GetFullVSSPath(CString strSavePath, BOOL & bVCProj);

protected:
	BOOL GetWordFromFile(HANDLE pfile, TCHAR * buffer, DWORD dwBufferSize, TCHAR * charset = NULL);
	int GetWordFromBuffer(TCHAR * inBuffer, DWORD dwInBufferSize,
		TCHAR * outBuffer, DWORD dwOutBufferSize, TCHAR * charset = NULL);
	BOOL GetVCProjName(HANDLE hFile, HANDLE tFile);
	BOOL GetSLNProjUniqueName(HANDLE hFile, HANDLE tFile, TCHAR * buf);
	BOOL GetSLNProjName(HANDLE hFile, HANDLE tFile, TCHAR * buf);

private:
	CString m_strVssProjectBase;
	CString m_strVssProjectFull;

};

#endif // _VSSHELPER_H_
