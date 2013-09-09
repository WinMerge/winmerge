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
#include "String.h"

namespace MAUtil {

#if defined(MAUTIL_VECTOR_DEBUGGING)
	int nV = 0;
#endif

	template<class Tchar> StringData<Tchar>::StringData(const Tchar* text, int len)
		: Vector<Tchar>(len + 1), RefCounted(1)
	{
		this->resize(len);
		this->mData[len] = 0;
		memcpy(this->mData, text, len * sizeof(Tchar));
	}


	template<class Tchar> StringData<Tchar>::StringData(const Tchar* text)
		: Vector<Tchar>(tstrlen(text) + 1), RefCounted(1)
	{
		int len = this->capacity() - 1;
		this->resize(len);
		memcpy(this->mData, text, this->capacity() * sizeof(Tchar));
	}


	template<class Tchar> StringData<Tchar>::StringData(int len)
		: Vector<Tchar>(len+1), RefCounted(1)
	{
		this->resize(len);
		this->mData[len] = 0;
	}



	template<class Tchar> StringData<Tchar>::StringData(const StringData& other)
		: Vector<Tchar>(other.mSize+1), RefCounted(1)
	{
		this->mSize = other.mSize;
		memcpy(this->mData, other.mData, (this->mSize+1) * sizeof(Tchar));
	}

#ifdef HAVE_EMPTY_STRING
	const BasicString* BasicString<Tchar>::EMPTY_STRING = NULL;
#endif

	template<class Tchar> static StringData<Tchar>* getEmptyData();

	static StringData<char> ed_char(0);
	template<>
	StringData<char>* getEmptyData<char>()
	{
		ed_char.addRef();
		return &ed_char;
	}

	static StringData<wchar_t> ed_wchar(0);
	template<>
	StringData<wchar_t>* getEmptyData<wchar_t>()
	{
		ed_wchar.addRef();
		return &ed_wchar;
	}

	template<class Tchar> BasicString<Tchar>::BasicString() {
		sd = getEmptyData<Tchar>();
	}

	template<class Tchar> BasicString<Tchar>::BasicString(int aCapacity) {
		sd = new StringData<Tchar>(aCapacity);
		resize(0);
	}

	template<class Tchar> void BasicString<Tchar>::allocStringData(const Tchar* text, int len) {
		if(len < 0) {
			maPanic(0, "BasicString(const Tchar* text, int len), passed a negative length.");
		}

		if(text == NULL || *text == 0 || len == 0) {
			sd = getEmptyData<Tchar>();
		} else {
			sd = new StringData<Tchar>(text, len);
			MAASSERT(sd);
		}		
	}

	template<class Tchar> BasicString<Tchar>::BasicString(const Tchar* text, int len) {
		if(text == NULL) maPanic(0, "BasicString(const Tchar* text, int len), passed a NULL pointer.");
		allocStringData(text, len);
	}

	template<class Tchar> BasicString<Tchar>::BasicString(const Tchar* text) {
		if(text == NULL) maPanic(0, "BasicString(const Tchar* text), passed a NULL pointer.");
		allocStringData(text, tstrlen(text));
	}

	template<class Tchar> BasicString<Tchar>::BasicString(const BasicString& s) {
		sd = s.sd;
		sd->addRef();
	}

	template<class Tchar> const Tchar* BasicString<Tchar>::c_str() const {
		return (const Tchar*) sd->mData;
	}

	template<class Tchar> BasicString<Tchar>& BasicString<Tchar>::operator=(const BasicString& s) {
		StringData<Tchar>* old = sd;
		sd = s.sd;
		sd->addRef();
		old->release();
		return *this;
	}

	template<class Tchar> bool BasicString<Tchar>::operator==(const BasicString& other) const {
		if(this->length() != other.length())
			return false;

		if(this->sd == other.sd)
			return true;

		return tstrcmp(this->sd->mData, other.sd->mData)==0;
	}

	template<class Tchar> bool BasicString<Tchar>::operator!=(const BasicString& other) const {
		return !((*this)==other);
	}


	template<class Tchar> bool BasicString<Tchar>::operator<(const BasicString& other) const {
		return tstrcmp(c_str(), other.c_str()) < 0;
	}


	template<class Tchar> bool BasicString<Tchar>::operator>(const BasicString& other) const {
		return tstrcmp(c_str(), other.c_str()) > 0;
	}


	template<class Tchar> bool BasicString<Tchar>::operator<=(const BasicString& other) const {
		return tstrcmp(c_str(), other.c_str()) <= 0;
	}


	template<class Tchar> bool BasicString<Tchar>::operator>=(const BasicString& other) const {
		return tstrcmp(c_str(), other.c_str()) >= 0;
	}


	template<class Tchar> Tchar& BasicString<Tchar>::operator[](int index) {
		//if memory is shared, do copy on write

		if(sd->getRefCount() > 1) {
			StringData<Tchar>* newSd = new StringData<Tchar>(*sd);
			MAASSERT(newSd);
			sd->release();
			sd = newSd;
		}

		return sd->mData[index];
	}

#ifndef NEW_OPERATORS
	template<class Tchar>
	BasicString<Tchar> BasicString<Tchar>::operator+(const BasicString<Tchar>& other) const {
#if 0
		BasicString ret = *this;
		//MAASSERT(sd->getRefCount() > 1);

		StringData* newSd = new StringData(&((*sd)[0]));
		MAASSERT(newSd);
		sd->release();
		ret.sd = newSd;
		for(int i = 0; i < other.size(); i++) {
			ret.sd->add(other[i]);
		}
		ret.sd->reserve(ret.size()+1);
		ret[ret.size()] = '\0';
		return ret;
#else
		BasicString<Tchar> s = *this;
		s += other;
		return s;
#endif
	}


	template<class Tchar> void BasicString<Tchar>::append(const Tchar* other, int len) {
		//order of operations is important here.
		int oldLen = length();
		int otherLen = len;
		int newLen = oldLen + otherLen;
		if(sd->getRefCount() > 1) {
			StringData<Tchar>* newSd = new StringData<Tchar>(newLen);
			memcpy(newSd->pointer(), sd->pointer(), oldLen * sizeof(Tchar));
			sd->release();
			sd = newSd;
		} else {
			sd->reserve(newLen + 1);
		}
		memcpy(sd->pointer() + oldLen, other, otherLen * sizeof(Tchar));
		pointer()[newLen] = 0;
		sd->resize(newLen);		
	}

	template<class Tchar>
	BasicString<Tchar>& BasicString<Tchar>::operator+=(const BasicString<Tchar>& other) {
		append(other.c_str(), other.length());
		return *this;
	}

#if 1
	template<class Tchar> BasicString<Tchar> BasicString<Tchar>::operator+(Tchar c) const {
		BasicString s = *this;
		s += c;
		return s;
	}

	template<class Tchar> BasicString<Tchar>& BasicString<Tchar>::operator+=(Tchar c) {
		append(&c, 1);
		return *this;
	}
#endif
#endif	//NEW_OPERATORS

	template<class Tchar>
	int BasicString<Tchar>::find(const BasicString<Tchar>& s, unsigned int offset) const {
		if (s.length()+offset <= (unsigned int)sd->size()) {
			if (!s.length())
				return ((int) offset);	// Empty string is always found
			const Tchar *str = (const Tchar*) sd->mData + offset;
			const Tchar *search = s.c_str();
			const Tchar *end = sd->mData + sd->size() - s.length() + 1;
			const Tchar *search_end = s.c_str() + s.length();
skipp:
			while (str != end) {
				if (*str++ == *search) {
					register Tchar *i,*j;
					i=(Tchar*) str;
					j=(Tchar*) search+1;
					while (j != search_end)
						if (*i++ != *j++) goto skipp;
					return (int) (str - sd->mData) - 1;
				}
			}
		}
		return npos;
	}

	template<class Tchar> int BasicString<Tchar>::findLastOf(const Tchar findThis) const {
		for(int i = this->length(); i >= 0; i--) {
			if((*this)[i] == findThis) return i;
		}
		return npos;
	}

	template<class Tchar>
	int BasicString<Tchar>::findFirstOf(const Tchar findThis, int position) const {
		for(int i = position; i < this->length(); i++) {
			if((*this)[i] == findThis) return i;
		}
		return npos;
	}

	template<class Tchar>
	int BasicString<Tchar>::findFirstNotOf(const Tchar findNotThis, int position) const {
		for(int i = position; i < this->length(); i++) {
			if((*this)[i] != findNotThis) return i;
		}
		return npos;
	}

	template<class Tchar>
	void BasicString<Tchar>::insert(int position, const BasicString<Tchar>& other) {
		int otherLen = other.length();
		int newLen = this->length() + otherLen;
		this->resize(newLen);
		int endpos = position + otherLen;
		for(int i = newLen-1; i >= endpos; i--) {
			(*sd)[i] = (*sd)[i-otherLen];
		}
		for(int i = 0; i < otherLen; i++) {
			(*sd)[position++] = other[i];
		}
	}

	//TODO: cleanup
	template<class Tchar> void BasicString<Tchar>::insert(int position, Tchar c) {
		int otherLen = 1;
		int newLen = this->length() + otherLen;
		this->resize(newLen);
		int endpos = position + otherLen;
		for(int i = newLen-1; i >= endpos; i--) {
			(*sd)[i] = (*sd)[i-otherLen];
		}
		for(int i = 0; i < otherLen; i++) {
			(*sd)[position++] = c;
		}
	}

	template<class Tchar> void BasicString<Tchar>::remove(int position, int number) {
		if(sd->getRefCount() > 1) {
			ASSERT_MSG(position >= 0 && position < this->length(), "invalid position");
			ASSERT_MSG(number > 0 && (position + number) <= this->length(), "invalid number");
			int newLen = size() - number;
			StringData<Tchar>* temp = new StringData<Tchar>(newLen);
			if(position > 0) {
				memcpy(temp->pointer(), sd->pointer(), position * sizeof(Tchar));
			}
			if(position < newLen) {
				memcpy(temp->pointer() + position, sd->pointer() + (position + number),
					(newLen - position) * sizeof(Tchar));
			}
			sd->release();
			sd = temp;
		} else {
			sd->remove(position, number);
			sd->reserve(sd->size() + 1);
		}
		(*sd)[sd->size()] = 0;
	}

	template<class Tchar>
	BasicString<Tchar> BasicString<Tchar>::substr(int startIndex, int len) const {
		ASSERT_MSG(startIndex >= 0 && startIndex <= this->length(), "invalid index");
		if(len == npos)
			len = this->length() - startIndex;
		ASSERT_MSG(len >= 0 && (startIndex+len) <= this->length(), "invalid length");

		BasicString retString;
#if 0
		for(int i = startIndex; i < startIndex + len; i++) {
			retString += (*this)[i];
		}
#else
		if(len > 0) {
			retString.sd->release();
			retString.sd = new StringData<Tchar>(len);
			memcpy(retString.sd->pointer(), sd->pointer() + startIndex, len * sizeof(Tchar));
			retString[len] = 0;
		}
#endif
		return retString;
	}


	template<class Tchar> const Tchar& BasicString<Tchar>::operator[](int index) const {
		return sd->mData[index];
	}

	template<class Tchar> int BasicString<Tchar>::size() const {
		return sd->size();
	}

	template<class Tchar> int BasicString<Tchar>::length() const {
		return sd->size();
	}

	template<class Tchar> int BasicString<Tchar>::capacity() const {
		return sd->capacity() - 1;
	}

	template<class Tchar> BasicString<Tchar>::~BasicString() {
		sd->release();
	}

	template<class Tchar> void BasicString<Tchar>::resize(int newLen) {
		reserve(newLen);
		sd->resize(newLen);
		(*sd)[newLen] = 0;
	}

	template<class Tchar> void BasicString<Tchar>::reserve(int newLen) {
		if(sd->getRefCount() > 1) {
			if(newLen < sd->capacity())
				newLen = sd->capacity();
			StringData<Tchar>* temp = new StringData<Tchar>(newLen);
			temp->resize(sd->size());
			memcpy(temp->pointer(), sd->pointer(), (sd->size() + 1) * sizeof(Tchar));
			sd->release();
			sd = temp;
		} else {
			sd->reserve(newLen+1);
			(*sd)[sd->size()] = 0;
		}
	}

	template<class Tchar> void BasicString<Tchar>::clear() {
		if(sd) {
			sd->release();
		}
		sd = getEmptyData<Tchar>();
	}

#ifdef HAVE_EMPTY_STRING
	template<class Tchar> const BasicString& BasicString<Tchar>::emptyString() {
		return *EMPTY_STRING;
	}
#endif

	template<class Tchar> void BasicString<Tchar>::setData(StringData<Tchar>* data) {
		sd->release();
		sd = data;
	}

	template<class Tchar> Tchar* BasicString<Tchar>::pointer() {
		return sd->pointer();
	}

	//explicit instantiation
	template class BasicString<char>;
	template class BasicString<wchar_t>;
	template class StringData<char>;
	template class StringData<wchar_t>;
}

template<> int tstrlen<char>(const char* str) { return static_cast<int>(strlen(str)); }
template<> int tstrcmp<char>(const char* a, const char* b) { return strcmp(a, b); }

template<class Tchar> int tstrlen(const Tchar* str) { return static_cast<int>(wcslen(str)); }
template<class Tchar> int tstrcmp(const Tchar* a, const Tchar* b) { return wcscmp(a, b); }

//explicit instantiation (needed to link with these functions in MAPIP.)
template int tstrlen<wchar_t>(const wchar_t*);
template int tstrcmp<wchar_t>(const wchar_t*, const wchar_t*);
