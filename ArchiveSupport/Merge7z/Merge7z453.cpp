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
2007-08-30	Jochen Neubeck		Branched from Merge7z409.cpp
*/

#include "stdafx.h"

#define INITGUID
#include <initguid.h>

#include "Merge7zCommon.h"

#include "7zip/UI/FileManager/OpenCallback.h"
#include "7zip/UI/FileManager/ExtractCallback.h"

#include "7zip/UI/Common/ArchiveExtractCallback.h"
#include "7zip/UI/GUI/UpdateCallbackGUI.h"

#include "Windows/Thread.h"

/**
 * @brief Extraction thread
 */
class CThreadExtracting : CThread
{
protected:
	THREAD_FUNC_RET_TYPE Process()
	{
		ExtractCallbackSpec->ProgressDialog.WaitCreating();
		result = Archive->Extract(indices, numItems, false, ArchiveExtractCallback);
		ExtractCallbackSpec->ProgressDialog.MyClose();
		return 0;
	}
	static THREAD_FUNC_RET_TYPE WINAPI Process(void *param)
	{
		return ((CThreadExtracting *)param)->Process();
	}
	CExtractCallbackImp *ExtractCallbackSpec;
	IInArchive *Archive;
	IArchiveExtractCallback *ArchiveExtractCallback;
	const UINT32 *indices;
	UINT32 numItems;
public:
	HRESULT result;

	CThreadExtracting
	(
		CExtractCallbackImp *ExtractCallbackSpec,
		IInArchive *Archive,
		IArchiveExtractCallback *ArchiveExtractCallback,
		const CSysString &title,
		const UINT32 *indices = 0,
		UINT32 numItems = -1
	):
		ExtractCallbackSpec(ExtractCallbackSpec),
		Archive(Archive),
		ArchiveExtractCallback(ArchiveExtractCallback),
		indices(indices),
		numItems(numItems)
	{
		result = E_FAIL;
		if (HRESULT hr = Create(Process, this))
		{
			Complain(hr, NULL);
		}
		ExtractCallbackSpec->StartProgressDialog(GetUnicodeString(title));
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
	COpenArchiveCallback *callbackImpl = new COpenArchiveCallback;
	//COpenCallbackImp *callbackImpl = new COpenCallbackImp;
	(archive = format->GetInArchive()) -> AddRef();
	(file = new CInFileStream) -> AddRef();
	(callback = callbackImpl) -> AddRef();
	callbackImpl->PasswordIsDefined = false;
	callbackImpl->ParentWindow = hwndParent;
	/*CMyComBSTR password;
	callback->CryptoGetTextPassword(&password);*/
	if COMPLAIN(!NFile::NFind::FindFile(path, fileInfo))
	{
		Complain(ERROR_FILE_NOT_FOUND, path);
	}
	if COMPLAIN(!file->Open(path))
	{
		Complain(ERROR_OPEN_FAILED, path);
	}
	if COMPLAIN(archive->Open(file, 0, callback) != S_OK)
	{
		Complain(ERROR_CANT_ACCESS_FILE, path);
	}
	passwordIsDefined = callbackImpl->PasswordIsDefined;
	password = callbackImpl->Password;
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
			if COMPLAIN(!NFile::NDirectory::CreateComplexDirectory(folder))
			{
				Complain(ERROR_CANNOT_MAKE, folder);
			}
		}

		(extractCallbackSpec2 = new CExtractCallbackImp) -> AddRef();

		extractCallbackSpec2->Init();

		extractCallbackSpec2->ParentWindow = hwndParent;
		extractCallbackSpec2->OverwriteMode = NExtract::NOverwriteMode::kWithoutPrompt;
		extractCallbackSpec2->PasswordIsDefined = passwordIsDefined;
		extractCallbackSpec2->Password = password;

		extractCallbackSpec2->ProgressDialog.MainWindow = 0;
		(extractCallbackSpec = new CArchiveExtractCallback) -> AddRef();

		extractCallbackSpec->InitForMulti
		(
			false,
			NExtract::NPathMode::kFullPathnames,
			NExtract::NOverwriteMode::kWithoutPrompt
		);
		extractCallbackSpec->Init
		(
			archive, 
			extractCallbackSpec2,
			false,											//stdOutMode
			GetUnicodeString(folder),
			UStringVector(),
			ustrDefaultName,
			fileInfo.LastWriteTime,
			fileInfo.Attributes,
			(UInt64)(Int64)-1
		);

		result = CThreadExtracting
		(
			extractCallbackSpec2,
			archive,
			extractCallbackSpec,
			PathFindFileName(path),
			indices,
			numItems
		).result;

		if COMPLAIN(extractCallbackSpec->_numErrors)
		{
			//	There is no canned system message for this one, so it won't
			//	localize. I can't help it.
			Complain(_T("%s:\n%I64u error(s)"), path, extractCallbackSpec->_numErrors);
		}
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
class CThreadUpdateCompress : CThread
{
protected:
	THREAD_FUNC_RET_TYPE Process()
	{
		updateCallbackGUI->ProgressDialog.WaitCreating();
		result = outArchive->UpdateItems(file, numItems, updateCallbackSpec);
		updateCallbackGUI->ProgressDialog.MyClose();
		return 0;
	}
	static THREAD_FUNC_RET_TYPE WINAPI Process(void *param)
	{
		return ((CThreadUpdateCompress *)param)->Process();
	}
	CUpdateCallbackGUI *updateCallbackGUI;
	IOutArchive *outArchive;
	CArchiveUpdateCallback *updateCallbackSpec;
	COutFileStream *file;
public:
	HRESULT result;
	UINT32 numItems;
	CThreadUpdateCompress
	(
		CUpdateCallbackGUI *updateCallbackGUI,
		IOutArchive *outArchive,
		CArchiveUpdateCallback *updateCallbackSpec,
		UINT32 numItems,
		COutFileStream *file,
		const CSysString &title
	):
		updateCallbackGUI(updateCallbackGUI),
		outArchive(outArchive),
		updateCallbackSpec(updateCallbackSpec),
		numItems(numItems),
		file(file)
	{
		result = E_FAIL;
		if (HRESULT hr = Create(Process, this))
		{
			Complain(hr, NULL);
		}
		updateCallbackGUI->StartProgressDialog(GetUnicodeString(title));
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
	(file = new COutFileStream) -> AddRef();
	if COMPLAIN(!file->Create(path, true))
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
		(updateCallbackSpec = new CArchiveUpdateCallback) -> AddRef();
		(updateCallbackGUI = new CUpdateCallbackGUI);// -> AddRef();

		// No items in dest archive. We always recreate the dest archive
		// Build the operationChain. One element per item
		CObjectVector<CUpdatePair2> operationChain;
		CUpdatePair2 pair2;
		pair2.IsAnti = false;
		pair2.ExistInArchive = false;
		pair2.ExistOnDisk = true;
		pair2.NewData = pair2.NewProperties = true;

		operationChain.Reserve(dirItems.Size());
		int i;
		for (i = 0 ; i < dirItems.Size() ; i++)
		{
			pair2.DirItemIndex = i;
			operationChain.Add(pair2);
		}

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
		updateCallbackSpec->ArchiveItems = &archiveItems;
		updateCallbackSpec->UpdatePairs = &operationChain;
		updateCallbackSpec->Callback = updateCallbackGUI;

		result = CThreadUpdateCompress
		(
			updateCallbackGUI,
			outArchive,
			updateCallbackSpec,
			operationChain.Size(),
			file,
			PathFindFileName(path)
		).result;

		//result = outArchive->UpdateItems(file, operationChain.Size(), updateCallbackSpec);
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
