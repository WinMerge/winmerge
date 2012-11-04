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

	String GetPath(bool bNormalized = true) const;
	String& GetRef();
	void SetPath(const TCHAR *path);
	void SetPath(const String & path);
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
	PathContext(const String& sLeft);
	PathContext(const String& sLeft, const String& sRight);
	PathContext(const String& sLeft, const String& sMiddle, const String& sRight);
	PathContext(const PathContext &paths);

	String GetAt(int nIndex) const;
	String& GetElement(int nIndex);
	void SetAt(int nIndex, const String& newElement);
	String operator[](int nIndex) const;
	String& operator[](int nIndex);

	String GetLeft(bool bNormalized = true) const;
	String GetRight(bool bNormalized = true) const;
	String GetMiddle(bool bNormalized = true) const;
	String GetPath(int index, bool bNormalized = true) const;
	void SetLeft(const String& path, bool bNormalized = true);
	void SetRight(const String& path, bool bNormalized = true);
	void SetMiddle(const String& path, bool bNormalized = true);
	void SetPath(int index, const String& path, bool bNormalized = true);
	void SetSize(int nFiles);
	int GetSize() const;
	void RemoveAll();
	void Swap();
private:
	int m_nFiles;
	PathInfo m_path[3]; /**< First, second, third path (left path at start) */
};

#endif  // _PATH_CONTEXT_H_
