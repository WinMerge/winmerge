/**
 *  @file WaitStatusCursor.h
 *
 *  @brief Declaration WaitStatusCursor classes.
 */ 
#pragma once

#include <memory>
#include "UnicodeString.h"

/**
 * @brief WaitStatusCursor enhanced with GUI status feedback.
 * 
 * A WaitStatusCursor is a limited CustomStatusCursor.
 * It is easier to create and display the standard wait cursor.
 */
class WaitStatusCursor
{
// public interface
public:
	WaitStatusCursor(const String& fmt, ...);
	~WaitStatusCursor();
private:
	WaitStatusCursor(const WaitStatusCursor &);
	WaitStatusCursor & operator=(const WaitStatusCursor &);

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
