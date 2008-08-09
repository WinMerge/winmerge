#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <commctrl.h>
#include <objbase.h>
#include <shlobj.h>
#include <limits.h>
#include "gtools.h"
#define Zero(a) memset(&(a),0,sizeof(a))
