/* Copyright (C) 2009 Mobile Sorcery AB

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

This is an edited version of code obtained from:
https://github.com/MoSync/MoSync/blob/master/libs/MAUtil

Last change: 2013-02-24 by Jochen Neubeck
*/

#include "precomp.h"
#include "RefCounted.h"

namespace MAUtil {

	RefCounted::RefCounted(int startCount) : mRefCount(startCount) {}
	
	void RefCounted::addRef() {
		mRefCount++;
	}

	void RefCounted::release() {
		mRefCount--;
		if(mRefCount == 0) {
			delete this;
		}
	}

	int RefCounted::getRefCount() {
		return mRefCount;
	}
}
