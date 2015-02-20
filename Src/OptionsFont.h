#pragma once

#include <windows.h>
#include "UnicodeString.h"

namespace Options { namespace Font {

void SetDefaults();
LOGFONT Load(const String& name);
void Save(const String& name, const LOGFONT* lf, bool bUseCustom);
void Reset(const String& name);

}}
