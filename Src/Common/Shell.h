#pragma once

#include "UnicodeString.h"

namespace shell
{
	void Open(const tchar_t * szFile);
	void Edit(const tchar_t * szFile);
	void OpenWith(const tchar_t * szFile);
	void OpenFileOrUrl(const tchar_t * szFile, const tchar_t * szUrl);
	void OpenParentFolder(const tchar_t * szFile);
}
