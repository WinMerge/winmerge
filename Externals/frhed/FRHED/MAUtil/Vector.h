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

/** \file Vector.h
 *
 * \brief Generic, dynamic vector container behaving like a subset of std::vector.
 *
 * \author Patrick Broman and Niklas Nummelin
 *
 */

#ifndef _SE_MSAB_MAUTIL_VECTOR_H_
#define _SE_MSAB_MAUTIL_VECTOR_H_

#ifdef MOSYNCDEBUG
#include <maassert.h>
#endif

//#define MAUTIL_VECTOR_DEBUGGING

/** \def MAUTIL_VECTOR_LOG
* A debugging macro.
*/
#ifdef MAUTIL_VECTOR_DEBUGGING
#ifndef MOSYNCDEBUG
#define MOSYNCDEBUG
#endif
#ifdef MAPIP
#define MAUTIL_VECTOR_LOG lprintfln
#else
#define MAUTIL_VECTOR_LOG(...)
#endif
#else
#define MAUTIL_VECTOR_LOG(...)
#endif

/** \def Vector_each(type,itr,set)
* A macro for iterating over a vector.
*
* Example: <tt>Vector_each(class MyType, itr, mySet) { myFunc(*itr); }</tt>
*/
#define Vector_each(type,itr,v)\
	for(MAUtil::Vector<type>::iterator itr = (v).begin(); itr != (v).end(); itr++)

// For use inside templates.
#define templateVector_each(type,itr,v)\
	for(typename MAUtil::Vector<type>::iterator itr = (v).begin(); itr != (v).end(); itr++)

/** \def Vector_each_const(type,itr,set)
* A macro for iterating over a const vector.
*/
#define Vector_each_const(type,itr,v)\
	for(MAUtil::Vector<type>::const_iterator itr = (v).begin(); itr != (v).end(); itr++)

#define templateVector_each_const(type,itr,v)\
	for(typename MAUtil::Vector<type>::const_iterator itr = (v).begin(); itr != (v).end(); itr++)

namespace MAUtil {

#if defined(MAUTIL_VECTOR_DEBUGGING)
	extern int nV;
#endif

	/** \brief A generic, dynamic, random-access container.
	*
	* Performance characteristics are as follows:
	*
	* Access to any element by index is very fast (constant time).
	*
	* add(), insert() and remove() at the end of the vector are fast
	* (constant time amortized over linear time).
	*
	* insert() and remove() anywhere else are slow, (linear time).
	*
	* \note All operations that modify the vector invalidates all iterators and references to
	* its elements. Never keep references, iterators or pointers to elements.
	* Indices may sometimes be used instead,
	* but even indices are invalidated by insert() and remove(), as well as
	* shrinking resize().
	*/
	template<typename Type> class Vector {

	public:
		/// Defines a typesafe iterator for the template instance.
		typedef Type* iterator;
		typedef const Type* const_iterator;

		/** \brief Constructs the Vector and sets its capacity to \a initialCapacity.
		 *  \param initialCapacity The initial capacity of the Vector.
		 */
		Vector(int initialCapacity=4) {
#if defined(MAUTIL_VECTOR_DEBUGGING)
			nV++;
#endif
			MAUTIL_VECTOR_LOG("Vector<%lu>(0x%08X, %i)", sizeof(Type), (int)this, initialCapacity);
			mData = new Type[initialCapacity];
			MAUTIL_VECTOR_LOG("Vector 2 0x%08X", mData);
			mCapacity = initialCapacity;
			MAUTIL_VECTOR_LOG("Vector 4");
			mSize = 0;
			MAUTIL_VECTOR_LOG("Vector done");
		}

		Vector(const Type* array, int _size) {
			mCapacity = _size;
			mData = new Type[mCapacity];
			mSize = 0;
			add(array, _size);
		}

		/**
		* Copies the \a other vector.
		*/
		Vector(const Vector& other) {
#if defined(MAUTIL_VECTOR_DEBUGGING)
			nV++;
#endif
			MAUTIL_VECTOR_LOG("oVector<%lu>(0x%08X, %i)", sizeof(Type), (uint)this, other.mCapacity);
			mCapacity = other.mCapacity;
			mSize = other.mSize;
			mData = new Type[mCapacity];
			for(int i=0; i<mSize; i++) {
				mData[i] = other.mData[i];
			}
			MAUTIL_VECTOR_LOG("oVector done");
		}

		/// Destructor
		~Vector() {
			delete[] mData;
#if defined(MAUTIL_VECTOR_DEBUGGING)
			nV--;
#endif
		}

		/**
		* Copies the \a other vector.
		* \returns A reference to this vector.
		*/
		Vector& operator=(const Vector& other) {
			delete[] mData;
			mCapacity = other.mCapacity;
			mSize = other.mSize;
			mData = new Type[mCapacity];
			for(int i=0; i<mSize; i++) {
				mData[i] = other.mData[i];
			}
			return *this;
		}

		/** \brief Adds an element to the end of the Vector.
		 *  \param val The element to be added.
		 */
		void add(const Type& val) {
			if(mSize >= mCapacity-1) {
				if(mCapacity != 0)
					reserve(mCapacity*2);
				else
					reserve(4);
			}
			mData[mSize++] = val;
		}

		/** \brief Adds several elements to the end of the Vector.
		 *  \param ptr A pointer to the elements.
		 *  \param num The number of elements.
		 */
		void add(const Type* ptr, int num) {
			int neededCapacity = mSize + num;
			if(mCapacity < neededCapacity) {
				int newCapacity = mCapacity;
				do {
					newCapacity *= 2;
				} while(newCapacity < neededCapacity);
				reserve(newCapacity);
			}
			for(int i=0; i<num; i++) {
				mData[mSize++] = *(ptr++);
			}
		}

		/** \brief Removes the element pointed to by iterator \a i.
		 * \param i An iterator pointing to the element that should be removed.
		 */
		void remove(iterator i) {
			iterator e = end();

#ifdef MOSYNCDEBUG
			ASSERT_MSG(i>=begin() && i<=e, "Remove iterator out of bounds");
#endif

			while(i != (e-1)) {
				*i = *(i+1);
				i++;
			}

			//mSize--;
			resize(mSize-1);
		}

		/** \brief Removes the element at \a index.
		 *  \param index The index of the element that should be removed.
		 */
		void remove(int index) {
			remove(mData + index);
		}

		/** \brief Removes several elements, starting at \a index.
		* \param index The index of the element that should be removed.
		* \param number The number of elements to remove.
		*/
		void remove(int index, int number) {
#ifdef MOSYNCDEBUG
			ASSERT_MSG(index >= 0 && index < mSize, "Remove index out of bounds");
			ASSERT_MSG(number > 0 && (index + number) < mSize, "Remove number out of bounds");
#endif
			int base = index;
			int next = index + number;
			while(next < mSize) {
				mData[base] = mData[next];
				base++;
				next++;
			}
			resize(mSize - number);
		}

		/** \brief Inserts the element at \a index, moving all existing elements beginning at 'index' one step forward.
		 *  \param index The index of the newly inserted element.
		 *  \param t The element itself.
		 */
		void insert(int index, Type t) {
			resize(mSize+1);
			iterator e = end()-1;
			iterator i = &(begin()[index]);

#ifdef MOSYNCDEBUG
			ASSERT_MSG(i>=begin() && i<=e, "Insert iterator out of bounds");
#endif

			while(e != i)
			{
				*(e) = *(e-1);
				e--;
			}
			*i = t;
		}

		/** \brief Returns the number of elements.
		 *  \return Returns the number of elements currently in the Vector.
		 *  \see capacity
		 */

		int size() const {
			return mSize;
		}

		/** \brief Resizes the Vector to contain \a size elements.
		 *  \param newSize The desired size of the Vector.
		 */
		void resize(int newSize) {
#ifdef MOSYNCDEBUG
			ASSERT_MSG(newSize>=0, "Resize negative");
#endif

			MAUTIL_VECTOR_LOG("resize 0x%08X %i", (uint)this, newSize);
			reserve(newSize);
			MAUTIL_VECTOR_LOG("resize 2");

			for(int i = newSize; i < mSize; i++) {
				mData[i] = Type();
			}

			mSize = newSize;
			MAUTIL_VECTOR_LOG("resize done");
		}

		/** \brief Reserves space in the Vector.
		 *  \param newCapacity The desired capacity of the Vector.
		 *  \note If \a newCapacity is less than the current capacity of the Vector, nothing will happen.
		 */
		void reserve(int newCapacity) {
			MAUTIL_VECTOR_LOG("reserve 0x%08X %i", (int)this, newCapacity);
			if(newCapacity <= mCapacity)
				return;
			MAUTIL_VECTOR_LOG("reserve 2");
			Type* newData = new Type[newCapacity];
			MAUTIL_VECTOR_LOG("reserve 4");
			for(int i=0; i < mSize; i++) {
				newData[i] = mData[i];
			}
			MAUTIL_VECTOR_LOG("reserve 5");
			mCapacity = newCapacity;
			delete[] mData;
			MAUTIL_VECTOR_LOG("reserve 6");
			mData = newData;
			MAUTIL_VECTOR_LOG("reserve done");
		}

		/** \brief Clears the Vector, setting its size to 0 but not altering its capacity
		 */
		void clear() {
			//mSize = 0;
			resize(0);
		}

		/** \brief Returns true iff the Vector is empty().
		*/
		bool empty() const {
			return mSize == 0;
		}

		/** \brief Returns the Vector's current capacity.
		 * \see size
		 */
		int capacity() const {
			return mCapacity;
		}

		/** \brief Returns an iterator pointing to the first element of the Vector.
		 */

		iterator begin() {
			return mData;
		}

		/** \brief Returns an iterator pointing beyond the last element of the Vector.
		 */

		iterator end() {
			return mData + mSize;
		}

		/** \brief Returns an iterator pointing to the first element of the Vector.
		 */

		const_iterator begin() const {
			return mData;
		}

		/** \brief Returns an iterator pointing beyond the last element of the Vector.
		 */

		const_iterator end() const {
			return mData + mSize;
		}

		/** \brief Returns a reference to the element at \a index. No range-checking is done.
		 *  \param index The index of the element to be returned.
		 */
		Type& operator[](int index) {
#ifdef MOSYNCDEBUG
			MAASSERT(index < mCapacity && index >= 0);
#endif
			return mData[index];
		}

		/** \brief Returns a const reference to the element at \a index. No range-checking is done.
		 *  \param index The index of the element to be returned.
		 */
		const Type& operator[](int index) const {
#ifdef MOSYNCDEBUG
			MAASSERT(index < mCapacity && index >= 0);
#endif
			return mData[index];
		}

		/** \brief Returns a const pointer to the Vector's storage array.
		 *  \note The pointer becomes invalid when the Vector is changed.
		 */
		const Type* pointer() const {
			return mData;
		}

		/** \brief Returns a pointer to the Vector's storage array.
		 *  \note The pointer becomes invalid when the Vector is changed.
		 */
		Type* pointer() {
			return mData;
		}

	protected:
		int mSize;
		int mCapacity;
		Type* mData;
	};

}

#endif	//_SE_MSAB_MAUTIL_VECTOR_H_
