/** 
 * @file  MovedLines.cpp
 *
 * @brief Implementation of MovedLines class.
 */

#include "pch.h"
#include "MovedLines.h"

/**
 * @brief clear the lists of moved blocks.
 */
void MovedLines::Clear()
{
	m_moved0.clear();
	m_moved1.clear();
}

/**
 * @brief Add moved block to the list.
 * @param [in] side1 First side we are mapping.
 * @param [in] line1 Linenumber in side first side.
 * @param [in] line2 Linenumber in second side.
 */
void MovedLines::Add(ML_SIDE side1, unsigned line1,	unsigned line2)
{
	MovedLinesMap *list;
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
int MovedLines::LineInBlock(unsigned line, ML_SIDE side) const
{
	if (side == SIDE_LEFT)
		return SecondSideInMovedBlock(line);
	else
		return FirstSideInMovedBlock(line);
}

/**
 * @brief Get right->left info for a moved line (real line number)
 */
int MovedLines::FirstSideInMovedBlock(unsigned secondSideLine) const
{
	MovedLinesMap::const_iterator iter;
	iter = m_moved1.find(secondSideLine);
	if (iter != m_moved1.end())
		return iter->second;
	else
		return -1;
}

/**
 * @brief Get left->right info for a moved line (real line number)
 */
int MovedLines::SecondSideInMovedBlock(unsigned firstSideLine) const
{
	MovedLinesMap::const_iterator iter;
	iter = m_moved0.find(firstSideLine);
	if (iter != m_moved0.end())
		return iter->second;
	else
		return -1;
}
