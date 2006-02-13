/**
 * @file  IntToIntMap.h
 *
 * @brief Declaration of Map from int to int, with a couple new methods
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef IntToIntMap_h_included
#define IntToIntMap_h_included

/**
 * @brief An int->int map with helper methods for finding largest bin
 */
class IntToIntMap : public CMap<int, int, int, int>
{
public:
	void Increment(int key)
	{
		int n=0;
		this->Lookup(key, n);
		this->SetAt(key, ++n);
	}
	int FindMaxKey() const
	{
		int max=0;
		int maxKey=0;
		int key=0;
		int n=0;
		for (POSITION pos = this->GetStartPosition(); pos; )
		{
			this->GetNextAssoc(pos, key, n);
			if (n>max)
			{
				max=n;
				maxKey = key;
			}
		}
		return maxKey;
	}
};


#endif // IntToIntMap_h_included
