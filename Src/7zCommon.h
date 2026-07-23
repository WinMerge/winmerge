#pragma once

#define DllBuild_Merge7z 10 // Minimum DllBuild of Merge7z plugin required

// We include dllpstub.h for Merge7z.h
// Merge7z::Proxy embeds a DLLPSTUB
#include <list>
#include <map>
#include <vector>
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
	CTempPathContext *m_pParent = nullptr;
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
 * @brief One file/folder to be added to the archive, fully resolved
 *        (archive-relative name + physical source path + recurse flag).
 *        No knowledge of CDirView/DIFFITEM/CDiffContext required.
 */
struct CompressibleItem
{
   String name;             // Name (relative path) inside the archive
   String fullPath;         // Physical source path
   bool   recurse = false;  // true: let 7-Zip physically recurse this folder
};

/**
 * @brief Merge7z::DirItemEnumerator to compress items from DirView.
 */
class CompressibleItemEnumerator : public Merge7z::DirItemEnumerator
{
private:
	struct Envelope : public Merge7z::Envelope
	{
		String Name;
		String FullPath;
		virtual void Free()
		{
			delete this;
		}
	};
	bool MultiStepCompressArchive(const tchar_t*);

public:
	explicit CompressibleItemEnumerator(std::vector<CompressibleItem> items);
	
	UINT Open() override;
	Envelope* Enum(Item& item) override;
	bool CompressArchive(const tchar_t* = 0);
private:
	std::vector<CompressibleItem> m_items;
	size_t m_itemPos = 0;
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
void Merge7zInitFlags();

