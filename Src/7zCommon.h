#pragma once

#define DllBuild_Merge7z 10 // Minimum DllBuild of Merge7z plugin required

// We include dllpstub.h for Merge7z.h
// Merge7z::Proxy embeds a DLLPSTUB
#include <list>
#include <map>
#include <PropIdl.h>
#include "dllpstub.h"
#include "../ArchiveSupport/Merge7z/Merge7z.h"
#include "paths.h"

class CDirView;
class CListCtrl;
class DIFFITEM;

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
	void Swap(int idx1, int idx2);
};

/**
 * @brief Merge7z::DirItemEnumerator to compress a single item.
 */
class SingleItemEnumerator : public Merge7z::DirItemEnumerator
{
	const tchar_t* FullPath;
	const tchar_t* Name;
public:
	virtual UINT Open();
	virtual Merge7z::Envelope *Enum(Item &);
	SingleItemEnumerator(const tchar_t* path, const tchar_t* FullPath, const tchar_t* Name = _T(""));
};

/**
 * @brief Merge7z::DirItemEnumerator to compress items from DirView.
 */
class DirItemEnumerator : public Merge7z::DirItemEnumerator
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
	int m_index;
	std::map<String, void *> m_rgImpliedFolders[3];
//	helper methods
	const DIFFITEM &Next();
	bool MultiStepCompressArchive(const tchar_t*);
public:
	enum
	{
		Left = 0x00,
		Middle = 0x10,
		Right = 0x20,
		Original = 0x40,
		Altered = 0x80,
		DiffsOnly = 0x100,
		BalanceFolders = 0x200
	};
	DirItemEnumerator(CDirView *, int);
	virtual UINT Open();
	virtual Merge7z::Envelope *Enum(Item &);
	void CompressArchive(const tchar_t* = 0);
};

int NTAPI HasZipSupport();

struct DecompressResult
{
	DecompressResult(const PathContext& files, CTempPathContext *pTempPathContext, paths::PATH_EXISTENCE pathsType) :
		files(files), pTempPathContext(pTempPathContext), pathsType(pathsType), hr(S_OK)
	{
	}
	PathContext files;
	CTempPathContext *pTempPathContext;
	paths::PATH_EXISTENCE pathsType;
	HRESULT hr;
};
DecompressResult DecompressArchive(HWND hWnd, const PathContext& infiles);
