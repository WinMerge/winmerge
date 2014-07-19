#define DllBuild_Merge7z 10 // Minimum DllBuild of Merge7z plugin required

// We include dllpstub.h for Merge7z.h
// Merge7z::Proxy embeds a DLLPSTUB
#include "dllpstub.h"
#include "../ArchiveSupport/Merge7z/Merge7z.h"

#include "DirView.h"
#include "paths.h"
#include <list>

class DirView;

extern __declspec(thread) Merge7z::Proxy Merge7z;

bool IsArchiveFile(const String&);
Merge7z::Format *ArchiveGuessFormat(const String&);

/**
 * @brief temp path context
 */
class CTempPathContext
{
public:
	CTempPathContext *m_pParent;
	String m_strDisplayRoot[3];
	String m_strRoot[3];
	CTempPathContext *DeleteHead();
};

/**
 * @brief Merge7z::DirItemEnumerator to compress a single item.
 */
class SingleItemEnumerator : public Merge7z::DirItemEnumerator
{
	LPCTSTR FullPath;
	LPCTSTR Name;
public:
	virtual UINT Open();
	virtual Merge7z::Envelope *Enum(Item &);
	SingleItemEnumerator(LPCTSTR path, LPCTSTR FullPath, LPCTSTR Name = _T(""));
};

/**
 * @brief Merge7z::DirItemEnumerator to compress items from DirView.
 */
class CDirView::DirItemEnumerator : public Merge7z::DirItemEnumerator
{
private:
	CDirView *m_pView;
	int m_nFlags;
	int m_nIndex;
	typedef CListCtrl *pView;
	struct Envelope : public Merge7z::Envelope
	{
		String Name;
		String FullPath;
		virtual void Free()
		{
			delete this;
		}
	};
	std::list<String> m_rgFolderPrefix;
	std::list<String>::iterator m_curFolderPrefix;
	String m_strFolderPrefix;
	BOOL m_bRight;
	std::map<String, void *> m_rgImpliedFoldersLeft;
	std::map<String, void *> m_rgImpliedFoldersRight;
//	helper methods
	const DIFFITEM &Next();
	bool MultiStepCompressArchive(LPCTSTR);
public:
	enum
	{
		Left = 0x00,
		Right = 0x10,
		Original = 0x20,
		Altered = 0x40,
		DiffsOnly = 0x80,
		BalanceFolders = 0x100
	};
	DirItemEnumerator(CDirView *, int);
	virtual UINT Open();
	virtual Merge7z::Envelope *Enum(Item &);
	void CompressArchive(LPCTSTR = 0);
	void CollectFiles(String &);
};

int NTAPI HasZipSupport();
void NTAPI Recall7ZipMismatchError();

BOOL NTAPI IsMerge7zEnabled();
DWORD NTAPI VersionOf7z(BOOL bLocal = FALSE);

struct DecompressResult
{
	DecompressResult(const PathContext& files, CTempPathContext *pTempPathContext, PATH_EXISTENCE pathsType) :
		files(files), pTempPathContext(pTempPathContext), pathsType(pathsType)
	{
	}
	PathContext files;
	CTempPathContext *pTempPathContext;
	PATH_EXISTENCE pathsType;
};
DecompressResult DecompressArchive(HWND hWnd, const PathContext& infiles);

/**
 * @brief assign BSTR to String, and return BSTR for optional SysFreeString()
 */
inline BSTR Assign(CString &dst, BSTR src)
{
	dst = src;
	return src;
}
