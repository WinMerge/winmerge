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
#include "precomp.h"

PList::PList()
: Count(0)
{
}

PList::~PList()
{
	DeleteContents();
}

void PList::DeleteContents()
{
	while (Flink != this)
		delete static_cast<PNode *>(Flink);
	Count = 0;
}

void PList::AddTail(PNode *p)
{
	p->Flink = Blink->Flink;
	p->Blink = Blink;
	Blink->Flink = p;
	Blink = p;
	++Count;
}

/**
 * @brief Simple wrapper around _vsntprintf.
 * @note The capacity is read from the passed-in buffer.
 */
int PFormat::Format(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	unsigned count = capacity - 1;
	int n = _vsntprintf(buffer, count, fmt, args);
	buffer[count] = _T('\0');
	va_end(args);
	return n >= 0 ? n : count;
}

void TRACE(const char* pszFormat,...)
{
#ifdef _DEBUG
	static bool bFirstTime = true;
	if (FILE *fp = fopen("Frhed.log", bFirstTime ? "w" : "a"))
	{
		bFirstTime = false;
		va_list args;
		va_start(args, pszFormat);
		vfprintf(fp, pszFormat, args);
		fclose(fp);
	}
#endif //_DEBUG
}
