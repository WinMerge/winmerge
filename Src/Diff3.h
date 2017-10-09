#pragma once

#include <vector>
#include <algorithm>
#include "DiffList.h"

/* diff3 algorithm. It is almost the same as GNU diff3's algorithm */
template<typename Element, typename Comp02Func>
size_t Make3wayDiff(std::vector<Element>& diff3, const std::vector<Element>& diff10, const std::vector<Element>& diff12,
	Comp02Func cmpfunc, bool ignore_regexp_list)
{
	size_t diff10count = diff10.size();
	size_t diff12count = diff12.size();

	size_t diff10i = 0;
	size_t diff12i = 0;
	size_t diff3i = 0;

	bool firstDiffBlockIsDiff12;

		Element dr3, dr10, dr12, dr10first, dr10last, dr12first, dr12last;

	int linelast0 = 0;
	int linelast1 = 0;
	int linelast2 = 0;

	for (;;)
	{
		if (diff10i >= diff10count && diff12i >= diff12count)
			break;

		/* 
		 * merge overlapped diff blocks
		 * diff10 is diff blocks between file1 and file0.
		 * diff12 is diff blocks between file1 and file2.
		 *
		 *                      diff12
		 *                 diff10            diff3
		 *                 |~~~|             |~~~|
		 * firstDiffBlock  |   |             |   |
		 *                 |   | |~~~|       |   |
		 *                 |___| |   |       |   |
		 *                       |   |   ->  |   |
		 *                 |~~~| |___|       |   |
		 * lastDiffBlock   |   |             |   |
		 *                 |___|             |___|
		 */

		if (diff10i >= diff10count && diff12i < diff12count)
		{
			dr12first = diff12.at(diff12i);
			dr12last = dr12first;
			firstDiffBlockIsDiff12 = true;
		}
		else if (diff10i < diff10count && diff12i >= diff12count)
		{
			dr10first = diff10.at(diff10i);	
			dr10last = dr10first;
			firstDiffBlockIsDiff12 = false;
		}
		else
		{
			dr10first = diff10.at(diff10i);	
			dr12first = diff12.at(diff12i);
			dr10last = dr10first;
			dr12last = dr12first;
			if (dr12first.begin[0] <= dr10first.begin[0])
				firstDiffBlockIsDiff12 = true;
			else
				firstDiffBlockIsDiff12 = false;
		}
		bool lastDiffBlockIsDiff12 = firstDiffBlockIsDiff12;

		size_t diff10itmp = diff10i;
		size_t diff12itmp = diff12i;
		for (;;)
		{
			if (diff10itmp >= diff10count || diff12itmp >= diff12count)
				break;

			dr10 = diff10.at(diff10itmp);
			dr12 = diff12.at(diff12itmp);

			if (dr10.end[0] == dr12.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;

				dr10last = dr10;
				dr12last = dr12;
				break;
			}

			if (lastDiffBlockIsDiff12)
			{
				if (std::max(dr12.begin[0], dr12.end[0]) < dr10.begin[0])
					break;
			}
			else
			{
				if (std::max(dr10.begin[0], dr10.end[0]) < dr12.begin[0])
					break;
			}

			if (dr12.end[0] > dr10.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;
			}
			else
			{
				diff12itmp++;
				lastDiffBlockIsDiff12 = false;
			}

			dr10last = dr10;
			dr12last = dr12;
		}

		if (lastDiffBlockIsDiff12)
			diff12itmp++;
		else
			diff10itmp++;

		if (firstDiffBlockIsDiff12)
		{
			dr3.begin[1] = dr12first.begin[0];
			dr3.begin[2] = dr12first.begin[1];
			if (diff10itmp == diff10i)
				dr3.begin[0] = dr3.begin[1] - linelast1 + linelast0;
			else
				dr3.begin[0] = dr3.begin[1] - dr10first.begin[0] + dr10first.begin[1];
		}
		else
		{
			dr3.begin[0] = dr10first.begin[1];
			dr3.begin[1] = dr10first.begin[0];
			if (diff12itmp == diff12i)
				dr3.begin[2] = dr3.begin[1] - linelast1 + linelast2;
			else
				dr3.begin[2] = dr3.begin[1] - dr12first.begin[0] + dr12first.begin[1];
		}

		if (lastDiffBlockIsDiff12)
		{
			dr3.end[1] = dr12last.end[0];
			dr3.end[2] = dr12last.end[1];
			if (diff10itmp == diff10i)
				dr3.end[0] = dr3.end[1] - linelast1 + linelast0;
			else
				dr3.end[0] = dr3.end[1] - dr10last.end[0] + dr10last.end[1];
		}
		else
		{
			dr3.end[0] = dr10last.end[1];
			dr3.end[1] = dr10last.end[0];
			if (diff12itmp == diff12i)
				dr3.end[2] = dr3.end[1] - linelast1 + linelast2;
			else
				dr3.end[2] = dr3.end[1] - dr12last.end[0] + dr12last.end[1];
		}

		linelast0 = dr3.end[0] + 1;
		linelast1 = dr3.end[1] + 1;
		linelast2 = dr3.end[2] + 1;

		if (diff10i == diff10itmp)
			dr3.op = OP_3RDONLY;
		else if (diff12i == diff12itmp)
			dr3.op = OP_1STONLY;
		else 
		{
			if (!cmpfunc(dr3))
				dr3.op = OP_DIFF;
			else
				dr3.op = OP_2NDONLY;
		}

		if (ignore_regexp_list)
		{
			bool bTrivialDiff10 = true;
			bool bTrivialDiff12 = true;
			size_t i;

			for (i = diff10i; i < diff10itmp; i++)
			{
				dr10 = diff10.at(i);
				if (dr10.op != OP_TRIVIAL)
				{
					bTrivialDiff10 = false;
					break;
				}
			}

			for (i = diff12i; i < diff12itmp; i++)
			{
				dr12 = diff12.at(i);
				if (dr12.op != OP_TRIVIAL)
				{
					bTrivialDiff12 = false;
					break;
				}
			}

			if (bTrivialDiff10 && bTrivialDiff12)
				dr3.op = OP_TRIVIAL;
		}

		diff3.push_back(dr3);

		diff3i++;
		diff10i = diff10itmp;
		diff12i = diff12itmp;
	}
	
	for (size_t i = 0; i < diff3i; i++)
	{
		Element& dr3r = diff3.at(i);
		if (i < diff3i - 1)
		{
			Element& dr3next = diff3.at(i + 1);
			for (int j = 0; j < 3; j++)
			{
				if (dr3r.end[j] >= dr3next.begin[j])
					dr3r.end[j] = dr3next.begin[j] - 1;
			}
		}
	}
	
	return diff3i;
}
