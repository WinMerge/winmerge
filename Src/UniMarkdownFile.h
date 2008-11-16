/**
 *  @file TempFile.h
 *
 *  @brief Declaration of UniMarkdownFile class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "Common/UniFile.h"

class CMarkdown;

/**
 * @brief XML file reader class.
 */
class UniMarkdownFile : public UniMemFile
{
public:
	UniMarkdownFile();
	virtual bool ReadString(String & line, String & eol, bool * lossy);
	virtual void Close();

protected:
	virtual bool DoOpen(LPCTSTR filename, DWORD dwOpenAccess,
		DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion,
		DWORD dwMappingProtect, DWORD dwMapViewAccess);

private:
	void Move();
	String maketstring(LPCSTR lpd, UINT len);

	int m_depth;
	bool m_bMove;
	LPBYTE m_transparent;
	CMarkdown *m_pMarkdown;
};
