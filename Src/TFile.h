#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/File.h>
#include <Poco/Exception.h>
#include "UnicodeString.h"
#include "unicoder.h"

class TFile : public Poco::File
{
public:
	explicit TFile(const String& path) : File(ucr::toUTF8(path)) {}
	void copyTo(const String& path) const { File::copyTo(ucr::toUTF8(path)); }
	void moveTo(const String& path) { File::moveTo(ucr::toUTF8(path)); }
	void renameTo(const String& path) { File::renameTo(ucr::toUTF8(path)); }
};
