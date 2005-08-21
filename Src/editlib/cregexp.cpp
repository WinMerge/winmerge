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

#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cregexp.h"

//#define DEBUG

#ifdef _UNICODE
#define mytcsnextc(p) (*p)
#else
__inline unsigned int mytcsnextc(LPCSTR p) {
	int ch = 0;
	if (_ismbblead((unsigned char)*p))
		ch = ((unsigned char)*p++<<8);
	ch += (unsigned char)*p;
	return ch;
}
#endif

static int RegCount = 0;

#ifdef DEBUG
static void RxDump(int N, RxNode *n);
#endif

static  RxNode *NewNode(int aWhat) {
    RxNode *N = (RxNode *) malloc(sizeof(RxNode));

    if (N) {
        memset(N, 0, sizeof(RxNode));
        N->fWhat = (short)aWhat;
    }
    return N;
}

static RxNode *NewChar(unsigned int Ch) {
    RxNode *A = NewNode(RE_CHAR);

    if (A) {
#ifdef _UNICODE
        A->fChar = (LPTSTR) malloc(2);
        A->fLen = 1;
        A->fChar[0] = Ch;
#else
        if (Ch > 0xff) {
            A->fChar = (LPTSTR) malloc(2);
            A->fLen = 2;
            A->fChar[0] = (unsigned char)(Ch >> 8);
            A->fChar[1] = (unsigned char)(Ch & 0xff);
        } else {
            A->fChar = (LPTSTR) malloc(1);
            A->fLen = 1;
            A->fChar[0] = (unsigned char)Ch;
        }
#endif
    }
    return A;
}

static RxNode *NewEscape(LPCTSTR *const Regexp) {
    unsigned int Ch = mytcsnextc(*Regexp);
    *Regexp = _tcsinc(*Regexp);
    switch (Ch) {
      case 0: return 0;
      case _T('a'): Ch = _T('\a'); break;
      case _T('b'): Ch = _T('\b'); break;
      case _T('f'): Ch = _T('\f'); break;
      case _T('n'): Ch = _T('\n'); break;
      case _T('r'): Ch = _T('\r'); break;
      case _T('t'): Ch = _T('\t'); break;
      case _T('v'): Ch = _T('\v'); break;
      case _T('e'): Ch = 27; break;
      case _T('s'): return NewNode(RE_WSPACE);
      case _T('S'): return NewNode(RE_NWSPACE);
      case _T('U'): return NewNode(RE_UPPER);
      case _T('L'): return NewNode(RE_LOWER);
      case _T('w'): return NewNode(RE_WORD);
      case _T('W'): return NewNode(RE_NWORD);
      case _T('d'): return NewNode(RE_DIGIT);
      case _T('D'): return NewNode(RE_NDIGIT);
      case _T('C'): return NewNode(RE_CASE);
      case _T('c'): return NewNode(RE_NCASE);
      case _T('N'):
        {
            unsigned int N = 0;
            unsigned int A = 0;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 9) return 0;
            (*Regexp)++;
            A = N * 100;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 9) return 0;
            (*Regexp)++;
            A = A + N * 10;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 9) return 0;
            (*Regexp)++;
            A = A + N;
            Ch = (TCHAR) A;
        }
        break;
    case _T('o'):
        {
            unsigned int N = 0;
            unsigned int A = 0;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 7) return 0;
            (*Regexp)++;
            A = N * 64;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 7) return 0;
            (*Regexp)++;
            A = A + N * 8;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 7) return 0;
            (*Regexp)++;
            A = A + N;
            Ch = (TCHAR) A;
        }
        break;
    case _T('x'):
        {
            unsigned int N = 0;
            unsigned int A = 0;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
            (*Regexp)++;
            A = N << 4;
            if (**Regexp == 0) return 0;
            N = _totupper(**Regexp) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
            (*Regexp)++;
            A = A + N;
            Ch = (TCHAR) A;
        }
        break;
    }
    return NewChar(Ch);
}


#define NNN 8192        // 8 * 8192 = 65536 (match set)

#ifdef _UNICODE
#define SETOP(set,n) \
    do { \
      set[(TCHAR)(n) >> 3] |= (TCHAR)(1 << ((TCHAR)(n) & 7)); \
    } while (0)
#else // _UNICODE
#define SETOP(set,n) \
    do { \
      set[(unsigned int)(n) >> 3] |= (unsigned char)(1 << ((unsigned char)(n) & 7)); \
    } while (0)
#endif // _UNICODE

static RxNode *NewSet(LPCTSTR * const Regexp) {
#ifdef _UNICODE
    TCHAR set[NNN];
#else // _UNICODE
    unsigned char set[NNN];
#endif // _UNICODE
    int s = 0;
    int c = 0;
    unsigned int i, xx;
#ifdef _UNICODE
    TCHAR Ch, C1 = 0, C2 = 0;
#else // _UNICODE
    unsigned int Ch, C1 = 0, C2 = 0;
#endif // _UNICODE
    int doset = 0;

    memset(set, 0, sizeof(set));
    s = 1;
    if (**Regexp == _T('^')) {
        s = 0;
        ++*Regexp;
    }
    c = 0;

    while (**Regexp) {
        Ch = mytcsnextc(*Regexp);
        *Regexp = _tcsinc(*Regexp);
        switch (Ch) {
          case _T(']'):
            if (doset == 1) return 0;
            {
                RxNode *N = NewNode(s?RE_INSET:RE_NOTINSET);
                N->fChar = (LPTSTR) malloc(sizeof(set));
                N->fLen = sizeof(set);
                if (N->fChar == 0) return 0;
                memcpy(N->fChar, (LPTSTR) set, sizeof(set));
                return N;
            }
          case _T('\\'):
            Ch = mytcsnextc(*Regexp);
            *Regexp = _tcsinc(*Regexp);
            switch (Ch) {
              case 0: return 0;
              case _T('a'): Ch = _T('\a'); break;
              case _T('b'): Ch = _T('\b'); break;
              case _T('f'): Ch = _T('\f'); break;
              case _T('n'): Ch = _T('\n'); break;
              case _T('r'): Ch = _T('\r'); break;
              case _T('t'): Ch = _T('\t'); break;
              case _T('v'): Ch = _T('\v'); break;
              case _T('e'): Ch = 27; break;
              case _T('N'):
                  {
                      unsigned int N = 0;
                      unsigned int A = 0;
                      if (**Regexp == 0) return 0;
                      N = _totupper(**Regexp) - 48; if (N > 9) return 0;
                      (*Regexp)++;
                      A = N * 100;
                      if (**Regexp == 0) return 0;
                      N = _totupper(**Regexp) - 48; if (N > 9) return 0;
                      (*Regexp)++;
                      A = A + N * 10;
                      if (**Regexp == 0) return 0;
                      N = _totupper(**Regexp) - 48; if (N > 9) return 0;
                      (*Regexp)++;
                      A = A + N;
#ifdef _UNICODE
                      Ch = (TCHAR)A;
#else // _UNICODE
                      Ch = (unsigned char)A;
#endif // _UNICODE
                  }
                  break;
            case _T('o'):
                {
                    unsigned int N = 0;
                    unsigned int A = 0;
                    if (**Regexp == 0) return 0;
                    N = _totupper(**Regexp) - 48; if (N > 7) return 0;
                    (*Regexp)++;
                    A = N * 64;
                    if (**Regexp == 0) return 0;
                    N = _totupper(**Regexp) - 48; if (N > 7) return 0;
                    (*Regexp)++;
                    A = A + N * 8;
                    if (**Regexp == 0) return 0;
                    N = _totupper(**Regexp) - 48; if (N > 7) return 0;
                    (*Regexp)++;
                    A = A + N;
#ifdef _UNICODE
                    Ch = (TCHAR)A;
#else // _UNICODE
                    Ch = (unsigned char)A;
#endif // _UNICODE
                }
                break;
            case _T('x'):
                {
                    unsigned int N = 0;
                    unsigned int A = 0;
                    if (**Regexp == 0) return 0;
                    N = _totupper(**Regexp) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
                    (*Regexp)++;
                    A = N << 4;
                    if (**Regexp == 0) return 0;
                    N = _totupper(**Regexp) - 48; if (N > 9) N = N + 48 - 65 + 10; if (N > 15) return 0;
                    (*Regexp)++;
                    A = A + N;
#ifdef _UNICODE
                    Ch = (TCHAR)A;
#else // _UNICODE
                    Ch = (unsigned char)A;
#endif // _UNICODE
                }
                break;
            case _T('s'):
                c += 4;
                SETOP(set, _T('\n'));
                SETOP(set, _T('\t'));
                SETOP(set, _T(' '));
                SETOP(set, _T('\r'));
                continue;
            case _T('S'):
                for (xx = 0; xx <= 255; xx++) {
                    if (xx != _T(' ') && xx != _T('\t') && xx != _T('\n') && xx != _T('\r')) {
                        c++;
                        SETOP(set, xx);
                    }
                }
                continue;
            case _T('w'):
                for (xx = 0; xx <= 255; xx++) {
                    if (_istalnum(xx)) {
                        c++;
                        SETOP(set, xx);
                    }
                }
                break;
            case _T('W'):
                for (xx = 0; xx <= 255; xx++) {
                    if (!_istalnum(xx)) {
                        c++;
                        SETOP(set, xx);
                    }
                }
                break;
            case _T('d'):
                for (xx = 0; xx <= 255; xx++) {
                    if (_istdigit(xx)) {
                        c++;
                        SETOP(set, xx);
                    }
                }
                break;
            case _T('D'):
                for (xx = 0; xx <= 255; xx++) {
                    if (!_istdigit(xx)) {
                        c++;
                        SETOP(set, xx);
                    }
                }
                break;
            case _T('U'):
                for (xx = _T('A'); xx <= _T('Z'); xx++) {
                    c++;
                    SETOP(set, xx);
                }
                continue;
            case _T('L'):
                for (xx = _T('a'); xx <= _T('z'); xx++) {
                    c++;
                    SETOP(set, xx);
                }
                continue;
            }
            break;
        }
        if (doset == 0 && ((**Regexp) == _T('-'))) {
            doset = 1;
            C1 = Ch;
            ++*Regexp;
            continue;
        } else if (doset == 1) {
            C2 = Ch;
            if (C2 < C1) return 0;
            for(i = C1; i <= C2; i++) SETOP(set, i);
            doset = 0;
            continue;
        }
        c++;
        SETOP(set, Ch);
    }
    return 0;
}

static int AddNode(RxNode **F, RxNode **N, RxNode *A) {
    if (A) {
        if (*F) {
            (*N)->fNext = A;
            A->fPrev = (*N);
            *N = A;
        } else {
            (*N) = (*F) = A;
            A->fPrev = A->fNext = 0;
        }
        return 1;
    }
    return 0;
}

static int CountWidth(RxNode *N) {
    int w = 0;

    while (N) {
        if (N->fWhat < 32) w += 0;
        else if (N->fWhat >= 32 && N->fWhat < 64)
            w += 1;
        N = N->fNext;
    }
    return w;
}

static int MakeSub(RxNode **F, RxNode **N, TCHAR What) {
    //_tprintf(_T("MakeSub: %c\n", What));
    if (*N) {
        RxNode *No;
        RxNode *New;
        RxNode *Jump, *Skip;
        RxNode *Last = (*N);

        if (Last->fWhat & RE_GROUP) {
            RxNode *P = Last->fPrev;
            int C = 1;

            while ((C > 0) && P) {
                //puts(_T("backtracking...-----"));
                //RxDump(0, P);
                if (P->fWhat & RE_GROUP) {
                    if (P->fWhat & RE_CLOSE) C++;
                    else C--;
                }
                Last = P;
                if (C == 0) break;
                P = P->fPrev;
            }
            //_tprintf(_T("P = %s, c = %d", P ? "ok":"null", C));
            if (C != 0) return 0;
        }
        assert(Last);
        if (What != _T('?') && What != _T('|'))
            if (CountWidth(Last) == 0) {
                //                puts(_T("FAILED count"));
                return 0;
            }
        switch (What) {
        case _T('?'):    /* BRANCH x NOTHING */
            New = NewNode(RE_BRANCH | RE_GREEDY | What);
            No = NewNode(RE_NOTHING);
            if (!New || !No) return 0;
            No->fPrev = *N;
            if (*N)
                (*N)->fNext = No;
            New->fNext = Last;
            New->fPrev = Last->fPrev;
            Last->fPrev = New;
            if (New->fPrev) {
                New->fPrev->fNext = New;
            } else {
                *F = New;
            }
            New->fPtr = No;
            No->fPtr = New;
            *N = No;
            //puts(_T("BRANCH ?"));
            break;

        case _T('*'):
        case _T('@'):
            New = NewNode(RE_BRANCH | What | ((What == _T('*')) ? RE_GREEDY : 0));
            Jump = NewNode(RE_JUMP);
            No = NewNode(RE_NOTHING);

            if (!New || !No || !Jump) return 0;
            No->fPrev = Jump;
            Jump->fNext = No;
            Jump->fPrev = *N;
            if (*N)
                (*N)->fNext = Jump;
            New->fNext = Last;
            New->fPrev = Last->fPrev;
            Last->fPrev = New;
            if (New->fPrev) {
                New->fPrev->fNext = New;
            } else {
                *F = New;
            }
            New->fPtr = No;
            No->fPtr = New;
            Jump->fPtr = New;
            *N = No;
            //puts(_T("BRANCH *"));
            break;

        case _T('#'):
        case _T('+'):
            New = NewNode(RE_BRANCH | What | ((What == _T('+')) ? RE_GREEDY : 0));
            Skip = NewNode(RE_JUMP);
            Jump = NewNode(RE_JUMP);
            No = NewNode(RE_NOTHING);

            if (!New || !No || !Jump) return 0;
            No->fPrev = Jump;
            Jump->fPrev = *N;
            Jump->fNext = No;

            Skip->fNext = New;
            New->fPrev = Skip;
            if (*N)
                (*N)->fNext = Jump;
            New->fNext = Last;
            Skip->fPrev = Last->fPrev;
            Last->fPrev = New;
            if (Skip->fPrev) {
                Skip->fPrev->fNext = Skip;
            } else {
                *F = Skip;
            }
            New->fPtr = No;
            No->fPtr = New;
            Jump->fPtr = New;
            Skip->fPtr = Last;
            *N = No;
            //puts(_T("BRANCH +"));
            break;
        case _T('|'):
            New = NewNode(RE_BRANCH | RE_GREEDY | What);
            Jump = NewNode(RE_BREAK);
            No = NewNode(RE_NOTHING);

            if (!New || !No || !Jump) return 0;
            No->fPrev = Jump;
            Jump->fNext = No;
            Jump->fPrev = *N;
            if (*N)
                (*N)->fNext = Jump;
            New->fNext = Last;
            New->fPrev = Last->fPrev;
            Last->fPrev = New;
            if (New->fPrev) {
                New->fPrev->fNext = New;
            } else {
                *F = New;
            }
            New->fPtr = No;
            No->fPtr = New;
            Jump->fPtr = New;
            *N = No;
            //puts(_T("BRANCH |"));
            break;
        }
        return 1;
    }
    return 0;
}

#define CHECK(n) do { if ((n) == 0) { return 0;} } while (0)

static RxNode *RxComp(LPCTSTR *Regexp) {
    RxNode *F = 0;
    RxNode *N = 0;
    int C;
    TCHAR Ch;

    while (**Regexp) {
        //        puts(*Regexp);
        switch (Ch = (*(*Regexp)++)) {
        case _T('?'):
        case _T('*'):
        case _T('+'):
        case _T('@'):
        case _T('#'):
        case _T('|'):
            CHECK(MakeSub(&F, &N, Ch));
            break;
        case _T('}'):
        case _T(')'):
            return F;
        case _T('{'):
            CHECK(AddNode(&F, &N, NewNode(RE_GROUP | RE_OPEN)));
            CHECK(AddNode(&F, &N, RxComp(Regexp)));
            while (N->fNext) N = N->fNext;
            CHECK(AddNode(&F, &N, NewNode(RE_GROUP | RE_CLOSE)));
            break;
        case _T('('):
            C = ++RegCount;
            CHECK(AddNode(&F, &N, NewNode(RE_GROUP | RE_OPEN | RE_MEM | C)));
            CHECK(AddNode(&F, &N, RxComp(Regexp)));
            while (N->fNext) N = N->fNext;
            CHECK(AddNode(&F, &N, NewNode(RE_GROUP | RE_CLOSE | RE_MEM | C)));
            break;
        case _T('\\'):CHECK(AddNode(&F, &N, NewEscape(Regexp)));     break;
        case _T('['): CHECK(AddNode(&F, &N, NewSet(Regexp)));        break;
        case _T('^'): CHECK(AddNode(&F, &N, NewNode(RE_ATBOL)));     break;
        case _T('$'): CHECK(AddNode(&F, &N, NewNode(RE_ATEOL)));     break;
        case _T('.'): CHECK(AddNode(&F, &N, NewNode(RE_ANY)));       break;
        case _T('<'): CHECK(AddNode(&F, &N, NewNode(RE_ATBOW)));     break;
        case _T('>'): CHECK(AddNode(&F, &N, NewNode(RE_ATEOW)));     break;
        default:
            --*Regexp;
            CHECK(AddNode(&F, &N, NewChar(mytcsnextc(*Regexp))));
            *Regexp = _tcsinc(*Regexp);
            break;
        }
    }
    return F;
}

RxNode *RxOptimize(RxNode *rx) {
    return rx;
}

RxNode *RxCompile(LPCTSTR Regexp) {
    RxNode *n = 0, *x;
    if (Regexp == 0) return 0;
    RegCount = 0;
    n = RxComp(&Regexp);
    if (n == 0) return 0;
    n = RxOptimize(n);
    x = n;
    while (x->fNext) x = x->fNext;
    x->fNext = NewNode(RE_END);
    return n;
}

void RxFree(RxNode *n) {
    RxNode *p;

    while (n) {
        p = n;
        n = n->fNext;
        switch (p->fWhat) {
        case RE_INSET:
        case RE_NOTINSET:
        case RE_CHAR:
            free(p->fChar);
            break;
        default:
            break;
        }
        free(p);
    }
}

#define ChClass(x) (((((x) >= _T('A')) && ((x) <= _T('Z'))) || (((x) >= _T('a')) && ((x) <= _T('z'))) || (((x) >= _T('0')) && ((x) <= _T('9'))))?1:0)

static RxMatchRes *match;
static LPCTSTR bop;
static LPCTSTR eop;
static int flags = RX_CASE;
static LPCTSTR rex;

int RxMatch(RxNode *rx) {
    RxNode *n = rx;

    //_tprintf(_T(">>"));
    while (n) {
        //_tprintf(_T("%-50.50s\n", rex));
        //RxDump(1, n);
        switch (n->fWhat) {
        case RE_NOTHING:
            break;
        case RE_CASE:
            flags |= RX_CASE;
            break;
        case RE_NCASE:
            flags &= ~RX_CASE;
            break;
        case RE_ATBOL:
            if (rex != bop) return 0;
            break;
        case RE_ATEOL:
            if (rex != eop) return 0;
            break;
        case RE_ANY:
            if (rex == eop) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_WSPACE:
            if (rex == eop) return 0;
            if (*rex != _T(' ') && *rex != _T('\n') && *rex != _T('\r') && *rex != _T('\t')) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_NWSPACE:
            if (rex == eop) return 0;
            if (*rex == _T(' ') || *rex == _T('\n') || *rex == _T('\r') || *rex == _T('\t')) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_WORD:
            if (rex == eop) return 0;
            if (!_istalnum(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_NWORD:
            if (rex == eop) return 0;
            if (_istalnum(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_DIGIT:
            if (rex == eop) return 0;
            if (!_istdigit(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_NDIGIT:
            if (rex == eop) return 0;
            if (_istdigit(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_UPPER:
            if (rex == eop) return 0;
            if (!_istupper(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_LOWER:
            if (rex == eop) return 0;
            if (!_istlower(*rex)) return 0;
            rex = _tcsinc(rex);
            break;
        case RE_ATBOW:
            if (rex >= eop) return 0;
            if (rex > bop) {
                if ((ChClass(*rex) != 1) || (ChClass(*(rex-1)) != 0)) return 0;
            }
            break;
        case RE_ATEOW:
            if (rex <= bop) return 0;
            if (rex < eop) {
                if ((ChClass(*rex) != 0) || (ChClass(*(rex-1)) != 1)) return 0;
            }
            break;
        case RE_CHAR:
            if (rex == eop) return 0;
            if (flags & RX_CASE) {
                if (*n->fChar != *rex) return 0;
                if (memcmp(rex, n->fChar, n->fLen) != 0) return 0;
            } else {
                for (int i = 0; i < n->fLen; i+= _tcsinc(&rex[i]) - &rex[i])
                    if (_totupper(mytcsnextc(&rex[i])) != _totupper(mytcsnextc(&n->fChar[i])))
                        return 0;
            }
            rex += n->fLen;
            break;
        case RE_INSET:
            if (rex == eop) return 0;
#ifdef _UNICODE
            if ((n->fChar[(TCHAR)(*rex) >> 3] & (1 << ((TCHAR)(*rex) & 7))) == 0) return 0;
#else // _UNICODE
            {
            unsigned int ch = mytcsnextc(rex);
            if ((n->fChar[ch >> 3] & (1 << ((unsigned char)(ch) & 7))) == 0) return 0;
            }
#endif // _UNICODE
            rex = _tcsinc(rex);
            break;
        case RE_NOTINSET:
            if (rex == eop) return 0;
#ifdef _UNICODE
            if (n->fChar[(TCHAR)(*rex) >> 3] & (1 << ((TCHAR)(*rex) & 7))) return 0;
#else // _UNICODE
            {
            unsigned int ch = mytcsnextc(rex);
            if (n->fChar[ch >> 3] & (1 << ((unsigned char)(ch) & 7))) return 0;
            }
#endif // _UNICODE
            rex = _tcsinc(rex);
            break;
        case RE_JUMP:
            n = n->fPtr;
            continue;
        case RE_END:
            return 1;
        case RE_BREAK:
            n = n->fNext;
            if (n->fNext == 0) break;
            n = n->fNext;
            if (n->fWhat & RE_BRANCH) {
                while ((n->fWhat & RE_BRANCH) && n->fPtr && ((n->fWhat & 0xFF) == _T('|')))
                    n = n->fPtr->fNext;
            }
            if (n->fWhat & RE_GROUP) {
                int C = 1;
                n = n->fNext;
                while ((C > 0) && n) {
                    if (n->fWhat & RE_GROUP) {
                        if (n->fWhat & RE_OPEN) C++;
                        else C--;
                    }
                    if (C == 0) break;
                    n = n->fNext;
                }
            }
            break;
        default:
            if (n->fWhat & RE_GROUP) {
                if (n->fWhat & RE_MEM) {
                    LPCTSTR save = rex;
                    int b = n->fWhat & 0xFF;
                    int fl = flags;

                    if (RxMatch(n->fNext) == 0) {
                        flags = fl;
                        if (n->fWhat & RE_OPEN)
                            match->Open[b] = -1;
                        else
                            match->Close[b] = -1;
                        return 0;
                    }

                    if (n->fWhat & RE_OPEN) {
                        //                        if (match->Open[b] == -1)
                        match->Open[b] = (int) (save - bop);
                    } else {
                        //                        if (match->Close[b] == -1)
                        match->Close[b] = (int) (save - bop);
                    }
                    return 1;
                }
            } else if (n->fWhat & RE_BRANCH) {
                LPCTSTR save = rex;
                int fl = flags;

                if ((n->fWhat & RE_GREEDY) == 0) {
                    if (RxMatch(n->fPtr) == 1) return 1;
                    flags = fl;
                    rex = save;
                } else {
                    if (RxMatch(n->fNext) == 1) return 1;
                    flags = fl;
                    rex = save;
                    n = n->fPtr;
                    continue;
                }
            }
            break;
        }
        n = n->fNext;
    }
    /* NOTREACHED */
    assert(1 == 0 /* internal regexp error */);
    return 0;
}

int RxTry(RxNode *rx, LPCTSTR s) {
    int fl = flags;
    rex = s;
    for (int i = 0; i < NSEXPS; i++)
        match->Open[i] = match->Close[i] = -1;
    if (RxMatch(rx)) {
        match->Open[0] = (int) (s - bop);
        match->Close[0] = (int) (rex - bop);
        return 1;
    }
    flags = fl;
    return 0;
}

int RxExec(RxNode *Regexp, LPCTSTR Data, int Len, LPCTSTR Start, RxMatchRes *Match, unsigned int RxOpt) {
    TCHAR Ch;
    if (Regexp == 0) return 0;

    match = Match;
    bop = Data;
    eop = Data + Len;

    flags = RxOpt;

    for (int i = 0; i < NSEXPS; i++) Match->Open[i] = Match->Close[i] = -1;

    switch (Regexp->fWhat) { // this should be more clever
    case RE_ATBOL:     // match is anchored
        return RxTry(Regexp, Start);
    case RE_CHAR:    // search for a character to match
        Ch = Regexp->fChar[0];
        if (Start == eop)
            break;
        if (flags & RX_CASE) {
            while (1) {
                while (Start < eop && *Start != Ch)
                    Start = _tcsinc(Start);
                if (Start == eop)
                    break;
                if (RxTry(Regexp, Start))
                    return 1;
                if ((Start = _tcsinc(Start)) == eop)
                    break;
            }
        } else {
            Ch = (TCHAR)_totupper(Ch);
            while (1) {
                while (Start < eop && (TCHAR) _totupper(*Start) != Ch) {
                    Start = _tcsinc(Start);
                }
                if (Start == eop)
                    break;
                if (RxTry(Regexp, Start))
                    return 1;
                if ((Start = _tcsinc(Start)) == eop)
                    break;
            }
        }
        break;
    default:         // (slow)
        do {
            if (RxTry(Regexp, Start)) return 1;
        } while ((Start = _tcsinc(Start)) < eop);
        break;
    }
    return 0;
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

#if 0

static void RxDump(int N, RxNode *n) {
    while (n) {
        for (int i = 0; i < N; i++) _tprintf(_T("    "));
        switch (n->fWhat) {
        case RE_NOTHING:   _tprintf(_T("NOTHING\n")); break;
        case RE_CHAR:      _tprintf(_T("CHAR '%.1s'\n"), n->fChar); break;
        case RE_ATBOL:     _tprintf(_T("^\n")); break;
        case RE_ATEOL:     _tprintf(_T("$\n")); break;
        case RE_ANY:       _tprintf(_T(".\n")); break;
        case RE_INSET:     _tprintf(_T("[\n")/*, n->fChar*/); break;
        case RE_NOTINSET:  _tprintf(_T("[^\n")/*, n->fChar*/); break;
        case RE_ATBOW:     _tprintf(_T("<\n")); break;
        case RE_ATEOW:     _tprintf(_T(">\n")); break;
        case RE_WSPACE:    _tprintf(_T("WSPACE\n")); break;
        case RE_NWSPACE:   _tprintf(_T("NWSPACE\n")); break;
        case RE_UPPER:     _tprintf(_T("UPPER\n")); break;
        case RE_LOWER:     _tprintf(_T("LOWER\n")); break;
        case RE_JUMP:      _tprintf(_T("JUMP\n")); break;
        case RE_BREAK:     _tprintf(_T("BREAK\n")); break;
        case RE_END:       _tprintf(_T("END\n")); break;
        default:
            if (n->fWhat & RE_GROUP) {
                if (n->fWhat & RE_MEM) {
                    if (n->fWhat & RE_OPEN)  _tprintf(_T("(  %d\n"), n->fWhat & 0xFF);
                    if (n->fWhat & RE_CLOSE) _tprintf(_T(")  %d\n"), n->fWhat & 0xFF);
                } else {
                    if (n->fWhat & RE_OPEN)  _tprintf(_T("{\n"));
                    if (n->fWhat & RE_CLOSE) _tprintf(_T("}\n"));
                }
            } else if (n->fWhat & RE_BRANCH) {
                if (n->fWhat & RE_GREEDY) {
                    _tprintf(_T("%c\n"), n->fWhat & 0xFF);
                } else {
                    _tprintf(_T("%c\n"), n->fWhat & 0xFF);
                }
            } else {
                _tprintf(_T("???????????????\n"));
            }
            break;
        }
        n = n->fNext;
    }
}

#define TEST(rc,rx,st) \
    _tcscpy(line,st); \
    assert((a = RxCompile(rx)) != 0); \
    puts(_T("\n--- " rx " -- " st " -- ")); \
    RxDump(0,a);\
    assert(rc == RxExec(a, line, _tcslen(line), line, &b)); \
    RxFree(a);

int main() {
    RxNode *a;
    RxMatchRes b;
    TCHAR line[1024];

    TEST(1, _T("a"), _T("a"));
    TEST(0, _T("b"), _T("a"));
    TEST(1, _T("aaaa"), _T("aaaa"));
    TEST(0, _T("bbbb"), _T("aaaa"));
    TEST(1, _T("."), _T("a"));
    TEST(0, _T("."), _T(""));
    TEST(1, _T("a.."), _T("axx"));
    TEST(0, _T("a.."), _T("b.."));
    TEST(1, _T("a?b"), _T("ab"));
    TEST(1, _T("a?b"), _T("xb"));
    TEST(0, _T("a?C"), _T("xb"));
    TEST(1, _T("{aa}?b"), _T("aab"));
    TEST(1, _T("{aa}?b"), _T("xab"));
    TEST(0, _T("{aa}?C"), _T("xxb"));
    TEST(1, _T("^aa"), _T("aa"));
    TEST(0, _T("^aa"), _T("baa"));
    TEST(1, _T("^aa$"),_T("aa"));
    TEST(0, _T("^aa$"), _T("baab"));
    TEST(1, _T("a*b"), _T("aaab"));
    TEST(0, _T("a*b"), _T("aaaa"));
    TEST(1, _T("{aa}*b"), _T("aaab"));
    TEST(0, _T("{aa}*b"), _T("aaaa"));
    TEST(1, _T("b+"), _T("bb"));
    TEST(1, _T("b+"), _T("b"));
    TEST(0, _T("b+"), _T("a"));
    TEST(1, _T("^b+$"), _T("b"));
    TEST(0, _T("^b+$"), _T("aba"));
    TEST(1, _T("a|b"), _T(" a"));
    TEST(1, _T("a|b"), _T(" b"));
    TEST(0, _T("a|b"), _T(" c"));
    TEST(1, _T("a|b|c|d|e"), _T(" a "));
    TEST(1, _T("a|b|c|d|e"), _T(" c "));
    TEST(1, _T("a|b|c|d|e"), _T(" e "));
    TEST(0, _T("a|b|c|d|e"), _T(" x "));
    TEST(1, _T("{a}|{b}|{c}|{d}|{e}"), _T(" a "));
    TEST(1, _T("{a}|{b}|{c}|{d}|{e}"), _T(" c "));
    TEST(1, _T("{a}|{b}|{c}|{d}|{e}"), _T(" e "));
    TEST(0, _T("{a}|{b}|{c}|{d}|{e}"), _T(" x "));
    TEST(1, _T("^xx{alpha}|{beta}xx$"), _T("xxalphaxx"));
    TEST(1, _T("^xx{alpha}|{beta}xx$"), _T("xxbetaxx"));
    TEST(1, _T("[a-z]"), _T("aaa"));
    TEST(1, _T("^{Error}|{Warning}"), _T("Warning search.cpp 35: Conversion may lose significant digits in function AskReplace()"));
    TEST(1, _T("^{Error}|{Warning} (.+)"), _T("Warning search.cpp 35: Conversion may lose significant digits in function AskReplace()"));
    TEST(1, _T("^{Error}|{Warning} ([a-z.]#) ([0-9]#)"), _T("Warning search.cpp 35: Conversion may lose significant digits in function AskReplace()"));
    TEST(1, _T("^{Error}|{Warning} (.+) ([0-9]+): (.*)$"), _T("Warning search.cpp 35: Conversion may lose significant digits in function AskReplace()"));
    TEST(1, _T("^{Error}|{Warning} (.+) ([0-9]+): (.*)$"), _T("Error search.cpp 35: Conversion may lose significant digits in function AskReplace()"));
    TEST(1, _T("^([a-z]+ +)*\\("), _T("blabla bla bla bla ("));
    TEST(1, _T("^([a-z]+\\s+)+\\("), _T("blabla bla bla bla ("));
    TEST(1, _T("^([a-z]+\\s*)+\\("), _T("blabla bla bla bla("));
    TEST(1, _T("^([a-z]+\\s+)+\\("), _T("blabla bla   bla bla ("));
    TEST(1, _T("^([a-z]+\\s*)+\\("), _T("blabla   bla bla bla("));
    TEST(1, _T("^([a-z]# #)*\\("), _T("blabla bla bla bla ("));
    TEST(1, _T("^([a-z]+ @)@\\("), _T("blabla bla bla bla ("));
    TEST(1, _T("^[\\x20-\\xFF]+$"), _T("blabla"));
    TEST(1, _T("{a{a{a{a|a}|{a|a}a}a}a|a}"), _T("aaaaaaaaaaaaaaaaa"));

    while (1) {
        _tprintf(_T ("Regexp: ")); fflush(stdout); gets(line);
        if (!*line) break;
        a = RxCompile(line); RxDump(0, a);
        _tprintf(_T ("String: ")); fflush(stdout); gets(line);
        _tprintf(_T ("rc = %d\n"), RxExec(a, line, _tcslen(line), line, &b));
        for (int i = 0; i < NSEXPS; i++) {
            if (b.Open[i] != -1) {
                _tprintf(_T ("%d: %d %d\n"), i, b.Open[i], b.Close[i]);
            }
        }
        RxFree(a);
    }
    return 0;
}

#endif
