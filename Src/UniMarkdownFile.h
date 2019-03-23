/**
 *  @file UniMarkdownFile.h
 *
 *  @brief Declaration of UniMarkdownFile class.
 */
#pragma once

#include <memory>
#include "Common/UniFile.h"

class CMarkdown;

/**
 * @brief XML file reader class.
 */
class UniMarkdownFile : public UniMemFile
{
public:
	UniMarkdownFile();
	virtual bool ReadString(String & line, String & eol, bool * lossy) override;
	virtual void Close() override;

protected:
	virtual bool DoOpen(const String& filename, AccessMode mode) override;

private:
	void Move();
	String maketstring(const char *lpd, size_t len);

	int m_depth;
	bool m_bMove;
	unsigned char *m_transparent;
	std::unique_ptr<CMarkdown> m_pMarkdown;
};
