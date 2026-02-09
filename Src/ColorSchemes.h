#pragma once

#include "UnicodeString.h"
#include <vector>

namespace ColorSchemes
{
std::vector<String> GetColorSchemesFolders();
String GetPrivateColorSchemesFolder(bool useMyDocument);
bool IsPrivateColorSchemePath(const String& path);
String GetColorSchemePath(const String& name);
std::vector<String> GetColorSchemeNames();
}
