#ifndef ViewableWhitespace_included_h
#define ViewableWhitespace_included_h


struct ViewableWhitespaceChars {
	int c_codepage; // unused in UNICODE
	LPCTSTR c_tab;
	LPCTSTR c_space;
	LPCTSTR c_cr;
	LPCTSTR c_lf;
	LPCTSTR c_eol; // generic eol
};

const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage);

#endif // ViewableWhitespace_included_h
