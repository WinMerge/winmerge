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
 * @file  FindCtxt.h
 *
 * @brief Declaration of the Find context class.
 *
 */
#ifndef _FIND_CTXT_H_
#define _FIND_CTXT_H_

class FindCtxt
{
public:
	enum { MAX_TEXT_LEN = 32 * 1024 };

	FindCtxt();
	~FindCtxt();
	
	void SetText(LPCSTR text);
	bool HasText() const { return m_pText != NULL; }
	void ClearText();
	LPCSTR GetText() const { return m_pText; }

	bool m_bMatchCase; /**< Do we match case? */
	int m_iDirection; /**< Direction of the find? */
	int m_bUnicode; /**< Find Unicode data? */

private:
	CHAR *m_pText; /**< Text to find. */
};

#endif // _FIND_CTXT_H_
