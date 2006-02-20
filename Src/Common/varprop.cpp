/**
 *  @file varprop.cpp
 *
 *  @brief Implementation of generic named property classes
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "varprop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace varprop {

/** @brief Construct empty property set */
PropertySet::PropertySet(int hashsize)
{
	if (hashsize>0)
		InitHashTable(hashsize);
}
/** @brief Construct object by copying specified source */
PropertySet::PropertySet(const PropertySet & src)
{
	// delegate to CopyFrom, so we only have to write the code once
	CopyFrom(src);
}

/** @brief Copy from other object (copy assignment operator) */
PropertySet & PropertySet::operator=(const PropertySet & src)
{
	// delegate to CopyFrom, so we only have to write the code once
	CopyFrom(src);
	return *this;
}

/** @brief clean up contained memory */
PropertySet::~PropertySet()
{
	// This looks the same as the base class, but this isn't a virtual method
	// so we have to override the destructor to call it directly
	// (Alternately, we could implement the DestructElements template helper)
	RemoveAll();
}

/** @brief deallocate & remove elements */
void PropertySet::RemoveAll()
{
	CString name;
	Property  * ptr=0;
	for (POSITION pos = this->GetStartPosition(); pos; )
	{
		this->GetNextAssoc(pos, name, ptr);
		delete ptr;
	}
	CTypedPtrMap<CMapStringToPtr, CString, Property*>::RemoveAll();
}

/** @brief Make this property set (to be) a copy of argument property set */
void PropertySet::CopyFrom(const PropertySet & src)
{
	RemoveAll();
	InitHashTable(src.GetHashTableSize());

	CString name;
	Property  * ptr=0;
	for (POSITION pos = src.GetStartPosition(); pos; )
	{
		src.GetNextAssoc(pos, name, ptr);
		SetAt(name, new Property(*ptr));
	}
}

/** @brief Assign a new property (or reassign existing) */
void PropertySet::SetProperty(const Property & property)
{
	Property * ptr = 0;
	if (Lookup(property.name, ptr))
		*ptr = property;
	else
	{
		if ((UINT)GetCount() > GetHashTableSize())
		{
			// TODO: enlarge hash table for better performance
		}
		SetAt(property.name, new Property(property));
	}
}
/** Assign a new time-valued property (or reassign existing) */
void PropertySet::SetProperty(LPCTSTR name, COleDateTime time)
{
	Property prop;
	prop.name = name;
	prop.value.SetTime(time);
	SetProperty(prop);
}
/** Assign a new string-valued property (or reassign existing) */
void PropertySet::SetProperty(LPCTSTR name, LPCTSTR value)
{
	Property prop;
	prop.name = name;
	prop.value.SetString(value);
	SetProperty(prop);
}
/** Assign a new string-valued property (or reassign existing) */
void PropertySet::SetProperty(LPCTSTR name, const CString & value)
{
	Property prop;
	prop.name = name;
	prop.value.SetString(value);
	SetProperty(prop);
}
/** Assign a new int-valued property (or reassign existing) */
void PropertySet::SetProperty(LPCTSTR name, int value)
{
	Property prop;
	prop.name = name;
	prop.value.SetInt(value);
	SetProperty(prop);
}

/** @brief Retrieve a property by name (NULL if no such property) */
Property * PropertySet::GetProperty(LPCTSTR szname)
{
	CString name = szname;
	Property * ptr = 0;
	Lookup(name, ptr);
	return ptr;
}

/** @brief Retrieve a property by name (NULL if no such property) */
const Property * PropertySet::GetProperty(LPCTSTR szname) const
{
	CString name = szname;
	Property * ptr = 0;
	Lookup(name, ptr);
	return ptr;
}

} // namespace
