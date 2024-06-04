/** 
 * @file  DirWatcher.h
 *
 * @brief Declaration file for DirWatcher
 */
#pragma once

#include "UnicodeString.h"
#include <functional>

class DirWatcher
{
public:
	enum ACTION { ACTION_ADDED = 1, ACTION_REMOVED, ACTION_MODIFIED, ACTION_RENAMED_OLD_NAME, ACTION_RENAMED_NEW_NAME };
	DirWatcher();
	~DirWatcher();
	bool Add(uintptr_t id, bool dir, const String& path, std::function<void(const String&, ACTION)> callback);
	bool Remove(uintptr_t id);
	void Clear();
private:
	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};

