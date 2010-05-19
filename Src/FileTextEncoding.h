/**
 * @file  FileTextEncoding.h
 *
 * @brief Declaration of FileTextEncoding structure
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FileTextEncoding_h_included
#define FileTextEncoding_h_included

#include "unicoder.h"

/**
 * @brief Text encoding (eg, UTF-8, or CP-1252)
 */
struct FileTextEncoding
{
	int m_codepage; /**< 8bit codepage, if applicable, -1 is unknown or N/A */
	ucr::UNICODESET m_unicoding; /**< Unicode encoding. */
	bool m_bom; /**< Unicode byte marker */
	bool m_guessed; /**< Whether encoding was guessed from content */
	bool m_binary;

	FileTextEncoding();
	void Clear();
	void SetCodepage(int codepage);
	void SetUnicoding(ucr::UNICODESET unicoding);
	String GetName() const;

	static int Collate(const FileTextEncoding & fte1, const FileTextEncoding & fte2);
};


#endif // FileTextEncoding_h_included
