///////////////////////////////////////////////////////////////////////////
//  File:    regexp.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Marko Macek
//  E-mail:     Marko.Macek@gmx.net or mark@hermes.si
//
//  Some handy stuff to deal with regular expressions
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cregexp.h"
#include "pcre.h"
#include "Ucs2Utf8.h"
#include "unicoder.h"

struct _RxNode {
	pcre *regexp;
	pcre_extra *pe;
};

RxNode *RxCompile(LPCTSTR Regexp, unsigned int RxOpt) {
    RxNode *n = 0;
    if (Regexp == 0) return 0;
    n = new RxNode();
    if (n == 0) return 0;

	const char * errormsg = NULL;
	int erroroffset = 0;
	char *regexString = UCS2UTF8_ConvertToUtf8(Regexp);
	int pcre_opts = 0;

#ifdef UNICODE
    pcre_opts |= PCRE_UTF8;
#endif
    pcre_opts |= PCRE_BSR_ANYCRLF;
    if ((RxOpt & RX_CASE) == 0)
		pcre_opts |= PCRE_CASELESS;

    n->regexp = pcre_compile(regexString, pcre_opts, &errormsg,
        &erroroffset, NULL);
	n->pe = NULL;
    if (n->regexp)
	{
        errormsg = NULL;
        n->pe = pcre_study(n->regexp, 0, &errormsg);		
	}

	UCS2UTF8_Dealloc(regexString);

    return n;
}

void RxFree(RxNode *n) {
	if (n)
	{
		pcre_free(n->regexp);
		pcre_free(n->pe);
		delete n;
	}
}

int RxExec(RxNode *Regexp, LPCTSTR Data, int Len, LPCTSTR Start, RxMatchRes *Match) {
    if (Regexp == 0) return 0;

	int i;
    for (i = 0; i < NSEXPS; i++) Match->Open[i] = Match->Close[i] = -1;

	int ovector[NSEXPS * 2];
	char *compString = UCS2UTF8_ConvertToUtf8(Data);
	int stringLen = strlen(compString);
#ifdef UNICODE
	int startoffset = ucr::Utf8len_of_string(Data, Start - Data);
#else
	int startoffset = Start - Data;
#endif
	int result = pcre_exec(Regexp->regexp, Regexp->pe, compString, stringLen,
			startoffset, 0, ovector, NSEXPS);
	if (result >= 0)
	{
		for (i = 0; i < result; i++)
		{
#ifdef UNICODE
			Match->Open[i] = ucr::stringlen_of_utf8(compString, ovector[i * 2]);
			Match->Close[i] = ucr::stringlen_of_utf8(compString, ovector[i * 2 + 1]);
#else
			Match->Open[i] = ovector[i * 2];
			Match->Close[i] = ovector[i * 2 + 1];
#endif
		}
	}
	UCS2UTF8_Dealloc(compString);

    return (result >= 0);
}

#define FLAG_UP_CASE     1
#define FLAG_DOWN_CASE   2
#define FLAG_UP_NEXT     4
#define FLAG_DOWN_NEXT   8

static int add(int *len, LPTSTR *s, LPCTSTR a, int alen, int &flag) {
    int NewLen = *len + alen;
    int i;

    NewLen = NewLen * 2;

    if (alen == 0)
        return 0;

    if (*s) {
        *s = (LPTSTR) realloc(*s, NewLen * sizeof(TCHAR));
        assert(*s);
        memcpy(*s + *len, a, alen * sizeof(TCHAR));
    } else {
        *s = (LPTSTR) malloc(NewLen * sizeof(TCHAR));
        assert(*s);
        memcpy(*s, a, alen * sizeof(TCHAR));
        *len = 0;
    }
    if (flag & FLAG_UP_CASE) {
        LPTSTR p = *s + *len;

        for (i = 0; i < alen; i++) {
            *p = (TCHAR)_totupper(*p);
            p++;
        }
    } else if (flag & FLAG_DOWN_CASE) {
        LPTSTR p = *s + *len;

        for (i = 0; i < alen; i++) {
            *p = (TCHAR)_totlower(*p);
            p++;
        }
    }
    if (flag & FLAG_UP_NEXT) {
        LPTSTR p = *s + *len;

        *p = (TCHAR)_totupper(*p);
        flag &= ~FLAG_UP_NEXT;
    } else if (flag & FLAG_DOWN_NEXT) {
        LPTSTR p = *s + *len;

        *p = (TCHAR)_totlower(*p);
        flag &= ~FLAG_DOWN_NEXT;
    }
    *len += alen;
    return 0;
}

int RxReplace(LPCTSTR rep, LPCTSTR Src, int /*len*/, RxMatchRes match, LPTSTR *Dest, int *Dlen) {
    int dlen = 0;
    LPTSTR dest = 0;
    TCHAR Ch;
    int n;
    int flag = 0;

    *Dest = 0;
    *Dlen = 0;
    //    add(&dlen, &dest, Src, match.Open[0]);
    while (*rep) {
        switch (Ch = *rep++) {
            //        case _T('&'):
            //            add(&dlen, &dest, Src + match.Open[0], match.Close[0] - match.Open[0], flag);
            //            break;
        case _T('\\'):
            switch (Ch = *rep++) {
            case _T('0'):
            case _T('1'): case _T('2'): case _T('3'):
            case _T('4'): case _T('5'): case _T('6'):
            case _T('7'): case _T('8'): case _T('9'):
                n = Ch - 48;

                if (match.Open[n] != -1 && match.Close[n] != -1) {
                    add(&dlen, &dest, Src + match.Open[n], match.Close[n] - match.Open[n], flag);
                } else return -1;
                break;
            case 0:
                if (dest) free(dest);
                return -1; // error
            case _T('r'): Ch = _T('\r'); add(&dlen, &dest, &Ch, 1, flag); break;
            case _T('n'): Ch = _T('\n'); add(&dlen, &dest, &Ch, 1, flag); break;
            case _T('b'): Ch = _T('\b'); add(&dlen, &dest, &Ch, 1, flag); break;
            case _T('a'): Ch = _T('\a'); add(&dlen, &dest, &Ch, 1, flag); break;
            case _T('t'): Ch = _T('\t'); add(&dlen, &dest, &Ch, 1, flag); break;
            case _T('U'): flag |= FLAG_UP_CASE; break;
            case _T('u'): flag |= FLAG_UP_NEXT; break;
            case _T('L'): flag |= FLAG_DOWN_CASE; break;
            case _T('l'): flag |= FLAG_DOWN_NEXT; break;
            case _T('E'):
            case _T('e'): flag &= ~(FLAG_UP_CASE | FLAG_DOWN_CASE); break;
            case _T('x'):
                {
                    int N = 0;
                    int A = 0;

                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
                    rep++;
                    A = N << 4;
                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
                    rep++;
                    A = A + N;
                    Ch = (TCHAR)A;
                }
                add(&dlen, &dest, &Ch, 1, flag);
                break;
            case _T('d'):
                {
                    int N = 0;
                    int A = 0;

                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 9) return 0;
                    rep++;
                    A = N * 100;
                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 9) return 0;
                    rep++;
                    A = N * 10;
                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 9) return 0;
                    rep++;
                    A = A + N;
                    Ch = (TCHAR)A;
                }
                add(&dlen, &dest, &Ch, 1, flag);
                break;
            case _T('o'):
                {
                    int N = 0;
                    int A = 0;

                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 7) return 0;
                    rep++;
                    A = N * 64;
                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 7) return 0;
                    rep++;
                    A = N * 8;
                    if (*rep == 0) return 0;
                    N = _totupper(*rep) - 48; if (N > 7) return 0;
                    rep++;
                    A = A + N;
                    Ch = (TCHAR)A;
                }
                add(&dlen, &dest, &Ch, 1, flag);
                break;
            default:
                add(&dlen, &dest, &Ch, 1, flag);
                break;
            }
            break;
        default:
            add(&dlen, &dest, &Ch, 1, flag);
            break;
        }
    }
    //    add(&dlen, &dest, Src + match.Close[0], len - match.Close[0]);
    *Dlen = dlen;
    *Dest = dest;
    return 0;
}
