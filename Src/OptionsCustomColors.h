#ifndef _OPTIONS_CUSTOMCOLORS_H_
#define _OPTIONS_CUSTOMCOLORS_H_

#include <windows.h>

namespace Options { namespace CustomColors {

void Load(COLORREF * colors);
void Save(const COLORREF * colors);

}}

#endif
