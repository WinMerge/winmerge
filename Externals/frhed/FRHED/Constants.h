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
 * @file Constants.h
 *
 * @brief Frhed constants, URLs, paths etc.
 *
 */

/** @brief Application name used in the GUI. */
const TCHAR ApplicationName[] = _T("Frhed");

/** @brief Frhed homepage URL. */
const TCHAR FrhedHomepageURL[] = _T("http://frhed.sourceforge.net/");

/** @brief Filename of the contributors list. */
const TCHAR ContributorsList[] = _T("Docs\\Contributors.txt");

/** @brief HtmlHelp file to open from Frhed program folder. */
const TCHAR HtmlHelpFile[] = _T("docs\\frhed.chm");

/** @brief Registry key for Frhed. */
const TCHAR OptionsRegistryPath[] = _T("Software\\Frhed");

/** @brief Registry key for Frhed settings. */
const TCHAR OptionsRegistrySettingsPath[] = _T("Software\\Frhed\\Settings");

/** @brief Subfolder where language files (PO files) are. */
const WCHAR LangFileSubFolder[] = L"Languages";
