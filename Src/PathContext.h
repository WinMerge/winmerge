/**
 *  @file PathContext.h
 *
 *  @brief Declarations of PathInfo and PathContext
 */
// ID line follows -- this is updated by SVN
// $Id: PathContext.h 4929 2008-01-18 20:03:57Z kimmov $


#ifndef _PATH_CONTEXT_H_
#define _PATH_CONTEXT_H_

#include "UnicodeString.h"

class PathContext;

/**
 * @brief Information for one path.
 *
 * Path is stored in normalized format (no trailing slash).
 */
class PathInfo
{
	friend PathContext;
public:
	PathInfo() {}
	PathInfo(const PathInfo &pi);

	String GetPath(BOOL bNormalized = TRUE) const;
	String& GetRef();
	void SetPath(LPCTSTR path);
	void SetPath(String & path);
	void NormalizePath();

private:
	String m_sPath;  /**< Directory / file path */
};

/**
 * @brief Holds path information of compared files/directories.
 */
class PathContext
{
public:
	PathContext();
	PathContext(LPCTSTR sLeft);
	PathContext(LPCTSTR sLeft, LPCTSTR sRight);
	PathContext(LPCTSTR sLeft, LPCTSTR sMiddle, LPCTSTR sRight);
	PathContext(const PathContext &paths);

	String GetAt(int nIndex) const;
	String& GetElement(int nIndex);
	void SetAt(int nIndex, const String& newElement);
	String operator[](int nIndex) const;
	String& operator[](int nIndex);

	String GetLeft(BOOL bNormalized = TRUE) const;
	String GetRight(BOOL bNormalized = TRUE) const;
	String GetMiddle(BOOL bNormalized = TRUE) const;
	String GetPath(int index, BOOL bNormalized = TRUE) const;
	void SetLeft(LPCTSTR path, bool bNormalized = true);
	void SetRight(LPCTSTR path, bool bNormalized = true);
	void SetMiddle(LPCTSTR path, bool bNormalized = true);
	void SetPath(int index, LPCTSTR path, bool bNormalized = true);
	void SetSize(int nFiles);
	int GetSize() const;
	void RemoveAll();
	void Swap();
private:
	int m_nFiles;
	PathInfo m_path[3]; /**< First, second, third path (left path at start) */
};

#endif  // _PATH_CONTEXT_H_
