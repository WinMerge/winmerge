/**
 *  @file PathContext.h
 *
 *  @brief Declarations of PathInfo and PathContext
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


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
	void SetPath(LPCTSTR path);
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
	PathContext(LPCTSTR sLeft, LPCTSTR sRight);
	String GetLeft(BOOL bNormalized = TRUE) const;
	String GetRight(BOOL bNormalized = TRUE) const;
	String GetPath(int index, BOOL bNormalized = TRUE) const;
	void SetLeft(LPCTSTR path);
	void SetRight(LPCTSTR path);
	void SetPath(int index, LPCTSTR path);
	void Swap();
private:
	PathInfo m_pathLeft; /**< First path (left path at start) */
	PathInfo m_pathRight; /**< Second path (right path at start */
};

/**
 * @brief Temp files for compared files
 */
class TempFileContext : public PathContext
{
public:
	~TempFileContext();
	BOOL CreateFiles(const PathContext &paths);
	BOOL FilesExist() const;
	void DeleteFiles();
	const CString & GetTempPath() const { return m_sTempPath; }

private:
	CString m_sTempPath;
};

#endif  // _PATH_CONTEXT_H_
