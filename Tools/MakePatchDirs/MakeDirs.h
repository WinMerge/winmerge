/** @file MakeDirs.h 
 *
 *  @brief Declaration of MakeDirs class
 */ 

#ifndef MakeDirs_h
#define MakeDirs_h

class MakeDirFilter
{
public:
	virtual bool handle(bool dir, const CString & subdir, const CString & filebase, const CString & ext) = 0;
};

class MakeDirs
{
public:
	static CString DoIt(const CString & rootDir, MakeDirFilter * pifilter);


// Implementation types
private:
	struct DiffArray
	{
		CStringArray Left;
		CStringArray Right;
		CStringArray Shared;
	};

// Implementation methods
private:
	MakeDirs(const CString & rootDir, MakeDirFilter * pifilter) : m_rootDir(rootDir), m_pifilter(pifilter) { }
	CString Worker();
	void LoadFiles(const CString & path, CStringArray * array);
	void LoadFilesWorker(const CString & path, const CString & subdir, CStringArray * array);
	void SortArray(CStringArray * array);
	void CheckDirectory(const CString & path);
	void SplitFiles(const CStringArray & left, const CStringArray & right, DiffArray * diffs);
	void TraceArray(const CStringArray & array);
	void MyCopyFile(CString src, CString dest);
	void Fail(LPCTSTR sz);
	bool DoesDirExist(const CString & path);
	bool FilesDiffer(CString oldf, CString newf);
	void MyDeleteDir(LPCTSTR szDir);
	void MyDeleteFile(LPCTSTR szFile);
	void RemoveDirIfExists(CString path);

// Implementation data
private:
	CString m_rootDir;
	MakeDirFilter * m_pifilter;
	CStringArray m_originalFiles;
	CStringArray m_appliedFiles;
};


#endif // MakeDirs_h
