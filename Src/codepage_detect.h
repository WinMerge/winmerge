#ifndef codepage_detect_h_included
#define codepage_detect_h_included

void GuessCodepageEncoding(const CString & filepath, int * unicoding, int * codepage, 
                           BOOL bGuessEncoding);

bool GuessEncoding_from_bytes(const CString & sExt, const char **data, int count, int *codepage);

#endif // codepage_detect_h_included
