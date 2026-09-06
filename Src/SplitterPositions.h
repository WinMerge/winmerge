#pragma once

#include "UnicodeString.h"
#include <functional>

class COptionsMgr;

namespace SplitterPositions
{
	void LoadPaneRatio(const String& name, int nRow, int nPanes, std::function<void(const double*, int)> func);
	void SavePaneRatios(const String& name, int nRow, int nPanes, std::function<double(int)> func);
	double LoadRowRatio(const String& name);
	void SaveRowRatio(const String& name, const double* ratio);
	int GetSplitterRowCount(const String& name);
}
