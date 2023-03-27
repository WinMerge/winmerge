#pragma once

#include <map>
#include <string>
#include "UnicodeString.h"

namespace Poco { class SharedMemory; }

class CMarkdown
{
//	Core class
public:
	typedef std::map<std::string, std::string> EntityMap;
	static void Load(EntityMap &entityMap);
	static std::string Resolve(const EntityMap &, const std::string& v);
	static std::string Entities(const std::string& v);
	void Load(EntityMap &entityMap, int dummy = 0);
	class FileImage;
	class File;
	const char *first;	// first char of current markup (valid after Move)
	const char *lower;	// beginning of enclosed text (valid after Move)
	const char *upper;	// end of enclosed text (initially beginning of file)
	const char *ahead;	// last char of file
	enum : unsigned
	{
		IgnoreCase = 0x10,
		HtmlUTags = 0x20,			// check for unbalanced tags
		Html = IgnoreCase|HtmlUTags	// shortcut
	};
	CMarkdown(const char *upper, const char *ahead, unsigned flags = 0);
	operator bool();				// is node ahead?
	void Scan();					// find closing tag
	CMarkdown &Move();				// move to next node
	CMarkdown &Move(const char *);	// move to next node with given name
	bool Pull();					// pull child nodes into view
	CMarkdown &Pop();				// irreversible pull for chained calls
	bool Push();					// reverse pull
	std::string GetTagName() const;	// tag name
	std::string GetTagText() const;	// tag name plus attributes
	std::string GetInnerText();		// text between enclosing tags
	std::string GetOuterText();		// text including enclosing tags
	std::string GetAttribute(const char *, std::string * = 0); // random or enumerate
private:
	int (*const memcmp)(const void *, const void *, size_t);
	const char *const utags;
	size_t FindTag(const char *, const char *) const;
	class Token;
};

class CMarkdown::FileImage
{
//	Map a file into process memory. Optionally convert UCS2 source to UTF8.
public:
	size_t cbImage;
	void *pImage;
	void *pCopy;
	enum : unsigned
	{
		Octets = 0x02 + 0x04,
		Mapping = 0x40
	};
	int nByteOrder;
	Poco::SharedMemory *m_pSharedMemory;
	explicit FileImage(const tchar_t *, size_t trunc = 0, unsigned flags = 0);
	FileImage(const FileImage& other) = delete;
	~FileImage();
	static int GuessByteOrder(unsigned);
};

class CMarkdown::File : public CMarkdown::FileImage, public CMarkdown
{
//	Construct CMarkdown object from file.
public:
	explicit File(const tchar_t * path, size_t trunc = 0, unsigned flags = Octets):
	CMarkdown::FileImage(path, trunc, flags),
	CMarkdown((const char *)pImage, (const char *)pImage + cbImage, flags)
	{
	}
};
