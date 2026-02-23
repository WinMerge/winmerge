/* charsets.c -- character set information and mappings

  (c) 1998-2004 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

* Changes 2005-01-09 Jochen Tucht
  This file originates from the Tidy library (tidy.sourceforge.net).
  Dependencies on other parts of the library have been removed to make it
  easily plug into a Win32 application that does not use Tidy elsewhere.
  Runtime-allocated indexes have been added to improve lookup speed.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "charsets.h"

enum { no, yes };

/* todo: documentation of table and data       */
/* todo: integration of data and routines      */
/* todo: minor cleanup for ambiguous relations */
static struct _charsetInfo
{
	unsigned id;
	const char *charset;
	unsigned codepage;
	unsigned stateful;
} const charsetInfo[] =
{
  {   1, "Adobe-Standard-Encoding",                           0,  no }, /* IANA */
  {   1, "csAdobeStandardEncoding",                           0,  no }, /* IANA */
  {   2, "Adobe-Symbol-Encoding",                             0,  no }, /* IANA */
  {   2, "csHPPSMath",                                        0,  no }, /* IANA */
  {   3, "iso-ir-99",                                         0,  no }, /* IANA */
  {   3, "ANSI_X3.110-1983",                                  0,  no }, /* IANA */
  {   3, "CSA_T500-1983",                                     0,  no }, /* IANA */
  {   3, "NAPLPS",                                            0,  no }, /* IANA */
  {   3, "csISO99NAPLPS",                                     0,  no }, /* IANA */
  {   4, "US-ASCII",                                      20127,  no }, /* IANA */
  {   4, "iso-ir-6",                                      20127,  no }, /* IANA */
  {   4, "ANSI_X3.4-1968",                                20127,  no }, /* IANA */
  {   4, "ANSI_X3.4-1986",                                20127,  no }, /* IANA */
  {   4, "ISO_646.irv:1991",                              20127,  no }, /* IANA */
  {   4, "ascii",                                         20127,  no }, /* IANA */
  {   4, "ISO646-US",                                     20127,  no }, /* IANA */
  {   4, "us",                                            20127,  no }, /* IANA */
  {   4, "IBM367",                                        20127,  no }, /* IANA */
  {   4, "cp367",                                         20127,  no }, /* IANA */
  {   4, "csASCII",                                       20127,  no }, /* IANA */
  {   4, "iso-ir-6us",                                    20127,  no }, /* MICROSOFT */
  {   5, "iso-ir-89",                                         0,  no }, /* IANA */
  {   5, "ASMO_449",                                          0,  no }, /* IANA */
  {   5, "ISO_9036",                                          0,  no }, /* IANA */
  {   5, "arabic7",                                           0,  no }, /* IANA */
  {   5, "csISO89ASMO449",                                    0,  no }, /* IANA */
  {   6, "Big5",                                            950,  no }, /* IANA */
  {   6, "csBig5",                                          950,  no }, /* IANA */
  {   6, "cn-big5",                                         950,  no }, /* MICROSOFT */
  {   6, "x-x-big5",                                        950,  no }, /* MICROSOFT */
  {   7, "Big5-HKSCS",                                      950,  no }, /* IANA */
  {   7, "cn-big5",                                         950,  no }, /* MICROSOFT */
  {   7, "x-x-big5",                                        950,  no }, /* MICROSOFT */
  {   8, "BOCU-1",                                            0,  no }, /* IANA */
  {   8, "csBOCU-1",                                          0,  no }, /* IANA */
  {   9, "iso-ir-4",                                          0,  no }, /* IANA */
  {   9, "BS_4730",                                           0,  no }, /* IANA */
  {   9, "ISO646-GB",                                         0,  no }, /* IANA */
  {   9, "gb",                                                0,  no }, /* IANA */
  {   9, "uk",                                                0,  no }, /* IANA */
  {   9, "csISO4UnitedKingdom",                               0,  no }, /* IANA */
  {  10, "iso-ir-47",                                         0,  no }, /* IANA */
  {  10, "BS_viewdata",                                       0,  no }, /* IANA */
  {  10, "csISO47BSViewdata",                                 0,  no }, /* IANA */
  {  11, "CESU-8",                                            0,  no }, /* IANA */
  {  11, "csCESU-8",                                          0,  no }, /* IANA */
  {  12, "iso-ir-121",                                        0,  no }, /* IANA */
  {  12, "CSA_Z243.4-1985-1",                                 0,  no }, /* IANA */
  {  12, "ISO646-CA",                                         0,  no }, /* IANA */
  {  12, "csa7-1",                                            0,  no }, /* IANA */
  {  12, "ca",                                                0,  no }, /* IANA */
  {  12, "csISO121Canadian1",                                 0,  no }, /* IANA */
  {  13, "iso-ir-122",                                        0,  no }, /* IANA */
  {  13, "CSA_Z243.4-1985-2",                                 0,  no }, /* IANA */
  {  13, "ISO646-CA2",                                        0,  no }, /* IANA */
  {  13, "csa7-2",                                            0,  no }, /* IANA */
  {  13, "csISO122Canadian2",                                 0,  no }, /* IANA */
  {  14, "iso-ir-123",                                        0,  no }, /* IANA */
  {  14, "CSA_Z243.4-1985-gr",                                0,  no }, /* IANA */
  {  14, "csISO123CSAZ24341985gr",                            0,  no }, /* IANA */
  {  15, "iso-ir-139",                                        0,  no }, /* IANA */
  {  15, "CSN_369103",                                        0,  no }, /* IANA */
  {  15, "csISO139CSN369103",                                 0,  no }, /* IANA */
  {  16, "DEC-MCS",                                           0,  no }, /* IANA */
  {  16, "dec",                                               0,  no }, /* IANA */
  {  16, "csDECMCS",                                          0,  no }, /* IANA */
  {  17, "iso-ir-21",                                     20106,  no }, /* IANA */
  {  17, "DIN_66003",                                     20106,  no }, /* IANA */
  {  17, "de",                                            20106,  no }, /* IANA */
  {  17, "ISO646-DE",                                     20106,  no }, /* IANA */
  {  17, "csISO21German",                                 20106,  no }, /* IANA */
  {  17, "x-ia5-german",                                  20106,  no }, /* MICROSOFT */
  {  18, "dk-us",                                             0,  no }, /* IANA */
  {  18, "csDKUS",                                            0,  no }, /* IANA */
  {  19, "DS_2089",                                           0,  no }, /* IANA */
  {  19, "DS2089",                                            0,  no }, /* IANA */
  {  19, "ISO646-DK",                                         0,  no }, /* IANA */
  {  19, "dk",                                                0,  no }, /* IANA */
  {  19, "csISO646Danish",                                    0,  no }, /* IANA */
  {  20, "EBCDIC-AT-DE",                                      0,  no }, /* IANA */
  {  20, "csIBMEBCDICATDE",                                   0,  no }, /* IANA */
  {  21, "EBCDIC-AT-DE-A",                                    0,  no }, /* IANA */
  {  21, "csEBCDICATDEA",                                     0,  no }, /* IANA */
  {  22, "EBCDIC-CA-FR",                                      0,  no }, /* IANA */
  {  22, "csEBCDICCAFR",                                      0,  no }, /* IANA */
  {  23, "EBCDIC-DK-NO",                                      0,  no }, /* IANA */
  {  23, "csEBCDICDKNO",                                      0,  no }, /* IANA */
  {  24, "EBCDIC-DK-NO-A",                                    0,  no }, /* IANA */
  {  24, "csEBCDICDKNOA",                                     0,  no }, /* IANA */
  {  25, "EBCDIC-ES",                                         0,  no }, /* IANA */
  {  25, "csEBCDICES",                                        0,  no }, /* IANA */
  {  26, "EBCDIC-ES-A",                                       0,  no }, /* IANA */
  {  26, "csEBCDICESA",                                       0,  no }, /* IANA */
  {  27, "EBCDIC-ES-S",                                       0,  no }, /* IANA */
  {  27, "csEBCDICESS",                                       0,  no }, /* IANA */
  {  28, "EBCDIC-FI-SE",                                      0,  no }, /* IANA */
  {  28, "csEBCDICFISE",                                      0,  no }, /* IANA */
  {  29, "EBCDIC-FI-SE-A",                                    0,  no }, /* IANA */
  {  29, "csEBCDICFISEA",                                     0,  no }, /* IANA */
  {  30, "EBCDIC-FR",                                         0,  no }, /* IANA */
  {  30, "csEBCDICFR",                                        0,  no }, /* IANA */
  {  31, "EBCDIC-IT",                                         0,  no }, /* IANA */
  {  31, "csEBCDICIT",                                        0,  no }, /* IANA */
  {  32, "EBCDIC-PT",                                         0,  no }, /* IANA */
  {  32, "csEBCDICPT",                                        0,  no }, /* IANA */
  {  33, "EBCDIC-UK",                                         0,  no }, /* IANA */
  {  33, "csEBCDICUK",                                        0,  no }, /* IANA */
  {  34, "EBCDIC-US",                                         0,  no }, /* IANA */
  {  34, "csEBCDICUS",                                        0,  no }, /* IANA */
  {  35, "iso-ir-111",                                        0,  no }, /* IANA */
  {  35, "ECMA-cyrillic",                                     0,  no }, /* IANA */
  {  35, "csISO111ECMACyrillic",                              0,  no }, /* IANA */
  {  36, "iso-ir-17",                                         0,  no }, /* IANA */
  {  36, "ES",                                                0,  no }, /* IANA */
  {  36, "ISO646-ES",                                         0,  no }, /* IANA */
  {  36, "csISO17Spanish",                                    0,  no }, /* IANA */
  {  37, "iso-ir-85",                                         0,  no }, /* IANA */
  {  37, "ES2",                                               0,  no }, /* IANA */
  {  37, "ISO646-ES2",                                        0,  no }, /* IANA */
  {  37, "csISO85Spanish2",                                   0,  no }, /* IANA */
  {  38, "EUC-KR",                                        51949,  no }, /* IANA */
  {  38, "csEUCKR",                                       51949,  no }, /* IANA */
  {  39, "Extended_UNIX_Code_Fixed_Width_for_Japanese",       0,  no }, /* IANA */
  {  39, "csEUCFixWidJapanese",                               0,  no }, /* IANA */
  {  40, "EUC-JP",                                        51932,  no }, /* IANA */
  {  40, "Extended_UNIX_Code_Packed_Format_for_Japanese", 51932,  no }, /* IANA */
  {  40, "csEUCPkdFmtJapanese",                           51932,  no }, /* IANA */
  {  40, "x-euc-jp",                                      51932,  no }, /* MICROSOFT */
  {  40, "x-euc",                                         51932,  no }, /* MICROSOFT */
  {  41, "GB18030",                                       54936,  no }, /* IANA */
  {  42, "GB2312",                                          936,  no }, /* IANA */
  {  42, "csGB2312",                                        936,  no }, /* IANA */
  {  42, "csgb231280",                                      936,  no }, /* MICROSOFT */
  {  42, "gb2312-80",                                       936,  no }, /* MICROSOFT */
  {  42, "gb231280",                                        936,  no }, /* MICROSOFT */
  {  42, "cn-gb",                                           936,  no }, /* MICROSOFT */
  {  43, "iso-ir-57",                                         0,  no }, /* IANA */
  {  43, "GB_1988-80",                                        0,  no }, /* IANA */
  {  43, "cn",                                                0,  no }, /* IANA */
  {  43, "ISO646-CN",                                         0,  no }, /* IANA */
  {  43, "csISO57GB1988",                                     0,  no }, /* IANA */
  {  44, "iso-ir-58",                                       936,  no }, /* IANA */
  {  44, "GB_2312-80",                                      936,  no }, /* IANA */
  {  44, "chinese",                                         936,  no }, /* IANA */
  {  44, "csISO58GB231280",                                 936,  no }, /* IANA */
  {  44, "csgb231280",                                      936,  no }, /* MICROSOFT */
  {  44, "gb2312-80",                                       936,  no }, /* MICROSOFT */
  {  44, "gb231280",                                        936,  no }, /* MICROSOFT */
  {  44, "cn-gb",                                           936,  no }, /* MICROSOFT */
  {  45, "GBK",                                             936,  no }, /* IANA */
  {  45, "CP936",                                           936,  no }, /* IANA */
  {  45, "MS936",                                           936,  no }, /* IANA */
  {  45, "Windows-936",                                     936,  no }, /* IANA */
  {  45, "csgb231280",                                      936,  no }, /* MICROSOFT */
  {  45, "gb2312-80",                                       936,  no }, /* MICROSOFT */
  {  45, "gb231280",                                        936,  no }, /* MICROSOFT */
  {  45, "cn-gb",                                           936,  no }, /* MICROSOFT */
  {  46, "iso-ir-153",                                        0,  no }, /* IANA */
  {  46, "GOST_19768-74",                                     0,  no }, /* IANA */
  {  46, "ST_SEV_358-88",                                     0,  no }, /* IANA */
  {  46, "csISO153GOST1976874",                               0,  no }, /* IANA */
  {  47, "iso-ir-150",                                        0,  no }, /* IANA */
  {  47, "greek-ccitt",                                       0,  no }, /* IANA */
  {  47, "csISO150",                                          0,  no }, /* IANA */
  {  47, "csISO150GreekCCITT",                                0,  no }, /* IANA */
  {  48, "iso-ir-88",                                         0,  no }, /* IANA */
  {  48, "greek7",                                            0,  no }, /* IANA */
  {  48, "csISO88Greek7",                                     0,  no }, /* IANA */
  {  49, "iso-ir-18",                                         0,  no }, /* IANA */
  {  49, "greek7-old",                                        0,  no }, /* IANA */
  {  49, "csISO18Greek7Old",                                  0,  no }, /* IANA */
  {  50, "HP-DeskTop",                                        0,  no }, /* IANA */
  {  50, "csHPDesktop",                                       0,  no }, /* IANA */
  {  51, "HP-Legal",                                          0,  no }, /* IANA */
  {  51, "csHPLegal",                                         0,  no }, /* IANA */
  {  52, "HP-Math8",                                          0,  no }, /* IANA */
  {  52, "csHPMath8",                                         0,  no }, /* IANA */
  {  53, "HP-Pi-font",                                        0,  no }, /* IANA */
  {  53, "csHPPiFont",                                        0,  no }, /* IANA */
  {  54, "hp-roman8",                                         0,  no }, /* IANA */
  {  54, "roman8",                                            0,  no }, /* IANA */
  {  54, "r8",                                                0,  no }, /* IANA */
  {  54, "csHPRoman8",                                        0,  no }, /* IANA */
  {  55, "HZ-GB-2312",                                    52936, yes }, /* IANA */
  {  56, "IBM-Symbols",                                       0,  no }, /* IANA */
  {  56, "csIBMSymbols",                                      0,  no }, /* IANA */
  {  57, "IBM-Thai",                                      20838,  no }, /* IANA */
  {  57, "csIBMThai",                                     20838,  no }, /* IANA */
  {  57, "x-ebcdic-thai",                                 20838,  no }, /* MICROSOFT */
  {  58, "IBM00858",                                        858,  no }, /* IANA */
  {  58, "CCSID00858",                                      858,  no }, /* IANA */
  {  58, "CP00858",                                         858,  no }, /* IANA */
  {  58, "PC-Multilingual-850+euro",                        858,  no }, /* IANA */
  {  59, "IBM00924",                                      20924,  no }, /* IANA */
  {  59, "CCSID00924",                                    20924,  no }, /* IANA */
  {  59, "CP00924",                                       20924,  no }, /* IANA */
  {  59, "ebcdic-Latin9--euro",                           20924,  no }, /* IANA */
  {  60, "IBM01140",                                       1140,  no }, /* IANA */
  {  60, "CCSID01140",                                     1140,  no }, /* IANA */
  {  60, "CP01140",                                        1140,  no }, /* IANA */
  {  60, "ebcdic-us-37+euro",                              1140,  no }, /* IANA */
  {  60, "x-ebcdic-cp-us-euro",                            1140,  no }, /* MICROSOFT */
  {  61, "IBM01141",                                       1141,  no }, /* IANA */
  {  61, "CCSID01141",                                     1141,  no }, /* IANA */
  {  61, "CP01141",                                        1141,  no }, /* IANA */
  {  61, "ebcdic-de-273+euro",                             1141,  no }, /* IANA */
  {  61, "x-ebcdic-germany-euro",                          1141,  no }, /* MICROSOFT */
  {  62, "IBM01142",                                       1142,  no }, /* IANA */
  {  62, "CCSID01142",                                     1142,  no }, /* IANA */
  {  62, "CP01142",                                        1142,  no }, /* IANA */
  {  62, "ebcdic-dk-277+euro",                             1142,  no }, /* IANA */
  {  62, "ebcdic-no-277+euro",                             1142,  no }, /* IANA */
  {  62, "x-ebcdic-denmarknorway-euro",                    1142,  no }, /* MICROSOFT */
  {  63, "IBM01143",                                       1143,  no }, /* IANA */
  {  63, "CCSID01143",                                     1143,  no }, /* IANA */
  {  63, "CP01143",                                        1143,  no }, /* IANA */
  {  63, "ebcdic-fi-278+euro",                             1143,  no }, /* IANA */
  {  63, "ebcdic-se-278+euro",                             1143,  no }, /* IANA */
  {  63, "x-ebcdic-france",                                1143,  no }, /* MICROSOFT */
  {  63, "x-ebcdic-finlandsweden-euro",                    1143,  no }, /* MICROSOFT */
  {  64, "IBM01144",                                       1144,  no }, /* IANA */
  {  64, "CCSID01144",                                     1144,  no }, /* IANA */
  {  64, "CP01144",                                        1144,  no }, /* IANA */
  {  64, "ebcdic-it-280+euro",                             1144,  no }, /* IANA */
  {  64, "x-ebcdic-italy-euro",                            1144,  no }, /* MICROSOFT */
  {  65, "IBM01145",                                       1145,  no }, /* IANA */
  {  65, "CCSID01145",                                     1145,  no }, /* IANA */
  {  65, "CP01145",                                        1145,  no }, /* IANA */
  {  65, "ebcdic-es-284+euro",                             1145,  no }, /* IANA */
  {  65, "x-ebcdic-spain-euro",                            1145,  no }, /* MICROSOFT */
  {  66, "IBM01146",                                       1146,  no }, /* IANA */
  {  66, "CCSID01146",                                     1146,  no }, /* IANA */
  {  66, "CP01146",                                        1146,  no }, /* IANA */
  {  66, "ebcdic-gb-285+euro",                             1146,  no }, /* IANA */
  {  66, "x-ebcdic-uk-euro",                               1146,  no }, /* MICROSOFT */
  {  67, "IBM01147",                                       1147,  no }, /* IANA */
  {  67, "CCSID01147",                                     1147,  no }, /* IANA */
  {  67, "CP01147",                                        1147,  no }, /* IANA */
  {  67, "ebcdic-fr-297+euro",                             1147,  no }, /* IANA */
  {  67, "x-ebcdic-france-euro",                           1147,  no }, /* MICROSOFT */
  {  68, "IBM01148",                                       1148,  no }, /* IANA */
  {  68, "CCSID01148",                                     1148,  no }, /* IANA */
  {  68, "CP01148",                                        1148,  no }, /* IANA */
  {  68, "ebcdic-international-500+euro",                  1148,  no }, /* IANA */
  {  68, "x-ebcdic-international-euro",                    1148,  no }, /* MICROSOFT */
  {  69, "IBM01149",                                       1149,  no }, /* IANA */
  {  69, "CCSID01149",                                     1149,  no }, /* IANA */
  {  69, "CP01149",                                        1149,  no }, /* IANA */
  {  69, "ebcdic-is-871+euro",                             1149,  no }, /* IANA */
  {  69, "x-ebcdic-icelandic-euro",                        1149,  no }, /* MICROSOFT */
  {  70, "IBM037",                                           37,  no }, /* IANA */
  {  70, "cp037",                                            37,  no }, /* IANA */
  {  70, "ebcdic-cp-us",                                     37,  no }, /* IANA */
  {  70, "ebcdic-cp-ca",                                     37,  no }, /* IANA */
  {  70, "ebcdic-cp-wt",                                     37,  no }, /* IANA */
  {  70, "ebcdic-cp-nl",                                     37,  no }, /* IANA */
  {  70, "csIBM037",                                         37,  no }, /* IANA */
  {  71, "IBM038",                                            0,  no }, /* IANA */
  {  71, "EBCDIC-INT",                                        0,  no }, /* IANA */
  {  71, "cp038",                                             0,  no }, /* IANA */
  {  71, "csIBM038",                                          0,  no }, /* IANA */
  {  72, "IBM1026",                                        1026,  no }, /* IANA */
  {  72, "CP1026",                                         1026,  no }, /* IANA */
  {  72, "csIBM1026",                                      1026,  no }, /* IANA */
  {  73, "IBM1047",                                           0,  no }, /* IANA */
  {  73, "IBM-1047",                                          0,  no }, /* IANA */
  {  74, "IBM273",                                        20273,  no }, /* IANA */
  {  74, "CP273",                                         20273,  no }, /* IANA */
  {  74, "csIBM273",                                      20273,  no }, /* IANA */
  {  74, "x-ebcdic-germany",                              20273,  no }, /* MICROSOFT */
  {  75, "IBM274",                                            0,  no }, /* IANA */
  {  75, "EBCDIC-BE",                                         0,  no }, /* IANA */
  {  75, "CP274",                                             0,  no }, /* IANA */
  {  75, "csIBM274",                                          0,  no }, /* IANA */
  {  76, "IBM275",                                            0,  no }, /* IANA */
  {  76, "EBCDIC-BR",                                         0,  no }, /* IANA */
  {  76, "cp275",                                             0,  no }, /* IANA */
  {  76, "csIBM275",                                          0,  no }, /* IANA */
  {  77, "IBM277",                                        20277,  no }, /* IANA */
  {  77, "EBCDIC-CP-DK",                                  20277,  no }, /* IANA */
  {  77, "EBCDIC-CP-NO",                                  20277,  no }, /* IANA */
  {  77, "csIBM277",                                      20277,  no }, /* IANA */
  {  77, "x-ebcdic-denmarknorway",                        20277,  no }, /* MICROSOFT */
  {  78, "IBM278",                                        20278,  no }, /* IANA */
  {  78, "CP278",                                         20278,  no }, /* IANA */
  {  78, "ebcdic-cp-fi",                                  20278,  no }, /* IANA */
  {  78, "ebcdic-cp-se",                                  20278,  no }, /* IANA */
  {  78, "csIBM278",                                      20278,  no }, /* IANA */
  {  78, "x-ebcdic-finlandsweden",                        20278,  no }, /* MICROSOFT */
  {  79, "IBM280",                                        20280,  no }, /* IANA */
  {  79, "CP280",                                         20280,  no }, /* IANA */
  {  79, "ebcdic-cp-it",                                  20280,  no }, /* IANA */
  {  79, "csIBM280",                                      20280,  no }, /* IANA */
  {  79, "x-ebcdic-italy",                                20280,  no }, /* MICROSOFT */
  {  80, "IBM281",                                            0,  no }, /* IANA */
  {  80, "EBCDIC-JP-E",                                       0,  no }, /* IANA */
  {  80, "cp281",                                             0,  no }, /* IANA */
  {  80, "csIBM281",                                          0,  no }, /* IANA */
  {  81, "IBM284",                                        20284,  no }, /* IANA */
  {  81, "CP284",                                         20284,  no }, /* IANA */
  {  81, "ebcdic-cp-es",                                  20284,  no }, /* IANA */
  {  81, "csIBM284",                                      20284,  no }, /* IANA */
  {  81, "x-ebcdic-spain",                                20284,  no }, /* MICROSOFT */
  {  82, "IBM285",                                        20285,  no }, /* IANA */
  {  82, "CP285",                                         20285,  no }, /* IANA */
  {  82, "ebcdic-cp-gb",                                  20285,  no }, /* IANA */
  {  82, "csIBM285",                                      20285,  no }, /* IANA */
  {  82, "x-ebcdic-uk",                                   20285,  no }, /* MICROSOFT */
  {  83, "IBM290",                                        20290,  no }, /* IANA */
  {  83, "cp290",                                         20290,  no }, /* IANA */
  {  83, "EBCDIC-JP-kana",                                20290,  no }, /* IANA */
  {  83, "csIBM290",                                      20290,  no }, /* IANA */
  {  83, "x-ebcdic-japanesekatakana",                     20290,  no }, /* MICROSOFT */
  {  84, "IBM297",                                        20297,  no }, /* IANA */
  {  84, "cp297",                                         20297,  no }, /* IANA */
  {  84, "ebcdic-cp-fr",                                  20297,  no }, /* IANA */
  {  84, "csIBM297",                                      20297,  no }, /* IANA */
  {  85, "IBM420",                                        20420,  no }, /* IANA */
  {  85, "cp420",                                         20420,  no }, /* IANA */
  {  85, "ebcdic-cp-ar1",                                 20420,  no }, /* IANA */
  {  85, "csIBM420",                                      20420,  no }, /* IANA */
  {  85, "x-ebcdic-arabic",                               20420,  no }, /* MICROSOFT */
  {  86, "IBM423",                                        20423,  no }, /* IANA */
  {  86, "cp423",                                         20423,  no }, /* IANA */
  {  86, "ebcdic-cp-gr",                                  20423,  no }, /* IANA */
  {  86, "csIBM423",                                      20423,  no }, /* IANA */
  {  86, "x-ebcdic-greek",                                20423,  no }, /* MICROSOFT */
  {  87, "IBM424",                                        20424,  no }, /* IANA */
  {  87, "cp424",                                         20424,  no }, /* IANA */
  {  87, "ebcdic-cp-he",                                  20424,  no }, /* IANA */
  {  87, "csIBM424",                                      20424,  no }, /* IANA */
  {  87, "x-ebcdic-hebrew",                               20424,  no }, /* MICROSOFT */
  {  88, "IBM437",                                          437,  no }, /* IANA */
  {  88, "cp437",                                           437,  no }, /* IANA */
  {  88, "437",                                             437,  no }, /* IANA */
  {  88, "csPC8CodePage437",                                437,  no }, /* IANA */
  {  88, "codepage437",                                     437,  no }, /* MICROSOFT */
  {  88, "cspc8",                                           437,  no }, /* MICROSOFT */
  {  89, "IBM500",                                          500,  no }, /* IANA */
  {  89, "CP500",                                           500,  no }, /* IANA */
  {  89, "ebcdic-cp-be",                                    500,  no }, /* IANA */
  {  89, "ebcdic-cp-ch",                                    500,  no }, /* IANA */
  {  89, "csIBM500",                                        500,  no }, /* IANA */
  {  90, "IBM775",                                          775,  no }, /* IANA */
  {  90, "cp775",                                           775,  no }, /* IANA */
  {  90, "csPC775Baltic",                                   775,  no }, /* IANA */
  {  91, "IBM850",                                          850,  no }, /* IANA */
  {  91, "cp850",                                           850,  no }, /* IANA */
  {  91, "850",                                             850,  no }, /* IANA */
  {  91, "csPC850Multilingual",                             850,  no }, /* IANA */
  {  92, "IBM851",                                            0,  no }, /* IANA */
  {  92, "cp851",                                             0,  no }, /* IANA */
  {  92, "851",                                               0,  no }, /* IANA */
  {  92, "csIBM851",                                          0,  no }, /* IANA */
  {  93, "IBM852",                                          852,  no }, /* IANA */
  {  93, "cp852",                                           852,  no }, /* IANA */
  {  93, "852",                                             852,  no }, /* IANA */
  {  93, "csPCp852",                                        852,  no }, /* IANA */
  {  94, "IBM855",                                          855,  no }, /* IANA */
  {  94, "cp855",                                           855,  no }, /* IANA */
  {  94, "855",                                             855,  no }, /* IANA */
  {  94, "csIBM855",                                        855,  no }, /* IANA */
  {  95, "IBM857",                                          857,  no }, /* IANA */
  {  95, "cp857",                                           857,  no }, /* IANA */
  {  95, "857",                                             857,  no }, /* IANA */
  {  95, "csIBM857",                                        857,  no }, /* IANA */
  {  96, "IBM860",                                          860,  no }, /* IANA */
  {  96, "cp860",                                           860,  no }, /* IANA */
  {  96, "860",                                             860,  no }, /* IANA */
  {  96, "csIBM860",                                        860,  no }, /* IANA */
  {  97, "IBM861",                                          861,  no }, /* IANA */
  {  97, "cp861",                                           861,  no }, /* IANA */
  {  97, "861",                                             861,  no }, /* IANA */
  {  97, "cp-is",                                           861,  no }, /* IANA */
  {  97, "csIBM861",                                        861,  no }, /* IANA */
  {  98, "IBM862",                                          862,  no }, /* IANA */
  {  98, "cp862",                                           862,  no }, /* IANA */
  {  98, "862",                                             862,  no }, /* IANA */
  {  98, "csPC862LatinHebrew",                              862,  no }, /* IANA */
  {  98, "dos-862",                                         862,  no }, /* MICROSOFT */
  {  99, "IBM863",                                          863,  no }, /* IANA */
  {  99, "cp863",                                           863,  no }, /* IANA */
  {  99, "863",                                             863,  no }, /* IANA */
  {  99, "csIBM863",                                        863,  no }, /* IANA */
  { 100, "IBM864",                                          864,  no }, /* IANA */
  { 100, "cp864",                                           864,  no }, /* IANA */
  { 100, "csIBM864",                                        864,  no }, /* IANA */
  { 101, "IBM865",                                          865,  no }, /* IANA */
  { 101, "cp865",                                           865,  no }, /* IANA */
  { 101, "865",                                             865,  no }, /* IANA */
  { 101, "csIBM865",                                        865,  no }, /* IANA */
  { 102, "IBM866",                                          866,  no }, /* IANA */
  { 102, "cp866",                                           866,  no }, /* IANA */
  { 102, "866",                                             866,  no }, /* IANA */
  { 102, "csIBM866",                                        866,  no }, /* IANA */
  { 103, "IBM868",                                            0,  no }, /* IANA */
  { 103, "CP868",                                             0,  no }, /* IANA */
  { 103, "cp-ar",                                             0,  no }, /* IANA */
  { 103, "csIBM868",                                          0,  no }, /* IANA */
  { 104, "IBM869",                                          869,  no }, /* IANA */
  { 104, "cp869",                                           869,  no }, /* IANA */
  { 104, "869",                                             869,  no }, /* IANA */
  { 104, "cp-gr",                                           869,  no }, /* IANA */
  { 104, "csIBM869",                                        869,  no }, /* IANA */
  { 105, "IBM870",                                          870,  no }, /* IANA */
  { 105, "CP870",                                           870,  no }, /* IANA */
  { 105, "ebcdic-cp-roece",                                 870,  no }, /* IANA */
  { 105, "ebcdic-cp-yu",                                    870,  no }, /* IANA */
  { 105, "csIBM870",                                        870,  no }, /* IANA */
  { 106, "IBM871",                                        20871,  no }, /* IANA */
  { 106, "CP871",                                         20871,  no }, /* IANA */
  { 106, "ebcdic-cp-is",                                  20871,  no }, /* IANA */
  { 106, "csIBM871",                                      20871,  no }, /* IANA */
  { 106, "x-ebcdic-icelandic",                            20871,  no }, /* MICROSOFT */
  { 107, "IBM880",                                        20880,  no }, /* IANA */
  { 107, "cp880",                                         20880,  no }, /* IANA */
  { 107, "EBCDIC-Cyrillic",                               20880,  no }, /* IANA */
  { 107, "csIBM880",                                      20880,  no }, /* IANA */
  { 107, "x-ebcdic-cyrillicrussian",                      20880,  no }, /* MICROSOFT */
  { 108, "IBM891",                                            0,  no }, /* IANA */
  { 108, "cp891",                                             0,  no }, /* IANA */
  { 108, "csIBM891",                                          0,  no }, /* IANA */
  { 109, "IBM903",                                            0,  no }, /* IANA */
  { 109, "cp903",                                             0,  no }, /* IANA */
  { 109, "csIBM903",                                          0,  no }, /* IANA */
  { 110, "IBM904",                                            0,  no }, /* IANA */
  { 110, "cp904",                                             0,  no }, /* IANA */
  { 110, "904",                                               0,  no }, /* IANA */
  { 110, "csIBBM904",                                         0,  no }, /* IANA */
  { 111, "IBM905",                                        20905,  no }, /* IANA */
  { 111, "CP905",                                         20905,  no }, /* IANA */
  { 111, "ebcdic-cp-tr",                                  20905,  no }, /* IANA */
  { 111, "csIBM905",                                      20905,  no }, /* IANA */
  { 111, "x-ebcdic-turkish",                              20905,  no }, /* MICROSOFT */
  { 112, "IBM918",                                            0,  no }, /* IANA */
  { 112, "CP918",                                             0,  no }, /* IANA */
  { 112, "ebcdic-cp-ar2",                                     0,  no }, /* IANA */
  { 112, "csIBM918",                                          0,  no }, /* IANA */
  { 113, "iso-ir-143",                                        0,  no }, /* IANA */
  { 113, "IEC_P27-1",                                         0,  no }, /* IANA */
  { 113, "csISO143IECP271",                                   0,  no }, /* IANA */
  { 114, "iso-ir-49",                                         0,  no }, /* IANA */
  { 114, "INIS",                                              0,  no }, /* IANA */
  { 114, "csISO49INIS",                                       0,  no }, /* IANA */
  { 115, "iso-ir-50",                                         0,  no }, /* IANA */
  { 115, "INIS-8",                                            0,  no }, /* IANA */
  { 115, "csISO50INIS8",                                      0,  no }, /* IANA */
  { 116, "iso-ir-51",                                         0,  no }, /* IANA */
  { 116, "INIS-cyrillic",                                     0,  no }, /* IANA */
  { 116, "csISO51INISCyrillic",                               0,  no }, /* IANA */
  { 117, "INVARIANT",                                         0,  no }, /* IANA */
  { 117, "csINVARIANT",                                       0,  no }, /* IANA */
  { 118, "ISO-10646-J-1",                                     0,  no }, /* IANA */
  { 119, "ISO-10646-UCS-2",                                1200,  no }, /* IANA */
  { 119, "csUnicode",                                      1200,  no }, /* IANA */
  { 119, "Unicode",                                        1200,  no }, /* MICROSOFT */
  { 120, "ISO-10646-UCS-4",                                   0,  no }, /* IANA */
  { 120, "csUCS4",                                            0,  no }, /* IANA */
  { 121, "ISO-10646-UCS-Basic",                               0,  no }, /* IANA */
  { 121, "csUnicodeASCII",                                    0,  no }, /* IANA */
  { 122, "ISO-10646-Unicode-Latin1",                          0,  no }, /* IANA */
  { 122, "ISO-10646",                                         0,  no }, /* IANA */
  { 122, "csUnicodeLatin1",                                   0,  no }, /* IANA */
  { 123, "ISO-10646-UTF-1",                                   0,  no }, /* IANA */
  { 123, "csISO10646UTF1",                                    0,  no }, /* IANA */
  { 124, "ISO-2022-CN",                                       0, yes }, /* IANA */
  { 125, "ISO-2022-CN-EXT",                                   0, yes }, /* IANA */
  { 126, "ISO-2022-JP",                                   50220, yes }, /* IANA */
  { 126, "csISO2022JP",                                   50220, yes }, /* IANA */
  { 127, "ISO-2022-JP-2",                                 50221, yes }, /* IANA */
  { 127, "csISO2022JP2",                                  50221, yes }, /* IANA */
  { 128, "ISO-2022-KR",                                   50225, yes }, /* IANA */
  { 128, "csISO2022KR",                                   50225, yes }, /* IANA */
  { 129, "ISO-8859-1-Windows-3.0-Latin-1",                    0,  no }, /* IANA */
  { 129, "csWindows30Latin1",                                 0,  no }, /* IANA */
  { 130, "ISO-8859-1-Windows-3.1-Latin-1",                    0,  no }, /* IANA */
  { 130, "csWindows31Latin1",                                 0,  no }, /* IANA */
  { 131, "ISO-8859-10",                                       0,  no }, /* IANA */
  { 131, "iso-ir-157",                                        0,  no }, /* IANA */
  { 131, "l6",                                                0,  no }, /* IANA */
  { 131, "ISO_8859-10:1992",                                  0,  no }, /* IANA */
  { 131, "csISOLatin6",                                       0,  no }, /* IANA */
  { 131, "latin6",                                            0,  no }, /* IANA */
  { 132, "ISO-8859-13",                                   28603,  no }, /* IANA */
  { 133, "ISO-8859-14",                                       0,  no }, /* IANA */
  { 133, "iso-ir-199",                                        0,  no }, /* IANA */
  { 133, "iso-celtic",                                        0,  no }, /* IANA */
  { 133, "ISO_8859-14:1998",                                  0,  no }, /* IANA */
  { 133, "ISO_8859-14",                                       0,  no }, /* IANA */
  { 133, "latin8",                                            0,  no }, /* IANA */
  { 133, "l8",                                                0,  no }, /* IANA */
  { 134, "ISO-8859-15",                                   28605,  no }, /* IANA */
  { 134, "ISO_8859-15",                                   28605,  no }, /* IANA */
  { 134, "Latin-9",                                       28605,  no }, /* IANA */
  { 134, "l9",                                            28605,  no }, /* MICROSOFT */
  { 134, "latin9",                                        28605,  no }, /* MICROSOFT */
  { 135, "ISO-8859-16",                                       0,  no }, /* IANA */
  { 135, "iso-ir-226",                                        0,  no }, /* IANA */
  { 135, "ISO_8859-16:2001",                                  0,  no }, /* IANA */
  { 135, "ISO_8859-16",                                       0,  no }, /* IANA */
  { 135, "latin10",                                           0,  no }, /* IANA */
  { 135, "l10",                                               0,  no }, /* IANA */
  { 136, "ISO-8859-2-Windows-Latin-2",                        0,  no }, /* IANA */
  { 136, "csWindows31Latin2",                                 0,  no }, /* IANA */
  { 137, "ISO-8859-9-Windows-Latin-5",                        0,  no }, /* IANA */
  { 137, "csWindows31Latin5",                                 0,  no }, /* IANA */
  { 138, "iso-ir-90",                                         0,  no }, /* IANA */
  { 138, "csISO90",                                           0,  no }, /* IANA */
  { 139, "ISO-Unicode-IBM-1261",                              0,  no }, /* IANA */
  { 139, "csUnicodeIBM1261",                                  0,  no }, /* IANA */
  { 140, "ISO-Unicode-IBM-1264",                              0,  no }, /* IANA */
  { 140, "csUnicodeIBM1264",                                  0,  no }, /* IANA */
  { 141, "ISO-Unicode-IBM-1265",                              0,  no }, /* IANA */
  { 141, "csUnicodeIBM1265",                                  0,  no }, /* IANA */
  { 142, "ISO-Unicode-IBM-1268",                              0,  no }, /* IANA */
  { 142, "csUnicodeIBM1268",                                  0,  no }, /* IANA */
  { 143, "ISO-Unicode-IBM-1276",                              0,  no }, /* IANA */
  { 143, "csUnicodeIBM1276",                                  0,  no }, /* IANA */
  { 144, "iso-ir-155",                                        0,  no }, /* IANA */
  { 144, "ISO_10367-box",                                     0,  no }, /* IANA */
  { 144, "csISO10367Box",                                     0,  no }, /* IANA */
  { 145, "iso-ir-98",                                         0,  no }, /* IANA */
  { 145, "ISO_2033-1983",                                     0,  no }, /* IANA */
  { 145, "e13b",                                              0,  no }, /* IANA */
  { 145, "csISO2033",                                         0,  no }, /* IANA */
  { 146, "iso-ir-37",                                         0,  no }, /* IANA */
  { 146, "ISO_5427",                                          0,  no }, /* IANA */
  { 146, "csISO5427Cyrillic",                                 0,  no }, /* IANA */
  { 147, "iso-ir-54",                                         0,  no }, /* IANA */
  { 147, "ISO_5427:1981",                                     0,  no }, /* IANA */
  { 147, "ISO5427Cyrillic1981",                               0,  no }, /* IANA */
  { 148, "iso-ir-55",                                         0,  no }, /* IANA */
  { 148, "ISO_5428:1980",                                     0,  no }, /* IANA */
  { 148, "csISO5428Greek",                                    0,  no }, /* IANA */
  { 149, "ISO_646.basic:1983",                                0,  no }, /* IANA */
  { 149, "ref",                                               0,  no }, /* IANA */
  { 149, "csISO646basic1983",                                 0,  no }, /* IANA */
  { 150, "iso-ir-2",                                      20105,  no }, /* IANA */
  { 150, "ISO_646.irv:1983",                              20105,  no }, /* IANA */
  { 150, "irv",                                           20105,  no }, /* IANA */
  { 150, "csISO2IntlRefVersion",                          20105,  no }, /* IANA */
  { 150, "x-ia5",                                         20105,  no }, /* MICROSOFT */
  { 151, "iso-ir-152",                                        0,  no }, /* IANA */
  { 151, "ISO_6937-2-25",                                     0,  no }, /* IANA */
  { 151, "csISO6937Add",                                      0,  no }, /* IANA */
  { 152, "iso-ir-142",                                        0,  no }, /* IANA */
  { 152, "ISO_6937-2-add",                                    0,  no }, /* IANA */
  { 152, "csISOTextComm",                                     0,  no }, /* IANA */
  { 153, "ISO-8859-1",                                    28591,  no }, /* IANA */
  { 153, "iso-ir-100",                                    28591,  no }, /* IANA */
  { 153, "ISO_8859-1:1987",                               28591,  no }, /* IANA */
  { 153, "ISO_8859-1",                                    28591,  no }, /* IANA */
  { 153, "latin1",                                        28591,  no }, /* IANA */
  { 153, "l1",                                            28591,  no }, /* IANA */
  { 153, "IBM819",                                        28591,  no }, /* IANA */
  { 153, "CP819",                                         28591,  no }, /* IANA */
  { 153, "csISOLatin1",                                   28591,  no }, /* IANA */
  { 153, "iso8859-1",                                     28591,  no }, /* MICROSOFT */
  { 154, "ISO-8859-2",                                    28592,  no }, /* IANA */
  { 154, "iso-ir-101",                                    28592,  no }, /* IANA */
  { 154, "ISO_8859-2:1987",                               28592,  no }, /* IANA */
  { 154, "ISO_8859-2",                                    28592,  no }, /* IANA */
  { 154, "latin2",                                        28592,  no }, /* IANA */
  { 154, "l2",                                            28592,  no }, /* IANA */
  { 154, "csISOLatin2",                                   28592,  no }, /* IANA */
  { 154, "iso8859-2",                                     28592,  no }, /* MICROSOFT */
  { 155, "ISO-8859-3",                                    28593,  no }, /* IANA */
  { 155, "iso-ir-109",                                    28593,  no }, /* IANA */
  { 155, "ISO_8859-3:1988",                               28593,  no }, /* IANA */
  { 155, "ISO_8859-3",                                    28593,  no }, /* IANA */
  { 155, "latin3",                                        28593,  no }, /* IANA */
  { 155, "l3",                                            28593,  no }, /* IANA */
  { 155, "csISOLatin3",                                   28593,  no }, /* IANA */
  { 156, "ISO-8859-4",                                    28594,  no }, /* IANA */
  { 156, "iso-ir-110",                                    28594,  no }, /* IANA */
  { 156, "ISO_8859-4:1988",                               28594,  no }, /* IANA */
  { 156, "ISO_8859-4",                                    28594,  no }, /* IANA */
  { 156, "latin4",                                        28594,  no }, /* IANA */
  { 156, "l4",                                            28594,  no }, /* IANA */
  { 156, "csISOLatin4",                                   28594,  no }, /* IANA */
  { 157, "ISO-8859-5",                                    28595,  no }, /* IANA */
  { 157, "iso-ir-144",                                    28595,  no }, /* IANA */
  { 157, "ISO_8859-5:1988",                               28595,  no }, /* IANA */
  { 157, "ISO_8859-5",                                    28595,  no }, /* IANA */
  { 157, "cyrillic",                                      28595,  no }, /* IANA */
  { 157, "csISOLatinCyrillic",                            28595,  no }, /* IANA */
  { 158, "ISO-8859-6-E",                                      0,  no }, /* IANA */
  { 158, "ISO_8859-6-E",                                      0,  no }, /* IANA */
  { 158, "csISO88596E",                                       0,  no }, /* IANA */
  { 159, "ISO-8859-6-I",                                      0,  no }, /* IANA */
  { 159, "ISO_8859-6-I",                                      0,  no }, /* IANA */
  { 159, "csISO88596I",                                       0,  no }, /* IANA */
  { 160, "ISO-8859-6",                                    28596,  no }, /* IANA */
  { 160, "iso-ir-127",                                    28596,  no }, /* IANA */
  { 160, "ISO_8859-6:1987",                               28596,  no }, /* IANA */
  { 160, "ISO_8859-6",                                    28596,  no }, /* IANA */
  { 160, "ECMA-114",                                      28596,  no }, /* IANA */
  { 160, "arabic",                                        28596,  no }, /* IANA */
  { 160, "csISOLatinArabic",                              28596,  no }, /* IANA */
  { 161, "ISO-8859-7",                                    28597,  no }, /* IANA */
  { 161, "iso-ir-126",                                    28597,  no }, /* IANA */
  { 161, "ISO_8859-7:1987",                               28597,  no }, /* IANA */
  { 161, "ISO_8859-7",                                    28597,  no }, /* IANA */
  { 161, "ELOT_928",                                      28597,  no }, /* IANA */
  { 161, "ECMA-118",                                      28597,  no }, /* IANA */
  { 161, "greek",                                         28597,  no }, /* IANA */
  { 161, "greek8",                                        28597,  no }, /* IANA */
  { 161, "csISOLatinGreek",                               28597,  no }, /* IANA */
  { 162, "ISO-8859-8-E",                                      0,  no }, /* IANA */
  { 162, "ISO_8859-8-E",                                      0,  no }, /* IANA */
  { 162, "csISO88598E",                                       0,  no }, /* IANA */
  { 163, "ISO-8859-8-I",                                  38598,  no }, /* IANA */
  { 163, "ISO_8859-8-I",                                  38598,  no }, /* IANA */
  { 163, "csISO88598I",                                   38598,  no }, /* IANA */
  { 164, "ISO-8859-8",                                    28598,  no }, /* IANA */
  { 164, "iso-ir-138",                                    28598,  no }, /* IANA */
  { 164, "ISO_8859-8:1988",                               28598,  no }, /* IANA */
  { 164, "ISO_8859-8",                                    28598,  no }, /* IANA */
  { 164, "hebrew",                                        28598,  no }, /* IANA */
  { 164, "csISOLatinHebrew",                              28598,  no }, /* IANA */
  { 164, "logical",                                       28598,  no }, /* MICROSOFT */
  { 164, "visual",                                        28598,  no }, /* MICROSOFT */
  { 165, "ISO-8859-9",                                    28599,  no }, /* IANA */
  { 165, "iso-ir-148",                                    28599,  no }, /* IANA */
  { 165, "ISO_8859-9:1989",                               28599,  no }, /* IANA */
  { 165, "ISO_8859-9",                                    28599,  no }, /* IANA */
  { 165, "latin5",                                        28599,  no }, /* IANA */
  { 165, "l5",                                            28599,  no }, /* IANA */
  { 165, "csISOLatin5",                                   28599,  no }, /* IANA */
  { 166, "iso-ir-154",                                        0,  no }, /* IANA */
  { 166, "ISO_8859-supp",                                     0,  no }, /* IANA */
  { 166, "latin1-2-5",                                        0,  no }, /* IANA */
  { 166, "csISO8859Supp",                                     0,  no }, /* IANA */
  { 167, "iso-ir-15",                                         0,  no }, /* IANA */
  { 167, "IT",                                                0,  no }, /* IANA */
  { 167, "ISO646-IT",                                         0,  no }, /* IANA */
  { 167, "csISO15Italian",                                    0,  no }, /* IANA */
  { 168, "iso-ir-13",                                         0,  no }, /* IANA */
  { 168, "JIS_C6220-1969-jp",                                 0,  no }, /* IANA */
  { 168, "JIS_C6220-1969",                                    0,  no }, /* IANA */
  { 168, "katakana",                                          0,  no }, /* IANA */
  { 168, "x0201-7",                                           0,  no }, /* IANA */
  { 168, "csISO13JISC6220jp",                                 0,  no }, /* IANA */
  { 169, "iso-ir-14",                                         0,  no }, /* IANA */
  { 169, "JIS_C6220-1969-ro",                                 0,  no }, /* IANA */
  { 169, "jp",                                                0,  no }, /* IANA */
  { 169, "ISO646-JP",                                         0,  no }, /* IANA */
  { 169, "csISO14JISC6220ro",                                 0,  no }, /* IANA */
  { 170, "iso-ir-42",                                         0,  no }, /* IANA */
  { 170, "JIS_C6226-1978",                                    0,  no }, /* IANA */
  { 170, "csISO42JISC62261978",                               0,  no }, /* IANA */
  { 171, "iso-ir-87",                                         0,  no }, /* IANA */
  { 171, "JIS_C6226-1983",                                    0,  no }, /* IANA */
  { 171, "x0208",                                             0,  no }, /* IANA */
  { 171, "JIS_X0208-1983",                                    0,  no }, /* IANA */
  { 171, "csISO87JISX0208",                                   0,  no }, /* IANA */
  { 172, "iso-ir-91",                                         0,  no }, /* IANA */
  { 172, "JIS_C6229-1984-a",                                  0,  no }, /* IANA */
  { 172, "jp-ocr-a",                                          0,  no }, /* IANA */
  { 172, "csISO91JISC62291984a",                              0,  no }, /* IANA */
  { 173, "iso-ir-92",                                         0,  no }, /* IANA */
  { 173, "JIS_C6229-1984-b",                                  0,  no }, /* IANA */
  { 173, "ISO646-JP-OCR-B",                                   0,  no }, /* IANA */
  { 173, "jp-ocr-b",                                          0,  no }, /* IANA */
  { 173, "csISO92JISC62991984b",                              0,  no }, /* IANA */
  { 174, "iso-ir-93",                                         0,  no }, /* IANA */
  { 174, "JIS_C6229-1984-b-add",                              0,  no }, /* IANA */
  { 174, "jp-ocr-b-add",                                      0,  no }, /* IANA */
  { 174, "csISO93JIS62291984badd",                            0,  no }, /* IANA */
  { 175, "iso-ir-94",                                         0,  no }, /* IANA */
  { 175, "JIS_C6229-1984-hand",                               0,  no }, /* IANA */
  { 175, "jp-ocr-hand",                                       0,  no }, /* IANA */
  { 175, "csISO94JIS62291984hand",                            0,  no }, /* IANA */
  { 176, "iso-ir-95",                                         0,  no }, /* IANA */
  { 176, "JIS_C6229-1984-hand-add",                           0,  no }, /* IANA */
  { 176, "jp-ocr-hand-add",                                   0,  no }, /* IANA */
  { 176, "csISO95JIS62291984handadd",                         0,  no }, /* IANA */
  { 177, "iso-ir-96",                                         0,  no }, /* IANA */
  { 177, "JIS_C6229-1984-kana",                               0,  no }, /* IANA */
  { 177, "csISO96JISC62291984kana",                           0,  no }, /* IANA */
  { 178, "JIS_Encoding",                                      0,  no }, /* IANA */
  { 178, "csJISEncoding",                                     0,  no }, /* IANA */
  { 179, "JIS_X0201",                                         0,  no }, /* IANA */
  { 179, "X0201",                                             0,  no }, /* IANA */
  { 179, "csHalfWidthKatakana",                               0,  no }, /* IANA */
  { 180, "iso-ir-159",                                        0,  no }, /* IANA */
  { 180, "JIS_X0212-1990",                                    0,  no }, /* IANA */
  { 180, "x0212",                                             0,  no }, /* IANA */
  { 180, "csISO159JISX02121990",                              0,  no }, /* IANA */
  { 181, "iso-ir-141",                                        0,  no }, /* IANA */
  { 181, "JUS_I.B1.002",                                      0,  no }, /* IANA */
  { 181, "ISO646-YU",                                         0,  no }, /* IANA */
  { 181, "js",                                                0,  no }, /* IANA */
  { 181, "yu",                                                0,  no }, /* IANA */
  { 181, "csISO141JUSIB1002",                                 0,  no }, /* IANA */
  { 182, "iso-ir-147",                                        0,  no }, /* IANA */
  { 182, "JUS_I.B1.003-mac",                                  0,  no }, /* IANA */
  { 182, "macedonian",                                        0,  no }, /* IANA */
  { 182, "csISO147Macedonian",                                0,  no }, /* IANA */
  { 183, "iso-ir-146",                                        0,  no }, /* IANA */
  { 183, "JUS_I.B1.003-serb",                                 0,  no }, /* IANA */
  { 183, "serbian",                                           0,  no }, /* IANA */
  { 183, "csISO146Serbian",                                   0,  no }, /* IANA */
  { 184, "KOI8-R",                                        20866,  no }, /* IANA */
  { 184, "csKOI8R",                                       20866,  no }, /* IANA */
  { 184, "koi8",                                          20866,  no }, /* MICROSOFT */
  { 184, "koi",                                           20866,  no }, /* MICROSOFT */
  { 184, "koi8r",                                         20866,  no }, /* MICROSOFT */
  { 185, "KOI8-U",                                        21866,  no }, /* IANA */
  { 185, "koi8-ru",                                       21866,  no }, /* MICROSOFT */
  { 186, "iso-ir-149",                                      949,  no }, /* IANA */
  { 186, "KS_C_5601-1987",                                  949,  no }, /* IANA */
  { 186, "KS_C_5601-1989",                                  949,  no }, /* IANA */
  { 186, "KSC_5601",                                        949,  no }, /* IANA */
  { 186, "korean",                                          949,  no }, /* IANA */
  { 186, "csKSC56011987",                                   949,  no }, /* IANA */
  { 186, "ks_c_5601_1987",                                  949,  no }, /* MICROSOFT */
  { 186, "ksc5601",                                         949,  no }, /* MICROSOFT */
  { 186, "ks_c_5601",                                       949,  no }, /* MICROSOFT */
  { 187, "KSC5636",                                           0,  no }, /* IANA */
  { 187, "ISO646-KR",                                         0,  no }, /* IANA */
  { 187, "csKSC5636",                                         0,  no }, /* IANA */
  { 188, "iso-ir-19",                                         0,  no }, /* IANA */
  { 188, "latin-greek",                                       0,  no }, /* IANA */
  { 188, "csISO19LatinGreek",                                 0,  no }, /* IANA */
  { 189, "iso-ir-27",                                         0,  no }, /* IANA */
  { 189, "Latin-greek-1",                                     0,  no }, /* IANA */
  { 189, "csISO27LatinGreek1",                                0,  no }, /* IANA */
  { 190, "iso-ir-158",                                        0,  no }, /* IANA */
  { 190, "latin-lap",                                         0,  no }, /* IANA */
  { 190, "lap",                                               0,  no }, /* IANA */
  { 190, "csISO158Lap",                                       0,  no }, /* IANA */
  { 191, "macintosh",                                     10000,  no }, /* IANA */
  { 191, "mac",                                           10000,  no }, /* IANA */
  { 191, "csMacintosh",                                   10000,  no }, /* IANA */
  { 192, "Microsoft-Publishing",                              0,  no }, /* IANA */
  { 192, "csMicrosoftPublishing",                             0,  no }, /* IANA */
  { 193, "MNEM",                                              0,  no }, /* IANA */
  { 193, "csMnem",                                            0,  no }, /* IANA */
  { 194, "MNEMONIC",                                          0,  no }, /* IANA */
  { 194, "csMnemonic",                                        0,  no }, /* IANA */
  { 195, "iso-ir-86",                                         0,  no }, /* IANA */
  { 195, "MSZ_7795.3",                                        0,  no }, /* IANA */
  { 195, "ISO646-HU",                                         0,  no }, /* IANA */
  { 195, "hu",                                                0,  no }, /* IANA */
  { 195, "csISO86Hungarian",                                  0,  no }, /* IANA */
  { 196, "iso-ir-9-1",                                        0,  no }, /* IANA */
  { 196, "NATS-DANO",                                         0,  no }, /* IANA */
  { 196, "csNATSDANO",                                        0,  no }, /* IANA */
  { 197, "iso-ir-9-2",                                        0,  no }, /* IANA */
  { 197, "NATS-DANO-ADD",                                     0,  no }, /* IANA */
  { 197, "csNATSDANOADD",                                     0,  no }, /* IANA */
  { 198, "iso-ir-8-1",                                        0,  no }, /* IANA */
  { 198, "NATS-SEFI",                                         0,  no }, /* IANA */
  { 198, "csNATSSEFI",                                        0,  no }, /* IANA */
  { 199, "iso-ir-8-2",                                        0,  no }, /* IANA */
  { 199, "NATS-SEFI-ADD",                                     0,  no }, /* IANA */
  { 199, "csNATSSEFIADD",                                     0,  no }, /* IANA */
  { 200, "iso-ir-151",                                        0,  no }, /* IANA */
  { 200, "NC_NC00-10:81",                                     0,  no }, /* IANA */
  { 200, "cuba",                                              0,  no }, /* IANA */
  { 200, "ISO646-CU",                                         0,  no }, /* IANA */
  { 200, "csISO151Cuba",                                      0,  no }, /* IANA */
  { 201, "iso-ir-69",                                         0,  no }, /* IANA */
  { 201, "NF_Z_62-010",                                       0,  no }, /* IANA */
  { 201, "ISO646-FR",                                         0,  no }, /* IANA */
  { 201, "fr",                                                0,  no }, /* IANA */
  { 201, "csISO69French",                                     0,  no }, /* IANA */
  { 202, "iso-ir-25",                                         0,  no }, /* IANA */
  { 202, "NF_Z_62-010_(1973)",                                0,  no }, /* IANA */
  { 202, "ISO646-FR1",                                        0,  no }, /* IANA */
  { 202, "csISO25French",                                     0,  no }, /* IANA */
  { 203, "iso-ir-60",                                     20108,  no }, /* IANA */
  { 203, "NS_4551-1",                                     20108,  no }, /* IANA */
  { 203, "ISO646-NO",                                     20108,  no }, /* IANA */
  { 203, "no",                                            20108,  no }, /* IANA */
  { 203, "csISO60DanishNorwegian",                        20108,  no }, /* IANA */
  { 203, "csISO60Norwegian1",                             20108,  no }, /* IANA */
  { 203, "x-ia5-norwegian",                               20108,  no }, /* MICROSOFT */
  { 204, "iso-ir-61",                                         0,  no }, /* IANA */
  { 204, "NS_4551-2",                                         0,  no }, /* IANA */
  { 204, "ISO646-NO2",                                        0,  no }, /* IANA */
  { 204, "no2",                                               0,  no }, /* IANA */
  { 204, "csISO61Norwegian2",                                 0,  no }, /* IANA */
  { 205, "PC8-Danish-Norwegian",                              0,  no }, /* IANA */
  { 205, "csPC8DanishNorwegian",                              0,  no }, /* IANA */
  { 206, "PC8-Turkish",                                       0,  no }, /* IANA */
  { 206, "csPC8Turkish",                                      0,  no }, /* IANA */
  { 207, "iso-ir-16",                                         0,  no }, /* IANA */
  { 207, "PT",                                                0,  no }, /* IANA */
  { 207, "ISO646-PT",                                         0,  no }, /* IANA */
  { 207, "csISO16Portuguese",                                 0,  no }, /* IANA */
  { 208, "iso-ir-84",                                         0,  no }, /* IANA */
  { 208, "PT2",                                               0,  no }, /* IANA */
  { 208, "ISO646-PT2",                                        0,  no }, /* IANA */
  { 208, "csISO84Portuguese2",                                0,  no }, /* IANA */
  { 209, "PTCP154",                                           0,  no }, /* IANA */
  { 209, "csPTCP154",                                         0,  no }, /* IANA */
  { 209, "PT154",                                             0,  no }, /* IANA */
  { 209, "CP154",                                             0,  no }, /* IANA */
  { 209, "Cyrillic-Asian",                                    0,  no }, /* IANA */
  { 210, "SCSU",                                              0,  no }, /* IANA */
  { 211, "iso-ir-10",                                     20107,  no }, /* IANA */
  { 211, "SEN_850200_B",                                  20107,  no }, /* IANA */
  { 211, "FI",                                            20107,  no }, /* IANA */
  { 211, "ISO646-FI",                                     20107,  no }, /* IANA */
  { 211, "ISO646-SE",                                     20107,  no }, /* IANA */
  { 211, "se",                                            20107,  no }, /* IANA */
  { 211, "csISO10Swedish",                                20107,  no }, /* IANA */
  { 211, "x-ia5-swedish",                                 20107,  no }, /* MICROSOFT */
  { 212, "iso-ir-11",                                         0,  no }, /* IANA */
  { 212, "SEN_850200_C",                                      0,  no }, /* IANA */
  { 212, "ISO646-SE2",                                        0,  no }, /* IANA */
  { 212, "se2",                                               0,  no }, /* IANA */
  { 212, "csISO11SwedishForNames",                            0,  no }, /* IANA */
  { 213, "Shift_JIS",                                       932,  no }, /* IANA */
  { 213, "MS_Kanji",                                        932,  no }, /* IANA */
  { 213, "csShiftJIS",                                      932,  no }, /* IANA */
  { 213, "shift-jis",                                       932,  no }, /* MICROSOFT */
  { 213, "x-ms-cp932",                                      932,  no }, /* MICROSOFT */
  { 213, "x-sjis",                                          932,  no }, /* MICROSOFT */
  { 213, "cp932",                                           932,  no },
  { 213, "sjis",                                            932,  no },
  { 214, "iso-ir-128",                                        0,  no }, /* IANA */
  { 214, "T.101-G2",                                          0,  no }, /* IANA */
  { 214, "csISO128T101G2",                                    0,  no }, /* IANA */
  { 215, "iso-ir-102",                                        0,  no }, /* IANA */
  { 215, "T.61-7bit",                                         0,  no }, /* IANA */
  { 215, "csISO102T617bit",                                   0,  no }, /* IANA */
  { 216, "iso-ir-103",                                        0,  no }, /* IANA */
  { 216, "T.61-8bit",                                         0,  no }, /* IANA */
  { 216, "T.61",                                              0,  no }, /* IANA */
  { 216, "csISO103T618bit",                                   0,  no }, /* IANA */
  { 217, "TIS-620",                                         874,  no }, /* IANA */
  { 217, "Windows-874",                                     874,  no }, /* MICROSOFT */
  { 217, "ISO-8859-11",                                     874,  no }, /* MICROSOFT */
  { 217, "dos-874",                                         874,  no }, /* MICROSOFT */
  { 218, "UNICODE-1-1",                                       0,  no }, /* IANA */
  { 218, "csUnicode11",                                       0,  no }, /* IANA */
  { 219, "UNICODE-1-1-UTF-7",                             65000, yes }, /* IANA */
  { 219, "csUnicode11UTF7",                               65000, yes }, /* IANA */
  { 219, "x-unicode-2-0-utf-7",                           65000, yes }, /* MICROSOFT */
  { 220, "UNKNOWN-8BIT",                                      0,  no }, /* IANA */
  { 220, "csUnknown8BiT",                                     0,  no }, /* IANA */
  { 221, "us-dk",                                             0,  no }, /* IANA */
  { 221, "csUSDK",                                            0,  no }, /* IANA */
  { 222, "UTF-16",                                         1200,  no }, /* IANA */
  { 222, "Unicode",                                        1200,  no }, /* MICROSOFT */
  { 223, "UTF-16BE",                                       1201,  no }, /* IANA */
  { 223, "UnicodeFFFE",                                    1201,  no }, /* MICROSOFT */
  { 224, "UTF-16LE",                                       1200,  no }, /* IANA */
  { 224, "Unicode",                                        1200,  no }, /* MICROSOFT */
  { 225, "UTF-32",                                            0,  no }, /* IANA */
  { 226, "UTF-32BE",                                          0,  no }, /* IANA */
  { 227, "UTF-32LE",                                          0,  no }, /* IANA */
  { 228, "UTF-7",                                         65000, yes }, /* IANA */
  { 228, "x-unicode-2-0-utf-7",                           65000, yes }, /* MICROSOFT */
  { 229, "UTF-8",                                         65001,  no }, /* IANA */
  { 229, "unicode-2-0-utf-8",                             65001,  no }, /* MICROSOFT */
  { 229, "unicode-1-1-utf-8",                             65001,  no }, /* MICROSOFT */
  { 229, "x-unicode-2-0-utf-8",                           65001,  no }, /* MICROSOFT */
  { 230, "Ventura-International",                             0,  no }, /* IANA */
  { 230, "csVenturaInternational",                            0,  no }, /* IANA */
  { 231, "Ventura-Math",                                      0,  no }, /* IANA */
  { 231, "csVenturaMath",                                     0,  no }, /* IANA */
  { 232, "Ventura-US",                                        0,  no }, /* IANA */
  { 232, "csVenturaUS",                                       0,  no }, /* IANA */
  { 233, "iso-ir-70",                                         0,  no }, /* IANA */
  { 233, "videotex-suppl",                                    0,  no }, /* IANA */
  { 233, "csISO70VideotexSupp1",                              0,  no }, /* IANA */
  { 234, "VIQR",                                              0,  no }, /* IANA */
  { 234, "csVIQR",                                            0,  no }, /* IANA */
  { 235, "VISCII",                                            0,  no }, /* IANA */
  { 235, "csVISCII",                                          0,  no }, /* IANA */
  { 236, "Windows-1250",                                   1250,  no }, /* IANA */
  { 236, "x-cp1250",                                       1250,  no }, /* MICROSOFT */
  { 237, "Windows-1251",                                   1251,  no }, /* IANA */
  { 237, "x-cp1251",                                       1251,  no }, /* MICROSOFT */
  { 238, "Windows-1252",                                   1252,  no }, /* IANA */
  { 238, "x-ansi",                                         1252,  no }, /* MICROSOFT */
  { 239, "Windows-1253",                                   1253,  no }, /* IANA */
  { 240, "Windows-1254",                                   1254,  no }, /* IANA */
  { 241, "Windows-1255",                                   1255,  no }, /* IANA */
  { 242, "Windows-1256",                                   1256,  no }, /* IANA */
  { 242, "cp1256",                                         1256,  no }, /* MICROSOFT */
  { 243, "Windows-1257",                                   1257,  no }, /* IANA */
  { 244, "Windows-1258",                                   1258,  no }, /* IANA */
  { 245, "Windows-31J",                                     932,  no }, /* IANA */
  { 245, "csWindows31J",                                    932,  no }, /* IANA */
  { 245, "shift-jis",                                       932,  no }, /* MICROSOFT */
  { 245, "x-ms-cp932",                                      932,  no }, /* MICROSOFT */
  { 245, "x-sjis",                                          932,  no }, /* MICROSOFT */
  { 246, "dos-720",                                         720,  no }, /* MICROSOFT */
  { 247, "euc-cn",                                        51936,  no }, /* MICROSOFT */
  { 248, "ibm737",                                          737,  no }, /* MICROSOFT */
  { 249, "johab",                                          1361,  no }, /* MICROSOFT */
  { 250, "x-chinese-cns",                                 20000,  no }, /* MICROSOFT */
  { 251, "x-chinese-eten",                                20002,  no }, /* MICROSOFT */
  { 252, "x-ebcdic-cyrillicserbianbulgarian",             21025,  no }, /* MICROSOFT */
  { 253, "x-ebcdic-greekmodern",                            875,  no }, /* MICROSOFT */
  { 254, "x-ebcdic-japaneseandjapaneselatin",             50939,  no }, /* MICROSOFT */
  { 255, "x-ebcdic-japaneseandkana",                      50930,  no }, /* MICROSOFT */
  { 256, "x-ebcdic-japaneseanduscanada",                  50931,  no }, /* MICROSOFT */
  { 257, "x-ebcdic-koreanandkoreanextended",              50933,  no }, /* MICROSOFT */
  { 258, "x-ebcdic-koreanextended",                       20833,  no }, /* MICROSOFT */
  { 259, "x-ebcdic-simplifiedchinese",                    50935,  no }, /* MICROSOFT */
  { 260, "x-ebcdic-traditionalchinese",                   50937,  no }, /* MICROSOFT */
  { 261, "x-euc-cn",                                      51936,  no }, /* MICROSOFT */
  { 262, "x-europa",                                      29001,  no }, /* MICROSOFT */
  { 263, "x-iscii-as",                                    57006,  no }, /* MICROSOFT */
  { 264, "x-iscii-be",                                    57003,  no }, /* MICROSOFT */
  { 265, "x-iscii-de",                                    57002,  no }, /* MICROSOFT */
  { 266, "x-iscii-gu",                                    57010,  no }, /* MICROSOFT */
  { 267, "x-iscii-ka",                                    57008,  no }, /* MICROSOFT */
  { 268, "x-iscii-ma",                                    57009,  no }, /* MICROSOFT */
  { 269, "x-iscii-or",                                    57007,  no }, /* MICROSOFT */
  { 270, "x-iscii-pa",                                    57011,  no }, /* MICROSOFT */
  { 271, "x-iscii-ta",                                    57004,  no }, /* MICROSOFT */
  { 272, "x-iscii-te",                                    57005,  no }, /* MICROSOFT */
  { 273, "x-mac-arabic",                                  10004,  no }, /* MICROSOFT */
  { 274, "x-mac-ce",                                      10029,  no }, /* MICROSOFT */
  { 275, "x-mac-chinesesimp",                             10008,  no }, /* MICROSOFT */
  { 276, "x-mac-chinesetrad",                             10002,  no }, /* MICROSOFT */
  { 277, "x-mac-cyrillic",                                10007,  no }, /* MICROSOFT */
  { 278, "x-mac-greek",                                   10006,  no }, /* MICROSOFT */
  { 279, "x-mac-hebrew",                                  10005,  no }, /* MICROSOFT */
  { 280, "x-mac-icelandic",                               10079,  no }, /* MICROSOFT */
  { 281, "x-mac-japanese",                                10001,  no }, /* MICROSOFT */
  { 282, "x-mac-korean",                                  10003,  no }, /* MICROSOFT */
  { 283, "x-mac-turkish",                                 10081,  no }, /* MICROSOFT */
  { 284, "cp20932",                                       20932,  no }, /* MICROSOFT */
  { 285, "ASMO-708",                                        708,  no }, /* IANA */

  /* final entry */
  {   0, NULL,                                                0,  no }
};

static struct _charsetInfo const **index1;
static struct _charsetInfo const **index2;
static struct _charsetInfo const **index3;

enum { numCharsetInfo = sizeof charsetInfo / sizeof *charsetInfo - 1 };

static int CompareByName(const void *elem1, const void *elem2)
{
	const struct _charsetInfo *p = *(const struct _charsetInfo **)elem1;
	const struct _charsetInfo *q = *(const struct _charsetInfo **)elem2;
	return _stricmp(p->charset, q->charset);
}

static int CompareByCodePage(const void *elem1, const void *elem2)
{
	const struct _charsetInfo *p = *(const struct _charsetInfo **)elem1;
	const struct _charsetInfo *q = *(const struct _charsetInfo **)elem2;
	return p->codepage - q->codepage;
}

static int SortCompareByCodePage(const void *elem1, const void *elem2)
{
	const struct _charsetInfo *p = *(const struct _charsetInfo **)elem1;
	const struct _charsetInfo *q = *(const struct _charsetInfo **)elem2;
	return (int)(p->codepage != q->codepage ? p->codepage - q->codepage : p - q);
}

static struct _charsetInfo const *FindByName(const char *name)
{
	struct _charsetInfo const *info = NULL;
	if (index1 && name)
	{
		struct _charsetInfo const key = {0, name, 0, no};
		struct _charsetInfo const *pkey = &key;
		struct _charsetInfo const **pinfo = (struct _charsetInfo **)bsearch(&pkey, index1, numCharsetInfo, sizeof *index1, CompareByName);
		if (pinfo != NULL)
		{
			info = *pinfo;
		}
	}
	return info;
}

static struct _charsetInfo const *FindById(unsigned id)
{
	size_t numIndex = charsetInfo[numCharsetInfo - 1].id + 1;
	return index2 && id < numIndex ? index2[id] : NULL;
}

static struct _charsetInfo const *FindByCodePage(unsigned codepage)
{
	struct _charsetInfo const *info = NULL;
	size_t numIndex = charsetInfo[numCharsetInfo - 1].id + 1;
	if (index3 && codepage)
	{
		struct _charsetInfo const key = {0, 0, codepage, no};
		struct _charsetInfo const *pkey = &key;
		struct _charsetInfo const **pinfo = (struct _charsetInfo **)bsearch(&pkey, index3, numIndex, sizeof(void *), CompareByCodePage);
		if (pinfo != NULL) do
		{
			info = *pinfo;
		} while (pinfo > index3 && CompareByCodePage(--pinfo, &pkey) == 0);
	}
	return info;
}

void charsets_init(void)
{
	size_t i;
	size_t numIndex = charsetInfo[numCharsetInfo - 1].id + 1;
	index1 = (struct _charsetInfo **)calloc(numCharsetInfo, sizeof(void *));
	index2 = (struct _charsetInfo **)calloc(numIndex, sizeof(void *));
	index3 = (struct _charsetInfo **)calloc(numIndex, sizeof(void *));
	if (!index1 || !index2 || !index3)
		return;
	for (i = numCharsetInfo ; i-- ; )
	{
		index1[i] = charsetInfo + i;
	}
	qsort((void*)index1, numCharsetInfo, sizeof(void *), CompareByName);
	for (i = numCharsetInfo ; i-- ; )
	{
		index2[charsetInfo[i].id] = charsetInfo + i;
	}
	for (i = numCharsetInfo + 1 ; i-- ; )
	{
		index3[charsetInfo[i].id] = charsetInfo + i;
	}
	qsort((void*)index3, numIndex, sizeof(void *), SortCompareByCodePage);
}

void charsets_cleanup(void)
{
	free((void *)index1);
	index1 = NULL;
	free((void *)index2);
	index2 = NULL;
	free((void *)index3);
	index3 = NULL;
}


unsigned GetEncodingIdFromName(const char *name)
{
	struct _charsetInfo const *info = FindByName(name);
	return info ? info->id : 0;
}

unsigned GetEncodingIdFromCodePage(unsigned cp)
{
	struct _charsetInfo const *info = FindByCodePage(cp);
	return info ? info->id : 0;
}

unsigned GetEncodingCodePageFromName(const char *name)
{
	struct _charsetInfo const *info = FindByName(name);
	return info ? info->codepage : 0;
}

unsigned GetEncodingCodePageFromId(unsigned id)
{
	struct _charsetInfo const *info = FindById(id);
	return info ? info->codepage : 0;
}

const char *GetEncodingNameFromId(unsigned id)
{
	struct _charsetInfo const *info = FindById(id);
	return info ? info->charset : NULL;
}

const char *GetEncodingNameFromCodePage(unsigned cp)
{
	struct _charsetInfo const *info = FindByCodePage(cp);
	return info ? info->charset : NULL;
}
