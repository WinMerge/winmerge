#pragma once

struct FileContentRef
{
	std::string path;

	std::string loadContent() const;
	bool operator==(const FileContentRef& other) const;
	bool operator<(const FileContentRef& other) const;
	bool operator==(const std::string& rhs) const;
	bool operator<(const std::string& rhs) const;
};
