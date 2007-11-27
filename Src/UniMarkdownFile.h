#include "Common/UniFile.h"

class UniMarkdownFile : public UniMemFile
{
public:
	UniMarkdownFile();
	virtual BOOL ReadString(CString & line, CString & eol, bool * lossy);
	virtual void Close();
	virtual bool ReadBom();
protected:
	virtual bool DoOpen(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess);
private:
	int m_depth;
	bool m_bMove;
	LPBYTE m_transparent;
	class CMarkdown *m_pMarkdown;
	void Move();
};
