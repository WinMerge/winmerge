#pragma once

#include "UnicodeString.h"

struct IListCtrl
{
	virtual int GetColumnCount() const = 0;
	virtual int GetRowCount() const = 0;
	virtual String GetColumnName(int col) const = 0;
	virtual String GetItemText(int row, int col) const = 0;
	virtual void *GetItemData(int row) const = 0;
	virtual int GetBackColor(int row) const = 0;
	virtual bool IsSelectedItem(int sel) const = 0;
	virtual int GetNextItem(int sel, bool selected = false, bool reverse = false) const = 0;
	virtual int GetNextSelectedItem(int sel, bool reverse = false) const = 0;
	virtual unsigned GetSelectedCount() const = 0;
	virtual int GetIndent(int row) const = 0;
	virtual int GetIconIndex(int row) const = 0;
	virtual int GetIconCount() const = 0;
	virtual std::string GetIconPNGData(int iconIndex) const = 0;
};
