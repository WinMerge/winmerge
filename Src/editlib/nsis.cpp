///////////////////////////////////////////////////////////////////////////
//  File:       cplusplus.cpp
//  Version:    1.2.0.5
//  Created:    29-Dec-1998
//
//  Copyright:  Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalEditView class, a part of the Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  16-Aug-99
//      Ferdinand Prantl:
//  +   FEATURE: corrected bug in syntax highlighting C comments
//  +   FEATURE: extended levels 1- 4 of keywords in some languages
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszNsisKeywordList[] =
  {
    _T ("Abort"),
    _T ("AddBrandingImage"),
    _T ("AddSharedDLL"),
    _T ("AddSize"),
    _T ("AllowRootDirInstall"),
    _T ("AllowSkipFiles"),
    _T ("AutoCloseWindow"),
    _T ("BGFont"),
    _T ("BGGradient"),
    _T ("BrandingText"),
    _T ("BringToFront"),
    _T ("Call"),
    _T ("CallInstDLL"),
    _T ("Caption"),
    _T ("ChangeUI"),
    _T ("CheckBitmap"),
    _T ("ClearErrors"),
    _T ("CloseWinamp"),
    _T ("CompareDLLVersions"),
    _T ("CompareFileTimes"),
    _T ("CompletedText"),
    _T ("ComponentText"),
    _T ("CopyFiles"),
    _T ("CRCCheck"),
    _T ("CreateDirectory"),
    _T ("CreateFont"),
    _T ("CreateShortCut"),
    _T ("Delete"),
    _T ("DeleteINISec"),
    _T ("DeleteINIStr"),
    _T ("DeleteNSPlug"),
    _T ("DeleteRegKey"),
    _T ("DeleteRegValue"),
    _T ("DetailPrint"),
    _T ("DetailsButtonText"),
    _T ("DirShow"),
    _T ("DirText"),
    _T ("DirVar"),
    _T ("DirVerify"),
    _T ("DisabledBitmap"),
    _T ("EnabledBitmap"),
    _T ("EnableWindow"),
    _T ("EnumRegKey"),
    _T ("EnumRegValue"),
    _T ("Exch"),
    _T ("Exec"),
    _T ("ExecShell"),
    _T ("ExecWait"),
    _T ("ExpandEnvStrings"),
    _T ("File"),
    _T ("FileBufSize"),
    _T ("FileClose"),
    _T ("FileErrorText"),
    _T ("FileOpen"),
    _T ("FileRead"),
    _T ("FileReadByte"),
    _T ("FileSeek"),
    _T ("FileWrite"),
    _T ("FileWriteByte"),
    _T ("FindClose"),
    _T ("FindFirst"),
    _T ("FindNext"),
    _T ("FindWindow"),
    _T ("FindWindowByTitle"),
    _T ("FlushINI"),
    _T ("Function"),
    _T ("FunctionEnd"),
    _T ("GetCurInstType"),
    _T ("GetCurrentAddress"),
    _T ("GetDlgItem"),
    _T ("GetDLLVersion"),
    _T ("GetDLLVersionLocal"),
    _T ("GetErrorLevel"),
    _T ("GetFileTime"),
    _T ("GetFileTimeLocal"),
    _T ("GetFullDLLPath"),
    _T ("GetFullPathName"),
    _T ("GetFunctionAddress"),
    _T ("GetIEVersion"),
    _T ("GetInstDirError"),
    _T ("GetLabelAddress"),
    _T ("GetParameters"),
    _T ("GetParent"),
    _T ("GetParentDir"),
    _T ("GetTempFileName"),
    _T ("GetWinampDSPPath"),
    _T ("GetWinampInstPath"),
    _T ("GetWinampSkinPath"),
    _T ("GetWinampVisPath"),
    _T ("GetWindowsVersion"),
    _T ("Goto"),
    _T ("HideWindow"),
    _T ("Icon"),
    _T ("IfAbort"),
    _T ("IfErrors"),
    _T ("IfFileExists"),
    _T ("IfRebootFlag"),
    _T ("IfSilent"),
    _T ("InitPluginsDir"),
    _T ("InstallButtonText"),
    _T ("InstallColors"),
    _T ("InstallDir"),
    _T ("InstallDirRegKey"),
    _T ("InstallNetscapePlugin"),
    _T ("InstNSPlug"),
    _T ("InstProgressFlags"),
    _T ("InstType"),
    _T ("InstTypeGetText"),
    _T ("InstTypeSetText"),
    _T ("IntCmp"),
    _T ("IntCmpU"),
    _T ("IntFmt"),
    _T ("IntOp"),
    _T ("IsFlashInstalled"),
    _T ("IsWindow"),
    _T ("LangString"),
    _T ("LangStringUP"),
    _T ("LicenseBkColor"),
    _T ("LicenseData"),
    _T ("LicenseForceSelection"),
    _T ("LicenseLangString"),
    _T ("LicenseText"),
    _T ("LoadLanguageFile"),
    _T ("LockWindow"),
    _T ("LogicLib"),
    _T ("LogSet"),
    _T ("LogText"),
    _T ("MessageBox"),
    _T ("MiscButtonText"),
    _T ("Name"),
    _T ("Nop"),
    _T ("OutFile"),
    _T ("PackEXEHeader"),
    _T ("Page"),
    _T ("PageCallbacks"),
    _T ("PageEx"),
    _T ("Pop"),
    _T ("Push"),
    _T ("Quit"),
    _T ("ReadEnvStr"),
    _T ("ReadINIStr"),
    _T ("ReadRegDWORD"),
    _T ("ReadRegStr"),
    _T ("Reboot"),
    _T ("RegDLL"),
    _T ("Rename"),
    _T ("ReserveFile"),
    _T ("Return"),
    _T ("RMDir"),
    _T ("SearchPath"),
    _T ("Section"),
    _T ("SectionDivider"),
    _T ("SectionEnd"),
    _T ("SectionGetFlags"),
    _T ("SectionGetInstTypes"),
    _T ("SectionGetSize"),
    _T ("SectionGetText"),
    _T ("SectionGroup"),
    _T ("SectionGroupEnd"),
    _T ("SectionIn"),
    _T ("SectionSetFlags"),
    _T ("SectionSetInstTypes"),
    _T ("SectionSetSize"),
    _T ("SectionSetText"),
    _T ("SendMessage"),
    _T ("SetAutoClose"),
    _T ("SetBrandingImage"),
    _T ("SetCompress"),
    _T ("SetCompressionLevel"),
    _T ("SetCompressor"),
    _T ("SetCompressorDictSize"),
    _T ("SetCtlColors"),
    _T ("SetCurInstType"),
    _T ("SetDatablockOptimize"),
    _T ("SetDateSave"),
    _T ("SetDetailsPrint"),
    _T ("SetDetailsView"),
    _T ("SetErrorLevel"),
    _T ("SetErrors"),
    _T ("SetFileAttributes"),
    _T ("SetFont"),
    _T ("SetOutPath"),
    _T ("SetOverwrite"),
    _T ("SetPluginUnload"),
    _T ("SetRebootFlag"),
    _T ("SetShellVarContext"),
    _T ("SetSilent"),
    _T ("SetStaticBkColor"),
    _T ("SetWindowLong"),
    _T ("ShowInstDetails"),
    _T ("ShowUninstDetails"),
    _T ("ShowWindow"),
    _T ("SilentInstall"),
    _T ("SilentUnInstall"),
    _T ("Sleep"),
    _T ("SpaceTexts"),
    _T ("StrCmp"),
    _T ("StrCpy"),
    _T ("StrLen"),
    _T ("StrStr"),
    _T ("SubCaption"),
    _T ("SubSection"),
    _T ("SubSectionEnd"),
    _T ("TrimNewlines"),
    _T ("UninstallButtonText"),
    _T ("UninstallCaption"),
    _T ("UninstallExeName"),
    _T ("UninstallIcon"),
    _T ("UninstallSubCaption"),
    _T ("UninstallText"),
    _T ("UninstPage"),
    _T ("UnRegDLL"),
    _T ("UpgradeDLL"),
    _T ("Var"),
    _T ("VIAddVersionKey"),
    _T ("VIProductVersion"),
    _T ("WindowIcon"),
    _T ("WriteINIStr"),
    _T ("WriteRegBin"),
    _T ("WriteRegDWORD"),
    _T ("WriteRegExpandStr"),
    _T ("WriteRegStr"),
    _T ("WriteUninstaller"),
    _T ("XPStyle"),
    NULL
  };

static LPTSTR s_apszUser1KeywordList[] =
  {
/*
    _T ("$0"),
    _T ("$1"),
    _T ("$2"),
    _T ("$3"),
    _T ("$4"),
    _T ("$5"),
    _T ("$6"),
    _T ("$7"),
    _T ("$8"),
    _T ("$9"),
    _T ("$ADMINTOOLS"),
    _T ("$APPDATA"),
    _T ("$CDBURN_AREA"),
    _T ("$CMDLINE"),
    _T ("$COMMONFILES"),
    _T ("$COOKIES"),
    _T ("$DESKTOP"),
    _T ("$DOCUMENTS"),
    _T ("$EXEDIR"),
    _T ("$FAVORITES"),
    _T ("$FONTS"),
    _T ("$HISTORY"),
    _T ("$HWNDPARENT"),
    _T ("$INSTDIR"),
    _T ("$INTERNET_CACHE"),
    _T ("$LANGUAGE"),
    _T ("$MUSIC"),
    _T ("$NETHOOD"),
    _T ("$OUTDIR"),
    _T ("$PICTURES"),
    _T ("$PLUGINSDIR"),
    _T ("$PRINTHOOD"),
    _T ("$PROFILE"),
    _T ("$PROGRAMFILES"),
    _T ("$QUICKLAUNCH"),
    _T ("$R0"),
    _T ("$R1"),
    _T ("$R2"),
    _T ("$R3"),
    _T ("$R4"),
    _T ("$R5"),
    _T ("$R6"),
    _T ("$R7"),
    _T ("$R8"),
    _T ("$R9"),
    _T ("$RECENT"),
    _T ("$RESOURCES"),
    _T ("$RESOURCES_LOCALIZED"),
    _T ("$SENDTO"),
    _T ("$SMPROGRAMS"),
    _T ("$SMSTARTUP"),
    _T ("$STARTMENU"),
    _T ("$SYSDIR"),
    _T ("$TEMP"),
    _T ("$TEMPLATES"),
    _T ("$VIDEOS"),
    _T ("$WINDIR"),
*/
    _T ("alwaysoff"),
    _T ("ARCHIVE"),
    _T ("auto"),
    _T ("both"),
    _T ("bottom"),
    _T ("bzip2"),
    _T ("components"),
    _T ("custom"),
    _T ("directory"),
    _T ("false"),
    _T ("FILE_ATTRIBUTE_ARCHIVE"),
    _T ("FILE_ATTRIBUTE_HIDDEN"),
    _T ("FILE_ATTRIBUTE_NORMAL"),
    _T ("FILE_ATTRIBUTE_OFFLINE"),
    _T ("FILE_ATTRIBUTE_READONLY"),
    _T ("FILE_ATTRIBUTE_SYSTEM"),
    _T ("FILE_ATTRIBUTE_TEMPORARY"),
    _T ("force"),
    _T ("HIDDEN"),
    _T ("hide"),
    _T ("HKCC"),
    _T ("HKCR"),
    _T ("HKCU"),
    _T ("HKDD"),
    _T ("HKEY_CLASSES_ROOT"),
    _T ("HKEY_CURRENT_CONFIG"),
    _T ("HKEY_CURRENT_USER"),
    _T ("HKEY_DYN_DATA"),
    _T ("HKEY_LOCAL_MACHINE"),
    _T ("HKEY_PERFORMANCE_DATA"),
    _T ("HKEY_USERS"),
    _T ("HKLM"),
    _T ("HKPD"),
    _T ("HKU"),
    _T ("IDABORT"),
    _T ("IDCANCEL"),
    _T ("IDIGNORE"),
    _T ("IDNO"),
    _T ("IDOK"),
    _T ("IDRETRY"),
    _T ("IDYES"),
    _T ("ifdiff"),
    _T ("ifnewer"),
    _T ("instfiles"),
    _T ("lastused"),
    _T ("leave"),
    _T ("left"),
    _T ("license"),
    _T ("listonly"),
    _T ("lzma"),
    _T ("manual"),
    _T ("MB_ABORTRETRYIGNORE"),
    _T ("MB_DEFBUTTON1"),
    _T ("MB_DEFBUTTON2"),
    _T ("MB_DEFBUTTON3"),
    _T ("MB_DEFBUTTON4"),
    _T ("MB_ICONEXCLAMATION"),
    _T ("MB_ICONINFORMATION"),
    _T ("MB_ICONQUESTION"),
    _T ("MB_ICONSTOP"),
    _T ("MB_ICONSTOP"),
    _T ("MB_OK"),
    _T ("MB_OKCANCEL"),
    _T ("MB_RETRYCANCEL"),
    _T ("MB_RIGHT"),
    _T ("MB_RTLREADING"),
    _T ("MB_SETFOREGROUND"),
    _T ("MB_TOPMOST"),
    _T ("MB_YESNO"),
    _T ("MB_YESNO"),
    _T ("MB_YESNOCANCEL"),
    _T ("nevershow"),
    _T ("none"),
    _T ("normal"),
    _T ("NORMAL"),
    _T ("off"),
    _T ("OFFLINE"),
    _T ("on"),
    _T ("READONLY"),
    _T ("right"),
    _T ("RO"),
    _T ("show"),
    _T ("silent"),
    _T ("silentlog"),
    _T ("SW_HIDE"),
    _T ("SW_SHOWMAXIMIZED"),
    _T ("SW_SHOWMINIMIZED"),
    _T ("SW_SHOWNORMAL"),
    _T ("SYSTEM"),
    _T ("TEMPORARY"),
    _T ("textonly"),
    _T ("top"),
    _T ("true"),
    _T ("try"),
    _T ("uninstConfirm"),
    _T ("zlib"),
    NULL
  };

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != NULL; L++)
    {
      if (_tcsncmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsNsisKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszNsisKeywordList, pszChars, nLength);
}

static BOOL
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszUser1KeywordList, pszChars, nLength);
}

static BOOL
IsNsisNumber (LPCTSTR pszChars, int nLength)
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
ParseLineNsis (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bWasCommentStart = FALSE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  for (int I = 0;; nPrevI = I, I = CharNext(pszChars+I) - pszChars)
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
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*::CharPrev(pszChars, pszChars + nPos)) && !xisalpha (*::CharNext(pszChars + nPos))))
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

      //  Extended comment /*....*/
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          // if (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*')
          if ((I > 1 && pszChars[I] == '/' && pszChars[nPrevI] == '*' /*&& *::CharPrev(pszChars, pszChars + nPrevI) != '/'*/ && !bWasCommentStart) || (I == 1 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = TRUE;
            }
          bWasCommentStart = FALSE;
          continue;
        }

      //if (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/')
      if (I > 0 && pszChars[nPrevI] == ';')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Preprocessor directive #....
      if (dwCookie & COOKIE_PREPROCESSOR)
        {
          if (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/')
            {
              DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_EXT_COMMENT;
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
      if (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          bWasCommentStart = TRUE;
          continue;
        }

      bWasCommentStart = FALSE;

      if (bFirstChar)
        {
          if (pszChars[I] == '!')
            {
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

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[nPrevI]) && !xisalpha (pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsNsisKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              else if (IsNsisNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = FALSE;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '(')
                            {
                              bFunction = TRUE;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
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
      if (IsNsisKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsNsisNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = TRUE;
                    }
                  break;
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  if (pszChars[nLength - 1] != '\\' || m_pTextBuffer->IsMBSTrail(nLineIndex, nLength - 1))
    dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
