////////////////////////////////////////////////////////////////////////////////
// RegExp.cpp
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegExp.h"


// definition	number	opnd?	meaning 
#define	END		0		// no	End of program. 
#define	BOL		1		// no	Match beginning of line. 
#define	EOL		2		// no	Match end of line. 
#define	ANY		3		// no	Match any character. 
#define	ANYOF	4		// str	Match any of these. 
#define	ANYBUT	5		// str	Match any but one of these. 
#define	BRANCH	6		// node	Match this, or the next..\&. 
#define	BACK	7		// no	"next" ptr points backward. 
#define	EXACTLY	8		// str	Match this string. 
#define	NOTHING	9		// no	Match empty string. 
#define	STAR	10		// node	Match this 0 or more times. 
#define	PLUS	11		// node	Match this 1 or more times. 
#define	OPEN	20		// no	Sub-RE starts here. 
						//		OPEN+1 is number 1, etc. 
#define	CLOSE	30		// no	Analogous to OPEN. 

// Utility definitions.
 
#define	FAIL(m)		{ regerror(m); return(NULL); }
#define	ISREPN(c)	((c) == _T('*') || (c) == _T('+') || (c) == _T('?'))
#define	META		_T("^$.[()|?+*\\")

// Flags to be passed up and down.
 
#define	HASWIDTH	01	// Known never to match null string. 
#define	SIMPLE		02	// Simple enough to be STAR/PLUS operand. 
#define	SPSTART		04	// Starts with * or +. 
#define	WORST		0	// Worst case. 


CRegExp::CRegExp()
{
	bCompiled = FALSE;
	program = NULL;
	sFoundText = NULL;

	for( int i = 0; i < NSUBEXP; i++ )
	{
		startp[i] = NULL;
		endp[i] = NULL;
	}
}

CRegExp::~CRegExp()
{
	delete program;
	delete sFoundText;
}


CRegExp* CRegExp::RegComp(const TCHAR *exp)
{
	TCHAR *scan;
	int flags;

	if (exp == NULL)
		return NULL;
	
	bCompiled = TRUE;

	// First pass: determine size, legality. 
	bEmitCode = FALSE;
	regparse = (TCHAR *)exp;
	regnpar = 1;
	regsize = 0L;
	regdummy[0] = NOTHING;
	regdummy[1] = regdummy[2] = 0;
	regcode = regdummy;
	if (reg(0, &flags) == NULL)
		return(NULL);

	// Allocate space. 
	delete program;
	program = new TCHAR[regsize];
	memset( program, 0, regsize * sizeof(TCHAR) );

	if (program == NULL)
		return NULL;

	// Second pass: emit code. 
	bEmitCode = TRUE;
	regparse = (TCHAR *)exp;
	regnpar = 1;
	regcode = program;
	if (reg(0, &flags) == NULL)
		return NULL;

	// Dig out information for optimizations. 
	regstart = _T('\0');		// Worst-case defaults. 
	reganch = 0;
	regmust = NULL;
	regmlen = 0;
	scan = program;		// First BRANCH. 
	if (OP(regnext(scan)) == END) 
	{	
		// Only one top-level choice. 
		scan = OPERAND(scan);

		// Starting-point info. 
		if (OP(scan) == EXACTLY)
			regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			reganch = 1;

		// If there's something expensive in the r.e., find the
		// longest literal string that must appear and make it the
		// regmust.  Resolve ties in favor of later strings, since
		// the regstart check works with the beginning of the r.e.
		// and avoiding duplication strengthens checking.  Not a
		// strong reason, but sufficient in the absence of others.
		 
		if (flags&SPSTART) 
		{
			LPTSTR longest = NULL;
			size_t len = 0;

			for (; scan != NULL; scan = regnext(scan))
				if (OP(scan) == EXACTLY && _tcslen(OPERAND(scan)) >= len) 
				{
					longest = OPERAND(scan);
					len = _tcslen(OPERAND(scan));
				}
			regmust = longest;
			regmlen = (int)len;
		}
	}

	return this;
}

// reg - regular expression, i.e. main body or parenthesized thing
//
// Caller must absorb opening parenthesis.
//
// Combining parenthesis handling with the base level of regular expression
// is a trifle forced, but the need to tie the tails of the branches to what
// follows makes it hard to avoid.
 


TCHAR *CRegExp::reg(int paren, int *flagp)
{
	TCHAR *ret;
	TCHAR *br;
	TCHAR *ender;
	int parno;
	int flags;

	*flagp = HASWIDTH;	// Tentatively. 

	if (paren) 
	{
		// Make an OPEN node. 
		if (regnpar >= NSUBEXP)
		{
			TRACE1("Too many (). NSUBEXP is set to %d\n", NSUBEXP );
			return NULL;
		}
		parno = regnpar;
		regnpar++;
		ret = regnode((TCHAR)(OPEN+parno));
	}

	// Pick up the branches, linking them together. 
	br = regbranch(&flags);
	if (br == NULL)
		return(NULL);
	if (paren)
		regtail(ret, br);	// OPEN -> first. 
	else
		ret = br;
	*flagp &= ~(~flags&HASWIDTH);	// Clear bit if bit 0. 
	*flagp |= flags&SPSTART;
	while (*regparse == _T('|')) {
		regparse++;
		br = regbranch(&flags);
		if (br == NULL)
			return(NULL);
		regtail(ret, br);	// BRANCH -> BRANCH. 
		*flagp &= ~(~flags&HASWIDTH);
		*flagp |= flags&SPSTART;
	}

	// Make a closing node, and hook it on the end. 
	ender = regnode((TCHAR)((paren) ? CLOSE+parno : END));
	regtail(ret, ender);

	// Hook the tails of the branches to the closing node. 
	for (br = ret; br != NULL; br = regnext(br))
		regoptail(br, ender);

	// Check for proper termination. 
	if (paren && *regparse++ != _T(')')) 
	{
		TRACE0("unterminated ()\n");
		return NULL;
	} 
	else if (!paren && *regparse != _T('\0')) 
	{
		if (*regparse == _T(')')) 
		{
			TRACE0("unmatched ()\n");
			return NULL;
		} 
		else
		{
			TRACE0("internal error: junk on end\n");
			return NULL;
		}
		// NOTREACHED 
	}

	return(ret);
}




//
// regbranch - one alternative of an | operator
//
// Implements the concatenation operator.
 
TCHAR *CRegExp::regbranch(int *flagp)
{
	TCHAR *ret;
	TCHAR *chain;
	TCHAR *latest;
	int flags;
	int c;

	*flagp = WORST;				// Tentatively. 

	ret = regnode(BRANCH);
	chain = NULL;
	while ((c = *regparse) != _T('\0') && c != _T('|') && c != _T(')')) {
		latest = regpiece(&flags);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
		if (chain == NULL)		// First piece. 
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest);
		chain = latest;
	}
	if (chain == NULL)			// Loop ran zero times. 
		(void) regnode(NOTHING);

	return(ret);
}

//
// regpiece - something followed by possible [*+?]
//
// Note that the branching code sequences used for ? and the general cases
// of * and + are somewhat optimized:  they use the same NOTHING node as
// both the endmarker for their branch list and the body of the last branch.
// It might seem that this node could be dispensed with entirely, but the
// endmarker role is not redundant.
 
TCHAR *CRegExp::regpiece(int *flagp)
{
	TCHAR *ret;
	TCHAR op;
	TCHAR *next;
	int flags;

	ret = regatom(&flags);
	if (ret == NULL)
		return(NULL);

	op = *regparse;
	if (!ISREPN(op)) {
		*flagp = flags;
		return(ret);
	}

	if (!(flags&HASWIDTH) && op != _T('?'))
	{
		TRACE0("*+ operand could be empty\n");
		return NULL;
	}

	switch (op) {
	case _T('*'):	*flagp = WORST|SPSTART;			break;
	case _T('+'):	*flagp = WORST|SPSTART|HASWIDTH;	break;
	case _T('?'):	*flagp = WORST;				break;
	}

	if (op == _T('*') && (flags&SIMPLE))
		reginsert(STAR, ret);
	else if (op == _T('*')) {
		// Emit x* as (x&|), where & means "self". 
		reginsert(BRANCH, ret);		// Either x 
		regoptail(ret, regnode(BACK));	// and loop 
		regoptail(ret, ret);		// back 
		regtail(ret, regnode(BRANCH));	// or 
		regtail(ret, regnode(NOTHING));	// null. 
	} else if (op == _T('+') && (flags&SIMPLE))
		reginsert(PLUS, ret);
	else if (op == _T('+')) {
		// Emit x+ as x(&|), where & means "self". 
		next = regnode(BRANCH);		// Either 
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back 
		regtail(next, regnode(BRANCH));	// or 
		regtail(ret, regnode(NOTHING));	// null. 
	} else if (op == _T('?')) {
		// Emit x? as (x|) 
		reginsert(BRANCH, ret);		// Either x 
		regtail(ret, regnode(BRANCH));	// or 
		next = regnode(NOTHING);		// null. 
		regtail(ret, next);
		regoptail(ret, next);
	}
	regparse++;
	if (ISREPN(*regparse))
	{
		TRACE0("nested *?+\n");
		return NULL;
	}

	return(ret);
}

//
// regatom - the lowest level
//
// Optimization:  gobbles an entire sequence of ordinary characters so that
// it can turn them into a single node, which is smaller to store and
// faster to run.  Backslashed characters are exceptions, each becoming a
// separate node; the code is simpler that way and it's not worth fixing.
 
TCHAR *CRegExp::regatom(int *flagp)
{
	TCHAR *ret;
	int flags;

	*flagp = WORST;		// Tentatively. 

	switch (*regparse++) {
	case _T('^'):
		ret = regnode(BOL);
		break;
	case _T('$'):
		ret = regnode(EOL);
		break;
	case _T('.'):
		ret = regnode(ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case _T('['): {
		int range;
		int rangeend;
		int c;

		if (*regparse == _T('^')) {	// Complement of range. 
			ret = regnode(ANYBUT);
			regparse++;
		} else
			ret = regnode(ANYOF);
		if ((c = *regparse) == _T(']') || c == _T('-')) {
			regc((TCHAR)c);
			regparse++;
		}
		while ((c = *regparse++) != _T('\0') && c != _T(']')) {
			if (c != _T('-'))
				regc((TCHAR)c);
			else if ((c = *regparse) == _T(']') || c == _T('\0'))
				regc(_T('-'));
			else 
			{
				range = (unsigned) (TCHAR)*(regparse-2);
				rangeend = (unsigned) (TCHAR)c;
				if (range > rangeend)
				{
					TRACE0("invalid [] range\n");
					return NULL;
				}
				for (range++; range <= rangeend; range++)
					regc((TCHAR)range);
				regparse++;
			}
		}
		regc(_T('\0'));
		if (c != _T(']'))
		{
			TRACE0("unmatched []\n");
			return NULL;
		}
		*flagp |= HASWIDTH|SIMPLE;
		break;
		}
	case _T('('):
		ret = reg(1, &flags);
		if (ret == NULL)
			return(NULL);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case _T('\0'):
	case _T('|'):
	case _T(')'):
		// supposed to be caught earlier 
		TRACE0("internal error: \\0|) unexpected\n");
		return NULL;
		break;
	case _T('?'):
	case _T('+'):
	case _T('*'):
		TRACE0("?+* follows nothing\n");
		return NULL;
		break;
	case _T('\\'):
		if (*regparse == _T('\0'))
		{
			TRACE0("trailing \\\n");
			return NULL;
		}
		ret = regnode(EXACTLY);
		regc(*regparse++);
		regc(_T('\0'));
		*flagp |= HASWIDTH|SIMPLE;
		break;
	default: {
		size_t len;
		TCHAR ender;

		regparse--;
		len = _tcscspn(regparse, META);
		if (len == 0)
		{
			TRACE0("internal error: strcspn 0\n");
			return NULL;
		}
		ender = *(regparse+len);
		if (len > 1 && ISREPN(ender))
			len--;		// Back off clear of ?+* operand. 
		*flagp |= HASWIDTH;
		if (len == 1)
			*flagp |= SIMPLE;
		ret = regnode(EXACTLY);
		for (; len > 0; len--)
			regc(*regparse++);
		regc(_T('\0'));
		break;
		}
	}

	return(ret);
}



// reginsert - insert an operator in front of already-emitted operand
//
// Means relocating the operand.
 
void CRegExp::reginsert(TCHAR op, TCHAR *opnd)
{
	TCHAR *place;

	if (!bEmitCode) {
		regsize += 3;
		return;
	}

	(void) memmove(opnd+3, opnd, (size_t)((regcode - opnd)*sizeof(TCHAR)));
	regcode += 3;

	place = opnd;		// Op node, where operand used to be. 
	*place++ = op;
	*place++ = _T('\0');
	*place++ = _T('\0');
}

//
// regtail - set the next-pointer at the end of a node chain
 
void CRegExp::regtail(TCHAR *p, TCHAR *val)
{
	TCHAR *scan;
	TCHAR *temp;
//	int offset;

	if (!bEmitCode)
		return;

	// Find last node. 
	for (scan = p; (temp = regnext(scan)) != NULL; scan = temp)
		continue;

	*((short *)(scan+1)) = (short)((OP(scan) == BACK) ? scan - val : val - scan);
}


// regoptail - regtail on operand of first argument; nop if operandless
 
void CRegExp::regoptail(TCHAR *p, TCHAR *val)
{
	// "Operandless" and "op != BRANCH" are synonymous in practice. 
	if (!bEmitCode || OP(p) != BRANCH)
		return;
	regtail(OPERAND(p), val);
}


// RegFind	- match a regexp against a string
// Returns	- Returns position of regexp or -1
//			  if regular expression not found
// Note		- The regular expression should have been
//			  previously compiled using RegComp
int CRegExp::RegFind(const TCHAR *str)
{
	TCHAR *string = (TCHAR *)str;	// avert const poisoning 
	TCHAR *s;

	// Delete any previously stored found string
	delete sFoundText;
	sFoundText = NULL;

	// Be paranoid. 
	if(string == NULL) 
	{
		TRACE0("NULL argument to regexec\n");
		return(-1);
	}

	// Check validity of regex
	if (!bCompiled) 
	{
		TRACE0("No regular expression provided yet.\n");
		return(-1);
	}

	// If there is a "must appear" string, look for it. 
	if (regmust != NULL && _tcsstr(string, regmust) == NULL)
		return(-1);

	// Mark beginning of line for ^
	regbol = string;

	// Simplest case:  anchored match need be tried only once. 
	if (reganch)
	{
		if( regtry(string) )
		{
			// Save the found substring in case we need it
			sFoundText = new TCHAR[GetFindLen()+1];
			sFoundText[GetFindLen()] = _T('\0');
			_tcsncpy(sFoundText, string, GetFindLen() );

			return 0;
		}
		//String not found
		return -1;
	}

	// Messy cases:  unanchored match. 
	if (regstart != _T('\0')) 
	{
		// We know what TCHAR it must start with. 
		for (s = string; s != NULL; s = _tcschr(s+1, regstart))
			if (regtry(s))
			{
				int nPos = s-str;

				// Save the found substring in case we need it later
				sFoundText = new TCHAR[GetFindLen()+1];
				sFoundText[GetFindLen()] = _T('\0');
				_tcsncpy(sFoundText, s, GetFindLen() );

				return nPos;
			}
		return -1;
	} 
	else 
	{
		// We don't -- general case
		for (s = string; !regtry(s); s++)
			if (*s == _T('\0'))
				return(-1);

		int nPos = s-str;

		// Save the found substring in case we need it later
		sFoundText = new TCHAR[GetFindLen()+1];
		sFoundText[GetFindLen()] = _T('\0');
		_tcsncpy(sFoundText, s, GetFindLen() );

		return nPos;
	}
	// NOTREACHED 
}


// regtry - try match at specific point
 
int	CRegExp::regtry(TCHAR *string)
{
	int i;
	TCHAR **stp;
	TCHAR **enp;

	reginput = string;

	stp = startp;
	enp = endp;
	for (i = NSUBEXP; i > 0; i--) 
	{
		*stp++ = NULL;
		*enp++ = NULL;
	}
	if (regmatch(program)) 
	{
		startp[0] = string;
		endp[0] = reginput;
		return(1);
	} 
	else
		return(0);
}

// regmatch - main matching routine
//
// Conceptually the strategy is simple:  check to see whether the current
// node matches, call self recursively to see whether the rest matches,
// and then act accordingly.  In practice we make some effort to avoid
// recursion, in particular by going through "ordinary" nodes (that don't
// need to know whether the rest of the match failed) by a loop instead of
// by recursion.
 
int	CRegExp::regmatch(TCHAR *prog)
{
	TCHAR *scan;	// Current node. 
	TCHAR *next;		// Next node. 

	for (scan = prog; scan != NULL; scan = next) {
		next = regnext(scan);

		switch (OP(scan)) {
		case BOL:
			if (reginput != regbol)
				return(0);
			break;
		case EOL:
			if (*reginput != _T('\0'))
				return(0);
			break;
		case ANY:
			if (*reginput == _T('\0'))
				return(0);
			reginput++;
			break;
		case EXACTLY: {
			size_t len;
			TCHAR *const opnd = OPERAND(scan);

			// Inline the first character, for speed. 
			if (*opnd != *reginput)
				return(0);
			len = _tcslen(opnd);
			if (len > 1 && _tcsncmp(opnd, reginput, len) != 0)
				return(0);
			reginput += len;
			break;
			}
		case ANYOF:
			if (*reginput == _T('\0') ||
					_tcschr(OPERAND(scan), *reginput) == NULL)
				return(0);
			reginput++;
			break;
		case ANYBUT:
			if (*reginput == _T('\0') ||
					_tcschr(OPERAND(scan), *reginput) != NULL)
				return(0);
			reginput++;
			break;
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1: case OPEN+2: case OPEN+3:
		case OPEN+4: case OPEN+5: case OPEN+6:
		case OPEN+7: case OPEN+8: case OPEN+9: {
			const int no = OP(scan) - OPEN;
			TCHAR *const input = reginput;

			if (regmatch(next)) {
				// Don't set startp if some later
				// invocation of the same parentheses
				// already has.
				 
				if (startp[no] == NULL)
					startp[no] = input;
				return(1);
			} else
				return(0);
			break;
			}
		case CLOSE+1: case CLOSE+2: case CLOSE+3:
		case CLOSE+4: case CLOSE+5: case CLOSE+6:
		case CLOSE+7: case CLOSE+8: case CLOSE+9: {
			const int no = OP(scan) - CLOSE;
			TCHAR *const input = reginput;

			if (regmatch(next)) {
				// Don't set endp if some later
				// invocation of the same parentheses
				// already has.
				 
				if (endp[no] == NULL)
					endp[no] = input;
				return(1);
			} else
				return(0);
			break;
			}
		case BRANCH: {
			TCHAR *const save = reginput;

			if (OP(next) != BRANCH)		// No choice. 
				next = OPERAND(scan);	// Avoid recursion. 
			else {
				while (OP(scan) == BRANCH) {
					if (regmatch(OPERAND(scan)))
						return(1);
					reginput = save;
					scan = regnext(scan);
				}
				return(0);
				// NOTREACHED 
			}
			break;
			}
		case STAR: 
		case PLUS: {
			const TCHAR nextch =
				(OP(next) == EXACTLY) ? *OPERAND(next) : _T('\0');
			size_t no;
			TCHAR *const save = reginput;
			const size_t min = (OP(scan) == STAR) ? 0 : 1;

			for (no = regrepeat(OPERAND(scan)) + 1; no > min; no--) {
				reginput = save + no - 1;
				// If it could work, try it. 
				if (nextch == _T('\0') || *reginput == nextch)
					if (regmatch(next))
						return(1);
			}
			return(0);
			break;
			}
		case END:
			return(1);	// Success! 
			break;
		default:
			TRACE0("regexp corruption\n");
			return(0);
			break;
		}
	}

	// We get here only if there's trouble -- normally "case END" is
	// the terminating point.
	 
	TRACE0("corrupted pointers\n");
	return(0);
}


// regrepeat - report how many times something simple would match
 
size_t CRegExp::regrepeat(TCHAR *node)
{
	size_t count;
	TCHAR *scan;
	TCHAR ch;

	switch (OP(node)) 
	{
	case ANY:
		return(_tcslen(reginput));
		break;
	case EXACTLY:
		ch = *OPERAND(node);
		count = 0;
		for (scan = reginput; *scan == ch; scan++)
			count++;
		return(count);
		break;
	case ANYOF:
		return(_tcsspn(reginput, OPERAND(node)));
		break;
	case ANYBUT:
		return(_tcscspn(reginput, OPERAND(node)));
		break;
	default:		// Oh dear.  Called inappropriately. 
		TRACE0("internal error: bad call of regrepeat\n");
		return(0);	// Best compromise. 
		break;
	}
	// NOTREACHED 
}

// regnext - dig the "next" pointer out of a node
 
TCHAR *CRegExp::regnext(TCHAR *p)
{
	const short &offset = *((short*)(p+1));

	if (offset == 0)
		return(NULL);

	return((OP(p) == BACK) ? p-offset : p+offset);
}

// GetReplaceString	- Converts a replace expression to a string
// Returns			- Pointer to newly allocated string
//					  Caller is responsible for deleting it
TCHAR* CRegExp::GetReplaceString( const TCHAR* sReplaceExp )
{
	TCHAR *src = (TCHAR *)sReplaceExp;
	TCHAR *buf;
	TCHAR c;
	int no;
	size_t len;

	if( sReplaceExp == NULL || sFoundText == NULL )
		return NULL;


	// First compute the length of the string
	int replacelen = 0;
	while ((c = *src++) != _T('\0')) 
	{
		if (c == _T('&'))
			no = 0;
		else if (c == _T('\\') && isdigit(*src))
			no = *src++ - _T('0');
		else
			no = -1;

		if (no < 0) 
		{	
			// Ordinary character. 
			if (c == _T('\\') && (*src == _T('\\') || *src == _T('&')))
				c = *src++;
			replacelen++;
		} 
		else if (startp[no] != NULL && endp[no] != NULL &&
					endp[no] > startp[no]) 
		{
			// Get tagged expression
			len = endp[no] - startp[no];
			replacelen += len;
		}
	}

	// Now allocate buf
	buf = new TCHAR[replacelen+1];
	if( buf == NULL )
		return NULL;

	TCHAR* sReplaceStr = buf;

	// Add null termination
	buf[replacelen] = _T('\0');
	
	// Now we can create the string
	src = (TCHAR *)sReplaceExp;
	while ((c = *src++) != _T('\0')) 
	{
		if (c == _T('&'))
			no = 0;
		else if (c == _T('\\') && isdigit(*src))
			no = *src++ - _T('0');
		else
			no = -1;

		if (no < 0) 
		{	
			// Ordinary character. 
			if (c == _T('\\') && (*src == _T('\\') || *src == _T('&')))
				c = *src++;
			*buf++ = c;
		} 
		else if (startp[no] != NULL && endp[no] != NULL &&
					endp[no] > startp[no]) 
		{
			// Get tagged expression
			len = endp[no] - startp[no];
			int tagpos = startp[no] - startp[0];

			_tcsncpy(buf, sFoundText + tagpos, len);
			buf += len;
		}
	}

	return sReplaceStr;
}

//Here's a function that will do global search and replace using regular expressions. Note that the CStringEx class described in the earlier section is being used here. The main reason for using CStringEx is that it provides the Replace() function which makes our task easier. 

/*int RegSearchReplace( CStringEx& string, LPCTSTR sSearchExp, 
					 LPCTSTR sReplaceExp )
{
	int nPos = 0;
	int nReplaced = 0;
	CRegExp r;
	LPTSTR str = (LPTSTR)(LPCTSTR)string;

	r.RegComp( sSearchExp );
	while( (nPos = r.RegFind((LPTSTR)str)) != -1 )
	{
		nReplaced++;
		TCHAR *pReplaceStr = r.GetReplaceString( sReplaceExp );

		int offset = str-(LPCTSTR)string+nPos;
		string.Replace( offset, r.GetFindLen(), 
				pReplaceStr );

		// Replace might have caused a reallocation
		str = (LPTSTR)(LPCTSTR)string + offset + _tcslen(pReplaceStr);
		delete pReplaceStr;
	}
	return nReplaced;
}*/

