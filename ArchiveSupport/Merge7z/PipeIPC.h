#include <stdio.h>
#include <string>
#include "Common/UnicodeString.h"

class PipeIPC
{
public:
	PipeIPC(const String & cmdline);
	~PipeIPC();
	std::string readLineStdout();
	char readCharStdout();
	size_t writeStdin(const std::string & str);
private:
	FILE *m_inWrite;
	FILE *m_outRead;
};
