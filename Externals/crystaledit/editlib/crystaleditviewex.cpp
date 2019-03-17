// CrystalEditViewEx.cpp: Implementierung der Klasse CCrystalEditViewEx.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "crystalparser.h"
#include "crystaleditviewex.h"

using CrystalLineParser::TEXTBLOCK;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCrystalEditViewEx,CCrystalEditView)


CCrystalEditViewEx::CCrystalEditViewEx()
: CCrystalEditView()
{
	m_pSyntaxParser = nullptr;
}


HINSTANCE CCrystalEditViewEx::GetResourceHandle()
{
	// -> HE
	// call inherited
	return CCrystalEditView::GetResourceHandle();
	// <- HE
	// OLD
	//return AfxGetInstanceHandle();
}


DWORD CCrystalEditViewEx::ParseLine( DWORD dwCookie, int nLineIndex, 
																		TEXTBLOCK *pBuf, int &nActualItems )
{
	if( m_pSyntaxParser != nullptr )
	{
		CCrystalTextBlock	*pTextBlock = 
			pBuf? new CCrystalTextBlock( (TEXTBLOCK*)pBuf, nActualItems ) : nullptr;
		dwCookie = m_pSyntaxParser->ParseLine( dwCookie, nLineIndex, pTextBlock );
		
		if( pTextBlock != nullptr )
			delete pTextBlock;

		return dwCookie;
	}
	else
		return 0;
}


CCrystalParser *CCrystalEditViewEx::SetSyntaxParser( CCrystalParser *pParser )
{
	CCrystalParser	*pOldParser = m_pSyntaxParser;

	m_pSyntaxParser = pParser;

	if( pParser != nullptr )
  //BEGIN FP
    pParser->m_pTextView = this;
		/*ORIGINAL
    pParser->m_pEditViewEx = this;
		*/
  //END FP

	return pOldParser;
}
