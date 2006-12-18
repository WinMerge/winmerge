/** 
 * @file  MovedLines.cpp
 *
 * @brief Implementation of MovedLines class.
 */

#include "stdafx.h"
#include "MovedLines.h"

/**
 * @brief clear the lists of moved blocks.
 */
void MovedLines::Clear()
{
	m_moved0.RemoveAll(); 
	m_moved1.RemoveAll(); 
}

/**
 * @brief Add moved block to the list.
 * @param [in] side1 First side we are mapping.
 * @param [in] line1 Linenumber in side first side.
 * @param [in] line2 Linenumber in second side.
 */
void MovedLines::Add(ML_SIDE side1, unsigned int line1,	unsigned int line2)
{
	MovedLineMap * list = NULL;
	if (side1 == SIDE_LEFT)
		list = &m_moved0;
	else
		list = &m_moved1;

	(*list)[line1] = line2;
}

/**
 * @brief Check if line is in moved block.
 * @param [in] line Linenumber to check.
 * @param [in] side Side of the linenumber.
 */
int MovedLines::LineInBlock(unsigned int line, ML_SIDE side)
{
	if (side == SIDE_LEFT)
		return SecondSideInMovedBlock(line);
	else
		return FirstSideInMovedBlock(line);
}

/**
 * @brief Get right->left info for a moved line (real line number)
 */
int MovedLines::FirstSideInMovedBlock(unsigned int secondSideLine)
{
	int firstSideLine;
	if (m_moved1.Lookup(secondSideLine, firstSideLine))
		return firstSideLine;
	else
		return -1;
}

/**
 * @brief Get left->right info for a moved line (real line number)
 */
int MovedLines::SecondSideInMovedBlock(unsigned int firstSideLine)
{
	int secondSideLine;
	if (m_moved0.Lookup(firstSideLine, secondSideLine))
		return secondSideLine;
	else
		return -1;
}
