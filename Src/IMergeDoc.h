#pragma once

#include "UnicodeString.h"

struct IDirDoc;
class PackingInfo;
class PrediffingInfo;
class EditorScriptInfo;

struct IMergeDoc
{
	enum class FileChange
	{
		NoChange,
		Changed,
		Removed,
	};

	virtual IDirDoc* GetDirDoc() const = 0;
	virtual void SetDirDoc(IDirDoc *pDirDoc) = 0;
	virtual bool CloseNow(void) = 0;
	virtual bool GenerateReport(const String &path) const = 0;
	virtual void DirDocClosing(IDirDoc * pDirDoc) = 0;
	virtual void CheckFileChanged() = 0;
	virtual const PackingInfo *GetUnpacker() const = 0;
	virtual void SetUnpacker(const PackingInfo *infoUnpacker) = 0;
	virtual const PrediffingInfo *GetPrediffer() const = 0;
	virtual const EditorScriptInfo *GetEditorScript() const = 0;
	virtual int GetFileCount() const = 0;
	virtual String GetPath(int pane) const = 0;
	virtual String GetDescription(int pane) const = 0;
	virtual bool GetReadOnly(int pane) const = 0;
	virtual int GetDiffCount() const = 0;
	virtual int GetTrivialCount() const = 0;
};

