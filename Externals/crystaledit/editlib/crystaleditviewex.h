// CrystalEditViewEx.h: Declaration for CCrystalEditViewEx class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ccrystaleditview.h"
#include "crystalparser.h"


class EDITPADC_CLASS CCrystalEditViewEx : public CCrystalEditView  
{
	DECLARE_DYNCREATE(CCrystalEditViewEx)

protected:
	friend class CCrystalParser;

/** @construction/destruction */
public:
	CCrystalEditViewEx();

/** @operations */
public:
	/**
	Attaches a syntax parser to this view.

	@param pParser
		The syntax parser to attach to this view. The Parser must be derived
		from CCrystalParser.

	@return
		The syntax parser that was used until the call to this function or
		`nullptr` if no one was attached to this view.
	*/
	CCrystalParser *SetSyntaxParser( CCrystalParser *pParser );

/** @overridables */
protected:
	/**
	@description
		Returns a handle to the instance (DLL or EXE)

	@return
		Handle to the Instance (DLL or EXE)

	@remark
		Equivalent to AfxGetInstanceHandle()
	*/
	virtual HINSTANCE GetResourceHandle() override;

	
	/**
	@description
		Is called by tht view, when there is a Line to parse.

	@param dwCookie
		The result of parsing the previous line. This parameter contains flags,
		you set while parsing the previous line. Normaly this will be flags that
		indicate a region that was still open at the end of the last line. You
		set those flags for a line by returning them ored (|).
	@param nLineIndex
		The zero-based index of the line to parse.
	@param pBuf
		You have to split the lines in to parts which are specified by there
		beginning (zero-based index of the character in this line) and there text
		color. You have to specifie each one of this blocks in this Buffer.
		You can define a new Block with the macro DEFINE_BLOCK( pos, colorindex ).

		When the function is called with this parameter set to `nullptr`, you only
		have to calculate the cookies.
	@param nActualItems
		Used by the macro DEFINE_BLOCK.

	@return 
		The calculated flags for this line (see dwCookie).
	*/
	virtual unsigned ParseLine( unsigned dwCookie, int nLineIndex, 
		CrystalLineParser::TEXTBLOCK *pBuf, int &nActualItems );
	
/** @attributes */
protected:
	/**
	@description
		The parser used to parse the Text or `nullptr` if no parser is used.
	*/
	CCrystalParser *m_pSyntaxParser;
};
