/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  Template.h
 *
 * @brief Declaration file for Template class.
 *
 */
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include "Simparr.h"
#include "hexwnd.h"  // For HexEditorWindow::BYTE_ENDIAN

/** @brief Maximum length of the type in template. */
const int TPL_TYPE_MAXLEN = 16;
/** @brief Maximum length of the variable name in template. */
const int TPL_NAME_MAXLEN = 128;

/**
 * @brief A class for applying template for the binary data.
 */
class Template
{
public:
	Template(const SimpleArray<BYTE> &);
	~Template();

	void SetOriginalFilename(LPCTSTR filename);
	bool OpenTemplate(LPCTSTR filename);
	bool LoadTemplateData();
	void CreateTemplateArray(int curByte);
	void ApplyTemplate(HexEditorWindow::BYTE_ENDIAN binaryMode, int curByte);
	LPCTSTR GetResult();

protected:
	bool ignore_non_code(char *pcTpl, int tpl_len, int &index);
	bool read_tpl_token(char *pcTpl, int tpl_len, int &index, TCHAR *name);

private:
	int m_filehandle; /**< File handle to template file. */
	TCHAR m_origFilename[MAX_PATH]; /**< Filename of the file in the editor. */
	TCHAR m_filename[MAX_PATH]; /**< Template file name. */
	int m_filelen; /**< Template file size. */
	char *m_tmplBuf; /**< Template file data buffer (read from file). */
	String m_resultString; /**< Resulting string for applied template. */
	const SimpleArray<BYTE> &m_dataArray; /**< Original data. */
};

#endif // _TEMPLATE_H_
