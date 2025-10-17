#pragma once

#include "FilterEngine/FilterError.h"
#include "UnicodeString.h"

struct FileFilterErrorInfo;
struct FilterExpression;

String GetFilterErrorMessage(FilterErrorCode code);
String FormatFilterErrorSummary(const FileFilterErrorInfo& fei);
String FormatFilterErrorSummary(const FilterExpression& fe);