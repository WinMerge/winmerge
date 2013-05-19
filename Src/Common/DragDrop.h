#include "UnicodeString.h"
#include <windows.h>
#include <vector>

bool GetDroppedFiles(HDROP dropInfo, std::vector<String>& files);
