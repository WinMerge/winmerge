#pragma once

#include "UnicodeString.h"
#include <vector>

namespace ColorSchemes
{
String GetColorSchemesFolder();
String GetPrivateColorSchemesFolder();
String GetColorSchemePath(const String& name);
std::vector<String> GetColorSchemeNames();
}
