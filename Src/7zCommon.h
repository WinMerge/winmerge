#include "../Merge7z/Merge7z.h"

extern Merge7z::Proxy Merge7z;

class CSilentException : public CException
{
public:
	CSilentException():CException(FALSE)
	{
	}
	virtual int ReportError(UINT, UINT)
	{
		return 0;
	}
};

class CTempPath
{
protected:
	TCHAR m_cPath[MAX_PATH + 40];
public:
	operator LPCTSTR()
	{
		return m_cPath;
	}
	CTempPath(LPVOID);
	void Clear();
	void MakeSibling(LPCTSTR);
};

/**
 * @brief Merge7z::DirItemEnumerator to compress a single file.
 *
 * Class has no extra leading C in its name because it is used like a function.
 */
class CompressSingleFile : public Merge7z::DirItemEnumerator
{
	LPCTSTR FullPath;
	LPCTSTR Name;
public:
	virtual UINT Open();
	virtual Merge7z::Envelope *Enum(Item &);
	CompressSingleFile(LPCTSTR path, LPCTSTR FullPath, LPCTSTR Name = _T(""));
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
	const DIFFITEM &Next();
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
};


