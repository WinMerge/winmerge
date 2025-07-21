#pragma once

#include "FilterEngine/FilterError.h"
#include "UnicodeString.h"

struct FileFilterErrorInfo;

String GetFilterErrorMessage(FilterErrorCode code);
String FormatFilterErrorSummary(const FileFilterErrorInfo& fei);