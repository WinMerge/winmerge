/** 
 * @file  IOptionsPanel.h
 *
 * @brief Declaration file for IOptionsPanel Interface
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _IOPTIONSPANEL_H_
#define _IOPTIONSPANEL_H_

/**
 * @brief Interface for Options Panels.
 *
 * Every options-page must implement this interface and functions in it.
 * - ReadOptions() read options from storage to UI
 * - SaveOptions() saves them from UI to storage
 */
class IOptionsPanel
{
public:
	virtual void ReadOptions() = 0;
	virtual void WriteOptions() = 0;
};

#endif // IOPTIONSPANEL_H_
