#pragma once

#include "UnicodeString.h"

struct TableProps { bool istable; tchar_t delimiter; tchar_t quote; bool allowNewlinesInQuotes; };

TableProps MakeTablePropertiesByFileName(const String& path);

