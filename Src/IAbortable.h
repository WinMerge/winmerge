#pragma once

/** @brief callback to check if dirscan needs to abort */
class IAbortable
{
public:
	virtual bool ShouldAbort() const = 0;
};
