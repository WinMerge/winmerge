// Interface for merge edit view to display status bar info
class IMergeEditStatus
{
public:
	virtual void SetLineInfo(LPCTSTR szLine, int nChars, LPCTSTR szEol) = 0;
};
