#pragma once

#include "UnicodeString.h"

class CDirDoc;

struct IMergeDoc
{
	enum class FileChange
	{
		NoChange,
		Changed,
		Removed,
	};

	virtual void SetDirDoc(CDirDoc *pDirDoc) = 0;
	virtual bool CloseNow(void) = 0;
	virtual bool GenerateReport(const String &path) const = 0;
	virtual void DirDocClosing(CDirDoc * pDirDoc) = 0;
	virtual void CheckFileChanged() = 0;
};

