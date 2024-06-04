// SPDX-License-Identifier: GPL-2.0-or-later
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
	std::vector<tchar_t> GetPathList(bool source) const;
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

