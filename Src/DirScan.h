#ifndef DirScan_h_included
#define DirScan_h_included

class CDiffContext;
int DirScan(const CString & subdir, CDiffContext * pCtxt, bool casesensitive, int depth=-1);

#endif // DirScan_h_included

