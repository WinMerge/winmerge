#include "StdAfx.h"
#define ICU_EXTERN
#include "icu.hpp"
#include "icurules.h"
#include "icucharbrkbinrules.h"
#include "icuwordbrkbinrules.h"

static ICULoader m_ICULoader; 
HMODULE ICULoader::m_hLibrary = nullptr;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<1>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<2>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<3>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharacterBreakIterator<4>;
thread_local std::unique_ptr<ICUBreakIterator> m_pWordBreakIterator;
