#pragma once

#include <windows.h>

class COptionsMgr;

namespace Options { namespace CustomColors {

void Load(COptionsMgr *pOptionsMgr, COLORREF * colors);
void Save(COptionsMgr *pOptionsMgr, const COLORREF * colors);

}}
