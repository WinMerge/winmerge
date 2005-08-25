#ifndef IAbortable_h_included
#define IAbortable_h_included

/** @brief callback to check if dirscan needs to abort */
class IAbortable
{
public:
	virtual bool ShouldAbort() const = 0;
};

#endif // IAbortable_h_included
