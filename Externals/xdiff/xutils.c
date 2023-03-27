/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003	Davide Libenzi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#include <limits.h>
#include <assert.h>
#include "xinclude.h"



static inline int is_eol(char const* ptr, char const* top)
{
	return (*ptr == '\n' || (*ptr == '\r' && (ptr == top - 1 || *(ptr + 1) != '\n')));
}

enum EOL_TYPE {  EOL_NONE, EOL_LF, EOL_CR, EOL_CRLF };

static enum EOL_TYPE eol_type (const char *l, long s)
{
	if (s <= 0)
		return EOL_NONE;
	else if (l[s - 1] == '\n') {
		if (s > 1 && l[s - 2] == '\r')
			return EOL_CRLF;
		return EOL_LF;
	}
	else if (l[s - 1] == '\r')
		return EOL_CR;
	return EOL_NONE;
}

long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	for (i = 1; n > 0; n >>= 2)
		i <<= 1;

	return i;
}


int xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb) {
	int i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (char *) pre;
	mb[0].size = psize;
	mb[1].ptr = (char *) rec;
	mb[1].size = size;
	if (size > 0 && !is_eol(rec + size - 1, rec + size)) {
		mb[2].ptr = (char *) "\n\\ No newline at end of file\n";
		mb[2].size = strlen(mb[2].ptr);
		i++;
	}
	if (ecb->out_line(ecb->priv, mb, i) < 0) {

		return -1;
	}

	return 0;
}

void *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	return mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	return mmf->size;
}


int xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = NULL;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = NULL;
	cha->scurr = 0;

	return 0;
}


void xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	for (cur = cha->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


void *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	void *data;

	if (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		if (!(ancur = (chanode_t *) xdl_malloc(sizeof(chanode_t) + cha->nsize))) {

			return NULL;
		}
		ancur->icurr = 0;
		ancur->next = NULL;
		if (cha->tail)
			cha->tail->next = ancur;
		if (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (char *) ancur + sizeof(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	return data;
}

long xdl_guess_lines(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	char const *data, *cur, *top;

	if ((cur = data = xdl_mmfile_first(mf, &size)) != NULL) {
		for (top = data + size; nl < sample && cur < top; ) {
			nl++;
			while (cur < top && !is_eol(cur, top))
				cur++;
			if (is_eol(cur, top))
				cur++;
		}
		tsize += (long) (cur - data);
	}

	if (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	return nl + 1;
}

int xdl_blankline(const char *line, long size, long flags)
{
	long i;

	if (!(flags & XDF_WHITESPACE_FLAGS))
		return (size <= 1);

	for (i = 0; i < size && XDL_ISSPACE(line[i]); i++)
		;

	return (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
static int ends_with_optional_cr(const char *l, long s, long i)
{
	const enum EOL_TYPE eol = eol_type(l, s);
	if ((eol == EOL_NONE && i >= s) ||
	    (eol == EOL_LF   && i >= s - 1) ||
	    (eol == EOL_CR   && i >= s - 1) ||
	    (eol == EOL_CRLF && i >= s - 2))
		return 1;
	return 0;
}
static inline int match_a_byte(char ch1, char ch2, long flags)
{
	if (ch1 == ch2)
		return 1;
	if (!(flags & XDF_IGNORE_CASE) || ((ch1 | ch2) & 0x80))
		return 0;
	if (isupper(ch1))
		ch1 = tolower(ch1);
	if (isupper(ch2))
		ch2 = tolower(ch2);
	return (ch1 == ch2);
}

int xdl_recmatch(const char *l1, long s1, const char *l2, long s2, long flags)
{
	int i1, i2;

	if (s1 == s2 && !memcmp(l1, l2, s1))
		return 1;
	if (!(flags & XDF_INEXACT_MATCH))
		return 0;

	i1 = 0;
	i2 = 0;

	/*
	 * -w matches everything that matches with -b, and -b in turn
	 * matches everything that matches with --ignore-space-at-eol,
	 * which in turn matches everything that matches with --ignore-cr-at-eol.
	 *
	 * Each flavor of ignoring needs different logic to skip whitespaces
	 * while we have both sides to compare.
	 */
	if (flags & XDF_IGNORE_WHITESPACE) {
		goto skip_ws;
		while (i1 < s1 && i2 < s2) {
			if (!match_a_byte(l1[i1++], l2[i2++], flags))
				return 0;
		skip_ws:
			while (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			while (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
			if (flags & XDF_IGNORE_NUMBERS) {
				while (i1 < s1 && XDL_ISDIGIT(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISDIGIT(l2[i2]))
					i2++;
			}
		}
	} else if (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		while (i1 < s1 && i2 < s2) {
			if (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				while (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				continue;
			} else if ((flags & XDF_IGNORE_NUMBERS) && (XDL_ISDIGIT(l1[i1]) || XDL_ISDIGIT(l2[i2]))) {
				while (i1 < s1 && XDL_ISDIGIT(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISDIGIT(l2[i2]))
					i2++;
				continue;
			}
			if (!match_a_byte(l1[i1++], l2[i2++], flags))
				return 0;
		}
	} else if (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		while (i1 < s1 && i2 < s2) {
			if ((flags & XDF_IGNORE_NUMBERS) && (XDL_ISDIGIT(l1[i1]) || XDL_ISDIGIT(l2[i2]))) {
				while (i1 < s1 && XDL_ISDIGIT(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISDIGIT(l2[i2]))
					i2++;
				continue;
			}
			if (match_a_byte(l1[i1], l2[i2], flags)) {
				i1++;
				i2++;
			} else {
				break;
			}
		}
	} else if (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		while (i1 < s1 && i2 < s2) {
			if ((flags & XDF_IGNORE_NUMBERS) && (XDL_ISDIGIT(l1[i1]) || XDL_ISDIGIT(l2[i2]))) {
				while (i1 < s1 && XDL_ISDIGIT(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISDIGIT(l2[i2]))
					i2++;
				continue;
			}
			if (match_a_byte(l1[i1], l2[i2], flags)) {
				i1++;
				i2++;
			} else {
				break;
			}
		}
		int has_eol1 = s1 > 0 && is_eol(l1 + s1 - 1, l1 + s1);
		int has_eol2 = s2 > 0 && is_eol(l2 + s2 - 1, l2 + s2);
		return (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2)) && 
			((has_eol1 && has_eol2) || (!has_eol1 && !has_eol2));
	} else {
		while (i1 < s1 && i2 < s2) {
			if ((flags & XDF_IGNORE_NUMBERS) && (XDL_ISDIGIT(l1[i1]) || XDL_ISDIGIT(l2[i2]))) {
				while (i1 < s1 && XDL_ISDIGIT(l1[i1]))
					i1++;
				while (i2 < s2 && XDL_ISDIGIT(l2[i2]))
					i2++;
				continue;
			}
			if (!match_a_byte(l1[i1++], l2[i2++], flags))
				return 0;
		}
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	int has_eol1 = s1 > 0 && is_eol(l1 + s1 - 1, l1 + s1);
	int has_eol2 = s2 > 0 && is_eol(l2 + s2 - 1, l2 + s2);
	if (!((has_eol1 && has_eol2) || (!has_eol1 && !has_eol2)))
		return 0;
	if (i1 < s1) {
		if (flags & XDF_IGNORE_NUMBERS) {
			while (i1 < s1 && (XDL_ISSPACE(l1[i1]) || XDL_ISDIGIT(l1[i1])))
				i1++;
		}
		else {
			while (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
		}
		if (s1 != i1)
			return 0;
	}
	if (i2 < s2) {
		if (flags & XDF_IGNORE_NUMBERS) {
			while (i2 < s2 && (XDL_ISSPACE(l2[i2]) || XDL_ISDIGIT(l2[i2])))
				i2++;
		}
		else {
			while (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
		if (s2 != i2)
			return 0;
	}
	if (!(flags & XDF_IGNORE_CR_AT_EOL))
	{
		if (eol_type(l1, s1) != eol_type(l2, s2))
			return 0;
	}
	return 1;
}

static inline unsigned long hash_a_byte(const char ch_, long flags)
{
	unsigned long ch = ch_ & 0xFF;
	if ((flags & XDF_IGNORE_CASE) && !(ch & 0x80) && isupper(ch))
		ch = tolower(ch);
	return ch;
}

static unsigned long xdl_hash_record_with_whitespace(char const **data,
		char const *top, long flags) {
	unsigned long ha = 5381;
	char const *ptr = *data;

	for (; ptr < top && !is_eol(ptr, top); ptr++) {
		if (*ptr == '\r') {
			if (flags & XDF_IGNORE_CR_AT_EOL)
				continue;
		}
		else if (XDL_ISSPACE(*ptr)) {
			const char *ptr2 = ptr;
			int at_eol;
			while (ptr + 1 < top && XDL_ISSPACE(ptr[1])
				&& !(ptr[1] == '\r' || ptr[1] == '\n'))
				ptr++;
			at_eol = (top <= ptr + 1 || (ptr[1] == '\r' || ptr[1] == '\n'));
			if (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			else if (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= hash_a_byte(' ', flags);
			}
			else if (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				while (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= hash_a_byte(*ptr2, flags);
					ptr2++;
				}
			}
			continue;
		}
		if ((flags & XDF_IGNORE_NUMBERS) && XDL_ISDIGIT(*ptr))
			continue;
		ha += (ha << 5);
		ha ^= hash_a_byte(*ptr, flags);
	}
	if (ptr < top) {
		ha += (ha << 5);
		ha ^= hash_a_byte((flags & XDF_IGNORE_CR_AT_EOL) ? '\n' : *ptr, flags);
		ptr++;
	}
	*data = ptr;

	return ha;
}

unsigned long xdl_hash_record(char const **data, char const *top, long flags) {
	unsigned long ha = 5381;
	char const *ptr = *data;

	if (flags & XDF_INEXACT_MATCH)
		return xdl_hash_record_with_whitespace(data, top, flags);

	for (; ptr < top && !is_eol(ptr, top); ptr++) {
		ha += (ha << 5);
		ha ^= hash_a_byte(*ptr, flags);
	}
	if (ptr < top) {
		ha += (ha << 5);
		ha ^= hash_a_byte(*ptr, flags);
		ptr++;
	}
	*data = ptr;

	return ha;
}

unsigned int xdl_hashbits(unsigned int size) {
	unsigned int val = 1, bits = 0;

	for (; val < size && bits < CHAR_BIT * sizeof(unsigned int); val <<= 1, bits++);
	return bits ? bits: 1;
}


int xdl_num_out(char *out, long val) {
	char *ptr, *str = out;
	char buf[32];

	ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';
	if (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	for (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	if (*ptr)
		for (; *ptr; ptr++, str++)
			*str = *ptr;
	else
		*str++ = '0';
	*str = '\0';

	return str - out;
}

static int xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       const char *func, long funclen,
			       xdemitcb_t *ecb) {
	int nb = 0;
	mmbuffer_t mb;
	char buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_num_out(buf + nb, c1 ? s1: s1 - 1);

	if (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_num_out(buf + nb, c2 ? s2: s2 - 1);

	if (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	if (func && funclen) {
		buf[nb++] = ' ';
		if (funclen > sizeof(buf) - nb - 1)
			funclen = sizeof(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	if (ecb->out_line(ecb->priv, &mb, 1) < 0)
		return -1;
	return 0;
}

int xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const char *func, long funclen,
		      xdemitcb_t *ecb) {
	if (!ecb->out_hunk)
		return xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	if (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		return -1;
	return 0;
}

int xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		int line1, int count1, int line2, int count2)
{
	/*
	 * This probably does not work outside Git, since
	 * we have a very simple mmfile structure.
	 *
	 * Note: ideally, we would reuse the prepared environment, but
	 * the libxdiff interface does not (yet) allow for diffing only
	 * ranges of lines instead of the whole files.
	 */
	mmfile_t subfile1, subfile2;
	xdfenv_t env;

	subfile1.ptr = (char *)diff_env->xdf1.recs[line1 - 1]->ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2]->ptr +
		diff_env->xdf1.recs[line1 + count1 - 2]->size - subfile1.ptr;
	subfile2.ptr = (char *)diff_env->xdf2.recs[line2 - 1]->ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2]->ptr +
		diff_env->xdf2.recs[line2 + count2 - 2]->size - subfile2.ptr;
	if (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		return -1;

	memcpy(diff_env->xdf1.rchg + line1 - 1, env.xdf1.rchg, count1);
	memcpy(diff_env->xdf2.rchg + line2 - 1, env.xdf2.rchg, count2);

	xdl_free_env(&env);

	return 0;
}
