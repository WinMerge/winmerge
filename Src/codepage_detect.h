#ifndef codepage_detect_h_included
#define codepage_detect_h_included

struct FileTextEncoding;

void GuessCodepageEncoding(LPCTSTR filepath, FileTextEncoding * encoding, BOOL bGuessEncoding);

bool GuessEncoding_from_bytes(LPCTSTR ext, const char **data, int count, FileTextEncoding * encoding);

#endif // codepage_detect_h_included
