#ifndef FileLocation_included
#define FileLocation_included

struct FileLocation
{
	CString filepath;
	int unicoding;
	int codepage;
	FileLocation() : unicoding(-1), codepage(-1)
	{
	}
	FileLocation(LPCTSTR path) : filepath(path), unicoding(-1), codepage(-1)
	{
	}
// Methods
	void setPath(const CString & sFilePath) { this->filepath = sFilePath; }

// Copy constructor & copy operator should do the obvious correct things
};

#endif // FileLocation_included
