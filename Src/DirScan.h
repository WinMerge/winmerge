#ifndef DirScan_h_included
#define DirScan_h_included

class CDiffContext;

/** @brief callback to check if dirscan needs to abort */
class IAbortable
{
public:
	virtual bool ShouldAbort() = 0;
};

int DirScan(const CString & subdir, CDiffContext * pCtxt, bool casesensitive,
	int depth, IAbortable * piAbortable);


#endif // DirScan_h_included

