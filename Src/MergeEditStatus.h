// Interface for merge edit view to display status bar info
class IMergeEditStatus
{
public:
	virtual void SetLineInfo(LPCTSTR szLine, int nChars, int nColumn,
			LPCTSTR szEol) = 0;
};
