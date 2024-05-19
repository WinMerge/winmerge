/*/Merge7z453.cpp

Copyright (c) Jochen Neubeck

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Please mind 2. b) of the GNU LGPL terms, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2009-09-26	Jochen Neubeck		Branched from Merge7z459.cpp
2010-04-24	Jochen Neubeck		Fix compression
*/

#include "stdafx.h"

#define INITGUID
#include <initguid.h>

#include "Merge7zCommon.h"

#include "7zip/UI/FileManager/OpenCallback.h"
#include "7zip/UI/FileManager/ExtractCallback.h"
#include "7zip/UI/FileManager/resourceGui.h"
#include "7zip/UI/FileManager/LangUtils.h"

#include "7zip/UI/Common/ArchiveExtractCallback.h"
#include "7zip/UI/GUI/UpdateCallbackGUI.h"

#include "Windows/Thread.h"

/**
 * @brief Extraction thread
 */
class CThreadExtracting : public CProgressThreadVirt
{
protected:
	virtual HRESULT ProcessVirt()
	{
		HRESULT result = Archive->Extract(indices, numItems, false, ArchiveExtractCallback);
		return result;
	}
	CExtractCallbackImp *ExtractCallbackSpec;
	IInArchive *Archive;
	IArchiveExtractCallback *ArchiveExtractCallback;
	const UINT32 *indices;
	UINT32 numItems;
public:
	//HRESULT result;

	CThreadExtracting
	(
		CExtractCallbackImp *ExtractCallbackSpec,
		IInArchive *Archive,
		IArchiveExtractCallback *ArchiveExtractCallback,
		const CSysString &title,
		HWND hwndParent,
		const UINT32 *indices = 0,
		UINT32 numItems = -1
	):
		ExtractCallbackSpec(ExtractCallbackSpec),
		Archive(Archive),
		ArchiveExtractCallback(ArchiveExtractCallback),
		indices(indices),
		numItems(numItems)
	{
		ExtractCallbackSpec->ProgressDialog = this;
		CompressingMode = false;
		//result = E_FAIL;
		if (HRESULT hr = Create(GetUnicodeString(title), hwndParent))
		{
			Complain(hr, NULL);
		}
	}
};

struct CThreadArchiveOpen
{
	UString Path;
	CInFileStream* File;
	NFile::NFind::CFileInfo* FileInfo;
	IInArchive* Archive;
	CMyComPtr<IProgress> OpenCallback;
	COpenCallbackImp* OpenCallbackSpec;
	COpenArchiveCallback OpenArchiveCallbackSpec;
	HRESULT Result;

	void Process()
	{
		CProgressCloser closer(OpenArchiveCallbackSpec.ProgressDialog);

		if COMPLAIN(!NFile::NFind::CFindFile().FindFirst(Path, *FileInfo))
		{
			Result = ERROR_FILE_NOT_FOUND;
			return;
		}
		if COMPLAIN(!File->Open(Path))
		{
			Result = ERROR_OPEN_FAILED;
			return;
		}
		static const UInt64 maxCheckStartPosition = ULLONG_MAX;
		if COMPLAIN(Archive->Open(File, &maxCheckStartPosition, OpenCallbackSpec) != S_OK)
		{
			Result = ERROR_CANT_ACCESS_FILE;
			return;
		}
		Result = ERROR_SUCCESS;
	}

	static THREAD_FUNC_DECL MyThreadFunction(void* param)
	{
		((CThreadArchiveOpen*)param)->Process();
		return 0;
	}
};

/**
 * @brief Initialize Inspector
 */
Format7zDLL::Interface::Inspector::Inspector(Format7zDLL::Interface *format, LPCTSTR path)
: format(format), archive(0), file(0), callback(0), path(path), ustrDefaultName(GetUnicodeString(path))
{
}

/**
 * @brief Initialize Inspector
 */
void Format7zDLL::Interface::Inspector::Init(HWND hwndParent)
{
	format->GetDefaultName(hwndParent, ustrDefaultName);

	CThreadArchiveOpen t;
	t.OpenCallbackSpec = new COpenCallbackImp;
	t.OpenCallbackSpec->Callback = &t.OpenArchiveCallbackSpec;
	t.OpenCallback = t.OpenCallbackSpec;
	t.OpenArchiveCallbackSpec.PasswordIsDefined = false;
	t.OpenArchiveCallbackSpec.ParentWindow = hwndParent;

	(archive = format->GetInArchive())->AddRef();
	static_cast<IInStream*>(file = new CInFileStream)->AddRef();
	t.File = file;
	t.Path = path;
	t.FileInfo = &fileInfo;
	t.Archive = archive;

	UString progressTitle = LangString(IDS_OPENNING);
	t.OpenArchiveCallbackSpec.ProgressDialog.MainWindow = hwndParent;
	t.OpenArchiveCallbackSpec.ProgressDialog.MainTitle = "7-Zip"; // LangString(IDS_APP_TITLE);
	t.OpenArchiveCallbackSpec.ProgressDialog.MainAddTitle = progressTitle + L' ';
	t.OpenArchiveCallbackSpec.ProgressDialog.WaitMode = true;

	{
		NWindows::CThread thread;
		thread.Create(CThreadArchiveOpen::MyThreadFunction, &t);
		t.OpenArchiveCallbackSpec.StartProgressDialog(progressTitle, thread);
	}
	if (t.Result != ERROR_SUCCESS)
		Complain(t.Result, path);

	passwordIsDefined = t.OpenArchiveCallbackSpec.PasswordIsDefined;
	password = t.OpenArchiveCallbackSpec.Password;
}

/**
 * @brief Extract set of items specified by index
 */
HRESULT Format7zDLL::Interface::Inspector::Extract(HWND hwndParent, LPCTSTR folder, const UINT32 *indices, UINT32 numItems)
{
	CArchiveExtractCallback *extractCallbackSpec = 0;
	CExtractCallbackImp *extractCallbackSpec2 = 0;
	HRESULT result = 0;
	try
	{
		if (*folder)
		{
			if COMPLAIN(!NFile::NDir::CreateComplexDir(folder))
			{
				Complain(ERROR_CANNOT_MAKE, folder);
			}
		}

		static_cast<IFolderArchiveExtractCallback*>(extractCallbackSpec2 = new CExtractCallbackImp) -> AddRef();

		extractCallbackSpec2->Init();

		extractCallbackSpec2->OverwriteMode = NExtract::NOverwriteMode::kOverwrite;
		extractCallbackSpec2->PasswordIsDefined = passwordIsDefined;
		extractCallbackSpec2->Password = password;

		static_cast<IArchiveExtractCallback*>(extractCallbackSpec = new CArchiveExtractCallback) -> AddRef();

		extractCallbackSpec->InitForMulti
		(
			false,
			NExtract::NPathMode::kFullPaths,
			NExtract::NOverwriteMode::kOverwrite,
			NExtract::NZoneIdMode::kNone,
			false
		);

		CExtractNtOptions ntOptions;
		CArc arc;
		arc.Archive = archive;
		arc.Path = GetUnicodeString(path);
		arc.DefaultName = ustrDefaultName;
		arc.FormatIndex = 0;
		arc.SubfileIndex = 0;
		extractCallbackSpec->Init
		(
			ntOptions,
			NULL,
			&arc,
			extractCallbackSpec2,
			false,											//stdOutMode
			false,											//testMode
			GetUnicodeString(folder),
			UStringVector(), false,
			(UInt64)(Int64)-1
		);

		result = CThreadExtracting
		(
			extractCallbackSpec2,
			archive,
			extractCallbackSpec,
			PathFindFileName(path),
			hwndParent,
			indices,
			numItems
		).Result;

		//.result;

		/*if COMPLAIN(extractCallbackSpec->_numErrors)
		{
			//	There is no canned system message for this one, so it won't
			//	localize. I can't help it.
			Complain(_T("%s:\n%I64u error(s)"), path, extractCallbackSpec->_numErrors);
		}*/
		if COMPLAIN(result != S_OK && result != E_ABORT)
		{
			Complain(result, path);
		}
	}
	catch (Complain *complain)
	{
		result = complain->Alert(hwndParent);
	}
	Release(static_cast<IArchiveExtractCallback*>(extractCallbackSpec));
	Release(static_cast<IFolderArchiveExtractCallback*>(extractCallbackSpec2));
	return result;
}

/**
 * @brief Compression thread
 */
class CThreadUpdating : public CProgressThreadVirt
{
protected:
	virtual HRESULT ProcessVirt()
	{
		HRESULT result = outArchive->UpdateItems(file, numItems, updateCallbackSpec);
		return result;
	}
	CUpdateCallbackGUI *updateCallbackGUI;
	IOutArchive *outArchive;
	CArchiveUpdateCallback *updateCallbackSpec;
	COutFileStream *file;
public:
	HRESULT result;
	UINT32 numItems;

	CThreadUpdating
	(
		CUpdateCallbackGUI *updateCallbackGUI,
		IOutArchive *outArchive,
		CArchiveUpdateCallback *updateCallbackSpec,
		UINT32 numItems,
		COutFileStream *file,
		const CSysString &title,
		HWND hwndParent
	):
		updateCallbackGUI(updateCallbackGUI),
		outArchive(outArchive),
		updateCallbackSpec(updateCallbackSpec),
		numItems(numItems),
		file(file)
	{
		result = E_FAIL;
		updateCallbackGUI->ProgressDialog = this;
		if (HRESULT hr = Create(GetUnicodeString(title), hwndParent))
		{
			Complain(hr, NULL);
		}
	}
};

/**
 * @brief Construct Updater
 */
Format7zDLL::Interface::Updater::Updater(Format7zDLL::Interface *format, LPCTSTR path)
: format(format), outArchive(0), file(0), path(path)
{
}

/**
 * @brief Initialize Updater
 */
void Format7zDLL::Interface::Updater::Init(HWND hwndParent)
{
	(outArchive = format->GetOutArchive()) -> AddRef();
	static_cast<IOutStream*>(file = new COutFileStream) -> AddRef();
	if COMPLAIN(!file->Create_ALWAYS(path))
	{
		Complain(ERROR_CANNOT_MAKE, path);
	}
}

/**
 * @brief Commit update
 */
HRESULT Format7zDLL::Interface::Updater::Commit(HWND hwndParent)
{
	CArchiveUpdateCallback *updateCallbackSpec = 0;
	CUpdateCallbackGUI *updateCallbackGUI = 0;
	HRESULT result = 0;
	try
	{
		//	Ref counts are not always accurate with 7-Zip.
		//	An extra AddRef() ensures that interfaces remain valid until they
		//	are explicitly released at the end of this function.
		static_cast<IArchiveUpdateCallback2*>((updateCallbackSpec = new CArchiveUpdateCallback)) -> AddRef();
		(updateCallbackGUI = new CUpdateCallbackGUI);// -> AddRef();

		updateCallbackGUI->Init();

		// No items in dest archive. We always recreate the dest archive
		// Build the operationChain. One element per item
		CRecordVector<CUpdatePair2> operationChain;
		CUpdatePair2 pair2;
		pair2.NewData = pair2.NewProps = true;

		operationChain.Reserve(dirItems.Items.Size());
		//
		unsigned i;
		for (i = 0 ; i < dirItems.Items.Size() ; i++)
		{
			pair2.DirIndex = i;
			operationChain.Add(pair2);
		}
		operationChain.ReserveDown();

		/* The following code may or may not help implement update on existing
		/* archives, which is currently not supported.
		UINT32 fileTimeType;
		outArchive->GetFileTimeType(&fileTimeType);

		CObjectVector<CUpdatePair> updatePairs;
		GetUpdatePairInfoList(dirItems, archiveItems, NFileTimeType::EEnum(fileTimeType), updatePairs); // must be done only once!!!
  
		CObjectVector<CUpdatePair2> operationChain;
		UpdateProduce(dirItems, archiveItems, updatePairs, NUpdateArchive::kAddActionSet, operationChain);*/

		// Now compress...
		updateCallbackSpec->DirItems = &dirItems;
		updateCallbackSpec->ArcItems = &archiveItems;
		updateCallbackSpec->UpdatePairs = &operationChain;
		updateCallbackSpec->Callback = updateCallbackGUI;

		result = CThreadUpdating
		(
			updateCallbackGUI,
			outArchive,
			updateCallbackSpec,
			operationChain.Size(),
			file,
			PathFindFileName(path),
			hwndParent
		).Result;

		if COMPLAIN(result != S_OK && result != E_ABORT)
		{
			Complain(result, path);
		}
	}
	catch (Complain *complain)
	{
		result = complain->Alert(hwndParent);
	}
	//	Always release interfaces in this order, or else all hell will break
	//	loose!
	Release(static_cast<IArchiveUpdateCallback*>(updateCallbackSpec));
	delete updateCallbackGUI;
	return result;
}
