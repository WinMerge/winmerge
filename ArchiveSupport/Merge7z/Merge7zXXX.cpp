#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include <time.h>
#include <shlobj.h>
#include <vector>
#include "dllpstub.h"
#include "Common/UnicodeString.h"
#include "PipeIPC.h"
#define DllBuild_Merge7z 27
#include "Merge7z.h"
#include "tools.h"
#include "resource.h"

typedef char SZ_EXTENSION[8];
typedef char CH_SIGNATURE[64]; //MAX(4 + IMAGE_SIZEOF_FILE_HEADER, 64)

HINSTANCE g_hInstance;
DWORD g_dwFlags;
String g_s7zpath;
DWORD g_7zversion;
DWORD g_7zaversion;

#define DEFINE_FORMAT(ext, sig, _7zver, _7zaver) {sizeof(ext) - 1, ext, sizeof(sig) - 1, sig, _7zver, _7zaver},

struct tagSupportedFormat {
	int extensionlen;
	char *extension;
	int signaturelen;
	char *signature;
	DWORD support7zversion;
	DWORD support7zaversion;
} g_SupportedFormat[] = {
DEFINE_FORMAT("7z", "7z\xBC\xAF\x27\x1C", 311, 311)
DEFINE_FORMAT("arj", "\x60\xEA", 311, 0xFFFFFFFF)
DEFINE_FORMAT("bz2 tbz2", "BZh", 311, 311)
DEFINE_FORMAT("cab", "MSCF", 311, 0xFFFFFFFF)
DEFINE_FORMAT("cpio", "", 311, 0xFFFFFFFF)
DEFINE_FORMAT("deb", "!<arch>\n", 311, 0xFFFFFFFF)
DEFINE_FORMAT("lzh lha", "@@-l@@-", 311, 0xFFFFFFFF)//"@-l" doesn't work because signature starts at offset 2
DEFINE_FORMAT("gz tgz", "\x1F\x8B", 311, 311)
DEFINE_FORMAT("rar", "Rar!\x1a\x07\x00", 311, 0xFFFFFFFF)
DEFINE_FORMAT("rpm", "", 311, 0xFFFFFFFF)
DEFINE_FORMAT("tar", "", 311, 311)
DEFINE_FORMAT("z", "\x1F\x9D", 311, 311)
DEFINE_FORMAT("zip jar war ear xpi", "PK\x03\x04", 311, 311)
DEFINE_FORMAT("chm chi chq chw hxs hxi hxr hxq hxw lit", "ITSF", 311, 0xFFFFFFFF)
DEFINE_FORMAT("iso", "", 311, -1)
DEFINE_FORMAT("", "@@@@\xEF\xBE\xAD\xDENullsoftInst", 311, 0xFFFFFFFF)
DEFINE_FORMAT("wim swm", "MSWIM\x00\x00\x00", 449, 0xFFFFFFFF)
DEFINE_FORMAT("", "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1", 452, 0xFFFFFFFF)
DEFINE_FORMAT("lzma lzma86", "", 458, 0xFFFFFFFF)
};

typedef unsigned __int64 uint64;

// Return major/minor version number from version resource (0 if not found)
static DWORD getFixedFileVersion(LPCTSTR pszPath)
{
	DWORD dwDummyHandle;
	DWORD dwVerInfoSize = ::GetFileVersionInfoSize(pszPath, &dwDummyHandle);
	BYTE *pbytVerInfo = new BYTE[dwVerInfoSize];
	if (!::GetFileVersionInfo(pszPath, 0, dwVerInfoSize, pbytVerInfo))
	{
		delete [] pbytVerInfo;
		return 0;
	}
	unsigned int verlen;
	LPVOID lpvi;
	if (!::VerQueryValue(pbytVerInfo, _T("\\"), &lpvi, &verlen))
	{
		delete [] pbytVerInfo;
		return 0;
	}
	delete [] pbytVerInfo;
	VS_FIXEDFILEINFO fileInfo;
	fileInfo = *(VS_FIXEDFILEINFO*)lpvi;
	DWORD ver = fileInfo.dwFileVersionMS;
	return ver;
}

static BSTR a2bstr(const char *pstr)
{
	if (!pstr)
		return NULL;
	size_t len = lstrlenA(pstr);
	wchar_t *pwstr = new wchar_t[len + 1];
	int wlen = MultiByteToWideChar(CP_ACP, 0, pstr, len, pwstr, len);
	pwstr[wlen] = 0;
	BSTR pbstr = SysAllocString(pwstr);
	delete [] pwstr;
	return pbstr;
}

static BSTR t2bstr(const TCHAR *pstr)
{
	if (!pstr)
		return NULL;
#ifdef _UNICODE
	return SysAllocString(pstr);
#else
	return a2bstr(pstr);
#endif
}

static INT CALLBACK PasswordDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TCHAR *pszPassword;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		pszPassword = (TCHAR *)lParam;
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hwndDlg, IDC_EDTPASSWORD, pszPassword, 80);
			EndDialog(hwndDlg, wParam);
			break;
		case IDCANCEL:
			pszPassword[0] = '\0';
			pszPassword = NULL;
			EndDialog(hwndDlg, wParam);
			break;
		}
		return TRUE;
	}
	return FALSE;
} 

static INT CALLBACK ErrorDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwndDlg, IDC_EDTMESSAGE, (TCHAR *)lParam);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			break;
		}
		return TRUE;
	}
	return FALSE;
} 

struct ArchiveFSItem
{
	ArchiveFSItem(String &relpath, unsigned int attribs, uint64 modifieddate, uint64 size, bool encrypted) :
		m_relpath(relpath), m_attribs(attribs), m_modifieddate(modifieddate), m_size(size), m_encrypted(encrypted) { };
	String m_relpath;
	unsigned int m_attribs;
	uint64 m_modifieddate;
	uint64 m_size;
	bool m_encrypted;
};

static String search7zpath()
{
	LONG result;
	TCHAR sz7zDir[MAX_PATH];
	TCHAR sz7zPath[MAX_PATH];
	HKEY hKey;
	DWORD cbData = sizeof(sz7zDir);
	result = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\7-Zip"), 0, KEY_READ, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = RegQueryValueEx(hKey, _T("Path"), NULL, NULL, (LPBYTE)sz7zDir, &cbData);
		RegCloseKey(hKey);
		if (result == ERROR_SUCCESS)
		{
			lstrcpy(sz7zPath, sz7zDir);
			PathAppend(sz7zPath, _T("7z.exe"));
			if (PathFileExists(sz7zPath))
				return sz7zPath;
			else
			{
				lstrcpy(sz7zPath, sz7zDir);
				PathAppend(sz7zPath, _T("7za.exe"));
				if (PathFileExists(sz7zPath))
					return sz7zPath;
			}
		}
	}
	DWORD len = SearchPath(NULL, _T("7z.exe"), NULL, sizeof(sz7zPath)/sizeof(sz7zPath[0]), sz7zPath, NULL);
	if (len > 0)
		return sz7zPath;
	len = SearchPath(NULL, _T("7za.exe"), NULL, sizeof(sz7zPath)/sizeof(sz7zPath[0]), sz7zPath, NULL);
	if (len > 0)
		return sz7zPath;
	return _T("");
}

static bool createArchiveList(std::vector<ArchiveFSItem *> & list, const String & zippath)
{
	if (!PathFileExists(zippath.c_str()))
		Complain(ERROR_FILE_NOT_FOUND, zippath.c_str());

	String s7zpath = search7zpath();
	if (s7zpath.length() == 0)
		Complain(_T("7z.exe or 7za.exe cannot be found"));

	String cmdline = _T("\"") + s7zpath + _T("\" l -slt \"") + zippath + _T("\"");
	PipeIPC pipc(cmdline);
	std::string line;
	std::string relpath;
	uint64 size = 0;
	unsigned int attribs = 0;
	bool encrypted = false;
	uint64 modifieddate = 0;
	
	while ((line = pipc.readLineStdout()) != "")
	{
		const char *p = line.c_str();
		if (strncmp(p, "Packed Size = ", sizeof("Packed Size = ") - 1) == 0)
		{
			/* gz or Z */
			relpath = "dummy";
		}
		else if (strncmp(p, "Path = ", sizeof("Path = ") - 1) == 0)
		{
			relpath = p + sizeof("Path = ") - 1;
			relpath.resize(relpath.length() - 1);
		}
		else if (strncmp(p, "Size = ", sizeof("Size = ") - 1) == 0)
		{
			size = _atoi64(p + sizeof("Size = ") - 1);
		}
		else if (strncmp(p, "Modified = ", sizeof("Modified = ") - 1) == 0)
		{
			struct tm tm;
			sscanf(p + sizeof("Modified = ") - 1, "%04d-%02d-%02d %02d:%02d:%02d",
				&tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
			tm.tm_year -= 1900;
			tm.tm_mon--;
			modifieddate = mktime(&tm) * 1000;
		}
		else if (strncmp(p, "Attributes = ", sizeof("Attributes = ") - 1) == 0)
		{
			attribs = 0;
			const char *pattribs = p + sizeof("Attributes = ") - 1;
			if (pattribs[0] == 'D')
				attribs |= FILE_ATTRIBUTE_DIRECTORY;
			if (pattribs[1] == 'R')
				attribs |= FILE_ATTRIBUTE_READONLY;
			if (pattribs[2] == 'H')
				attribs |= FILE_ATTRIBUTE_HIDDEN;
			if (pattribs[3] == 'S')
				attribs |= FILE_ATTRIBUTE_SYSTEM;
			if (pattribs[4] == 'A')
				attribs |= FILE_ATTRIBUTE_ARCHIVE;
		}
		else if (strncmp(p, "Encrypted = ", sizeof("Encrypted = ") - 1) == 0)
		{
			if (*(p + sizeof("Encrypted = ") - 1) == '+')
				encrypted = true;
			else
				encrypted = false;
		}
		else if (*p == '\n' && !relpath.empty())
		{
#ifdef _UNICODE
			BSTR bstrRelpath = a2bstr(relpath.c_str());
			ArchiveFSItem *pItem = new ArchiveFSItem(String(bstrRelpath), size, modifieddate, attribs, encrypted);
			SysFreeString(bstrRelpath);
#else
			ArchiveFSItem *pItem = new ArchiveFSItem(relpath, size, modifieddate, attribs, encrypted);
#endif
			list.push_back(pItem);

			relpath.erase();
			size = 0;
			attribs = 0;
			encrypted = false;
			modifieddate = 0;
		}
	}

	return true;
}

static void freeArchiveList(std::vector<ArchiveFSItem *> & list)
{
	std::vector<ArchiveFSItem *>::iterator i;
	for (i = list.begin(); i != list.end(); i++)
		delete (*i);
}

struct FormatImpl: Merge7z::Format
{
	virtual HRESULT DeCompressArchive(HWND hwndParent, LPCTSTR path, LPCTSTR folder)
	{
		HRESULT result = E_FAIL;
		if (Merge7z::Format::Inspector *inspector = Open(hwndParent, path))
		{
			result = inspector->Extract(hwndParent, folder);
			inspector->Free();
		}
		return result;
	}
	virtual HRESULT CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *)
	{
		return S_FALSE;
	}
	interface Inspector : Merge7z::Format::Inspector
	{
		virtual void Free()
		{
			freeArchiveList(m_list);
		}

		void Init(HWND hwndParent)
		{
			m_hwndParent = hwndParent;
			createArchiveList(m_list, m_zippath);
			bool encrypted = false;
			std::vector<ArchiveFSItem *>::iterator i;
			for (i = m_list.begin(); i != m_list.end(); i++)
			{
				totalSize += (*i)->m_size;
				if ((*i)->m_encrypted)
					passwordIsDefined = true;
			}
		}
		virtual UINT32 Open()
		{
			return m_list.size();
		}
		virtual HRESULT Extract(HWND hwnd, LPCTSTR folder, const UINT32 *indices = 0, UINT32 numItems = -1)
		{
			;
			uint64 sum = 0;
			wchar_t buf[512];
			IProgressDialog *pProgressDlg;
			bool bsucceeded = false;

			try
			{
				String s7zpath = search7zpath();
				if (s7zpath.length() == 0)
					Complain(_T("Can not find 7z.exe"));

				HRESULT hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**)&pProgressDlg);
				if (FAILED(hr))
					Complain(_T("CoCreateInstance(CLSID_ProgressDialog) error. hresult=%08x"), hr);

				String cmdline = _T("\"") + s7zpath + _T("\" x -yo\"") + folder + _T("\" ");
				if (passwordIsDefined)
				{
					TCHAR szPassword[80];
					int nResult = DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_PASSWORD), hwnd, PasswordDlgProc, (LPARAM)szPassword);
					if (nResult == IDCANCEL)
						return S_FALSE;
					password = szPassword;
					cmdline += _T("-p") + password + _T(" ");
				}
				cmdline += _T("\"") + m_zippath + _T("\"");
				PipeIPC pipc(cmdline);
				std::string line;
				std::string lines;

#ifdef _UNICODE
				wsprintfW(buf, L"Extracting %s", m_zippath.c_str());
#else
				wsprintfW(buf, L"Extracting %S", m_zippath.c_str());
#endif
				pProgressDlg->SetTitle(buf);
				pProgressDlg->SetProgress(0, totalSize);
				pProgressDlg->StartProgressDialog(hwnd, NULL, PROGDLG_NORMAL | PROGDLG_NOMINIMIZE, NULL);

				std::vector<ArchiveFSItem *>::iterator i = m_list.begin();
				while ((line = pipc.readLineStdout()) != "")
				{
					lines += line;
					lines += "\r\n";
					if (strncmp(line.c_str(), "Everything is Ok", sizeof("Everything is OK") - 1) == 0)
					{
						bsucceeded = true;
					}
					else if (strncmp(line.c_str(), "Extracting", sizeof("Extracting") - 1) == 0)
					{
						BSTR bstrMessage = a2bstr(line.c_str());
						pProgressDlg->SetLine(1, bstrMessage, FALSE, NULL);
						pProgressDlg->SetProgress(sum, totalSize);
						sum += (*i)->m_size;
						i++;
						SysFreeString(bstrMessage);
					}

					if (pProgressDlg->HasUserCancelled())
						break;
				}

				pProgressDlg->StopProgressDialog();
				pProgressDlg->Release();

				if (!bsucceeded)
				{
#ifdef _UNICODE
					int len = lstrlenA(lines.c_str());
					wchar_t *pszMessage = new wchar_t[len + 1];
					MultiByteToWideChar(CP_ACP, 0, (char *)lines.c_str(), len + 1, pszMessage, len + 1);
					DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ERROR), hwnd, ErrorDlgProc, (LPARAM)pszMessage);
					delete [] pszMessage;
#else
					DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ERROR), hwnd, ErrorDlgProc, (LPARAM)lines.c_str());
#endif
				}
			}
			catch (Complain *complain)
			{
				complain->Alert(hwnd);
			}
			return bsucceeded ? S_OK : S_FALSE;
		}
		virtual HRESULT GetProperty(UINT32, PROPID, PROPVARIANT *, VARTYPE) { return S_FALSE; }
		virtual BSTR GetPath(UINT32 index)
		{
			if (index < 0 || index >= m_list.size())
				return NULL;
			return t2bstr(m_list[index]->m_relpath.c_str());
		}
		virtual BSTR GetName(UINT32 index)
		{
			if (index < 0 || index >= m_list.size())
				return NULL;
			return t2bstr(PathFindFileName(m_list[index]->m_relpath.c_str()));
		}
		virtual BSTR GetExtension(UINT32 index)
		{
			if (index < 0 || index >= m_list.size())
				return NULL;
			return t2bstr(PathFindExtension(m_list[index]->m_relpath.c_str()));
		}
		virtual VARIANT_BOOL IsFolder(UINT32 index)
		{
			if (index < 0 || index >= m_list.size())
				return VARIANT_FALSE;
			return (m_list[index]->m_attribs & FILE_ATTRIBUTE_DIRECTORY) ? VARIANT_TRUE : VARIANT_FALSE;
		}
		virtual FILETIME LastWriteTime(UINT32 index)
		{
			FILETIME a = {0}; return a;
		}
		virtual BSTR GetDefaultName()
		{
			return t2bstr(strDefaultName.c_str());
		}
		Inspector(FormatImpl *format, LPCTSTR zippath) :
			m_zippath(zippath), m_hwndParent(NULL), passwordIsDefined(false), totalSize(0)
		{
		}
		String m_zippath;
		HWND m_hwndParent;
		bool passwordIsDefined;
		String password;
		uint64 totalSize;
		std::vector<ArchiveFSItem *> m_list;
		String strDefaultName;
	};
	virtual Merge7z::Format::Inspector *Open(HWND hwndParent, LPCTSTR path)
	{
		Inspector *inspector = new Inspector(this, path);
		try
		{
			inspector->Init(hwndParent);
		}
		catch (Complain *complain)
		{
			complain->Alert(hwndParent);
			inspector->Free();
			inspector = 0;
		}
		return inspector;
	}
	interface Updater : Merge7z::Format::Updater
	{
		virtual void Free() {}
		virtual UINT32 Add(Merge7z::DirItemEnumerator::Item &) { return 0; }
		virtual HRESULT Commit(HWND) { return S_FALSE; }
		Updater() {}
	};
	virtual Merge7z::Format::Updater *Update(HWND, LPCTSTR) { return NULL; }
	virtual HRESULT GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE) { return S_FALSE; }
	virtual BSTR GetHandlerName(HWND) { return NULL; }
	virtual BSTR GetHandlerClassID(HWND) { return NULL; }
	virtual BSTR GetHandlerExtension(HWND) { return NULL; }
	virtual BSTR GetHandlerAddExtension(HWND) { return NULL; }
	virtual VARIANT_BOOL GetHandlerUpdate(HWND) { return VARIANT_FALSE; }
	virtual VARIANT_BOOL GetHandlerKeepName(HWND) { return VARIANT_FALSE; }
	virtual BSTR GetDefaultName(HWND hwndParent, LPCTSTR path)
	{
		return SysAllocString(L"");	
	}
} g_formatImpl;


Merge7z::Merge7z() :
	Format7z(g_formatImpl), 
	ZipHandler(g_formatImpl),
	RarHandler(g_formatImpl),
	BZip2Handler(g_formatImpl),
	TarHandler(g_formatImpl)
{
}

LPCTSTR Merge7z::LoadLang(LPCTSTR langFile) { return NULL; }

/**
 * @brief Initialize the library.
 */
int Merge7z::Initialize(DWORD dwFlags)
{
	g_dwFlags = dwFlags;
	g_s7zpath = search7zpath();
	g_7zversion = 0;
	g_7zaversion = 0;
	if (g_s7zpath.length() > 0)
	{
		DWORD version = getFixedFileVersion(g_s7zpath.c_str());
		if (g_s7zpath.find(_T("7za.exe")) != String::npos)
			g_7zaversion = version;
		else
			g_7zversion = version;
	}
	return 0;
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormat(LPCTSTR path)
{
	if (g_dwFlags & Initialize::GuessFormatBySignature)
		return GuessFormatBySignature(path, g_dwFlags & Initialize::GuessFormatByExtension ? path : 0);
	return GuessFormatByExtension(path);
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatEx(LPCSTR ext, LPCH sig, int cchSig)
{
	Merge7z::Format *pFormatByExtension = 0;
	for (int i = 0; i < sizeof(g_SupportedFormat)/sizeof(g_SupportedFormat[0]); i++)
	{
		static const char aBlank[] = " ";
		LPCSTR pchExtension = g_SupportedFormat[i].extension;
		int cchExtension = g_SupportedFormat[i].extensionlen;
		int cchSignature = g_SupportedFormat[i].signaturelen;
		if (g_7zversion >= g_SupportedFormat[i].support7zversion || g_7zaversion >= g_SupportedFormat[i].support7zaversion)
		{
			if (cchSig > 0 && cchSignature)
			{
				LPCSTR pchSignature = g_SupportedFormat[i].signature;
				char joker = *pchSignature++;
				--cchSignature;
				if (cchSig >= cchSignature)
				{
					while (cchSignature--)
					{
						char expected = pchSignature[cchSignature];
						if (expected != joker && sig[cchSignature] != expected)
							break;
					}
					if (cchSignature == -1)
						return &g_formatImpl;
				}
			}
			else while
			(
				ext
			&&	pFormatByExtension == 0
			&&	(cchExtension = StrCSpnA(pchExtension += StrSpnA(pchExtension, aBlank), aBlank)) != 0
			)
			{
				if (StrIsIntlEqualA(FALSE, pchExtension, ext, cchExtension) && ext[cchExtension] == '\0')
				{
					pFormatByExtension = &g_formatImpl;
				}
				pchExtension += cchExtension;
			}
		}
	}
	return pFormatByExtension;
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatByExtension(LPCTSTR path)
{
	SZ_EXTENSION ext;
	if (PathIsDirectory(path))
		return 0;
	return GuessFormatEx(GetExtension(path, ext), 0, 0);
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatBySignature(LPCTSTR path, LPCTSTR extension)
{
	SZ_EXTENSION ext;
	CH_SIGNATURE sig;
	if (PathIsDirectory(path))
		return 0;
	return GuessFormatEx(GetExtension(extension, ext), sig, GetSignature(path, sig));
}

/**
 * @brief Get filename extension as ANSI characters.
 */
LPCSTR Merge7z::GetExtension(LPCTSTR path, SZ_EXTENSION ext)
{
	if (path == NULL)
		return NULL;
	path = PathFindExtension(path);
#ifdef UNICODE
	return WideCharToMultiByte(CP_ACP, 0, path, -1, ext, sizeof(SZ_EXTENSION), 0, 0) > 1 ? ext + 1 : 0;
#else
	ext[sizeof(SZ_EXTENSION) - 2] = '\0';
	lstrcpynA(ext, path, sizeof(SZ_EXTENSION));
	return ext[0] != '\0' && ext[sizeof(SZ_EXTENSION) - 2] == '\0' ? ext + 1 : 0;
#endif
}

/**
 * @brief Read start signature from given file.
 */
DWORD Merge7z::GetSignature(LPCTSTR path, CH_SIGNATURE sig)
{
	if (sig == NULL)
		return sizeof(CH_SIGNATURE);
	DWORD cchSig = 0;
	HANDLE h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
		if (cchSig >= 64 && MAKEWORD(sig[0],sig[1]) == IMAGE_DOS_SIGNATURE)
		{
			DWORD offset = 
			(
				LPDWORD(sig)[5]	//DOS CS:IP
			?	512UL * (LPWORD(sig)[1] ? LPWORD(sig)[2] - 1 : LPWORD(sig)[2]) + LPWORD(sig)[1]
			:	LPDWORD(sig)[15]
			);
			if (SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
			{
				ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
				if (cchSig >= 4 + sizeof(IMAGE_FILE_HEADER) && MAKELONG(MAKEWORD(sig[0],sig[1]), MAKEWORD(sig[2],sig[3])) == MAKELONG(MAKEWORD('P','E'), 0))
				{
					cchSig = 0;
					IMAGE_FILE_HEADER *pImageFileHeader = (IMAGE_FILE_HEADER *) (sig + 4);
					offset += 4 + sizeof(IMAGE_FILE_HEADER) + pImageFileHeader->SizeOfOptionalHeader;
					if (SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
					{
						int iSection = pImageFileHeader->NumberOfSections;
						while (iSection--)
						{
							IMAGE_SECTION_HEADER ImageSectionHeader;
							DWORD cbImageSectionHeader = 0;
							ReadFile(h, &ImageSectionHeader, sizeof ImageSectionHeader, &cbImageSectionHeader, 0);
							if (cbImageSectionHeader != sizeof ImageSectionHeader)
								break;
							if (memcmp(ImageSectionHeader.Name, "_winzip_", 8) == 0)
							{
								// looks like WinZip Self-Extractor
								memcpy(sig, "PK\x03\x04", cchSig = 4);
								break;
							}
							DWORD ahead = ImageSectionHeader.PointerToRawData + ImageSectionHeader.SizeOfRawData;
							if (offset < ahead)
								offset = ahead;
						}
						if (iSection == -1 && SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
						{
							ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
						}
					}
				}
			}
		}
		CloseHandle(h);
	}
	return cchSig;
}

/**
 * @brief Export instance of Merge7z interface.
 */
EXTERN_C
{
	__declspec(dllexport) Merge7z Merge7z;
}

/**
 * @brief Dll entry point
 */
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInstance;
		CoInitialize(0);
	}
	return TRUE;
}

EXTERN_C HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO *pdvi)
{
	// Compute dwBuild from revision.txt
	static const DWORD dwBuild =
	(
		sizeof""
#		define VERSION(MAJOR,MINOR)
#		include "revision.txt"
#		undef VERSION
	);
	C_ASSERT(dwBuild == DllBuild_Merge7z);
	// Compute dwVersion from revision.txt
	static const DWORD dwVersion =
	(
		0*sizeof""
#		define VERSION(MAJOR,MINOR) , MAKELONG(MINOR,MAJOR) + 0*sizeof""
#		include "revision.txt"
#		undef VERSION
	);
	static const DLLVERSIONINFO dvi =
	{
		sizeof dvi,
		HIWORD(dwVersion), LOWORD(dwVersion), dwBuild, DLLVER_PLATFORM_WINDOWS
	};
	CopyMemory(pdvi, &dvi, pdvi->cbSize < dvi.cbSize ? pdvi->cbSize : dvi.cbSize);
	return S_OK;
}
