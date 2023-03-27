#include <StdAfx.h>
#include "DropHandler.h"
#include <memory>
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <comip.h>
#include "paths.h"
#include "Environment.h"
#include "unicoder.h"

namespace
{
	struct HandleDeleter {
		typedef HANDLE pointer;
		void operator()(HANDLE h) { if (h != nullptr && h != INVALID_HANDLE_VALUE) ::CloseHandle(h); }
	};

	typedef std::unique_ptr<HANDLE, HandleDeleter> unique_handle;
	typedef _com_ptr_t<_com_IIID<IFileOperation, &__uuidof(IFileOperation)>> IFileOperationPtr;
	typedef _com_ptr_t<_com_IIID<IShellItem, &__uuidof(IShellItem)>> IShellItemPtr;

	bool CopyFileOrFolder(const String& src, const String& dst)
	{
		std::vector<tchar_t> srcpath(src.length() + 2, 0);
		std::vector<tchar_t> dstpath(dst.length() + 2, 0);
		memcpy(&srcpath[0], src.c_str(), src.length() * sizeof(tchar_t));
		memcpy(&dstpath[0], dst.c_str(), dst.length() * sizeof(tchar_t));
		SHFILEOPSTRUCT fileop = { 0, FO_COPY, &srcpath[0], &dstpath[0], FOF_NOCONFIRMATION, 0, 0, 0 };
		return SHFileOperation(&fileop) == 0;
	}

	//
	//	OnDropFiles code from CDropEdit
	//	Copyright 1997 Chris Losinger
	//
	//	shortcut expansion code modified from :
	//	CShortcut, 1996 Rob Warner
	//

	std::vector<String> GetDroppedFiles(HDROP dropInfo)
	{
		std::vector<String> files;
		// Get the number of pathnames that have been dropped
		UINT wNumFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, nullptr, 0);

		// get all file names. but we'll only need the first one.
		for (UINT x = 0; x < wNumFilesDropped; x++)
		{
			// Get the number of bytes required by the file's full pathname
			UINT wPathnameSize = DragQueryFile(dropInfo, x, nullptr, 0);

			// Allocate memory to contain full pathname & zero byte
			wPathnameSize += 1;
			auto npszFile = std::make_unique<tchar_t[]>(wPathnameSize);

			// Copy the pathname into the buffer
			DragQueryFile(dropInfo, x, npszFile.get(), wPathnameSize);

			files.push_back(npszFile.get());
		}
		return files;
	}

	std::vector<String> FilterFiles(const std::vector<String>& files_src)
	{
		std::vector<String> files(files_src);
		tchar_t szTempPath[MAX_PATH];
		tchar_t szTempPathShort[MAX_PATH];
		GetTempPath(sizeof(szTempPath) / sizeof(szTempPath[0]), szTempPath);
		GetShortPathName(szTempPath, szTempPathShort, sizeof(szTempPathShort) / sizeof(szTempPathShort[0]));

		for (UINT i = 0; i < files.size(); i++)
		{
			if (paths::IsShortcut(files[i]))
			{
				// if this was a shortcut, we need to expand it to the target path
				String expandedFile = paths::ExpandShortcut(files[i]);

				// if that worked, we should have a real file name
				if (!expandedFile.empty())
					files[i] = expandedFile;
			}
			else if (paths::IsDecendant(files[i], szTempPath) || paths::IsDecendant(files[i], szTempPathShort))
			{
				String tmpdir = env::GetTempChildPath();
				CopyFileOrFolder(files[i], tmpdir);
				files[i] = paths::ConcatPath(tmpdir, paths::FindFileName(files[i]));
			}
		}
		return files;
	}

	HRESULT IStream_WriteToFile(IStream *pStream, const String& filename)
	{
		unique_handle hFile(CreateFile(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (hFile.get() == INVALID_HANDLE_VALUE)
			return E_FAIL;
		for (;;)
		{
			char buf[65536];
			DWORD dwWritten;
			ULONG size = 0;
			HRESULT hr = pStream->Read(buf, sizeof(buf), &size);
			if (FAILED(hr))
				return hr;
			if (size == 0)
				break;
			WriteFile(hFile.get(), buf, size, &dwWritten, nullptr);
		}
		return S_OK;
	}

	HRESULT HGLOBAL_WriteToFile(HGLOBAL hGlobal, const String& filename)
	{
		unique_handle hFile(CreateFile(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (hFile.get() == INVALID_HANDLE_VALUE)
			return E_FAIL;
		char *p = static_cast<char *>(GlobalLock(hGlobal));
		if (p == nullptr)
			return E_FAIL;
		SIZE_T size = GlobalSize(hGlobal);
		while (size > 0)
		{
			DWORD dwWritten;
			if (WriteFile(hFile.get(), p, (size > INT_MAX) ? INT_MAX : static_cast<DWORD>(size), &dwWritten, nullptr) == FALSE)
			{
				GlobalUnlock(hGlobal);
				return E_FAIL;
			}
			p += dwWritten;
			size -= dwWritten;
		}
		GlobalUnlock(hGlobal);
		return S_OK;
	}

	HRESULT SetFileWriteTime(const String& filename, const FILETIME& writetime)
	{
		unique_handle hFile(CreateFile(filename.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (hFile.get() == INVALID_HANDLE_VALUE)
			return E_FAIL;
		return SetFileTime(hFile.get(), nullptr, nullptr, &writetime) ? S_OK : E_FAIL;
	}

	HRESULT GetFileItemsFromIDataObject_CF_HDROP(IDataObject *pDataObj, std::vector<String>& files)
	{
		FORMATETC fmtetc_cf_hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM medium = { 0 };
		HRESULT hr;
		if ((hr = pDataObj->GetData(&fmtetc_cf_hdrop, &medium)) == S_OK)
		{
			HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
			if (hDrop != nullptr)
			{
				files = FilterFiles(GetDroppedFiles(hDrop));
				GlobalUnlock(medium.hGlobal);
			}
			ReleaseStgMedium(&medium);
		}
		return hr;
	}

#define HIDA_GetPIDLFolder(pida) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define HIDA_GetPIDLItem(pida, i) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])

	HRESULT GetFileItemsFromIDataObject_ShellIDList(IDataObject *pDataObj, std::vector<String>& root_files)
	{
		String tmpdir;
		FORMATETC fmtetc_filedescriptor = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_SHELLIDLIST)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM medium = { 0 };
		HRESULT hr;
		if ((hr = pDataObj->GetData(&fmtetc_filedescriptor, &medium)) == S_OK)
		{
			CIDA *pcida = (CIDA *)GlobalLock(medium.hGlobal);
			if (pcida  != nullptr)
			{
				LPCITEMIDLIST pidlParent = HIDA_GetPIDLFolder(pcida);
				for (unsigned i = 0; i < pcida->cidl; ++i)
				{
					IShellItemPtr pShellItem;
					if (SUCCEEDED(hr = SHCreateShellItem(pidlParent, nullptr, HIDA_GetPIDLItem(pcida, i), &pShellItem)))
					{
						SFGAOF sfgaof = 0;
						if (SUCCEEDED(hr = pShellItem->GetAttributes(SFGAO_FOLDER, &sfgaof)) && (sfgaof & SFGAO_FOLDER))
						{
							// Folder item
							wchar_t *pPath = nullptr;
							if (SUCCEEDED(hr = pShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pPath)))
							{
								root_files.push_back(ucr::toTString(pPath));
								CoTaskMemFree(pPath);
							}
						}
						else
						{
							// File item
							IFileOperationPtr pFileOperation;
							if (SUCCEEDED(hr = pFileOperation.CreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL)))
							{
								if (tmpdir.empty())
									tmpdir = env::GetTempChildPath();
								pFileOperation->SetOperationFlags(0);
								PIDLIST_ABSOLUTE pidlDest;
								if (SUCCEEDED(hr = SHParseDisplayName(ucr::toUTF16(tmpdir).c_str(), nullptr, &pidlDest, 0, nullptr)))
								{
									IShellItemPtr pShellItemDest;
									SHCreateShellItem(nullptr, nullptr, pidlDest, &pShellItemDest);
									pFileOperation->CopyItem(pShellItem, pShellItemDest, nullptr, nullptr);
									if (SUCCEEDED(hr = pFileOperation->PerformOperations()))
									{
										wchar_t *pName;
										if (SUCCEEDED(hr = pShellItem->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pName)))
										{
											root_files.push_back(paths::ConcatPath(tmpdir, ucr::toTString(pName)));
											CoTaskMemFree(pName);
										}
									}
								}
							}
						}
					}
				}
				GlobalUnlock(medium.hGlobal);
			}
			ReleaseStgMedium(&medium);
		}
		return hr;
	}

	HRESULT ExtractFileItemFromIDataObject_FileContents(IDataObject *pDataObj, int lindex, const String& filepath)
	{
		FORMATETC fmtetc_filecontents = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_FILECONTENTS)), nullptr, DVASPECT_CONTENT, lindex, TYMED_HGLOBAL | TYMED_ISTREAM };
		STGMEDIUM medium = { 0 };
		HRESULT hr;
		if ((hr = pDataObj->GetData(&fmtetc_filecontents, &medium)) == S_OK)
		{
			hr = E_FAIL;
			if (medium.tymed == TYMED_HGLOBAL)
				hr = HGLOBAL_WriteToFile(medium.hGlobal, filepath);
			else if (medium.tymed == TYMED_ISTREAM)
				hr = IStream_WriteToFile(medium.pstm, filepath);
			ReleaseStgMedium(&medium);
		}
		return hr;
	}

	HRESULT GetFileItemsFromIDataObject_FileDescriptor(IDataObject *pDataObj, std::vector<String>& root_files)
	{
		String tmpdir = env::GetTempChildPath();
		FORMATETC fmtetc_filedescriptor = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM medium = { 0 };
		HRESULT hr;
		if ((hr = pDataObj->GetData(&fmtetc_filedescriptor, &medium)) == S_OK)
		{
			FILEGROUPDESCRIPTOR *file_group_descriptor = (FILEGROUPDESCRIPTOR *)GlobalLock(medium.hGlobal);
			if (file_group_descriptor)
			{
				for (unsigned i = 0; i < file_group_descriptor->cItems; ++i)
				{
					String filename = file_group_descriptor->fgd[i].cFileName;
					String filepath = paths::ConcatPath(tmpdir, filename);
					if (file_group_descriptor->fgd[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						paths::CreateIfNeeded(filepath);
					else
					{
						ExtractFileItemFromIDataObject_FileContents(pDataObj, i, filepath);
						if (file_group_descriptor->fgd[i].dwFlags & FD_WRITESTIME)
							SetFileWriteTime(filepath, file_group_descriptor->fgd[i].ftLastWriteTime);
					}
					if (filename.find('\\') == String::npos)
						root_files.push_back(filepath);
				}
				GlobalUnlock(medium.hGlobal);
			}
			ReleaseStgMedium(&medium);
		}
		return hr;
	}

}

DropHandler::DropHandler(std::function<void(const std::vector<String>&)> callback) 
	: m_cRef(0), m_callback(callback)
{
}

DropHandler::~DropHandler() = default;

HRESULT STDMETHODCALLTYPE DropHandler::QueryInterface(REFIID riid, void **ppvObject)
{
	if (!IsEqualIID(riid, IID_IUnknown) && !IsEqualIID(riid, IID_IDropTarget))
	{
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}
	*ppvObject = static_cast<IDropTarget *>(this);
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE DropHandler::AddRef(void)
{
	return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE DropHandler::Release(void)
{
	ULONG cRef = InterlockedDecrement(&m_cRef); 
	if (cRef == 0) {
		delete this;
		return 0;
	}
	return cRef;
}

HRESULT STDMETHODCALLTYPE DropHandler::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	FORMATETC fmtetc_cf_hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	FORMATETC fmtetc_shellidlist = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_SHELLIDLIST)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	FORMATETC fmtetc_filedescriptor = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (pDataObj->QueryGetData(&fmtetc_cf_hdrop) == S_OK ||
	    pDataObj->QueryGetData(&fmtetc_shellidlist) == S_OK ||
	    pDataObj->QueryGetData(&fmtetc_filedescriptor) == S_OK)
	{
		*pdwEffect = DROPEFFECT_COPY;
		return S_OK;
	}
	*pdwEffect = DROPEFFECT_NONE;
	return DRAGDROP_S_CANCEL;
}

HRESULT STDMETHODCALLTYPE DropHandler::DragOver(DWORD, POINTL, DWORD *)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE DropHandler::DragLeave(void)
{
	return S_OK;
}

HRESULT DropHandler::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	bool ok = false;
	CWaitCursor waitstatus;
	std::vector<String> files;
	FORMATETC fmtetc_cf_hdrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	FORMATETC fmtetc_shellidlist = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_SHELLIDLIST)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	FORMATETC fmtetc_filedescriptor = { static_cast<WORD>(RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR)), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (pDataObj->QueryGetData(&fmtetc_cf_hdrop) == S_OK &&
		GetFileItemsFromIDataObject_CF_HDROP(pDataObj, files) == S_OK && files.size() > 0)
		ok = true;
	else if (pDataObj->QueryGetData(&fmtetc_shellidlist) == S_OK &&
		GetFileItemsFromIDataObject_ShellIDList(pDataObj, files) == S_OK && files.size() > 0)
		ok = true;
	else if (pDataObj->QueryGetData(&fmtetc_filedescriptor) == S_OK &&
		GetFileItemsFromIDataObject_FileDescriptor(pDataObj, files) == S_OK && files.size() > 0)
		ok = true;
	if (files.size() > 3)
		files.resize(3);
	if (!files.empty())
		m_callback(files);
	return ok ? S_OK : E_FAIL;
}
