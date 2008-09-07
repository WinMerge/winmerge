/**
 *  @file TempFile.h
 *
 *  @brief Declaration of UniMarkdownFile class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "UniFile.h"

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
	virtual bool ReadBom();

protected:
	virtual bool DoOpen(LPCTSTR filename, DWORD dwOpenAccess,
		DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion,
		DWORD dwMappingProtect, DWORD dwMapViewAccess);

private:
	void Move();

	int m_depth;
	bool m_bMove;
	LPBYTE m_transparent;
	CMarkdown *m_pMarkdown;
};
