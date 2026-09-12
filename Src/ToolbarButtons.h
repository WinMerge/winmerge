/**
 * @file ToolbarButtons.h
 * @brief Header file for toolbar button management.
 */
#pragma once

#include "FrameType.h"
#include <vector>

namespace ToolbarButtons
{
	std::vector<UINT> GetToolbarButtons(FRAMETYPE frameType, int nFiles, bool hasDirDoc);
	int GetToolbarImageIndex(UINT nID);
	int GetToolbarImageCount();
}