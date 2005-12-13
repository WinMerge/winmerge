#include "stdafx.h"
#include <fstream>

#include "Unpacker.h"
#include "unicheck.h"
#include "Setting.h"

using namespace std;


static unsigned int char_to_unicode(unsigned char ch);

class StdFile
{
public:
	StdFile(FILE * fp = 0) : m_fp(fp) { }
	~StdFile() { Close(); }
	bool Open(LPCTSTR filepath, LPCTSTR mode) 
		{
			Close();
			m_fp = fopen(filepath, mode);
			return (m_fp != 0);
		}
	void Close()
		{
			if (m_fp)
			{
				fflush(m_fp);
				fclose(m_fp);
				m_fp = 0;
			}
		}
	operator FILE * () { return m_fp; }
private:
	FILE * m_fp;
};

class Packer
{
public:
	static int HexValue(TCHAR uch);
public:
	Packer(CWSTR srcFilepath, CWSTR destFilepath);

	bool Unpack();
	bool Pack();

private:
	void UnpackChar(unsigned int ch);
	void WriteChar(unsigned int ch);
	void LogError(LPCTSTR fmt, ...);

private:
	CWSTR m_srcFilepath;
	CWSTR m_destFilepath;
	StdFile m_in;
	StdFile m_out;
	unicodingInfo m_uninfo;
	unsigned char m_escape;
};

bool Unpack(CWSTR srcFilepath, CWSTR destFilepath)
{
	Packer pack(srcFilepath, destFilepath);
	if (!pack.Unpack()) return false;
	return true;
}



bool Pack(CWSTR srcFilepath, CWSTR destFilepath)
{
	Packer pack(srcFilepath, destFilepath);
	if (!pack.Pack()) return false;
	return true;
}

// Is specified character a valid escape character?
static bool
ValidEscape(TCHAR ch)
{
	if (ch >= 0 && ch <= 0x20)
		return false;
	if (Packer::HexValue(ch) >= 0)
		return false;
	return true;
}

Packer::Packer(CWSTR srcFilepath, CWSTR destFilepath)
: m_srcFilepath(srcFilepath)
, m_destFilepath(destFilepath)
{
	m_escape = '#';
	TCHAR buff[2] = _T("#");
	LPCTSTR settings = _T("Software\\Thingamahoochie\\WinMerge_Plugins\\EditBinaryFiles");
	if (ReadSetting(settings, _T("EscapeChar"), buff, sizeof(buff)/sizeof(buff[0])))
	{
		if (buff[1] == 0 && ValidEscape(buff[0]))
			m_escape = buff[0];
	}
}

bool Packer::Unpack()
{
	USES_CONVERSION;

	LPCTSTR szSrcFile = W2T(m_srcFilepath);
	if (!m_in.Open(szSrcFile, _T("rb")))
	{
		LogError(_T("Error opening Unpack src file [%s]"), szSrcFile);
		return false;
	}

	LPCTSTR szDestFile = W2T(m_destFilepath);
	if (!m_out.Open(szDestFile, _T("wb")))
	{
		LogError(_T("Error opening Unpack dest file [%s]"), szDestFile);
		return false;
	}

	fseek(m_in, 0, 2); // go to end of file
	int len = ftell(m_in);
	fseek(m_in, 0, 0); // go back to start of file

	unsigned char buffer[65536];

	int curlen = (len>4 ? 4 : len);
	if (!curlen)
		return false; // do not support empty files

	int rtn = fread(buffer, 1, curlen, m_in);
	if (rtn != curlen) return false;
	len -= curlen;
	CheckForBom(&buffer[0], curlen, &m_uninfo);

	// For now we don't handle any Unicode files at all
	// because they require the added complication
	// of alignment (for UCS-2/UTF-16/UCS-4)
	// and varying length (for UTF-8/UTF-16)
	// Handling the varying length across buffers is a pain I've not figured out

	if (m_uninfo.type != unicodingInfo::UNICODE_NONE)
		return false;

	// Process file
	while (1)
	{
		if (!curlen) break;
		for (int i=0; i<curlen; ++i)
		{
			UnpackChar(char_to_unicode(buffer[i]));
		}
		curlen = len;
		if (curlen > 65536)
			curlen = 65536;
		if (!curlen)
			break;
		int rtn = fread(buffer, 1, curlen, m_in);
		len -= curlen;
		if (rtn != curlen)
			return false;
	}
	m_in.Close();
	m_out.Close();
	return true;
}

// Return hex value of specified digit, or -1
int Packer::HexValue(TCHAR uch)
{
	if (uch >= '0' && uch <= '9')
		return (uch - '0');
	else if (uch >= 'a' && uch <= 'f')
		return (uch - 'a') + 10;
	else if (uch >= 'A' && uch <= 'F')
		return (uch - 'A') + 10;
	else
		return -1;
}

bool Packer::Pack()
{
	USES_CONVERSION;

	LPCTSTR szSrcFile = W2T(m_srcFilepath);
	if (!m_in.Open(szSrcFile, _T("rb")))
	{
		LogError(_T("Error opening Pack src file [%s]"), szSrcFile);
		return false;
	}

	LPCTSTR szDestFile = W2T(m_destFilepath);
	if (!m_out.Open(szDestFile, _T("wb")))
	{
		LogError(_T("Error opening Pack dest file [%s]"), szDestFile);
		return false;
	}

	fseek(m_in, 0, 2); // go to end of file
	int len = ftell(m_in);
	fseek(m_in, 0, 0); // go back to start of file

	unsigned char buffer[65536];

	int curlen = (len>4 ? 4 : len);
	if (!curlen)
		return false; // do not support empty files

	int rtn = fread(buffer, 1, curlen, m_in);
	if (rtn != curlen) return false;
	len -= curlen;
	CheckForBom(&buffer[0], curlen, &m_uninfo);

	// For now we don't handle any Unicode files at all
	// because they require the added complication
	// of alignment (for UCS-2/UTF-16/UCS-4)
	// and varying length (for UTF-8/UTF-16)
	// Handling the varying length across buffers is a pain I've not figured out

	if (m_uninfo.type != unicodingInfo::UNICODE_NONE)
		return false;

	bool escaped = false; // true when processing escape
	int digit1=-1, digit2=-1; // escaped digits

	// Process file
	while (1)
	{
		if (!curlen) break;
		for (int i=0; i<curlen; ++i)
		{
			unsigned int uch = char_to_unicode(buffer[i]);
			if (escaped)
			{
				int newdigit = -1;
				if (uch >= '0' && uch <= '9')
					newdigit = (uch - '0');
				else if (uch >= 'a' && uch <= 'f')
					newdigit = (uch - 'a') + 10;
				else if (uch >= 'A' && uch <= 'F')
					newdigit = (uch - 'A') + 10;
				else
					return false;
				if (digit1 == -1)
				{
					digit1 = newdigit;
					digit2 = -1;
				}
				else
				{
					digit2 = newdigit;
					uch = (digit1 * 16 + digit2);
					WriteChar(uch);
					escaped = false;
				}
			}
			else
			{
				if (uch == m_escape)
				{
					escaped = true;
					digit1 = -1;
					digit2 = -1;
				}
				else
				{
					WriteChar(uch);
				}
			}
		}
		curlen = len;
		if (curlen > 65536)
			curlen = 65536;
		if (!curlen)
			break;
		int rtn = fread(buffer, 1, curlen, m_in);
		len -= curlen;
		if (rtn != curlen)
			return false;
	}


	m_in.Close();
	m_out.Close();
	
	return true;
}

// convert input byte to unicode codepoint
static unsigned int
char_to_unicode(unsigned char ch)
{
	return ch;
}

// Unpack character
// This means escaping binary bytes or occurrences of escape character
void
Packer::UnpackChar(unsigned int ch)
{
	if (ch == m_escape)
	{
		WriteChar(m_escape);
		WriteChar(m_escape);
		return;
	}
	else if (ch < 0x20)
	{
		if (ch != '\n'
			&& ch != '\r'
			&& ch != '\t')
		{
			WriteChar(m_escape);
			TCHAR code[5];
			sprintf(code, "%02x", ch);
			for (int j=0; code[j] != 0; ++j)
			{
				WriteChar(code[j]);
			}
			return;
		}
	}
	WriteChar(ch);
}

// Write out a unicode codepoint to output
// Handles various unicode encodings
// probably does not handle UTF-8 correctly
void
Packer::WriteChar(unsigned int ch)
{
	unsigned char oval = '?';
	if (ch < 0x100)
		oval = (unsigned char)ch;
	int rtn = fwrite(&oval, 1, 1, m_out);
}

// Log error to log file if available
void
Packer::LogError(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[4096];
	// _vsnprintf into local buffer
	// so we can see error even tho logging not yet implemented
	_vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), fmt, args);
	va_end(args);

	if (0)
	{
		// TODO: Prefix with date
		StdFile logfile;
		if (logfile.Open(_T("EditBinaryFiles.log"), _T("a")))
		{
			fprintf(logfile, buffer);
		}
	}
}

