#pragma once

#include "UnicodeString.h"

class COptionsMgr;

namespace Options { namespace EditorSyntax {

void GetDefaults(COptionsMgr* pOptionsMgr, String* pExtension);
void Init(COptionsMgr *pOptionsMgr);
void Load(COptionsMgr *pOptionsMgr, String* pExtension);
void Save(COptionsMgr *pOptionsMgr, const String* const pExtension);

}}
