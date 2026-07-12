#pragma once

#include "UnicodeString.h"

struct IDirDoc;
class PackingInfo;
class PrediffingInfo;
class EditorScriptInfo;
class UniStdioFile;

struct ReportContext
{
	ReportContext(UniStdioFile& file, bool includeAllImagePages, const String& outputDirectory)
		: file(file), includeAllImagePages(includeAllImagePages), outputDirectory(outputDirectory) {}
	UniStdioFile& file;
	int index = 0;
	bool includeAllImagePages = false;
	String outputDirectory;
};

struct IMergeDoc
{
	enum class FileChange
	{
		NoChange,
		Changed,
		Removed,
	};

	enum class DocumentType
	{
		Text,         // CMergeDoc
		Table,        // CMergeDoc
		Image,        // CImgMergeFrame
		WebPage,      // CWebPageDiffFrame
		Unknown
	};

	virtual IDirDoc* GetDirDoc() const = 0;
	virtual void SetDirDoc(IDirDoc *pDirDoc) = 0;
	virtual bool CloseNow(void) = 0;
	virtual bool GenerateReport(ReportContext& reportContext) const = 0;
	virtual DocumentType GetDocumentType() const = 0;
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

