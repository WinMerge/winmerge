#define DllBuild_Merge7z 10 // Minimum DllBuild of Merge7z plugin required

// We include dllpstub.h for Merge7z.h
// Merge7z::Proxy embeds a DLLPSTUB
#include "dllpstub.h"
#include "../ArchiveSupport/Merge7z/Merge7z.h"

#include "DirView.h"

extern Merge7z::Proxy Merge7z;

Merge7z::Format *ArchiveGuessFormat(LPCTSTR);

CString NTAPI GetClearTempPath(LPVOID pOwner, LPCTSTR pchExt);

/**
 * @brief temp path context
 */
class CTempPathContext
{
public:
	CTempPathContext *m_pParent;
	CString m_strLeftDisplayRoot;
	CString m_strRightDisplayRoot;
	CString m_strLeftRoot;
	CString m_strRightRoot;
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
		CString Name;
		CString FullPath;
		virtual void Free()
		{
			delete this;
		}
	};
	CStringList m_rgFolderPrefix;
	POSITION m_curFolderPrefix;
	CString m_strFolderPrefix;
	BOOL m_bRight;
	CMapStringToPtr m_rgImpliedFoldersLeft;
	CMapStringToPtr m_rgImpliedFoldersRight;
//	helper methods
	DIFFITEM Next();
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
	void CollectFiles(CString &);
};

int NTAPI HasZipSupport();
void NTAPI Recall7ZipMismatchError();

DWORD NTAPI VersionOf7z(BOOL bLocal = FALSE);

/**
 * @brief assign BSTR to CString, and return BSTR for optional SysFreeString()
 */
inline BSTR Assign(CString &dst, BSTR src)
{
	dst = src;
	return src;
}
