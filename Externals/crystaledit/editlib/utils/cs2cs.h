/* Ferda Prantl - character coding conversion engine  */

#pragma once

#include "ctchar.h"

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

#ifndef __cplusplus
#define false 0
#define true 1
typedef int bool;
#endif /* __cplusplus */

typedef struct type_codes_t
  {
    const tchar_t* name, *codes;
  }
type_codes;

extern type_codes source_codes[];
extern type_codes destination_codes[];

tchar_t EDITPADC_CLASS iconvert_char (tchar_t ch, int source_coding, int destination_coding, bool alphabet_only);
int EDITPADC_CLASS iconvert (tchar_t* string, int source_coding, int destination_coding, bool alphabet_only);
int EDITPADC_CLASS iconvert_new (const tchar_t* source, tchar_t* *destination, int source_coding, int destination_coding, bool alphabet_only);

