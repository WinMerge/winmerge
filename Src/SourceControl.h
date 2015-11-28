/** 
 * @file  SourceControl.h
 *
 * @brief Declaration file for SourceControl
 */
#pragma once

#include "UnicodeString.h"
#include "VSSHelper.h"

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
	
	SourceControl();
	void InitializeSourceControlMembers();
	bool SaveToVersionControl(const String& strSavePath);
	void CheckinToClearCase(const String& strDestinationPath);
protected:
	String m_strVssUser; /**< Visual Source Safe User ID */
	String m_strVssPassword; /**< Visual Source Safe Password */
	String m_strVssDatabase; /**< Visual Source Safe database */
	String m_strCCComment; /**< ClearCase comment */
public:
	VSSHelper m_vssHelper; /**< Helper class for VSS integration */
	bool m_bCheckinVCS;     /**< TRUE if files should be checked in after checkout */
	bool m_CheckOutMulti; /**< Suppresses VSS int. code asking checkout for every file */
	bool m_bVCProjSync; /**< VC project opened from VSS sync? */
	bool m_bVssSuppressPathCheck; /**< Suppresses VSS int code asking about different path */
};

