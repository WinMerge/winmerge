#pragma once

#include "PathContext.h"
#include "FileTransform.h"

class ProjectFile;

// COpenDoc document

class COpenDoc : public CDocument
{
	DECLARE_DYNCREATE(COpenDoc)

public:
	COpenDoc();
	virtual ~COpenDoc();
	void RefreshOptions();

	DWORD   m_dwFlags[3];
	PathContext m_files;
	bool	m_bRecurse;
	String	m_strExt;
	String	m_strUnpacker;
	PackingInfo m_infoHandler;

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
