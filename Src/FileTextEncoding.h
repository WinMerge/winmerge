/**
 * @file  FileTextEncoding.h
 *
 * @brief Declaration of FileTextEncoding structure
 */
#pragma once

#include "unicoder.h"

/**
 * @brief Text encoding (eg, UTF-8, or CP-1252)
 */
struct FileTextEncoding
{
	int m_codepage; /**< 8bit codepage, if applicable, -1 is unknown or N/A */
	ucr::UNICODESET m_unicoding; /**< Unicode encoding. */
	bool m_bom; /**< Unicode byte marker */

	FileTextEncoding();
	void Clear();
	void SetCodepage(int codepage);
	void SetUnicoding(ucr::UNICODESET unicoding);
	String GetName() const;

	static int Collate(const FileTextEncoding & fte1, const FileTextEncoding & fte2);
	bool operator==(const FileTextEncoding & fte2) const { return Collate(*this, fte2) == 0; }
	bool operator!=(const FileTextEncoding & fte2) const { return Collate(*this, fte2) != 0; }
};
