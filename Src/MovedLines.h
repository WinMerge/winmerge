/** 
 * @file  MovedLines.h
 *
 * @brief Declaration of MovedLines class
 */
#pragma once

#include <map>

/**
 * @brief Container class for moved lines/blocks.
 * This class contains list of moved blocs/lines we detect
 * when comparing files.
 */
class MovedLines
{
public:
	/** @brief Sides for mapping functions. */
	enum class SIDE
	{
		LEFT,
		RIGHT,
	};

	void Clear();
	void Add(SIDE side1, unsigned line1, unsigned line2);
	int LineInBlock(unsigned line, SIDE side) const;

protected:
	int FirstSideInMovedBlock(unsigned secondSideLine) const;
	int SecondSideInMovedBlock(unsigned firstSideLine) const;

private:
	typedef std::map<int, int> MovedLinesMap;
	MovedLinesMap m_moved0; /**< Moved lines map for first side */
	MovedLinesMap m_moved1; /**< Moved lines map for second side */
};
