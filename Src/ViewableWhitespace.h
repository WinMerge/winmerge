#ifndef ViewableWhitespace_included_h
#define ViewableWhitespace_included_h

struct ViewableWhitespaceChars {
	int c_codepage; // unused in UNICODE
	TCHAR c_tab[3];
	TCHAR c_space[3];
	TCHAR c_cr[3];
	TCHAR c_lf[3];
	TCHAR c_eol[3]; // generic eol
};

const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage);

#endif // ViewableWhitespace_included_h
