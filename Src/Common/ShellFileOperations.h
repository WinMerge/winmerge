/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  ShellFileOperations.h
 *
 * @brief Declaration file for ShellFileOperations class.
 */
#pragma once

#include <vector>
#include <Windows.h>
#include <ShellAPI.h>
#include "UnicodeString.h"

/**
 * @brief A wrapper class for SHFileOperation().
 * This class offers easier way to use SHFileOperation(). User can just add
 * paths without needing to care about adding correct amount of NULLs.
 */
class ShellFileOperations
{
public:
	ShellFileOperations();

	void AddSourceAndDestination(const String &source, const String &destination);
	void AddSource(const String &source);
	void SetDestination(const String &destination);
	void SetOperation(UINT operation, FILEOP_FLAGS flags, HWND parentWindow = nullptr);
	bool Run();
	bool IsCanceled() const;
	void Reset();

protected:
	std::vector<TCHAR> GetPathList(bool source) const;
	size_t CountStringSize(bool source) const;

private:
	std::vector<String> m_sources; /**< Source paths. */
	std::vector<String> m_destinations; /**< Destination paths. */
	UINT m_function; /**< Operation used, copy, move, rename or delete. */
	FILEOP_FLAGS m_flags; /**< Flags for the operation. */
	HWND m_parentWindow; /**< Parent window getting notifications. */
	bool m_bOneToOneMapping; /**< Same amount of sources and destinations? */
	bool m_isCanceled; /**< Did user cancel the operation? */
};

/**
 * @brief Did the user cancel the operation?
 * @return true if the operation was canceled by the user.
 */
inline bool ShellFileOperations::IsCanceled() const
{
	return m_isCanceled;
}

