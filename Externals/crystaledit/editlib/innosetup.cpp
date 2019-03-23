///////////////////////////////////////////////////////////////////////////
//  File:    pascal.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Pascal syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "crystallineparser.h"
#include "SyntaxColors.h"
#include "string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPCTSTR s_apszInnoSetupKeywordList[] =
  {
    _T ("AdminPrivilegesRequired"),
    _T ("AfterInstall"),
    _T ("AllowCancelDuringInstall"),
    _T ("AllowNoIcons"),
    _T ("AllowRootDirectory"),
    _T ("AllowUNCPath"),
    _T ("AlwaysRestart"),
    _T ("AlwaysShowComponentsList"),
    _T ("AlwaysShowDirOnReadyPage"),
    _T ("AlwaysShowGroupOnReadyPage"),
    _T ("AlwaysUsePersonalGroup"),
    _T ("and"),
    _T ("AppComments"),
    _T ("AppContact"),
    _T ("AppCopyright"),
    _T ("AppendDefaultDirName"),
    _T ("AppendDefaultGroupName"),
    _T ("AppId"),
    _T ("AppModifyPath"),
    _T ("AppMutex"),
    _T ("AppName"),
    _T ("AppPublisher"),
    _T ("AppPublisherURL"),
    _T ("AppReadmeFile"),
    _T ("AppSupportURL"),
    _T ("AppUpdatesURL"),
    _T ("AppVerName"),
    _T ("AppVersion"),
    _T ("array"),
    _T ("asm"),
    _T ("assembler"),
    _T ("Attribs"),
    _T ("BackColor"),
    _T ("BackColor2"),
    _T ("BackColorDirection"),
    _T ("BackSolid"),
    _T ("BeforeInstall"),
    _T ("begin"),
    _T ("case"),
    _T ("ChangesAssociations"),
    _T ("ChangesEnvironment"),
    _T ("Check"),
    _T ("CodeFile"),
    _T ("Comment"),
    _T ("Components"),
    _T ("Compression"),
    _T ("const"),
    _T ("constructor"),
    _T ("CopyMode"),
    _T ("CreateAppDir"),
    _T ("CreateUninstallRegKey"),
    _T ("DefaultDirName"),
    _T ("DefaultGroupName"),
    _T ("DefaultUserInfoName"),
    _T ("DefaultUserInfoOrg"),
    _T ("DefaultUserInfoSerial"),
    _T ("Description"),
    _T ("DestDir"),
    _T ("DestName"),
    _T ("destructor"),
    _T ("DirExistsWarning"),
    _T ("DisableDirPage"),
    _T ("DisableFinishedPage"),
    _T ("DisableProgramGroupPage"),
    _T ("DisableReadyMemo"),
    _T ("DisableReadyPage"),
    _T ("DisableStartupPrompt"),
    _T ("DiskClusterSize"),
    _T ("DiskSliceSize"),
    _T ("DiskSpaceMBLabel"),
    _T ("DiskSpanning"),
    _T ("div"),
    _T ("do"),
    _T ("DontMergeDuplicateFiles"),
    _T ("downto"),
    _T ("else"),
    _T ("EnableDirDoesntExistWarning"),
    _T ("Encryption"),
    _T ("end"),
    _T ("Excludes"),
    _T ("exit"),
    _T ("external"),
    _T ("ExtraDiskSpaceRequired"),
    _T ("far"),
    _T ("file"),
    _T ("Filename"),
    _T ("Flags"),
    _T ("FlatComponentsList"),
    _T ("FontInstall"),
    _T ("for"),
    _T ("function"),
    _T ("goto"),
    _T ("GroupDescription"),
    _T ("HotKey"),
    _T ("IconFilename"),
    _T ("IconIndex"),
    _T ("if"),
    _T ("implementation"),
    _T ("InfoAfterFile"),
    _T ("InfoBeforeFile"),
    _T ("inherited"),
    _T ("inline"),
    _T ("interface"),
    _T ("InternalCompressLevel"),
    _T ("Key"),
    _T ("label"),
    _T ("LanguageDetectionMethod"),
    _T ("Languages"),
    _T ("LicenseFile"),
    _T ("MergeDuplicateFiles"),
    _T ("MessagesFile"),
    _T ("MinVersion"),
    _T ("mod"),
    _T ("Name"),
    _T ("near"),
    _T ("nil"),
    _T ("not"),
    _T ("object"),
    _T ("of"),
    _T ("OnlyBelowVersion"),
    _T ("or"),
    _T ("OutputBaseFilename"),
    _T ("OutputDir"),
    _T ("OutputManifestFile"),
    _T ("Parameters"),
    _T ("Password"),
    _T ("Permissions"),
    _T ("PrivilegesRequired"),
    _T ("procedure"),
    _T ("program"),
    _T ("record"),
    _T ("repeat"),
    _T ("ReserveBytes"),
    _T ("RestartIfNeededByRun"),
    _T ("Root"),
    _T ("RunOnceId"),
    _T ("Section"),
    _T ("set"),
    _T ("SetupIconFile"),
    _T ("ShowComponentSizes"),
    _T ("ShowLanguageDialog"),
    _T ("ShowTasksTreeLines"),
    _T ("SlicesPerDisk"),
    _T ("SolidCompression"),
    _T ("Source"),
    _T ("SourceDir"),
    _T ("StatusMsg"),
    _T ("string"),
    _T ("Subkey"),
    _T ("Tasks"),
    _T ("then"),
    _T ("TimeStampRounding"),
    _T ("TimeStampsInUTC"),
    _T ("to"),
    _T ("TouchDate"),
    _T ("TouchTime"),
    _T ("type"),
    _T ("Type"),
    _T ("Types"),
    _T ("Uninstallable"),
    _T ("UninstallDisplayIcon"),
    _T ("UninstallDisplayName"),
    _T ("UninstallFilesDir"),
    _T ("UninstallIconFile"),
    _T ("UninstallLogMode"),
    _T ("UninstallRestartComputer"),
    _T ("UninstallStyle"),
    _T ("unit"),
    _T ("until"),
    _T ("UpdateUninstallLogAppName"),
    _T ("UsePreviousAppDir"),
    _T ("UsePreviousGroup"),
    _T ("UsePreviousSetupType"),
    _T ("UsePreviousTasks"),
    _T ("UsePreviousUserInfo"),
    _T ("UserInfoPage"),
    _T ("uses"),
    _T ("UseSetupLdr"),
    _T ("ValueData"),
    _T ("ValueName"),
    _T ("ValueType"),
    _T ("var"),
    _T ("VersionInfoCompany"),
    _T ("VersionInfoDescription"),
    _T ("VersionInfoTextVersion"),
    _T ("VersionInfoVersion"),
    _T ("virtual"),
    _T ("while"),
    _T ("WindowResizable"),
    _T ("WindowShowCaption"),
    _T ("WindowStartMaximized"),
    _T ("WindowVisible"),
    _T ("with"),
    _T ("WizardImageBackColor"),
    _T ("WizardImageFile"),
    _T ("WizardImageStretch"),
    _T ("WizardSmallImageBackColor"),
    _T ("WizardSmallImageFile"),
    _T ("WizardStyle"),
    _T ("WorkingDir"),
    _T ("xor"),
  };

static LPCTSTR s_apszUser1KeywordList[] =
  {
    _T ("alwaysoverwrite"),
    _T ("alwaysskipifsameorolder"),
    _T ("append"),
    _T ("binary"),
    _T ("classic"),
    _T ("closeonexit"),
    _T ("comparetimestamp"),
    _T ("confirmoverwrite"),
    _T ("createkeyifdoesntexist"),
    _T ("createonlyiffileexists"),
    _T ("createvalueifdoesntexist"),
    _T ("deleteafterinstall"),
    _T ("deletekey"),
    _T ("deletevalue"),
    _T ("dirifempty"),
    _T ("disablenouninstallwarning"),
    _T ("dontcloseonexit"),
    _T ("dontcopy"),
    _T ("dontcreatekey"),
    _T ("dword"),
    _T ("exclusive"),
    _T ("expandsz"),
    _T ("external"),
    _T ("files"),
    _T ("filesandordirs"),
    _T ("fixed"),
    _T ("fontisnttruetype"),
    _T ("HKCC"),
    _T ("HKCR"),
    _T ("HKCU"),
    _T ("HKLM"),
    _T ("HKU"),
    _T ("ignoreversion"),
    _T ("iscustom"),
    _T ("isreadme"),
    _T ("modern"),
    _T ("multisz"),
    _T ("new"),
    _T ("noerror"),
    _T ("none"),
    _T ("normal"),
    _T ("nowait"),
    _T ("onlyifdestfileexists"),
    _T ("onlyifdoesntexist"),
    _T ("onlyifnewer"),
    _T ("overwrite"),
    _T ("overwritereadonly"),
    _T ("postinstall"),
    _T ("preservestringtype"),
    _T ("promptifolder"),
    _T ("regserver"),
    _T ("regtypelib"),
    _T ("restart"),
    _T ("restartreplace"),
    _T ("runhidden"),
    _T ("runmaximized"),
    _T ("runminimized"),
    _T ("sharedfile"),
    _T ("shellexec"),
    _T ("showcheckbox"),
    _T ("silent"),
    _T ("skipifdoesntexist"),
    _T ("skipifnotsilent"),
    _T ("skipifsilent"),
    _T ("skipifsourcedoesntexist"),
    _T ("sortfilesbyextension"),
    _T ("unchecked"),
    _T ("uninsalwaysuninstall"),
    _T ("uninsclearvalue"),
    _T ("uninsdeleteentry"),
    _T ("uninsdeletekey"),
    _T ("uninsdeletekeyifempty"),
    _T ("uninsdeletesection"),
    _T ("uninsdeletesectionifempty"),
    _T ("uninsdeletevalue"),
    _T ("uninsneveruninstall"),
    _T ("useapppaths"),
    _T ("verysilent"),
    _T ("waituntilidle"),
  };

static bool
IsInnoSetupKeyword (LPCTSTR pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszInnoSetupKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

static bool
IsInnoSetupNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return false;
        }
      return true;
    }
  if (!_istdigit (pszChars[0]))
    return false;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return false;
    }
  return true;
}

DWORD
CrystalLineParser::ParseLineInnoSetup (DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_EXT_COMMENT2);

  bool bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(::CharNext(pszChars+I) - pszChars))
    {
      if (I == nPrevI)
        {
          // CharNext did not advance, so we're at the end of the string
          // and we already handled this character, so stop
          break;
        }

      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT | COOKIE_EXT_COMMENT2))
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
          else if (dwCookie & COOKIE_SECTION)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_FUNCNAME);
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
                  bRedefineBlock = true;
                  bDecIndex = true;
                  goto out;
                }
            }
          bRedefineBlock = false;
          bDecIndex = false;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength || pszChars[I] == 0)
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
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment (*....*)
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          // if (I > 0 && pszChars[I] == ')' && pszChars[nPrevI] == '*')
          if ((I > 1 && pszChars[I] == ')' && pszChars[nPrevI] == '*' && *::CharPrev(pszChars, pszChars + nPrevI) != '(') || (I == 1 && pszChars[I] == ')' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment {....}
      if (dwCookie & COOKIE_EXT_COMMENT2)
        {
          if (pszChars[I] == '}')
            {
              dwCookie &= ~COOKIE_EXT_COMMENT2;
              bRedefineBlock = true;
            }
          continue;
        }

      if (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      // Section header [...]
      if (dwCookie & COOKIE_SECTION)
        {
          if (pszChars[I] == ']')
            {
              dwCookie &= ~COOKIE_SECTION;
              bRedefineBlock = true;
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
      if (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '(')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          continue;
        }

      if (pszChars[I] == '{')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT2;
          continue;
        }

      if (bFirstChar)
        {
          if (pszChars[I] == ';')
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_COMMENT;
              continue;
            }
          if (pszChars[I] == '#')
            {
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              continue;
            }
          if (pszChars[I] == '[')
            {
              DEFINE_BLOCK (I, COLORINDEX_FUNCNAME);
              dwCookie |= COOKIE_SECTION;
              continue;
            }
          if (!xisspace (pszChars[I]))
            bFirstChar = false;
        }

      if (pBuf == nullptr)
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
              if (IsInnoSetupKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              else if (IsInnoSetupNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = false;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '(')
                            {
                              bFunction = true;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsInnoSetupKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsInnoSetupNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = false;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = true;
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

  if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
    dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_EXT_COMMENT2);
  return dwCookie;
}
