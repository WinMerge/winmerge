/* Ferda Prantl - character coding conversion engine  */

#ifndef __CS2CS_H__INCLUDED__
#define __CS2CS_H__INCLUDED__

#define FD_CODE_ASCII      0
#define FD_CODE_CP1250     1
#define FD_CODE_CP1252     2
#define FD_CODE_CP850      3
#define FD_CODE_CP852      4
#define FD_CODE_IBM852     5
#define FD_CODE_ISO_8859_1 6
#define FD_CODE_ISO_8859_2 7
#define FD_CODE_KEYBCS2    8
#define FD_CODE_KOI8_CS    9
#define FD_CODE_MAC        10
#define FD_CODE_MACCE      11
#define FD_CODE_CORK       12
#define FD_CODE_TEX1       13
#define FD_CODE_TEX2       14
#define FD_CODE_HTML1      15
#define FD_CODE_HTML2      16
#define FD_CODE_RTF1       17
#define FD_CODE_RTF2       18

#ifndef NULL
#define NULL 0
#endif /* NULL */

#ifndef _T
typedef char TCHAR;
typedef const char *LPCTSTR;
typedef char *LPTSTR;
#define _T(x) x
#define _tcscpy strcpy
#define _tcslen strlen
#define _tcsnicmp strnicmp
#endif /* _T */

#ifndef __cplusplus
#define false 0
#define true 1
typedef int bool;
#endif /* __cplusplus */

typedef struct type_codes_t
  {
    LPCTSTR name, codes;
  }
type_codes;

extern type_codes source_codes[];
extern type_codes destination_codes[];

TCHAR EDITPADC_CLASS iconvert_char (TCHAR ch, int source_coding, int destination_coding, bool alphabet_only);
int EDITPADC_CLASS iconvert (LPTSTR string, int source_coding, int destination_coding, bool alphabet_only);
int EDITPADC_CLASS iconvert_new (LPCTSTR source, LPTSTR *destination, int source_coding, int destination_coding, bool alphabet_only);

#endif // __CS2CS_H__INCLUDED__
