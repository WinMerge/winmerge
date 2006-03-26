#ifndef FileTextStats_h_included
#define FileTextStats_h_included

struct FileTextStats
{
	int ncrs;
	int nlfs;
	int ncrlfs;
	int nzeros;
	int first_zero; // byte offset, initially -1
	int last_zero; // byte offset, initially -1
	int nlosses;
	FileTextStats() { clear(); }
	void clear() { ncrs = nlfs = ncrlfs = nzeros = nlosses = 0; first_zero = -1; last_zero = -1; }
};


#endif // FileTextStats_h_included
