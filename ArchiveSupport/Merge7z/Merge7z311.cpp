/* Merge7z311.cpp: Provide a handy C++ interface to access 7Zip services
 * Copyright (c) 2003 Jochen Tucht
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Remarks:	Based on 7z311 sources. May or may not compile/run with earlier or
 *			later versions. See Merge7z311.dsp for dependencies. Some less
 *			version-dependent parts of code reside in Merge7zCommon.cpp.

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/12/09	Jochen Tucht		Created
2004/03/15	Jochen Tucht		Fix Visual Studio 2003 build issue
2004/08/19	Laurent Ganier		Compression of folders
								Through EnumerateDirectory (from code of 7zip)
2004/10/17	Jochen Tucht		Leave decision whether to recurse into folders
								to enumerator (Mask.Recurse)
2005/01/15	Jochen Tucht		Changed as explained in revision.txt
2005/02/26	Jochen Tucht		Changed as explained in revision.txt
2005/03/19	Jochen Tucht		Don't show error message on intentional abort
*/

#include "stdafx.h"

#define INITGUID
#include <initguid.h>

#include "Merge7zCommon.h"

#include "7zip/FileManager/OpenCallback.h"
#include "7zip/FileManager/ExtractCallback.h"
#include "7zip/FileManager/UpdateCallback100.h"

#include "7zip/UI/Agent/ArchiveExtractCallback.h"
#include "7zip/UI/Agent/ArchiveUpdateCallback.h"

/**
 * @brief Extraction thread
 */
class CThreadExtracting : CThread
{
protected:
	DWORD Process()
	{
		ExtractCallbackSpec->ProgressDialog.WaitCreating();
		result = Archive->Extract(indices, numItems, false, ArchiveExtractCallback);
		ExtractCallbackSpec->ProgressDialog.MyClose();
		return 0;
	}
	static DWORD WINAPI Process(void *param)
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
		if COMPLAIN(!Create(Process, this))
		{
			Complain(GetLastError(), NULL);
		}
		ExtractCallbackSpec->StartProgressDialog(GetUnicodeString(title));
	}
};

/**
 * @brief Construct Inspector
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
	(archive = format->GetInArchive()) -> AddRef();
	(file = new CInFileStream) -> AddRef();
	(callback = callbackImpl) -> AddRef();
	callbackImpl->_passwordIsDefined = false;
	callbackImpl->_parentWindow = hwndParent;
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
				Complain(_T("Can not create output directory"));
			}
		}

		(extractCallbackSpec2 = new CExtractCallbackImp) -> AddRef();

		extractCallbackSpec2->Init
		(
			NExtractionMode::NOverwrite::kWithoutPrompt,	// overwriteMode
			false,											// passwordIsDefined
			UString()										// password
		);

		extractCallbackSpec2->_parentWindow = hwndParent;

		extractCallbackSpec2->ProgressDialog.MainWindow = 0;
		(extractCallbackSpec = new CArchiveExtractCallback) -> AddRef();

		extractCallbackSpec->Init
		(
			archive, 
			extractCallbackSpec2,
			GetUnicodeString(folder),
			NExtractionMode::NPath::kFullPathnames, 
			NExtractionMode::NOverwrite::kWithoutPrompt,
			UStringVector(),
			ustrDefaultName, 
			fileInfo.LastWriteTime,
			fileInfo.Attributes
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
	DWORD Process()
	{
		updateCallback100->ProgressDialog.WaitCreating();
		result = outArchive->UpdateItems(file, numItems, updateCallbackSpec);
		updateCallback100->ProgressDialog.MyClose();
		return 0;
	}
	static DWORD WINAPI Process(void *param)
	{
		return ((CThreadUpdateCompress *)param)->Process();
	}
	CUpdateCallback100Imp *updateCallback100;
	IOutArchive *outArchive;
	CArchiveUpdateCallback *updateCallbackSpec;
	IOutStream *file;
public:
	HRESULT result;
	UINT32 numItems;
	CThreadUpdateCompress
	(
		CUpdateCallback100Imp *updateCallback100,
		IOutArchive *outArchive,
		CArchiveUpdateCallback *updateCallbackSpec,
		UINT32 numItems,
		IOutStream *file,
		const CSysString &title
	):
		updateCallback100(updateCallback100),
		outArchive(outArchive),
		updateCallbackSpec(updateCallbackSpec),
		numItems(numItems),
		file(file)
	{
		result = E_FAIL;
		if COMPLAIN(!Create(Process, this))
		{
			Complain(GetLastError(), NULL);
		}
		updateCallback100->StartProgressDialog(GetUnicodeString(title));
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
	if COMPLAIN(!file->Open(path))
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
	CUpdateCallback100Imp *updateCallback100 = 0;
	HRESULT result = 0;
	try
	{
		//	Ref counts are not always accurate with 7-Zip.
		//	An extra AddRef() ensures that interfaces remain valid until they
		//	are explicitly released at the end of this function.
		(updateCallbackSpec = new CArchiveUpdateCallback) -> AddRef();
		(updateCallback100 = new CUpdateCallback100Imp) -> AddRef();
		updateCallback100->Init
		(
			hwndParent,
			false,											// passwordIsDefined
			UString()										// password
		);

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

		// No items in dest archive. We always recreate the dest archive

		// Now compress...
		updateCallbackSpec->Init(UString()/*folderPrefix*/, &dirItems, &archiveItems,
			&operationChain, NULL, updateCallback100);

		result = CThreadUpdateCompress
		(
			updateCallback100,
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
	Release(static_cast<IFolderArchiveUpdateCallback*>(updateCallback100));
	return result;
}
