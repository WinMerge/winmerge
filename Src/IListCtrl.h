#ifndef _ILISTCTRL_H_
#define _ILISTCTRL_H_

#include "UnicodeString.h"

struct IListCtrl
{
	virtual int GetColumnCount() const = 0;
	virtual int GetRowCount() const = 0;
	virtual String GetColumnName(int col) const = 0;
	virtual String GetItemText(int row, int col) const = 0;
};

#endif
