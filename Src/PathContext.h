/**
 *  @file PathContext.h
 *
 *  @brief Declarations of PathInfo and PathContext
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#ifndef _PATH_CONTEXT_H_
#define _PATH_CONTEXT_H_

/**
 * @brief Information for one path.
 */
class PathInfo
{
public:
	PathInfo() {}
	CString & GetPath() { return m_sNormalizedPath; }

	const CString & GetPath(BOOL bNormalized = TRUE) const
		{ return (bNormalized ? m_sNormalizedPath : m_sPath); }
	void SetPath(CString path);
	void NormalizePath();

private:
	CString m_sPath;  /**< Directory / file path */
	CString m_sNormalizedPath; /**< Normalized version of m_sPath (preferred)*/
};

/**
 * @brief Holds path information of compared files/directories.
 */
class PathContext
{
public:
	PathContext();
	const CString & GetLeft(BOOL bNormalized = TRUE) const;
	const CString & GetRight(BOOL bNormalized = TRUE) const;
	void SetLeft(LPCTSTR path);
	void SetRight(LPCTSTR path);

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
	BOOL FilesExist();
	void DeleteFiles();
	const CString & GetPath() const { return m_sTempPath; }

private:
	CString m_sTempPath;
};

#endif  // _PATH_CONTEXT_H_
