///////////////////////////////////////////////////////////////////////////
//  File:    regexp.h
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

#ifndef __REGEX_H
#define __REGEX_H

/*
 * Operator:
 *
 * ^            Match the beginning of line
 * $            Match the end of line
 * .            Match any character
 * [ ]          Match characters in set
 * [^ ]         Match characters not in set
 * ?            Match previous pattern 0 or 1 times (greedy)
 * |            Match previous or next pattern
 * @            Match previous pattern 0 or more times (non-greedy)
 * #            Match previous pattern 1 or more times (non-greedy)
 * *            Match previous pattern 0 or more times (greedy)
 * +            Match previous pattern 1 or more times (greedy)
 * { }          Group characters to form one pattern
 * ( )          Group and remember
 * \            Quote next character (only of not a-z)
 * <            Match beginning of a word
 * >            Match end of a word
 * \x##         Match character with ASCII code ## (hex)
 * \N###        Match ascii code ### (dec)
 * \o###        Match ascii code
 * \a           Match \a              \r           Match 0x13 (cr)
 * \b           Match \b              \t           Match 0x09 (tab)
 * \f           Match \f              \v           Match \v
 * \n           Match 0x10 (lf)       \e           Match escape (^E)
 * \s           Match whitespace (cr/lf/tab/space)
 * \S           Match nonwhitespace (!\S)
 * \w           Match word character
 * \W           Match non-word character
 * \d           Match digit character
 * \D           Match non-digit character
 * \U           Match uppercase
 * \L           Match lowercase
 * \C           Match case sensitively from here on
 * \c           Match case ingnore from here on
 */

#define RE_NOTHING         0  // nothing
#define RE_JUMP            1  // jump to
#define RE_BREAK           2  // break |
#define RE_ATBOL           3  // match at beginning of line
#define RE_ATEOL           4  // match at end of line
#define RE_ATBOW           5  // match beginning of word
#define RE_ATEOW           6  // match end of word
#define RE_CASE            7  // match case sensitively from here
#define RE_NCASE           8  // ignore case from here.
#define RE_END            31  // end of regexp

#define RE_ANY      (32 +  1) // match any character
#define RE_INSET    (32 +  2) // match if in set
#define RE_NOTINSET (32 +  3) // match if not in set
#define RE_CHAR     (32 +  4) // match character string
#define RE_WSPACE   (32 +  5) // match whitespace
#define RE_NWSPACE  (32 +  6) // match whitespace
#define RE_UPPER    (32 +  7) // match uppercase
#define RE_LOWER    (32 +  8) // match lowercase
#define RE_DIGIT    (32 +  9) // match digit
#define RE_NDIGIT   (32 + 10) // match non-digit
#define RE_WORD     (32 + 11) // match word
#define RE_NWORD    (32 + 12) // match non-word

#define RE_GROUP         256  // grouping
#define RE_OPEN          512  // open (
#define RE_CLOSE        1024  // close )
#define RE_MEM          2048  // store () match

#define RE_BRANCH       4096
#define RE_GREEDY       2048  // do a greedy match (as much as possible)

#define NSEXPS            64  // for replace only 0-9

#define RX_CASE         1  // matchcase

typedef struct _RxNode RxNode;

struct _RxNode {
    short fWhat;
    short fLen;
    RxNode *fPrev;
    RxNode *fNext;
    union {
        LPTSTR fChar;
        RxNode *fPtr;
    };
};

typedef struct {
    int Open[NSEXPS];    // -1 = not matched
    int Close[NSEXPS];
} RxMatchRes;

RxNode EDITPADC_CLASS *RxCompile(LPCTSTR Regexp);
int EDITPADC_CLASS RxExec(RxNode *Regexp, LPCTSTR Data, int Len, LPCTSTR Start, RxMatchRes *Match, unsigned int RxOpt = RX_CASE);
int EDITPADC_CLASS RxReplace(LPCTSTR rep, LPCTSTR Src, int len, RxMatchRes match, LPTSTR *Dest, int *Dlen);
void EDITPADC_CLASS RxFree(RxNode *Node);

#endif
