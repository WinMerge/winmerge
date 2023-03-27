/**
 *  @file PathContext.h
 *
 *  @brief Declarations of PathInfo and PathContext
 */
#pragma once

#include "UnicodeString.h"
#include <vector>

class PathContext;
class PathContextIterator;

/**
 * @brief Information for one path.
 *
 * Path is stored in normalized format (no trailing slash).
 */
class PathInfo
{
	friend class PathContext;
public:
	PathInfo() = default;

	String GetPath(bool bNormalized = true) const;
	String& GetRef() { return m_sPath; }
	void SetPath(const tchar_t *path);
	void SetPath(const String & path);
	void NormalizePath();

private:
	String m_sPath;  /**< Directory / file path */
};

/**
 * @brief Set path.
 * @param [in] sPath New path for item.
 */
inline void PathInfo::SetPath(const tchar_t *sPath)
{
	m_sPath = sPath;
}

inline void PathInfo::SetPath(const String & sPath)
{
	m_sPath = sPath;
}

/**
 * @brief Holds path information of compared files/directories.
 */
class PathContext
{
public:
	typedef PathContextIterator const_iterator;

	PathContext();
	explicit PathContext(const String& sLeft);
	PathContext(const String& sLeft, const String& sRight);
	PathContext(const String& sLeft, const String& sMiddle, const String& sRight);
	explicit PathContext(const std::vector<String>& paths);

	String GetAt(int nIndex) const;
	String& GetElement(int nIndex);
	void SetAt(int nIndex, const String& newElement);
	String operator[](int nIndex) const { return GetAt(nIndex); }
	String& operator[](int nIndex) { return GetElement(nIndex); }

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
	void Swap(int nFromIndex, int nToIndex);

	const_iterator begin() const;
	const_iterator end() const;
private:
	int m_nFiles;
	PathInfo m_path[3]; /**< First, second, third path (left path at start) */
};

/**
 * @brief set number of files.
 */
inline void PathContext::SetSize(int nFiles)
{
	m_nFiles = nFiles;
}

/**
 * @brief Return number of files.
 */
inline int PathContext::GetSize() const
{
	return m_nFiles;
}

class PathContextIterator : public std::iterator<std::forward_iterator_tag, String>
{
public:
	explicit PathContextIterator(const PathContext *pPathContext) : m_pPathContext(pPathContext)
	{
		m_sel =  (pPathContext->GetSize() == 0) ? -1 : 0;
	}

	PathContextIterator() : m_pPathContext(nullptr), m_sel(-1)
	{
	}

	~PathContextIterator() = default;

	PathContextIterator& operator=(const PathContextIterator& it)
	{
		m_sel = it.m_sel;
		m_pPathContext = it.m_pPathContext;
		return *this;
	}

	PathContextIterator& operator++()
	{
		m_sel++;
		if (m_sel >= m_pPathContext->GetSize())
			m_sel = -1;
		return *this;
	}

	String operator*()
	{
		return m_pPathContext->GetAt(m_sel);
	}

	bool operator==(const PathContextIterator& it) const
	{
		return m_sel == it.m_sel;
	}

	bool operator!=(const PathContextIterator& it) const
	{
		return m_sel != it.m_sel;
	}

public:
	const PathContext *m_pPathContext;
	int m_sel;
};

inline PathContextIterator PathContext::begin() const
{
	return PathContextIterator(this);
}

inline PathContextIterator PathContext::end() const
{
	return PathContextIterator();
}
