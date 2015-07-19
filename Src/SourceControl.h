/** 
 * @file  SourceControl.h
 *
 * @brief Declaration file for SourceControl
 */

#pragma once

class SourceControl
{
public:
	/**
	 * @brief Supported versioncontrol systems.
	 */
	enum
	{
		VCS_NONE = 0,
		VCS_VSS4,
		VCS_VSS5,
		VCS_CLEARCASE,
	};
};

