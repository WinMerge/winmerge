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
 * @file  Template.cpp
 *
 * @brief Implementation of the Template class.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "Template.h"
#include "AnsiConvert.h"
#include "StringTable.h"

/**
 * @brief Constructor.
 */
Template::Template(const SimpleArray<BYTE> &dataArray)
: m_filehandle(-1)
, m_tmplBuf(NULL)
, m_filelen(0)
, m_dataArray(dataArray)
{
}

/**
 * @brief Destructor.
 * Releases file resources and buffers after use.
 */
Template::~Template()
{
	delete [] m_tmplBuf;
	if (m_filehandle != -1)
		_close(m_filehandle);
}

/**
 * @brief Sets the filename of the file in the editor.
 * This function sets name of the file into whose data the template is been
 * applied.
 * @param [in] filename Name of the file in the editor.
 */
void Template::SetOriginalFilename(LPCTSTR filename)
{
	_tcscpy(m_origFilename, filename);
}

/**
 * @brief Open template file.
 * @param [in] filename Filename of the template file to open.
 * @return true if opening succeeds, false otherwise.
 */
bool Template::OpenTemplate(LPCTSTR filename)
{
	m_filehandle = _topen(filename, _O_RDONLY | _O_BINARY);
	if (m_filehandle == -1)
		return false;
	else
	{
		_tcscpy(m_filename, filename);
		return true;
	}
}

/**
 * @brief Load template data from template file.
 * @return true if reading succeeds, false otherwise.
 */
bool Template::LoadTemplateData()
{
	if (m_filehandle == -1)
		return false;

	m_filelen = _filelength(m_filehandle);
	if (m_filelen == 0)
		return false;

	m_tmplBuf = new char[m_filelen + 1];
	if (m_tmplBuf)
	{
		memset(m_tmplBuf, 0, m_filelen + 1);
		int ret = _read(m_filehandle, m_tmplBuf, m_filelen);
		if (ret == -1)
		{
			delete [] m_tmplBuf;
			return false;
		}
	}
	return true;
}

/**
 * @brief Create template info array.
 * Format template info text.
 * @param [in] curByte Current byte index in the open hex file.
 */
void Template::CreateTemplateArray(int curByte)
{
	// Print filename and current offset to output.
	m_resultString += GetLangString(IDS_TPL_FILENAME);
	m_resultString += _T(" ");
	m_resultString += m_origFilename;
	m_resultString += _T("\r\n");
	m_resultString += GetLangString(IDS_TPL_TEMPLATE_FILE);
	m_resultString += _T(" ");
	m_resultString += m_filename;
	m_resultString += _T("\r\n");
	m_resultString += GetLangString(IDS_TPL_APPLIED_AT);
	m_resultString += _T(" ");
	TCHAR buf[16];
	_stprintf(buf, _T("%d\r\n\r\n"), curByte);
	m_resultString += buf;
}

//-------------------------------------------------------------------
// Applies the template code in pcTpl of length tpl_len on the current file
// from the current offset and outputs the result to the ResultArray.
void Template::ApplyTemplate(HexEditorWindow::BYTE_ENDIAN binaryMode, int curByte)
{
	// Use source code in pcTpl to decipher data in file.
	int index = 0;
	int fpos = curByte;
	// While there is still code left...
	while (index < m_filelen)
	{
		// Read in the var type.
		if (ignore_non_code(m_tmplBuf, m_filelen, index))
		{
			// index now points to first code character.
			// Get var type.
			TCHAR cmd[TPL_TYPE_MAXLEN]; // This holds the variable type, like byte or word.
			if (read_tpl_token(m_tmplBuf, m_filelen, index, cmd))
			{
				// cmd holds 0-terminated var type, index set to position of first space-
				// character after the type. Now test if valid type was given.
				//---- type BYTE ---------------------------------
				if (_tcscmp(cmd, _T("BYTE")) == 0 || _tcscmp(cmd, _T("char")) == 0)
				{
					// This is a byte/char.
					if (ignore_non_code(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a byte?
						if (m_dataArray.GetLength() - fpos >= 1)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							// index is set to a non-space character by last call to ignore_non_code.
							// Therefore the variable name can be read into buffer name.
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type and name to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							m_resultString += name;
							// Write value to output.
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							if (m_dataArray[fpos] != 0)
							{
								_stprintf(buf, GetLangString(IDS_TPL_FMT_BYTE_0),
									(int) (signed char) m_dataArray[fpos], m_dataArray[fpos],
									m_dataArray[fpos], m_dataArray[fpos]);
							}
							else
							{
								_stprintf(buf, GetLangString(IDS_TPL_FMT_BYTE),
									(int) (signed char) m_dataArray[fpos], m_dataArray[fpos],
									m_dataArray[fpos]);
							}
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							// Increase pointer for next variable.
							fpos += 1;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_BYTE);
							return;
						}
					}
					else
					{
						// No non-spaces after variable type up to end of array, so
						// no space for variable name.
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return;
					}
				}
				else if (_tcscmp(cmd, _T("WORD")) == 0 || _tcscmp(cmd, _T("short")) == 0)
				{
					// This is a word.
					if (ignore_non_code(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a word?
						if (m_dataArray.GetLength() - fpos >= 2)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							WORD wd = 0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								wd = *((WORD*)&m_dataArray[fpos]);
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 2; i++)
								{
									((TCHAR*)&wd)[i] = m_dataArray[fpos + 1 - i];
								}
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							_stprintf(buf, GetLangString(IDS_TPL_FMT_WORD),
								(int) (signed short) wd, wd, wd);
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 2;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_WORD);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("DWORD")) == 0 || _tcscmp(cmd, _T("int")) == 0 ||
					_tcscmp(cmd, _T("long")) == 0 || _tcscmp(cmd, _T("LONG")) == 0)
				{
					// This is a longword.
					if (ignore_non_code(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a longword?
						if (m_dataArray.GetLength() - fpos >= 4)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							DWORD dw = 0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								dw = *((DWORD*)&m_dataArray[fpos]);
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 4; i++)
									((char*)&dw)[i] = m_dataArray[fpos + 3 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							_stprintf(buf, GetLangString(IDS_TPL_FTM_DWORD),
								(signed long) dw, (unsigned long) dw, dw);
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 4;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_DWORD);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("float")) == 0)
				{
					// This is a float.
					if (ignore_non_code(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a float?
						if (m_dataArray.GetLength() - fpos >= 4)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							float f = 0.0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								f = *((float*)&m_dataArray[fpos]);
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 4; i++)
									((char*)&f)[i] = m_dataArray[fpos + 3 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							_stprintf(buf, GetLangString(IDS_TPL_FMT_FLOAT),
								f, (unsigned long) *((int*) &f));
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 4;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_FLOAT);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("double")) == 0)
				{
					// This is a double.
					if (ignore_non_code(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a double?
						if (m_dataArray.GetLength() - fpos >= 8)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							double d = 0.0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								d = *((double*)&m_dataArray[fpos]);
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 8; i++)
									((char*)&d)[i] = m_dataArray[fpos + 7 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							_stprintf(buf, GetLangString(IDS_TPL_FMT_DOUBLE), d);
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 8;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_DOUBLE);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else
				{
					m_resultString += GetLangString(IDS_TPL_ERR_UNK_TYPE);
					m_resultString += _T(" \"");
					m_resultString += cmd;
					m_resultString += _T("\"");
					return;
				}
			}
			else
			{
				// After the type there is only the array end. Therefore
				// no space for a variable name.
				m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
				return;
			}
		}
		else
		{
			// No non-spaces up to the end of the array.
			break;
		}
	}
	// No more code left in m_tmplBuf.
	TCHAR buf[128];
	_stprintf(buf, GetLangString(IDS_TPL_LENGTH), fpos - curByte);
	m_resultString += _T("\r\n");
	m_resultString += buf;
	m_resultString += _T("\r\n");
}

LPCTSTR Template::GetResult()
{
	return m_resultString.c_str();
}

//-------------------------------------------------------------------
// This will set index to the position of the next non-space-character.
// Return is FALSE if there are no non-spaces left up to the end of the array.
bool Template::ignore_non_code(char* pcTpl, int tpl_len, int& index)
{
	while (index < tpl_len)
	{
		// If code found, return.
		switch(pcTpl[index])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		default:
			return true;
		}
		index++;
	}
	return false;
}

//-------------------------------------------------------------------
// Writes all non-space characters from index to dest and closes dest
// with a zero-byte. index is set to position of the first space-
// character. Return is false if there is only the array end after the
// keyword. In that case index is set to tpl_len.
bool Template::read_tpl_token(char *pcTpl, int tpl_len, int &index, TCHAR *dest)
{
	int i = 0;
	while (index + i < tpl_len)
	{
		switch (pcTpl[index + i])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			dest[i] = '\0';
			index += i;
			return true;

		default:
			dest[i] = pcTpl[index + i];
		}
		i++;
	}
	dest[i] = '\0';
	index += i;
	return false;
}
