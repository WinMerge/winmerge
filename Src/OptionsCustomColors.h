#pragma once

#include <windows.h>

namespace Options { namespace CustomColors {

void Load(COLORREF * colors);
void Save(const COLORREF * colors);

}}
