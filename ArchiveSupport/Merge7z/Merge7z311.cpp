/* File:	Merge7z311.cpp
 * Author:	Jochen Tucht 2003/12/09
 *			Copyright (C) Jochen Tucht
 *
 * Purpose:	Provide a handy C++ interface to access 7Zip services
 *
 * Remarks:	Based on 7z311 sources. May or may not compile/run with earlier or
 *			later versions. See Merge7z311.dsp for dependencies. Some less
 *			version-dependent parts of code reside in Merge7zCommon.cpp.
 *
 *	*** SECURITY ALERT ***
 *	Be aware of 2. a) of the GNU General Public License. Please log your changes
 *	at the end of this comment.
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

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2004/03/15	Jochen Tucht		Fix Visual Studio 2003 build issue
2004/08/19	Laurent Ganier		Compression of folders
								Through EnumerateDirectory (from code of 7zip)

*/

#include "stdafx.h"

#include "Merge7zCommon.h"

#include "7zip/FileManager/OpenCallback.h"
#include "7zip/FileManager/ExtractCallback.h"
#include "7zip/FileManager/UpdateCallback100.h"
#include "7zip/UI/Common/EnumDirItems.h"

/**
 * @brief Extraction thread
 */
class CThreadExtracting : CThread
{
protected:
	DWORD Process()
	{
		ExtractCallbackSpec->ProgressDialog.WaitCreating();
		result = Archive->Extract(0, -1, false, ArchiveExtractCallback);
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
public:
	HRESULT result;

	CThreadExtracting
	(
		CExtractCallbackImp *ExtractCallbackSpec,
		IInArchive *Archive,
		IArchiveExtractCallback *ArchiveExtractCallback,
		const CSysString &title
	):
		ExtractCallbackSpec(ExtractCallbackSpec),
		Archive(Archive),
		ArchiveExtractCallback(ArchiveExtractCallback)
	{
		result = E_FAIL;
		if COMPLAIN(!Create(Process, this))
		{
			Complain(_T("Failed to create extraction thread"));
		}
		ExtractCallbackSpec->StartProgressDialog(GetUnicodeString(title));
	}
};

/**
 * @brief Extraction method accessible from outside
 */
HRESULT Format7zDLL::Interface::DeCompressArchive(HWND hwndParent, LPCTSTR path, LPCTSTR folder)
{
	IInArchive *archive = 0;
	CInFileStream *file = 0;
	COpenArchiveCallback *callback = 0;
	CArchiveExtractCallback *extractCallbackSpec = 0;
	CExtractCallbackImp *extractCallbackSpec2 = 0;
	HRESULT result = 0;
	try
	{
		//	Ref counts are not always accurate with 7-Zip.
		//	An extra AddRef() ensures that interfaces remain valid until they
		//	are explicitly released at the end of this function.
		(archive = GetInArchive()) -> AddRef();
		(file = new CInFileStream) -> AddRef();
		(callback = new COpenArchiveCallback) -> AddRef();
		callback->_passwordIsDefined = false;
		callback->_parentWindow = hwndParent;
		/*CMyComBSTR password;
		callback->CryptoGetTextPassword(&password);*/
		if COMPLAIN(!file->Open(path))
		{
			ComplainCantOpen(path);
		}
		if COMPLAIN(archive->Open(file, 0, callback) != S_OK)
		{
			ComplainCantOpen(path);
		}

		NFile::NFind::CFileInfo fileInfo;
		if COMPLAIN(!NFile::NFind::FindFile(path, fileInfo))
		{
			ComplainCantOpen(path);
		}

		if (*folder)
		{
			if COMPLAIN(!NFile::NDirectory::CreateComplexDirectory(folder))
			{
				Complain(_T("Can not create output directory"));
			}
		}

		//	if path is whatever.tar.bz2, default to whatever.tar:
		UString ustrDefaultName = GetUnicodeString(path);
		int dot = ustrDefaultName.ReverseFind('.');
		int slash = ustrDefaultName.ReverseFind('\\');
		if (dot > slash)
		{
			if (StrChrW(L"Tt", ustrDefaultName[dot + 1]))
			{
				ustrDefaultName.ReleaseBuffer(dot + 2);
				ustrDefaultName += L"ar";
			}
			else
			{
				ustrDefaultName.ReleaseBuffer(dot);
			}
			ustrDefaultName.Delete(0, slash + 1);
		}
		else
		{
			ustrDefaultName = L"noname";
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
			PathFindFileName(path)
		).result;

		if COMPLAIN(extractCallbackSpec->_numErrors)
		{
			Complain(_T("%I64u error(s)"), extractCallbackSpec->_numErrors);
		}
	}
	catch (Complain *complain)
	{
		result = complain->Alert(hwndParent);
	}
	//	Always release interfaces in this order, or else all hell will break
	//	loose!
	Release(static_cast<IArchiveExtractCallback*>(extractCallbackSpec));
	Release(archive);
	Release(static_cast<IFolderArchiveExtractCallback*>(extractCallbackSpec2));
	Release(static_cast<IInStream*>(file));
	Release(static_cast<IArchiveOpenCallback*>(callback));
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
	COutFileStream *file;
public:
	HRESULT result;
	UINT32 numItems;
	CThreadUpdateCompress
	(
		CUpdateCallback100Imp *updateCallback100,
		IOutArchive *outArchive,
		CArchiveUpdateCallback *updateCallbackSpec,
		UINT32 numItems,
		COutFileStream *file,
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
			Complain(_T("Failed to create extraction thread"));
		}
		updateCallback100->StartProgressDialog(GetUnicodeString(title));
	}
};


/**
 * @brief Fill in dirItems with the files from a folder and its subfolders
 *
 * @note Duplication from 7zip source (EnumDirItems.cpp), because the function is static
 */
using namespace NFile;
using namespace NName;
static void EnumerateDirectory(
	const UString &baseFolderPrefix,
	const UString &directory, 
	const UString &prefix,
	CObjectVector<CDirItem> &dirItems)
{
	NFind::CEnumeratorW enumerator(baseFolderPrefix + directory + wchar_t(kAnyStringWildcard));
	NFind::CFileInfoW fileInfo;
	while (enumerator.Next(fileInfo))
	{
		AddDirFileInfo(prefix, directory + fileInfo.Name, fileInfo, dirItems);
		if (fileInfo.IsDirectory())
		{
			EnumerateDirectory(baseFolderPrefix, directory + fileInfo.Name + wchar_t(kDirDelimiter),
				prefix + fileInfo.Name + wchar_t(kDirDelimiter), dirItems);
		}
	}
}

/**
 * @brief Compression method accessible from outside
 *
 * @note See CAgent::DoOperation (in 7zip source) for model
 */
HRESULT Format7zDLL::Interface::CompressArchive(HWND hwndParent, LPCTSTR path, Merge7z::DirItemEnumerator *etor)
{
	UINT codePage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	IOutArchive *outArchive = 0;
	COutFileStream *file = 0;
	COpenArchiveCallback *callback = 0;
	CArchiveUpdateCallback *updateCallbackSpec = 0;
	CUpdateCallback100Imp *updateCallback100 = 0;
	HRESULT result = 0;
	try
	{
		//	Ref counts are not always accurate with 7-Zip.
		//	An extra AddRef() ensures that interfaces remain valid until they
		//	are explicitly released at the end of this function.
		(outArchive = GetOutArchive()) -> AddRef();
		(file = new COutFileStream) -> AddRef();
		(callback = new COpenArchiveCallback) -> AddRef();
		callback->_passwordIsDefined = false;
		callback->_parentWindow = hwndParent;
		/*CMyComBSTR password;
		callback->CryptoGetTextPassword(&password);*/
		if COMPLAIN(!file->Open(path))
		{
			ComplainCantOpen(path);
		}
		(updateCallbackSpec = new CArchiveUpdateCallback) -> AddRef();
		(updateCallback100 = new CUpdateCallback100Imp) -> AddRef();
		updateCallback100->Init
		(
			hwndParent,
			false,											// passwordIsDefined
			UString()										// password
		);

		// Get the fileTimeType to compare new files and files already in archive
		// We do not really care because we consider that the dest archive is empty
		/*
		NFileTimeType::EEnum fileTimeType;
		UINT32 value;
		if COMPLAIN(outArchive->GetFileTimeType(&value) != S_OK)
		{
			ComplainCantOpen(path);
		}
		switch(value)
		{
		case NFileTimeType::kWindows:
		case NFileTimeType::kDOS:
		case NFileTimeType::kUnix:
			fileTimeType = NFileTimeType::EEnum(value);
			break;
		default:
			ComplainCantOpen(path);
		}
		*/

		// First fill the items to compress
		CObjectVector<CDirItem> dirItems;
		UINT count = etor->Open();
		while (count--)
		{
			Merge7z::DirItemEnumerator::Item etorItem;
			etorItem.Mask.Item = 0;
			Merge7z::Envelope *envelope = etor->Enum(etorItem);

			// fill in the default values from the enumerator
			CDirItem item;
			if (etorItem.Mask.Item & etorItem.Mask.Name)
				item.Name = GetUnicodeString(etorItem.Name);
			if (etorItem.Mask.Item & etorItem.Mask.FullPath)
				item.FullPath = GetUnicodeString(etorItem.FullPath);
			if (etorItem.Mask.Item & etorItem.Mask.Attributes)
				item.Attributes = etorItem.Attributes;
			if (etorItem.Mask.Item & etorItem.Mask.Size)
				item.Size = etorItem.Size;
			if (etorItem.Mask.Item & etorItem.Mask.CreationTime)
				item.CreationTime = etorItem.CreationTime;
			if (etorItem.Mask.Item & etorItem.Mask.LastAccessTime)
				item.LastAccessTime = etorItem.LastAccessTime;
			if (etorItem.Mask.Item & etorItem.Mask.LastWriteTime)
				item.LastWriteTime = etorItem.LastWriteTime;
			if (envelope)
			{
				envelope->Free();
			}

			if (etorItem.Mask.Item && (etorItem.Mask.Item & (etorItem.Mask.NeedFindFile|etorItem.Mask.CheckIfPresent)) != etorItem.Mask.NeedFindFile)
			{
				// Check the info from the disk
				NFile::NFind::CFileInfoW fileInfo;
				if (NFile::NFind::FindFile(item.FullPath, fileInfo))
				{
					if (!(etorItem.Mask.Item & etorItem.Mask.Name))
						item.Name = fileInfo.Name;
					if (!(etorItem.Mask.Item & etorItem.Mask.Attributes))
						item.Attributes = fileInfo.Attributes;
					if (!(etorItem.Mask.Item & etorItem.Mask.Size))
						item.Size = fileInfo.Size;
					if (!(etorItem.Mask.Item & etorItem.Mask.CreationTime))
						item.CreationTime = fileInfo.CreationTime;
					if (!(etorItem.Mask.Item & etorItem.Mask.LastAccessTime))
						item.LastAccessTime = fileInfo.LastAccessTime;
					if (!(etorItem.Mask.Item & etorItem.Mask.LastWriteTime))
						item.LastWriteTime = fileInfo.LastWriteTime;
					dirItems.Add(item);

					// Recurse into directories (call a function of 7zip)
					const UString baseFolderPrefix = L"";
					if (fileInfo.IsDirectory())
					{
						EnumerateDirectory(baseFolderPrefix, item.FullPath + L'\\', 
								item.Name + L'\\', dirItems);
					}
				}
				else
				{
					// file not valid, forget it
					if COMPLAIN(!(etorItem.Mask.Item & etorItem.Mask.CheckIfPresent))
					{
						ComplainCantOpen(GetSystemString(item.FullPath));
					}
				}
			}
			else if (etorItem.Mask.Item)
			{
				// No check from disk, simply use info from enumerators (risky)
				dirItems.Add(item);
			}
		}

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
		CObjectVector<CArchiveItem> archiveItems;

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
		if COMPLAIN(result != S_OK)
		{
			ComplainCantOpen(path);
		}
	}
	catch (Complain *complain)
	{
		result = complain->Alert(hwndParent);
	}
	//	Always release interfaces in this order, or else all hell will break
	//	loose!
	Release(static_cast<IArchiveUpdateCallback*>(updateCallbackSpec));
	Release(outArchive);
	Release(static_cast<IFolderArchiveUpdateCallback*>(updateCallback100));
	Release(static_cast<IOutStream*>(file));
	Release(static_cast<IArchiveOpenCallback*>(callback));
	return result;
}
