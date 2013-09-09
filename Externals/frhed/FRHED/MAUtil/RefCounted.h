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
*/

/** \file RefCounted.h
* \brief Mix-in class for reference counting.
*/

#ifndef _SE_MSAB_MAUTIL_REFCOUNTED_H_
#define _SE_MSAB_MAUTIL_REFCOUNTED_H_

namespace MAUtil {

	/// A base class for reference counting.
	class RefCounted {
	public:
		/// Constructor, initializes the initial reference count to startCount.
		RefCounted(int startCount=1);
		/// Increments the reference count by one.
		void addRef();
		/// Decrements the reference count by one.
		void release();
		/// Returns the current reference count.
		int getRefCount();

		virtual ~RefCounted() {}
	private:
		/// A variable that keeps track of the reference count.
		int mRefCount;
	};

}

#endif
