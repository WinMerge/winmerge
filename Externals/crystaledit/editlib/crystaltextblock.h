////////////////////////////////////////////////////////////////////////////
//	File:		CrystalTextBlock.h
//
//	Author:		Sven Wiegand
//	E-mail:		sven.wiegand@gmx.de
//
//	Interface of the CCrystalTextBlock class, a part of Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use or modify this code to the following restrictions:
//	- Acknowledge me somewhere in your about box, simple "Parts of code by.."
//	will be enough. If you can't (or don't want to), contact me personally.
//	- LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace CrystalLineParser { struct TEXTBLOCK; }

/**
Objects of this class are passed to CCrystalParser::ParseLine() to get the
positions of color changes in a text line.

You can simply add a color change using the Add()-Method. You do add the
color changes sorted by position -- the Add()-Method will do this for you.

@author Sven Wiegand
*/
class EDITPADC_CLASS CCrystalTextBlock  
{
	friend class CCrystalTextView;
	friend class CCrystalEditViewEx;

// construction/destruction
protected:
	CCrystalTextBlock( CrystalLineParser::TEXTBLOCK *pTextBlock, int &rnActualItems );

// operations
public:
	/**
	Adds a text block to the array.

	You do not need to add the text blocks in the order of character position.

	@param nCharPos
		Character position of the line, where the text block starts.
	@param nColorIndex
		Index of the color to use to colorize the text block.
	*/
	bool Add( int nCharPos, int nColorIndex );

// attributes
protected:
	/** text block to work on */
	CrystalLineParser::TEXTBLOCK	*m_pTextBlock;

	/** Index */
	int &m_rnActualItems;
};
