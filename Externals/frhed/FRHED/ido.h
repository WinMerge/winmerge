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
 * @file  ido.h
 *
 * @brief Declarations of clipboard data handling classes.
 *
 */
#ifndef ido_h
#define ido_h

struct DataSpecifier
{
	STGMEDIUM medium;
	FORMATETC format;
};

class CEnumFORMATETC;

class CDataObject: public IDataObject
{
	friend CEnumFORMATETC;
private:
	ULONG m_cRefCount;

	bool allowSetData;      //Allow the IDataObject::SetData call
	DataSpecifier *data;    //Pointer to an array of STGMEDIUM+FORMATETC pairs
	unsigned int numdata;   //Number of data objects in data
	CEnumFORMATETC **enums; //An array of pointers to IEnumFORMATETC objects
	unsigned int numenums;  //Number of pointers in enums

public:
	//Members
	CDataObject();
	~CDataObject();
	void DisableSetData();
	void Empty();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IDataObject members
	STDMETHODIMP GetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium);
	STDMETHODIMP GetDataHere(FORMATETC *pFormatetc, STGMEDIUM* pmedium);
	STDMETHODIMP QueryGetData(FORMATETC *pFormatetc);
	STDMETHODIMP GetCanonicalFormatEtc(FORMATETC *pFormatetcIn, FORMATETC *pFormatetcOut);
	STDMETHODIMP SetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium, BOOL fRelease);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatetc);
	STDMETHODIMP DAdvise(FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(IEnumSTATDATA **ppenumAdvise);
};

class CEnumFORMATETC : public IEnumFORMATETC
{
private:
	DWORD m_cRefCount;
	bool deleteself;
	CEnumFORMATETC** pthis;

	CDataObject* parent; //Pointer to the parent IDataObject
	unsigned int index;  //The index of the data in the parent's data storage

public:
	//Members
	CEnumFORMATETC(CDataObject *);
	~CEnumFORMATETC();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IEnumFORMATETC members
	STDMETHODIMP Next(ULONG celt, FORMATETC *rgelt, ULONG* pceltFetched);
	STDMETHODIMP Skip(ULONG celt);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumFORMATETC ** ppenum);
};

#endif // ido_h