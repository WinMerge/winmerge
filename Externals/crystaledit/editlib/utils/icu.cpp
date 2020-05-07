#include "StdAfx.h"
#define ICU_EXTERN
#include "icu.hpp"

static ICULoader m_ICULoader; 
HMODULE ICULoader::m_hLibrary = nullptr;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharaterBreakIterator<1>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharaterBreakIterator<2>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharaterBreakIterator<3>;
template <> thread_local std::unique_ptr<ICUBreakIterator> m_pCharaterBreakIterator<4>;

// This rule set is based on character-break iterator rules of ICU 63.1
  // <https://github.com/unicode-org/icu/blob/release-63-1/icu4c/source/data/brkitr/rules/char.txt>.
const UChar* ICUBreakIterator::kCustomRules =
u"$CR          = [\\p{Grapheme_Cluster_Break = CR}];"
u"$LF          = [\\p{Grapheme_Cluster_Break = LF}];"
u"$Control     = [[\\p{Grapheme_Cluster_Break = Control}]];"
u"$VoiceMarks  = [\\uFF9E\\uFF9F];"
u"$Extend      = [[\\p{Grapheme_Cluster_Break = Extend}] - $VoiceMarks];"
u"$ZWJ         = [\\p{Grapheme_Cluster_Break = ZWJ}];"
u"$Regional_Indicator = [\\p{Grapheme_Cluster_Break = Regional_Indicator}];"
u"$Prepend     = [\\p{Grapheme_Cluster_Break = Prepend}];"
u"$SpacingMark = [\\p{Grapheme_Cluster_Break = SpacingMark}];"
u"$L           = [\\p{Grapheme_Cluster_Break = L}];"
u"$V           = [\\p{Grapheme_Cluster_Break = V}];"
u"$T           = [\\p{Grapheme_Cluster_Break = T}];"
u"$LV          = [\\p{Grapheme_Cluster_Break = LV}];"
u"$LVT         = [\\p{Grapheme_Cluster_Break = LVT}];"
u"$Extended_Pict = [:ExtPict:];"
u"!!chain;"
u"!!lookAheadHardBreak;"
u"$VoiceMarks;"
u"$CR $LF;"
u"$L ($L | $V | $LV | $LVT);"
u"($LV | $V) ($V | $T);"
u"($LVT | $T) $T;"
u"[^$Control $CR $LF] ($Extend | $ZWJ);"
u"[^$Control $CR $LF] $SpacingMark;"
u"$Prepend [^$Control $CR $LF];"
u"$Extended_Pict $Extend* $ZWJ $Extended_Pict;"
u"^$Prepend* $Regional_Indicator $Regional_Indicator / $Regional_Indicator;"
u"^$Prepend* $Regional_Indicator $Regional_Indicator;"
u".;";
