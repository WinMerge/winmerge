///////////////////////////////////////////////////////////////////////////
//  File:    batch.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  MS-DOS batch syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszBatKeywordList[] =
  {
    _T (".AND."),
    _T (".OR."),
    _T (".XOR."),
    _T ("?"),
    _T ("ACTIVATE"),
    _T ("ALIAS"),
    _T ("ASSOC"),
    _T ("ATTRIB"),
    _T ("BATCOMP"),
    _T ("BDEBUGGER"),
    _T ("BEEP"),
    _T ("BREAK"),
    _T ("BY"),
    _T ("CALL"),
    _T ("CANCEL"),
    _T ("CASE"),
    _T ("CD"),
    _T ("CDD"),
    _T ("CHCP"),
    _T ("CHDIR"),
    _T ("CLS"),
    _T ("COLOR"),
    _T ("COPY"),
    _T ("CTTY"),
    _T ("DATE"),
    _T ("DEFAULT"),
    _T ("DEFINED"),
    _T ("DEL"),
    _T ("DELAY"),
    _T ("DESCRIBE"),
    _T ("DETACH"),
    _T ("DIR"),
    _T ("DIREXIST"),
    _T ("DIRHISTORY"),
    _T ("DIRS"),
    _T ("DO"),
    _T ("DRAWBOX"),
    _T ("DRAWHLINE"),
    _T ("DRAWVLINE"),
    _T ("ECHO"),
    _T ("ECHOERR"),
    _T ("ECHOS"),
    _T ("ECHOSERR"),
    _T ("ELSE"),
    _T ("ELSEIFF"),
    _T ("ENDDO"),
    _T ("ENDIFF"),
    _T ("ENDLOCAL"),
    _T ("ENDSWITCH"),
    _T ("ENDTEXT"),
    _T ("EQ"),
    _T ("EQL"),
    _T ("EQU"),
    _T ("ERASE"),
    _T ("ERROR"),
    _T ("ERRORLEVEL"),
    _T ("ERRORMSG"),
    _T ("ESET"),
    _T ("EVENTLOG"),
    _T ("EXCEPT"),
    _T ("EXIST"),
    _T ("EXIT"),
    _T ("FFIND"),
    _T ("FOR"),
    _T ("FOREVER"),
    _T ("FREE"),
    _T ("FTYPE"),
    _T ("FUNCTION"),
    _T ("GE"),
    _T ("GEQ"),
    _T ("GLOBAL"),
    _T ("GOSUB"),
    _T ("GOTO"),
    _T ("GT"),
    _T ("GTR"),
    _T ("HEAD"),
    _T ("HELP"),
    _T ("HISTORY"),
    _T ("IF"),
    _T ("IFF"),
    _T ("IFTP"),
    _T ("IN"),
    _T ("INKEY"),
    _T ("INPUT"),
    _T ("ISALIAS"),
    _T ("ISAPP"),
    _T ("ISDIR"),
    _T ("ISFUNCTION"),
    _T ("ISINTERNAL"),
    _T ("ISLABEL"),
    _T ("ISWINDOW"),
    _T ("ITERATE"),
    _T ("KEYBD"),
    _T ("KEYS"),
    _T ("KEYSTACK"),
    _T ("LE"),
    _T ("LEAVE"),
    _T ("LEQ"),
    _T ("LH"),
    _T ("LIST"),
    _T ("LOADBTM"),
    _T ("LOADHIGH"),
    _T ("LOCK"),
    _T ("LOG"),
    _T ("LSS"),
    _T ("LT"),
    _T ("MD"),
    _T ("MEMORY"),
    _T ("MKDIR"),
    _T ("MKLNK"),
    _T ("MOVE"),
    _T ("MSGBOX"),
    _T ("NE"),
    _T ("NEQ"),
    _T ("NOT"),
    _T ("ON"),
    _T ("OPTION"),
    _T ("PATH"),
    _T ("PAUSE"),
    _T ("PDIR"),
    _T ("PLAYAVI"),
    _T ("PLAYSOUND"),
    _T ("POPD"),
    _T ("PRINT"),
    _T ("PROMPT"),
    _T ("PUSHD"),
    _T ("QUERYBOX"),
    _T ("QUIT"),
    _T ("RD"),
    _T ("REBOOT"),
    _T ("RECYCLE"),
    _T ("REM"),
    _T ("REN"),
    _T ("RENAME"),
    _T ("RETURN"),
    _T ("RMDIR"),
    _T ("SCREEN"),
    _T ("SCRPUT"),
    _T ("SELECT"),
    _T ("SENDMAIL"),
    _T ("SET"),
    _T ("SETDOS"),
    _T ("SETLOCAL"),
    _T ("SHIFT"),
    _T ("SHORTCUT"),
    _T ("SHRALIAS"),
    _T ("SMPP"),
    _T ("SNPP"),
    _T ("START"),
    _T ("SWAPPING"),
    _T ("SWITCH"),
    _T ("TAIL"),
    _T ("TASKEND"),
    _T ("TASKLIST"),
    _T ("TEE"),
    _T ("TEXT"),
    _T ("THEN"),
    _T ("TIME"),
    _T ("TIMER"),
    _T ("TITLE"),
    _T ("TOUCH"),
    _T ("TREE"),
    _T ("TRUENAME"),
    _T ("TYPE"),
    _T ("UNALIAS"),
    _T ("UNFUNCTION"),
    _T ("UNLOCK"),
    _T ("UNSET"),
    _T ("UNTIL"),
    _T ("VER"),
    _T ("VERIFY"),
    _T ("VOL"),
    _T ("VSCRPUT"),
    _T ("WHICH"),
    _T ("WHILE"),
    _T ("WINDOW"),
    _T ("Y"),
    NULL
  };

static LPTSTR s_apszUser1KeywordList[] =
  {
    _T ("APPEND"),
    _T ("ATTRIB"),
    _T ("BUSETUP"),
    _T ("CHKDSK"),
    _T ("CHOICE"),
    _T ("COMMAND"),
    _T ("DEBUG"),
    _T ("DEFRAG"),
    _T ("DELOLDOS"),
    _T ("DELTREE"),
    _T ("DISKCOMP"),
    _T ("DISKCOPY"),
    _T ("DOSKEY"),
    _T ("DRVSPACE"),
    _T ("EDIT"),
    _T ("EMM386"),
    _T ("EXPAND"),
    _T ("FASTHELP"),
    _T ("FASTOPEN"),
    _T ("FC"),
    _T ("FDISK"),
    _T ("FIND"),
    _T ("FORMAT"),
    _T ("GRAPHICS"),
    _T ("HELP"),
    _T ("INTERLNK"),
    _T ("INTERSVR"),
    _T ("KEYB"),
    _T ("LABEL"),
    _T ("LOADFIX"),
    _T ("MEM"),
    _T ("MEMMAKER"),
    _T ("MODE"),
    _T ("MORE"),
    _T ("MOUSE"),
    _T ("MOVE"),
    _T ("MSAV"),
    _T ("MSBACKUP"),
    _T ("MSCDEX"),
    _T ("MSD"),
    _T ("MWAV"),
    _T ("MWAVTSR"),
    _T ("MWBACKUP"),
    _T ("MWUNDEL"),
    _T ("NLSFUNC"),
    _T ("POWER"),
    _T ("PRINT"),
    _T ("QBASIC"),
    _T ("REPLACE"),
    _T ("RESTORE"),
    _T ("SCANDISK"),
    _T ("SETUP"),
    _T ("SETVER"),
    _T ("SHARE"),
    _T ("SIZER"),
    _T ("SMARTDRV"),
    _T ("SMARTMON"),
    _T ("SORT"),
    _T ("SUBST"),
    _T ("SYS"),
    _T ("TREE"),
    _T ("UNDELETE"),
    _T ("UNFORMAT"),
    _T ("UNINSTAL"),
    _T ("VSAFE"),
    _T ("XCOPY"),
    NULL
  };

static LPTSTR s_apszUser2KeywordList[] =
  {
    _T ("@ABS"),
    _T ("@AGEDATE"),
    _T ("@ALIAS"),
    _T ("@ALTNAME"),
    _T ("@ASCII"),
    _T ("@ATTRIB"),
    _T ("@CAPS"),
    _T ("@CDROM"),
    _T ("@CEILING"),
    _T ("@CHAR"),
    _T ("@CLIP"),
    _T ("@CLIPW"),
    _T ("@COLOR"),
    _T ("@COMMA"),
    _T ("@CONSOLE"),
    _T ("@CONVERT"),
    _T ("@CRC32"),
    _T ("@DATE"),
    _T ("@DAY"),
    _T ("@DEC"),
    _T ("@DECIMAL"),
    _T ("@DESCRIPT"),
    _T ("@DEVICE"),
    _T ("@DIGITS"),
    _T ("@DIRSTACK"),
    _T ("@DISKFREE"),
    _T ("@DISKTOTAL"),
    _T ("@DISKUSED"),
    _T ("@DOSMEM"),
    _T ("@DOW"),
    _T ("@DOWF"),
    _T ("@DOWI"),
    _T ("@DOY"),
    _T ("@EMS"),
    _T ("@ENUMSERVERS"),
    _T ("@ENUMSHARES"),
    _T ("@ERRTEXT"),
    _T ("@EVAL"),
    _T ("@EXEC"),
    _T ("@EXECSTR"),
    _T ("@EXETYPE"),
    _T ("@EXPAND"),
    _T ("@EXT"),
    _T ("@EXTENDED"),
    _T ("@FIELD"),
    _T ("@FIELDS"),
    _T ("@FILEAGE"),
    _T ("@FILECLOSE"),
    _T ("@FILEDATE"),
    _T ("@FILENAME"),
    _T ("@FILEOPEN"),
    _T ("@FILEREAD"),
    _T ("@FILES"),
    _T ("@FILESEEK"),
    _T ("@FILESEEKL"),
    _T ("@FILESIZE"),
    _T ("@FILETIME"),
    _T ("@FILEWRITE"),
    _T ("@FILEWRITEB"),
    _T ("@FINDCLOSE"),
    _T ("@FINDFIRST"),
    _T ("@FINDNEXT"),
    _T ("@FLOOR"),
    _T ("@FORMAT"),
    _T ("@FORMATN"),
    _T ("@FSTYPE"),
    _T ("@FULL"),
    _T ("@FUNCTION"),
    _T ("@GETDIR"),
    _T ("@GETFILE"),
    _T ("@GETFOLDER"),
    _T ("@HISTORY"),
    _T ("@IDOW"),
    _T ("@IDOWF"),
    _T ("@IF"),
    _T ("@INC"),
    _T ("@INDEX"),
    _T ("@INIREAD"),
    _T ("@INIWRITE"),
    _T ("@INSERT"),
    _T ("@INSTR"),
    _T ("@INT"),
    _T ("@IPADDRESS"),
    _T ("@IPNAME"),
    _T ("@ISALNUM"),
    _T ("@ISALPHA"),
    _T ("@ISASCII"),
    _T ("@ISCNTRL"),
    _T ("@ISDIGIT"),
    _T ("@ISPRINT"),
    _T ("@ISPUNCT"),
    _T ("@ISSPACE"),
    _T ("@ISXDIGIT"),
    _T ("@LABEL"),
    _T ("@LEFT"),
    _T ("@LEN"),
    _T ("@LFN"),
    _T ("@LINE"),
    _T ("@LINES"),
    _T ("@LOWER"),
    _T ("@LPT"),
    _T ("@LTRIM"),
    _T ("@MAKEAGE"),
    _T ("@MAKEDATE"),
    _T ("@MAKETIME"),
    _T ("@MASTER"),
    _T ("@MAX"),
    _T ("@MD5"),
    _T ("@MIN"),
    _T ("@MONTH"),
    _T ("@NAME"),
    _T ("@NUMERIC"),
    _T ("@OPTION"),
    _T ("@PATH"),
    _T ("@RANDOM"),
    _T ("@READSCR"),
    _T ("@READY"),
    _T ("@REGCREATE"),
    _T ("@REGDELKEY"),
    _T ("@REGEXIST"),
    _T ("@REGQUERY"),
    _T ("@REGSET"),
    _T ("@REGSETENV"),
    _T ("@REMOTE"),
    _T ("@REMOVABLE"),
    _T ("@REPEAT"),
    _T ("@REPLACE"),
    _T ("@REXX"),
    _T ("@RIGHT"),
    _T ("@RTRIM"),
    _T ("@SEARCH"),
    _T ("@SELECT"),
    _T ("@SFN"),
    _T ("@STRIP"),
    _T ("@SUBST"),
    _T ("@SUBSTR"),
    _T ("@TIME"),
    _T ("@TIMER"),
    _T ("@TRIM"),
    _T ("@TRUENAME"),
    _T ("@UNC"),
    _T ("@UNICODE"),
    _T ("@UNIQUE"),
    _T ("@UPPER"),
    _T ("@VERINFO"),
    _T ("@WATTRIB"),
    _T ("@WILD"),
    _T ("@WINCLASS"),
    _T ("@WINEXENAME"),
    _T ("@WININFO"),
    _T ("@WINMEMORY"),
    _T ("@WINMETRICS"),
    _T ("@WINSTATE"),
    _T ("@WINSYSTEM"),
    _T ("@WORD"),
    _T ("@WORDS"),
    _T ("@XMS"),
    _T ("@YEAR"),
    _T ("_"),
    _T ("_4VER"),
    _T ("_ACSTATUS"),
    _T ("_ALIAS"),
    _T ("_ANSI"),
    _T ("_APMAC"),
    _T ("_APMBATT"),
    _T ("_APMLIFE"),
    _T ("_BATCH"),
    _T ("_BATCHLINE"),
    _T ("_BATCHNAME"),
    _T ("_BATCHTYPE"),
    _T ("_BATTERY"),
    _T ("_BATTERYLIFE"),
    _T ("_BATTERYPERCENT"),
    _T ("_BG"),
    _T ("_BOOT"),
    _T ("_BUILD"),
    _T ("_CAPSLOCK"),
    _T ("_CHILDPID"),
    _T ("_CI"),
    _T ("_CMDPROC"),
    _T ("_CO"),
    _T ("_CODEPAGE"),
    _T ("_COLUMN"),
    _T ("_COLUMNS"),
    _T ("_COUNTRY"),
    _T ("_CPU"),
    _T ("_CPUUSAGE"),
    _T ("_CTRL"),
    _T ("_CWD"),
    _T ("_CWDS"),
    _T ("_CWP"),
    _T ("_CWPS"),
    _T ("_DATE"),
    _T ("_DAY"),
    _T ("_DETACHPID"),
    _T ("_DISK"),
    _T ("_DNAME"),
    _T ("_DOS"),
    _T ("_DOSVER"),
    _T ("_DOW"),
    _T ("_DOWF"),
    _T ("_DOWI"),
    _T ("_DOY"),
    _T ("_DPMI"),
    _T ("_DV"),
    _T ("_ECHO"),
    _T ("_ENV"),
    _T ("_FG"),
    _T ("_FTPERROR"),
    _T ("_HLOGFILE"),
    _T ("_HOST"),
    _T ("_HOUR"),
    _T ("_HWPROFILE"),
    _T ("_IDOW"),
    _T ("_IDOWF"),
    _T ("_IMONTH"),
    _T ("_IMONTHF"),
    _T ("_ININAME"),
    _T ("_IP"),
    _T ("_ISODATE"),
    _T ("_KBHIT"),
    _T ("_KSTACK"),
    _T ("_LALT"),
    _T ("_LASTDISK"),
    _T ("_LCTRL"),
    _T ("_LOGFILE"),
    _T ("_LSHIFT"),
    _T ("_MINUTE"),
    _T ("_MONITOR"),
    _T ("_MONTH"),
    _T ("_MONTHF"),
    _T ("_MOUSE"),
    _T ("_NDP"),
    _T ("_NUMLOCK"),
    _T ("_PID"),
    _T ("_PIPE"),
    _T ("_PPID"),
    _T ("_RALT"),
    _T ("_RCTRL"),
    _T ("_ROW"),
    _T ("_ROWS"),
    _T ("_RSHIFT"),
    _T ("_SCROLLLOCK"),
    _T ("_SECOND"),
    _T ("_SELECTED"),
    _T ("_SHELL"),
    _T ("_SHIFT"),
    _T ("_SHRALIAS"),
    _T ("_STARTPATH"),
    _T ("_STARTPID"),
    _T ("_SWAPPING"),
    _T ("_SYSERR"),
    _T ("_TIME"),
    _T ("_TRANSIENT"),
    _T ("_UNICODE"),
    _T ("_VIDEO"),
    _T ("_WIN"),
    _T ("_WINDIR"),
    _T ("_WINFGWINDOW"),
    _T ("_WINNAME"),
    _T ("_WINSYSDIR"),
    _T ("_WINTICKS"),
    _T ("_WINTITLE"),
    _T ("_WINUSER"),
    _T ("_WINVER"),
    _T ("_XPIXELS"),
    _T ("_YEAR"),
    _T ("_YPIXELS"),
    NULL
  };

static BOOL
IsBatKeyword (LPCTSTR pszChars, int nLength)
{
  for (int L = 0; s_apszBatKeywordList[L] != NULL; L++)
    {
      if (_tcsnicmp (s_apszBatKeywordList[L], pszChars, nLength) == 0
            && s_apszBatKeywordList[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  TCHAR buffer[13];

  for (int L = 0; s_apszUser1KeywordList[L] != NULL; L++)
    {
      if (_tcsnicmp (s_apszUser1KeywordList[L], pszChars, nLength) == 0
            && s_apszUser1KeywordList[L][nLength] == 0)
        return TRUE;
      _tcscpy (buffer, s_apszUser1KeywordList[L]);
      _tcscat (buffer, _T (".COM"));
      if (_tcsnicmp (buffer, pszChars, nLength) == 0
            && buffer[nLength] == 0)
        return TRUE;
      _tcscpy (buffer, s_apszUser1KeywordList[L]);
      _tcscat (buffer, _T (".EXE"));
      if (_tcsnicmp (buffer, pszChars, nLength) == 0
            && buffer[nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
    for (int L = 0; apszKeywords[L] != NULL; L++)
      {
        if (_tcsnicmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
            return TRUE;
      }
    return FALSE;
}

static BOOL
IsUser2Keyword (LPCTSTR pszChars, int nLength)
{
    return IsXKeyword (s_apszUser2KeywordList, pszChars, nLength);
}

static BOOL
IsBatNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return FALSE;
        }
      return TRUE;
    }
  if (!_istdigit (pszChars[0]))
    return FALSE;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return FALSE;
    }
  return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010

DWORD CCrystalTextView::
ParseLineBatch (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bDecIndex = FALSE;
  BOOL bOneWord = TRUE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  for (int I = 0;; nPrevI = I, I = ::CharNext(pszChars+I) - pszChars)
    {
      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
              if (!bOneWord) dwCookie &= ~COOKIE_PREPROCESSOR;
                bOneWord = FALSE;
            }
          else
            {
              if (pszChars[nPos] == _T ('_') || pszChars[nPos] == _T ('@') || xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  goto out;
                }
            }
          bRedefineBlock = FALSE;
          bDecIndex = FALSE;
        }
out:

      if (I == nLength)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }
      if (pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (bFirstChar)
        {
          if (nLength >= I + 4 &&!_tcsnicmp (pszChars + I, _T ("TEXT"), 4))
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_EXT_COMMENT;
              continue;
            }
          if (nLength >= I + 7 &&!_tcsnicmp (pszChars + I, _T ("ENDTEXT"), 7))
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie &= ~COOKIE_EXT_COMMENT;
              continue;
            }
          if (dwCookie & COOKIE_EXT_COMMENT)
              continue;
          if (nLength >= I + 3 && !_tcsnicmp (pszChars + I, _T ("REM"), 3) && (xisspace (pszChars[I + 3]) || nLength == I + 3))
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_COMMENT;
              break;
            }

          if (pszChars[I] == ':')
            {
              if (nLength > I + 2 && !(xisalnum (pszChars[I+1]) || xisspace (pszChars[I+1])))
                {
                  DEFINE_BLOCK (I, COLORINDEX_COMMENT);
                  dwCookie |= COOKIE_COMMENT;
                  break;
                }
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              continue;
            }
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (pszChars[I] == _T ('_') || pszChars[I] == _T ('@') || xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if ((dwCookie & COOKIE_PREPROCESSOR))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_PREPROCESSOR);
                }
              else 
                {
                  if (dwCookie & COOKIE_EXT_COMMENT)
                    {
                        DEFINE_BLOCK (nIdentBegin, COLORINDEX_COMMENT);
                    }
                  else if (IsBatKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                  if ((I - nIdentBegin ==4 && !_tcsnicmp (pszChars + nIdentBegin, _T ("GOTO"), 4)) ||
                      (I - nIdentBegin ==5 && !_tcsnicmp (pszChars + nIdentBegin, _T ("GOSUB"), 5))
                      )
                    {
                        dwCookie=COOKIE_PREPROCESSOR;
                    }
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
                  else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                    }
              else if (IsBatNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_COMMENT);
        }
      else if (IsBatKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
          if ((I - nIdentBegin ==4 && !_tcsnicmp (pszChars + nIdentBegin, _T ("GOTO"), 4)) ||
                (I - nIdentBegin ==5 && !_tcsnicmp (pszChars + nIdentBegin, _T ("GOSUB"), 5))
                )
              {
                dwCookie=COOKIE_PREPROCESSOR;
              }
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
      else if (IsBatNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
