#pragma once

#include "PathContext.h"
#include "FileTransform.h"
#include "FileOpenFlags.h"
#include <array>

class ProjectFile;

// COpenDoc document

class COpenDoc : public CDocument
{
	DECLARE_DYNCREATE(COpenDoc)

public:
	COpenDoc();
	virtual ~COpenDoc();
	void RefreshOptions();
	void UpdateResources();

	std::array<fileopenflags_t, 3> m_dwFlags;
	PathContext m_files;
	bool	m_bRecurse;
	String	m_strExt;
	String	m_strUnpackerPipeline;
	String	m_strPredifferPipeline;
	String	m_strDesc[3];
	int		m_nWindowType;
	tchar_t	m_cTableDelimiter;
	tchar_t	m_cTableQuote;
	bool	m_bTableAllowNewLinesInQuotes;
	std::vector<String> m_hiddenItems;

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
