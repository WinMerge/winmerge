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
#include "ids.h"
#include "hexwnd.h"

//Members
CDropSource::CDropSource()
: m_cRefCount(0)
{
}

CDropSource::~CDropSource()
{
	assert(m_cRefCount == 0);
}

//IUnknown members
STDMETHODIMP CDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IDropSource)
	{
		*ppvObject = static_cast<IDropSource *>(this);
		AddRef();
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropSource::AddRef()
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropSource::Release()
{
	return --m_cRefCount;
}

//IDropSource members
STDMETHODIMP CDropSource::GiveFeedback(DWORD)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	//Cancel when escape is pressed
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	//Complete if there are no mouse buttons pressed
	if(!(grfKeyState & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
		return DRAGDROP_S_DROP;
	//Otherwise continue the drag operation
	return S_OK;
}
