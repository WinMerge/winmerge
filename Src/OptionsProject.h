#pragma once

class COptionsMgr;

namespace Options { namespace Project {

/** @brief Types of operations for project files */
enum class Operation
{
	Open,
	Load,
	Save
};
constexpr int OperationCount = static_cast<int>(Operation::Save) + 1;

/** @brief Types of items to save to or restore from the project file */
enum class Item
{
	FileFilter,
	IncludeSubfolders,
	Plugin,
	CompareOptions,
	HiddenItems
};
constexpr int ItemCount = static_cast<int>(Item::HiddenItems) + 1;

typedef bool Settings[OperationCount][ItemCount];

void Init(COptionsMgr *pOptionsMgr);
void Load(COptionsMgr *pOptionsMgr, Settings settings);
void Save(COptionsMgr *pOptionsMgr, const Settings& settings);
bool Get(COptionsMgr* pOptionsMgr, Operation operation, Item item);
bool GetDefault(COptionsMgr* pOptionsMgr, Operation operation, Item item);
}}
