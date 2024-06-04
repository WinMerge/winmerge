#ifndef unicheck_h_included
#define unicheck_h_included

/**
 * @brief Information about whether a file is in a Unicode encoding
 */
struct unicodingInfo
{
	typedef enum { 
		UNICODE_NONE,
		UNICODE_UTF8,     // UTF-8
		UNICODE_UCS2LE,   // UCS-2LE or UTF-16LE
		UNICODE_UCS2BE,   // UCS-2BE or UTF-16BE
		UNICODE_UTF32LE,  // UTF-32LE
		UNICODE_UTF32BE   // UTF-32BE
	} UNICODE_TYPE;
	UNICODE_TYPE type;
	int bom_width; // in bytes
	int char_width; // in bytes (1 for NONE or UTF-8)
	int low_byte; // offset in bytes
};

bool CheckForBom(char * buffer, size_t len, unicodingInfo * uinfo);

#endif // unicheck_h_included
