#ifndef FileLocation_included
#define FileLocation_included

#ifndef FileTextEncoding_h_included
#include "FileTextEncoding.h"
#endif

struct FileLocation
{
	CString filepath;
	FileTextEncoding encoding;
	FileLocation()
	{
	}
	FileLocation(LPCTSTR path) : filepath(path)
	{
	}
// Methods
	void setPath(const CString & sFilePath) { this->filepath = sFilePath; }

// Copy constructor & copy operator should do the obvious correct things
};

#endif // FileLocation_included
