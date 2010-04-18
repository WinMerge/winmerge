#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include "PipeIPC.h"

static bool popen2(const String & cmdline, FILE **inWrite, FILE **outRead)
{
	TCHAR *pcmdline = _tcsdup(cmdline.c_str());
	HANDLE hOutReadTmp = NULL, hOutRead = NULL, hOutWrite = NULL;
	HANDLE hInWriteTmp = NULL, hInRead = NULL, hInWrite = NULL;
	HANDLE hErrWrite = NULL;
	HANDLE hCurProc = GetCurrentProcess();
	SECURITY_ATTRIBUTES sa = {0};
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hOutReadTmp, &hOutWrite, &sa, 0))
		goto ERR;
	if (!DuplicateHandle(hCurProc, hOutWrite, hCurProc, &hErrWrite, 0, TRUE, DUPLICATE_SAME_ACCESS))
		goto ERR;
	if (!CreatePipe(&hInRead, &hInWriteTmp, &sa, 0))
		goto ERR;
	if (!DuplicateHandle(hCurProc, hOutReadTmp, hCurProc, &hOutRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
		goto ERR;
	if (!DuplicateHandle(hCurProc, hInWriteTmp, hCurProc, &hInWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
		goto ERR;

	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hOutWrite;
	si.hStdInput = hInRead;
	si.hStdError = hErrWrite;

	if (!CreateProcess(NULL, pcmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		goto ERR;

	*inWrite = _fdopen(_open_osfhandle((int)hInWrite, _O_TEXT), "w");
	*outRead = _fdopen(_open_osfhandle((int)hOutRead, _O_TEXT | _O_RDONLY), "r");
	setvbuf(*inWrite, NULL, _IONBF, 0);
	setvbuf(*outRead, NULL, _IONBF, 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	CloseHandle(hOutWrite);
	CloseHandle(hInRead);
	CloseHandle(hErrWrite);

	CloseHandle(hOutReadTmp);
	CloseHandle(hInWriteTmp);

	free(pcmdline);
	return true;

ERR:
	if (hInWriteTmp)
		CloseHandle(hInWriteTmp);
	if (hInRead)
		CloseHandle(hInRead);
	if (hInWrite)
		CloseHandle(hInWrite);
	if (hErrWrite)
		CloseHandle(hErrWrite);
	if (hOutReadTmp)
		CloseHandle(hOutReadTmp);
	if (hOutRead)
		CloseHandle(hOutRead);
	if (hOutWrite)
		CloseHandle(hOutWrite);
	free(pcmdline);
	return false;
}

PipeIPC::PipeIPC(const String & cmdline) : m_inWrite(NULL), m_outRead(NULL)
{
	popen2(cmdline, &m_inWrite, &m_outRead);
}

PipeIPC::~PipeIPC()
{
	if (m_inWrite)
		fclose(m_inWrite);
	if (m_outRead)
		fclose(m_outRead);
}

std::string PipeIPC::readLineStdout()
{
	if (!m_outRead)
		return "";

	std::string line;
	char buf[512];
	while (fgets(buf, sizeof(buf), m_outRead))
	{
		line += buf;
		if (strchr(buf, '\n') != NULL)
			break;
	}
	return line;
}

char PipeIPC::readCharStdout()
{
	if (!m_outRead)
		return EOF;
	return fgetc(m_outRead);
}


size_t PipeIPC::writeStdin(const std::string & str)
{
	return fwrite(str.c_str(), str.length(), 1, m_inWrite);
}
