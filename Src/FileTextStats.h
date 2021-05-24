/** 
 * @file FileTextStats.h
 *
 * @brief Declaration file for FileTextStats structure.
 */
#pragma once

/**
 * @brief Structure containing statistics about compared file.
 * This structure contains EOL-byte and zero-byte statistics from compared
 * file. Those statistics can be used to determine EOL style and binary
 * status of the file.
 */
struct FileTextStats
{
	int ncrs; /**< Count of MAC (CR-byte) EOLs. */
	int nlfs; /**< Count of Unix (LF-byte) EOLs. */
	int ncrlfs; /**< Count of DOS (CR+LF-bytes) EOLs. */
	int nzeros; /**< Count of zero-bytes. */
	FileTextStats() { clear(); }
	void clear() { ncrs = nlfs = ncrlfs = nzeros = 0; }
};
