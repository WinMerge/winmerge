#include "StdAfx.h"
#define ICU_EXTERN
#include "icu.hpp"

HMODULE ICULoader::m_hLibrary = nullptr;
static ICULoader m_ICULoader; 