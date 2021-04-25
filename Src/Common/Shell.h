#include <tchar.h>

namespace shell
{
	void Open(const TCHAR * szFile);
	void Edit(const TCHAR * szFile);
	void OpenWith(const TCHAR * szFile);
	void OpenFileOrUrl(const TCHAR * szFile, const TCHAR * szUrl);
	void OpenParentFolder(const TCHAR * szFile);
}
