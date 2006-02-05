#ifndef MergeLineFlags_h_included
#define MergeLineFlags_h_included

/** 
 The Crystal Editor keeps a DWORD of flags for each line.
 It does not use all of the available bits.
 WinMerge uses some of the high bits to keep WinMerge-specific
 information; here are the list of WinMerge flags.
 So, these constants are used with the SetLineFlags(2) calls.
*/
enum MERGE_LINEFLAGS
{
	LF_DIFF = 0x00200000L,
	LF_TRIVIAL = 0x00800000L,
	LF_MOVED = 0x01000000L,
};


// WINMERGE_FLAGS is MERGE_LINEFLAGS | GHOST_LINEFLAGS | LF_TRIVIAL | LF_MOVED
#define LF_WINMERGE_FLAGS    0x01E00000

#endif // MergeLineFlags_h_included
