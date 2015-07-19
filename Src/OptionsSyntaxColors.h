/** 
 * @file  OptionsSyntaxColors.h
 *
 * @brief Declaration file for SyntaxColors class utility
 */
#pragma once

class SyntaxColors;
class COptionsMgr;

namespace Options { namespace SyntaxColors {

void Load(COptionsMgr *pOptionsMgr, ::SyntaxColors *pSyntaxColors);
void Save(COptionsMgr *pOptionsMgr, const ::SyntaxColors *pSyntaxColors);

}}
