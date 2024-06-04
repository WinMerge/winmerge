/**
 *  @file MergeLineFlags.h
 *
 *  @brief Additional lineflags for editor.
 */ 
#pragma once

#include "LineInfo.h"

/** 
 The Crystal Editor keeps a DWORD of flags for each line.
 It does not use all of the available bits.
 WinMerge uses some of the high bits to keep WinMerge-specific
 information; here are the list of WinMerge flags.
 So, these constants are used with the SetLineFlag(2) calls.
*/
enum MERGE_LINEFLAGS : lineflags_t
{
	LF_DIFF = 0x00200000UL,
	//	LF_GHOST = 0x00400000UL, 
	LF_TRIVIAL = 0x00800000UL,
	LF_MOVED = 0x01000000UL,
	LF_SNP = 0x02000000UL,
	// LF_WINMERGE_FLAGS is LF_DIFF | LF_GHOST | LF_TRIVIAL | LF_MOVED | LF_SNP
	LF_WINMERGE_FLAGS = 0x03E00000UL,
	// Flags for non-ignored difference
	// Note that we must include ghost flag to include ghost lines
	LF_NONTRIVIAL_DIFF = ((LF_DIFF | LF_GHOST) & (~LF_TRIVIAL))
};

