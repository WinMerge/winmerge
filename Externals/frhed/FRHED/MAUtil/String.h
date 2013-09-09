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

/** \file String.h
* \brief Reference-counted string behaving like a subset of std::string.
* \author Patrick Broman and Niklas Nummelin
*
*/


#ifndef _SE_MSAB_MAUTIL_STRING_H_
#define _SE_MSAB_MAUTIL_STRING_H_

#include "RefCounted.h"
#include "Vector.h"
//#include <ma.h>

#if 0//ndef MAPIP
#define HAVE_EMPTY_STRING
#endif

template<class Tchar> int tstrlen(const Tchar*);
template<class Tchar> int tstrcmp(const Tchar*, const Tchar*);

namespace MAUtil {

#ifdef NEW_OPERATORS
	class StringData;
	class String;
	class StringStream;

	namespace StringTranscribe {
		/**
		* Returns the maximum number of bytes that might be required to
		* convert \a t to a string.
		*/
		//int requiredSpace(T t);

		/**
		* Writes a string representation of \a t, including a null terminator,
		* into \a dst. Must not write more bytes than requiredSpace() returns for this
		* object, but may write less.
		* Returns the number of bytes actually written.
		*/
		//int transcribe(T t, char* dst);
	}
	// make specializations for int, double, maybe char and String.
	// Others will have to make their own specializations.
	// Compound types should make specializations using references, like this:
	// StringTranslator<MARect&>

	class StringStream {
	public:
		template<class T> StringStream operator+(T st) {
			mS += st;
			return *this;
		}
	private:
		StringStream(String& s) : mS(s) {}
		//StringStream(StringStream& o) : mS(o.mS) {}
		String& mS;
		template<class T> friend
			StringStream operator+=(String& s, T st);
	};

#endif	//NEW_OPERATORS

	/**
	* \brief A class that holds the actual data used by String.
	*
	* It's a reference counted vector.
	*/
	template<class Tchar> class StringData : public Vector<Tchar>, public RefCounted {
	public:
		StringData(const Tchar* text);
		StringData(const Tchar* text, int len);
		StringData(int len);
		StringData(const StringData& other);
		template<class T> friend class BasicString;
		virtual ~StringData() {}
	};

	/**
	* \brief A dynamic, reference-counted string that behaves much like a subset of std::string.

	* Each String references an instance of StringData, and these
	* instances are shared between strings as much as possible by using
	* the copy-on-write idiom.
	*/
	template<class Tchar> class BasicString {
	public:

		enum {
			npos = -1
		};

		/**
		* Initializes the new string with shared empty data.
		*/
		BasicString();

		/**
		* Initializes the string with empty data and the given capacity.
		*/
		explicit BasicString(int capacity);

		/**
		* Copies the specified null-terminated string into the new string.
		*/
		BasicString(const Tchar* text);
		
		/**
		* Copies \a len bytes of the string \a text into the new string.
		*/
		BasicString(const Tchar* text, int len);


		/** Makes the new string share the data of \a s. */
		BasicString(const BasicString& s);

		/** Returns a pointer to the null-terminated character data.
		* This pointer becomes invalid when the object is destroyed,
		* or a non-const method of this class is called.
		*/
		const Tchar* c_str() const;

		/** Makes this string share the \a other string's data. */
		BasicString& operator=(const BasicString& other);

		/** Returns a reference to the character at position \a index. */
		Tchar& operator[](int index);

		/** Returns a const reference to the character at position \a index. */
		const Tchar& operator[](int index) const;

		/** Returns true if the strings are equal, false otherwise. */
		bool operator==(const BasicString& other) const;

		/** Returns false if the strings are equal, true otherwise. */
		bool operator!=(const BasicString& other) const;

		/** Returns true if \a this is lexiographically less than \a other,
		* false otherwise. */
		bool operator<(const BasicString& other) const;

 		/** Returns true if \a this is lexiographically greater than other,
		* false otherwise. */
		bool operator>(const BasicString& other) const;

		/** Returns true if \a this is lexiographically less than or equal to \a other,
		* false otherwise. */
		bool operator<=(const BasicString& other) const;

 		/** Returns true if \a this is lexiographically greater than or equal to \a other,
		* false otherwise. */
		bool operator>=(const BasicString& other) const;

#ifdef NEW_OPERATORS
		template<class T> friend
			StringStream operator+=(BasicString& s, T st);
#else
		/** Returns a copy of this string concatenated with the \a other. */
		BasicString operator+(const BasicString& other) const;

		/** Concatenates a string in-place with \a other and returns a reference to itself. */
		BasicString& operator+=(const BasicString& other);

#if 1
		/** Returns a copy of this string concatenated with the \a c. */
		BasicString operator+(Tchar c) const;

		/** Concatenates a string in-place with \a c and returns a reference to itself. */
		BasicString& operator+=(Tchar c);
#endif
#endif	//NEW_OPERATORS

		/**
		* Returns the index of the first instance of the given string inside this string,
		* starting at the given position. Returns npos if not found.
		*/
		int find(const BasicString& s, unsigned int offset = 0) const;

		/** Returns the last index of the given character. Returns npos if not found. */
		int findLastOf(const Tchar findThis) const;

		/**
		* Returns the first index of the given character starting at the given position.
		* Returns npos if not found.
		*/
		int findFirstOf(const Tchar findThis, int position = 0) const;

		/**
		* Returns the first index _not_ matching the given character, starting at the given position.
		* Returns npos if not found.
		*/
		int findFirstNotOf(const Tchar findNotThis, int position = 0) const;

		/** Inserts the given string at the given position. */
		void insert(int position, const BasicString& other);

		/** Inserts the given character at the given position. */
		void insert(int position, Tchar c);

		/** Removes the given number of characters starting at the given position. */
		void remove(int position, int number);

		/** Returns a new string that is a copy of the specified portion of this string. */
		BasicString substr(int startIndex, int length = npos) const;

		/** Returns the size (in characters) of the reserved space in the string data object. */
		int capacity() const;

		/** Returns the number of characters in the string. */
		int size() const;

		/** Returns the number of characters in the string. */
		int length() const;

		/** Resizes the string. Any new characters are undefined. */
		void resize(int newLen);

		/** Reserves space in the string data object. */
		void reserve(int newLen);

		/** Resizes the string to zero. */
		void clear();

		/** Appends a string at the end of the string. */
		void append(const Tchar* other, int len);


#ifdef HAVE_EMPTY_STRING
		/** Returns a reference to an empty string. */
		static const BasicString& emptyString();
#endif

		/** Replaces this string's data object. */
		void setData(StringData<Tchar>* data);

		/**
		* Returns a pointer to the string data. The pointer becomes invalidated by
		* any non-const method of this class.
		*/
		Tchar* pointer();

		/**
		* Returns a const pointer to the string data. The pointer becomes invalidated by
		* any non-const method of this class.
		*/
		const Tchar* pointer() const { return c_str(); }

		~BasicString();

	protected:
		void allocStringData(const Tchar *text, int len);
	
		/** A pointer to the string data object shared by this string. */
		StringData<Tchar>* sd;
#ifdef HAVE_EMPTY_STRING
		/** a single empty string for convenience. */
		static const BasicString* EMPTY_STRING;
#endif
	};

	typedef BasicString<char> String;
	typedef BasicString<wchar_t> WString;

#ifdef NEW_OPERATORS

	class StringDupeStream {
	public:
		template<class T> StringDupeStream operator+(T st) {
			mS += st;
			return *this;
		}
		operator String() {	//conversion operator
			return mS;
		}
	private:
		StringDupeStream(const String& s) : mS(s) {}
		//StringDupeStream(StringDupeStream& o) : mS(o.mS) {}
		String mS;
		template<class T> friend
			StringDupeStream operator+(const String& s, T st);
	};
	
	template<class T>
	StringStream operator+=(String& s, T t) {
		int newCap = StringTranscribe::requiredSpace(t) + s.size();
		if(s.capacity() < newCap) {
			s.reserve(newCap);
		}
		int usedSpace = StringTranscribe::transcribe(t, s.sd->pointer() + s.size());
		s.resize(s.size() + usedSpace);
		return StringStream(s);
	}
	template<class T>
	StringDupeStream operator+(const String& s, T st) {
		String dupe(s);
		dupe += st;
		return StringDupeStream(dupe);
	}

#if 1
	namespace StringTranscribe {
		inline int requiredSpace(const String& t) {
			return t.size();
		}
		inline int transcribe(const String& t, char* dst) {
			strcpy(dst, t.c_str());
			return t.size();
		}

		inline int requiredSpace(const char* t) {
			return strlen(t);
		}
		inline int transcribe(const char* t, char* dst) {
			strcpy(dst, t);
			return strlen(t);
		}

		inline int requiredSpace(int t) {
			return 11;
		}
		inline int transcribe(int t, char* dst) {
			_itoa(t, dst, 10);
			return strlen(dst);
		}

		template<class T> class StringTranscriber {
		public:
			T mT;

			//friend StringTranscriber<T> toString(T t);
		//private:
			StringTranscriber(T t) : mT(t) {}
		};
		template<class T> inline int requiredSpace(StringTranscriber<T> st) {
			return requiredSpace(st.mT);
		}
		template<class T> inline int transcribe(StringTranscriber<T> st, char* dst) {
			return transcribe(st.mT, dst);
		}
	}
	template<class T> StringTranscribe::StringTranscriber<T> toString(T t) {
		return StringTranscribe::StringTranscriber<T>(t);
	}
#else
	template<> class StringTranslator<const String&> {
	public:
		int requiredSpace() { return mT.size(); }
		int translate(char* dst) {
			strcpy(dst, mT.c_str());
			return mT.size();
		}
		StringTranslator(const String& t) : mT(t) {}
		friend StringTranslator<const String&> toString(const String&);
	private:
		const String& mT;
	};

	template<> class StringTranslator<const char*> {
	public:
		int requiredSpace() { return mLen; }
		int translate(char* dst) {
			strcpy(dst, mT);
			return mLen;
		}
		StringTranslator(const char* t) : mT(t), mLen(strlen(t)) {}
	private:
		const char* mT;
		const int mLen;
	};
#endif	//0
#else
	/**
	* \returns A new string that is the concatenation of \a c and \a s.
	*/
	template<class Tchar> inline BasicString<Tchar> operator+(const Tchar* c,
		const BasicString<Tchar>& s)
	{
		return BasicString<Tchar>(c)+s;
	}
#endif	//NEW_OPERATORS
}

#endif	//_SE_MSAB_MAUTIL_STRING_H_
