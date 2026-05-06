#pragma once
#include <string_view>

struct ILineDataProvider
{
	enum LINEFLAGS : unsigned
	{
		LF_BOOKMARK_FIRST = 0x00000001UL,
		LF_EXECUTION = 0x00010000UL,
		LF_BREAKPOINT = 0x00020000UL,
		LF_COMPILATION_ERROR = 0x00040000UL,
		LF_BOOKMARKS = 0x00080000UL,
		LF_INVALID_BREAKPOINT = 0x00100000UL,
		LF_INVISIBLE = 0x80000000UL,
		LF_DIFF = 0x00200000UL,
		LF_GHOST = 0x00400000UL, 
		LF_TRIVIAL = 0x00800000UL,
		LF_MOVED = 0x01000000UL,
		LF_SNP = 0x02000000UL,
	};

	virtual ~ILineDataProvider() = default;
	virtual int GetLineCount() const = 0;
	virtual std::string GetLine(int pane, int lineIndex) const = 0;
	virtual int GetColumnCount(int pane, int lineIndex) const = 0;
	virtual std::string GetColumn(int pane, int lineIndex, int columnIndex) const = 0;
	virtual int GetRealLineNumber(int pane, int lineIndex) const = 0;
	virtual unsigned GetLineFlags(int pane, int lineIndex) const = 0;
};