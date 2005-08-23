#include <limits.h>
#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <basetyps.h>
#include <vector>
//<jtuc>
//-	help 7z sources compile with old SDK headers
#ifndef GWLP_WNDPROC
#define INT_PTR int // MS has switched to int
typedef long LONG_PTR;

#define SetWindowLongPtr SetWindowLong
#define GetWindowLongPtr GetWindowLong

#define GWLP_WNDPROC GWL_WNDPROC
#define GWLP_USERDATA GWL_USERDATA
#define DWLP_MSGRESULT DWL_MSGRESULT
//#define BIF_NEWDIALOGSTYLE 0
//#define BTNS_BUTTON 0	
#endif

#ifndef ListView_SetCheckState
#define	ListView_SetCheckState(hLv, iItem, bCheck) ListView_SetItemState(hLv, iItem, bCheck ? INDEXTOSTATEIMAGEMASK(2) : INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK)
#endif

typedef unsigned short UINT16, *PUINT16;
//</jtuc>

#define LANG        

// the following defines control how ReadArchiverInfoList() will collect the
// list of available formats.

/*#define EXCLUDE_COM
#define FORMAT_7Z
#define FORMAT_BZIP2
#define FORMAT_GZIP
#define FORMAT_TAR
#define FORMAT_ZIP
#define FORMAT_CPIO
#define FORMAT_RPM
#define FORMAT_ARJ*/
  
//#define _SFX
