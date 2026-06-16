////////////////////////////////////////////////////////////////////////////
//	File:		CrystalParser.h
//
//	Author:		Sven Wiegand
//	E-mail:		sven.wiegand@gmx.de
//
//	Interface of the CCrystalParser class, a part of Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use or modify this code to the following restrictions:
//	- Acknowledge me somewhere in your about box, simple "Parts of code by.."
//	will be enough. If you can't (or don't want to), contact me personally.
//	- LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

/**
Objects of this class are used to describe language properties. At this time
language properties are:
<ul>
	<li> Syntax highlighting (method ParseLine())
	<li> Word wrapping (method WrapLine())
</ul>

Derive your own class from CCrystalParser and override the method ParseLine()
to modify syntax highlighting and/or WrapLine() to modify line wrapping.

Use the CCrystalTextView::SetParser() function to attach a parser to a text
view.

@author Sven Wiegand
*/
class EDITPADC_CLASS CCrystalParser : public CObject  
{
	DECLARE_DYNAMIC(CCrystalParser)
	friend class CCrystalTextView;
	friend class CCrystalEditView;

// construction/destruction
public:
	CCrystalParser();
	virtual ~CCrystalParser();

// overridables
public:
	/**
	Called by CCrystalTextView-object to wrap the line with the given index into sublines.

	The standard implementation wraps the line behind the first whitespace in front
	of the word that exceeds the visible line width (nMaxLineWidth). Override
	this function to provide your own word wrapping.

	<b>Attention:</b> Never call this function directly in other parts of the code
	call WrapLineCached() instead, which will work much faster in most cases.

	@param nLineIndex The index of the line to wrap

	@param nMaxLineWidth The number of characters a subline of this line should
	not exceed

	@param anBreaks An array of integers. Put the positions where to wrap the line
	in that array (its allready allocated). If this pointer is `nullptr`, the function
	has only to compute the number of breaks (the parameter nBreaks).

	@param nBreaks The number of breaks this line has (number of sublines - 1). When
	the function is called, this variable is 0. If the line is not wrapped, this value
	should be 0 after the call.

	@see WrapLineCached()
	*/
	virtual void WrapLine( int nLineIndex, int nMaxLineWidth, std::vector<int> *anBreaks, int &nBreaks );

// attributes
protected:
	/**	The text view this parser is attached to. */
	CCrystalTextView *m_pTextView;
//	ICUBreakIterator m_iterChar;
};
