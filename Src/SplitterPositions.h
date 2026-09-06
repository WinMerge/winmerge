#pragma once

#include "UnicodeString.h"
#include <functional>

class COptionsMgr;

namespace SplitterPositions
{
	void Load(const String& name, int nRow, int nPanes, std::function<void(const double*, int)> func);
	void Save(const String& name, int nRow, int nPanes, std::function<double(int)> func);
	double LoadRowPosition(const String& name);
	void SaveRowPosition(const String& name, const double* ratio);
	int GetRowCount(const String& name);
}
