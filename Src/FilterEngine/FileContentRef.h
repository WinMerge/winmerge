#pragma once

#include "DiffFileInfo.h"
#include <string>

namespace Poco { class RegularExpression; }

struct FileContentRef
{
	String path;
	DiffFileInfo item;
	bool operator==(const FileContentRef& other) const;
	bool Contains(const std::string& str) const;
	bool REContains(const Poco::RegularExpression& regexp) const;
	std::string Sublines(int64_t start, int64_t len) const;
	size_t LineCount() const;
};
