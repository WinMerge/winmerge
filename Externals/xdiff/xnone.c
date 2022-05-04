#include "xinclude.h"
#include "xtypes.h"
#include "xdiff.h"

static int none_diff(mmfile_t *file1, mmfile_t *file2,
		xpparam_t const *xpp, xdfenv_t *env,
		int line1, int count1, int line2, int count2)
{
	while(count1 != 0 && count2 != 0) {
		xrecord_t *rec1 = env->xdf1.recs[line1 - 1];
		xrecord_t *rec2 = env->xdf2.recs[line2 - 1];
		int match = xdl_recmatch(
			rec1->ptr, rec1->size,
			rec2->ptr, rec2->size,
			xpp->flags);
		env->xdf1.rchg[line1++ - 1] = !match;
		env->xdf2.rchg[line2++ - 1] = !match;
		count1--;
		count2--;
	}

	if (!count1) {
		while(count2--)
			env->xdf2.rchg[line2++ - 1] = 1;
	} else if (!count2) {
		while(count1--)
			env->xdf1.rchg[line1++ - 1] = 1;
	}
	return 0;
}

int xdl_do_none_diff(mmfile_t *file1, mmfile_t *file2,
		xpparam_t const *xpp, xdfenv_t *env)
{
	if (xdl_prepare_env(file1, file2, xpp, env) < 0)
		return -1;

	/* environment is cleaned up in xdl_diff() */
	return none_diff(file1, file2, xpp, env,
			1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
