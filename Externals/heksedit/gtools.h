#ifndef gtools_h
#define gtools_h

class PNode : public LIST_ENTRY
{
public:
	PNode()
	{
		Flink = Blink = this;
	}
	virtual ~PNode()
	{
		Blink->Flink = Flink;
		Flink->Blink = Blink;
	}
private:
	PNode(const PNode &); // disallow copy construction
	void operator=(const PNode &); // disallow assignment
};

class PList : public PNode
{
public:
	PList();
	virtual ~PList();
	void AddTail(PNode *);
	bool IsEmpty()
	{
		return Flink == this;
	}
	void DeleteContents();
	long Count;
};

template<size_t n>
class PString
{
	TCHAR buffer[n];
public:
	operator LPTSTR() { return buffer; }
};

void TRACE(const char* pszFormat,...);

#endif // gtools_h
