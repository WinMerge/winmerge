#ifndef codepage_detect_h_included
#define codepage_detect_h_included

struct FileTextEncoding;

void GuessCodepageEncoding(LPCTSTR filepath, FileTextEncoding * encoding, BOOL bGuessEncoding);

unsigned GuessEncoding_from_bytes(LPCTSTR ext, const char *src, size_t len);

#endif // codepage_detect_h_included
