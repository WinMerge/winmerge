// CrystalEditViewEx.cpp: Implementierung der Klasse CCrystalEditViewEx.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrystalParser.h"
#include "CrystalEditViewEx.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCrystalEditViewEx,CCrystalEditView)


CCrystalEditViewEx::CCrystalEditViewEx()
: CCrystalEditView()
{
	m_pSyntaxParser = NULL;
}


HINSTANCE CCrystalEditViewEx::GetResourceHandle()
{
#ifdef CE_DLL_BUILD
	return g_hDllInstance;
#else
	// -> HE
	// call inherited
	return CCrystalEditView::GetResourceHandle();
	// <- HE
	// OLD
	//return AfxGetInstanceHandle();
#endif
}


DWORD CCrystalEditViewEx::ParseLine( DWORD dwCookie, int nLineIndex, 
																		TEXTBLOCK *pBuf, int &nActualItems )
{
	if( m_pSyntaxParser )
	{
		CCrystalTextBlock	*pTextBlock = 
			pBuf? new CCrystalTextBlock( (CCrystalTextBlock::TEXTBLOCK*)pBuf, nActualItems ) : NULL;
		dwCookie = m_pSyntaxParser->ParseLine( dwCookie, nLineIndex, pTextBlock );
		
		if( pTextBlock )
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

	if( pParser )
  //BEGIN FP
    pParser->m_pTextView = this;
		/*ORIGINAL
    pParser->m_pEditViewEx = this;
		*/
  //END FP

	return pOldParser;
}
