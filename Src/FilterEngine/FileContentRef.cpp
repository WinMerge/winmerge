#include "pch.h"
#include "FileContentRef.h"

bool FileContentRef::operator==(const FileContentRef& other) const
{
	return path == other.path;
}