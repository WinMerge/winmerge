/** 
 * @file FileTextStats.h
 *
 * @brief Declaration file for FileTextStats structure.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef FileTextStats_h_included
#define FileTextStats_h_included

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
	int first_zero; /**< Byte offset to first zero-byte, initially -1 */
	int last_zero; /**< Byte offset to last zero-byte, initially -1 */
	int nlosses;
	FileTextStats() { clear(); }
	void clear() { ncrs = nlfs = ncrlfs = nzeros = nlosses = 0; first_zero = -1; last_zero = -1; }
};


#endif // FileTextStats_h_included
