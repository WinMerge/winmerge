#pragma once

#include <windows.h>
#include "UnicodeString.h"

class COptionsMgr;

namespace Options { namespace Font {


//void InitializeLogFont(LOGFONT &logfont, int lfHeight, int lfCharSet, int lfPitchAndFamily, String lfFaceName);
void SetDefaults(COptionsMgr *pOptionsMgr);
LOGFONT Load(const COptionsMgr *pOptionsMgr, const String& name);
void Save(COptionsMgr *pOptionsMgr, const String& name, const LOGFONT* lf, bool bUseCustom);
void Reset(COptionsMgr *pOptionsMgr, const String& name);

}}
