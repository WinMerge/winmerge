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
class IntToIntMap
{
private:
	std::map<int, int> m_map;
public:
	void Increment(int key)
	{
		++m_map[key];
	}
	int FindMaxKey() const
	{
		int max=0;
		int maxKey=0;
		int key=0;
		int n=0;
		std::map<int, int>::const_iterator pos = m_map.begin();
		while (pos != m_map.end())
		{
			if (pos->second > max)
			{
				max = pos->second;
				maxKey = pos->first;
			}
			++pos;
		}
		return maxKey;
	}
};


#endif // IntToIntMap_h_included
