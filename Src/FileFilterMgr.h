/////////////////////////////////////////////////////////////////////////////
// FileFilterMgr.h : declaration file
//
// The FileFilterMgr loads a collection of named file filters from disk,
// and provides lookup access by name, or array access by index, to these
// named filters. It also provides test functions for actually using the filters.
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#ifndef FileFilter_h_included
#define FileFilter_h_included

// Uses MFC C++ template containers
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

struct FileFilter;

class FileFilterMgr
{
private:

public:
	~FileFilterMgr();
	// Reload filter array from specified directory (passed to CFileFind)
	void LoadFromDirectory(LPCTSTR szPattern);

	// access to array of filters
	int GetFilterCount() const { return m_filters.GetSize(); }
	CString GetFilterName(int i);
	FileFilter * GetFilter(LPCTSTR szFilterName);

	// methods to actually use filter
	BOOL TestFileNameAgainstFilter(FileFilter * pFilter, LPCTSTR szFileName);
	BOOL TestDirNameAgainstFilter(FileFilter * pFilter, LPCTSTR szDirName);


// Implementation methods
protected:
	// Clear the list of known filters
	void DeleteAllFilters();
	// Load a filter from a file (if syntax is valid)
	void LoadFilterFile(LPCTSTR szFilepath, LPCTSTR szFilename);

// Implementation data
private:
	CTypedPtrArray<CPtrArray, FileFilter *> m_filters;
};


#endif // FileFilter_h_included

