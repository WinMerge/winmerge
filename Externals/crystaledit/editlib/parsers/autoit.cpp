///////////////////////////////////////////////////////////////////////////
//  File:    autoit.cpp
//  Version: 1.1.0.6
//  Updated: 26-Apr-2021
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  AutoIt syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  (Visual) AutoIt keywords
static const tchar_t * s_apszAutoItKeywordList[] =
  {
    _T ("Abs"),
    _T ("ACos"),
    _T ("AdlibRegister"),
    _T ("AdlibUnRegister"),
    _T ("And"),
    _T ("Asc"),
    _T ("AscW"),
    _T ("ASin"),
    _T ("Assign"),
    _T ("ATan"),
    _T ("AutoItSetOption"),
    _T ("AutoItWinGetTitle"),
    _T ("AutoItWinSetTitle"),
    _T ("Beep"),
    _T ("Binary"),
    _T ("BinaryLen"),
    _T ("BinaryMid"),
    _T ("BinaryToString"),
    _T ("BitAND"),
    _T ("BitNOT"),
    _T ("BitOR"),
    _T ("BitRotate"),
    _T ("BitShift"),
    _T ("BitXOR"),
    _T ("BlockInput"),
    _T ("Break"),
    _T ("ByRef"),
    _T ("Call"),
    _T ("Case"),
    _T ("CDTray"),
    _T ("Ceiling"),
    _T ("Chr"),
    _T ("ChrW"),
    _T ("ClipGet"),
    _T ("ClipPut"),
    _T ("ConsoleRead"),
    _T ("ConsoleWrite"),
    _T ("ConsoleWriteError"),
    _T ("Const"),
    _T ("ContinueCase"),
    _T ("ContinueLoop"),
    _T ("ControlClick"),
    _T ("ControlCommand"),
    _T ("ControlDisable"),
    _T ("ControlEnable"),
    _T ("ControlFocus"),
    _T ("ControlGetFocus"),
    _T ("ControlGetHandle"),
    _T ("ControlGetPos"),
    _T ("ControlGetText"),
    _T ("ControlHide"),
    _T ("ControlListView"),
    _T ("ControlMove"),
    _T ("ControlSend"),
    _T ("ControlSetText"),
    _T ("ControlShow"),
    _T ("ControlTreeView"),
    _T ("Cos"),
    _T ("Dec"),
    _T ("Default"),
    _T ("Dim"),
    _T ("DirCopy"),
    _T ("DirCreate"),
    _T ("DirGetSize"),
    _T ("DirMove"),
    _T ("DirRemove"),
    _T ("DllCall"),
    _T ("DllCallAddress"),
    _T ("DllCallbackFree"),
    _T ("DllCallbackGetPtr"),
    _T ("DllCallbackRegister"),
    _T ("DllClose"),
    _T ("DllOpen"),
    _T ("DllStructCreate"),
    _T ("DllStructGetData"),
    _T ("DllStructGetPtr"),
    _T ("DllStructGetSize"),
    _T ("DllStructGetString"),
    _T ("DllStructSetData"),
    _T ("Do"),
    _T ("DriveGetDrive"),
    _T ("DriveGetFileSystem"),
    _T ("DriveGetLabel"),
    _T ("DriveGetSerial"),
    _T ("DriveGetType"),
    _T ("DriveMapAdd"),
    _T ("DriveMapDel"),
    _T ("DriveMapGet"),
    _T ("DriveSetLabel"),
    _T ("DriveSpaceFree"),
    _T ("DriveSpaceTotal"),
    _T ("DriveStatus"),
    _T ("Else"),
    _T ("ElseIf"),
    _T ("EndFunc"),
    _T ("EndIf"),
    _T ("EndSelect"),
    _T ("EndSwitch"),
    _T ("EndWith"),
    _T ("Enum"),
    _T ("EnvGet"),
    _T ("EnvSet"),
    _T ("EnvUpdate"),
    _T ("Eval"),
    _T ("Execute"),
    _T ("Exit"),
    _T ("ExitLoop"),
    _T ("Exp"),
    _T ("False"),
    _T ("FileChangeDir"),
    _T ("FileClose"),
    _T ("FileCopy"),
    _T ("FileCreateNTFSLink"),
    _T ("FileCreateShortcut"),
    _T ("FileDelete"),
    _T ("FileExists"),
    _T ("FileFindFirstFile"),
    _T ("FileFindNextFile"),
    _T ("FileFlush"),
    _T ("FileGetAttrib"),
    _T ("FileGetEncoding"),
    _T ("FileGetLongName"),
    _T ("FileGetPos"),
    _T ("FileGetShortcut"),
    _T ("FileGetShortName"),
    _T ("FileGetSize"),
    _T ("FileGetTime"),
    _T ("FileGetVersion"),
    _T ("FileInstall"),
    _T ("FileMove"),
    _T ("FileOpen"),
    _T ("FileOpenDialog"),
    _T ("FileRead"),
    _T ("FileReadLine"),
    _T ("FileReadToArray"),
    _T ("FileRecycle"),
    _T ("FileRecycleEmpty"),
    _T ("FileSaveDialog"),
    _T ("FileSelectFolder"),
    _T ("FileSetAttrib"),
    _T ("FileSetEnd"),
    _T ("FileSetPos"),
    _T ("FileSetTime"),
    _T ("FileWrite"),
    _T ("FileWriteLine"),
    _T ("Floor"),
    _T ("For"),
    _T ("FtpSetProxy"),
    _T ("Func"),
    _T ("FuncGetStack"),
    _T ("FuncName"),
    _T ("Global"),
    _T ("GUICreate"),
    _T ("GUICtrlCreateAvi"),
    _T ("GUICtrlCreateButton"),
    _T ("GUICtrlCreateCheckbox"),
    _T ("GUICtrlCreateCombo"),
    _T ("GUICtrlCreateContextMenu"),
    _T ("GUICtrlCreateDate"),
    _T ("GUICtrlCreateDummy"),
    _T ("GUICtrlCreateEdit"),
    _T ("GUICtrlCreateGraphic"),
    _T ("GUICtrlCreateGroup"),
    _T ("GUICtrlCreateIcon"),
    _T ("GUICtrlCreateInput"),
    _T ("GUICtrlCreateLabel"),
    _T ("GUICtrlCreateList"),
    _T ("GUICtrlCreateListView"),
    _T ("GUICtrlCreateListViewItem"),
    _T ("GUICtrlCreateMenu"),
    _T ("GUICtrlCreateMenuItem"),
    _T ("GUICtrlCreateMonthCal"),
    _T ("GUICtrlCreateObj"),
    _T ("GUICtrlCreatePic"),
    _T ("GUICtrlCreateProgress"),
    _T ("GUICtrlCreateRadio"),
    _T ("GUICtrlCreateSlider"),
    _T ("GUICtrlCreateTab"),
    _T ("GUICtrlCreateTabItem"),
    _T ("GUICtrlCreateTreeView"),
    _T ("GUICtrlCreateTreeViewItem"),
    _T ("GUICtrlCreateUpdown"),
    _T ("GUICtrlDelete"),
    _T ("GUICtrlGetHandle"),
    _T ("GUICtrlGetState"),
    _T ("GUICtrlRead"),
    _T ("GUICtrlRecvMsg"),
    _T ("GUICtrlRegisterListViewSort"),
    _T ("GUICtrlSendMsg"),
    _T ("GUICtrlSendToDummy"),
    _T ("GUICtrlSetBkColor"),
    _T ("GUICtrlSetColor"),
    _T ("GUICtrlSetCursor"),
    _T ("GUICtrlSetData"),
    _T ("GUICtrlSetDefBkColor"),
    _T ("GUICtrlSetDefColor"),
    _T ("GUICtrlSetFont"),
    _T ("GUICtrlSetGraphic"),
    _T ("GUICtrlSetImage"),
    _T ("GUICtrlSetLimit"),
    _T ("GUICtrlSetOnEvent"),
    _T ("GUICtrlSetPos"),
    _T ("GUICtrlSetResizing"),
    _T ("GUICtrlSetState"),
    _T ("GUICtrlSetStyle"),
    _T ("GUICtrlSetTip"),
    _T ("GUIDelete"),
    _T ("GUIGetCursorInfo"),
    _T ("GUIGetMsg"),
    _T ("GUIGetStyle"),
    _T ("GUIRegisterMsg"),
    _T ("GUISetAccelerators"),
    _T ("GUISetBkColor"),
    _T ("GUISetCoord"),
    _T ("GUISetCursor"),
    _T ("GUISetFont"),
    _T ("GUISetHelp"),
    _T ("GUISetIcon"),
    _T ("GUISetOnEvent"),
    _T ("GUISetState"),
    _T ("GUISetStyle"),
    _T ("GUIStartGroup"),
    _T ("GUISwitch"),
    _T ("Hex"),
    _T ("HotKeySet"),
    _T ("HttpSetProxy"),
    _T ("HttpSetUserAgent"),
    _T ("HWnd"),
    _T ("If"),
    _T ("In"),
    _T ("InetClose"),
    _T ("InetGet"),
    _T ("InetGetInfo"),
    _T ("InetGetSize"),
    _T ("InetRead"),
    _T ("IniDelete"),
    _T ("IniRead"),
    _T ("IniReadSection"),
    _T ("IniReadSectionNames"),
    _T ("IniRenameSection"),
    _T ("IniWrite"),
    _T ("IniWriteSection"),
    _T ("InputBox"),
    _T ("Int"),
    _T ("IsAdmin"),
    _T ("IsArray"),
    _T ("IsBinary"),
    _T ("IsBool"),
    _T ("IsDeclared"),
    _T ("IsDllStruct"),
    _T ("IsFloat"),
    _T ("IsFunc"),
    _T ("IsHWnd"),
    _T ("IsInt"),
    _T ("IsKeyword"),
    _T ("IsMap"),
    _T ("IsNumber"),
    _T ("IsObj"),
    _T ("IsPtr"),
    _T ("IsString"),
    _T ("Local"),
    _T ("Log"),
    _T ("MapAppend"),
    _T ("MapExists"),
    _T ("MapKeys"),
    _T ("MapRemove"),
    _T ("MemGetStats"),
    _T ("Mod"),
    _T ("MouseClick"),
    _T ("MouseClickDrag"),
    _T ("MouseDown"),
    _T ("MouseGetCursor"),
    _T ("MouseGetPos"),
    _T ("MouseMove"),
    _T ("MouseUp"),
    _T ("MouseWheel"),
    _T ("MsgBox"),
    _T ("Next"),
    _T ("Not"),
    _T ("Null"),
    _T ("Number"),
    _T ("ObjCreate"),
    _T ("ObjCreateInterface"),
    _T ("ObjEvent"),
    _T ("ObjEvent"),
    _T ("ObjGet"),
    _T ("ObjName"),
    _T ("OnAutoItExitRegister"),
    _T ("OnAutoItExitUnRegister"),
    _T ("Opt"),
    _T ("Or"),
    _T ("Ping"),
    _T ("PixelChecksum"),
    _T ("PixelGetColor"),
    _T ("PixelSearch"),
    _T ("ProcessClose"),
    _T ("ProcessExists"),
    _T ("ProcessGetStats"),
    _T ("ProcessList"),
    _T ("ProcessSetPriority"),
    _T ("ProcessWait"),
    _T ("ProcessWaitClose"),
    _T ("ProgressOff"),
    _T ("ProgressOn"),
    _T ("ProgressSet"),
    _T ("Ptr"),
    _T ("Random"),
    _T ("ReDim"),
    _T ("RegDelete"),
    _T ("RegEnumKey"),
    _T ("RegEnumVal"),
    _T ("RegRead"),
    _T ("RegWrite"),
    _T ("Return"),
    _T ("Round"),
    _T ("Run"),
    _T ("RunAs"),
    _T ("RunAsWait"),
    _T ("RunWait"),
    _T ("Select"),
    _T ("Send"),
    _T ("SendKeepActive"),
    _T ("SetError"),
    _T ("SetExtended"),
    _T ("ShellExecute"),
    _T ("ShellExecuteWait"),
    _T ("Shutdown"),
    _T ("Sin"),
    _T ("Sleep"),
    _T ("SoundPlay"),
    _T ("SoundSetWaveVolume"),
    _T ("SplashImageOn"),
    _T ("SplashOff"),
    _T ("SplashTextOn"),
    _T ("Sqrt"),
    _T ("SRandom"),
    _T ("Static"),
    _T ("StatusbarGetText"),
    _T ("StderrRead"),
    _T ("StdinWrite"),
    _T ("StdioClose"),
    _T ("StdoutRead"),
    _T ("Step"),
    _T ("String"),
    _T ("StringAddCR"),
    _T ("StringCompare"),
    _T ("StringFormat"),
    _T ("StringFromASCIIArray"),
    _T ("StringInStr"),
    _T ("StringIsAlNum"),
    _T ("StringIsAlpha"),
    _T ("StringIsASCII"),
    _T ("StringIsDigit"),
    _T ("StringIsFloat"),
    _T ("StringIsInt"),
    _T ("StringIsLower"),
    _T ("StringIsSpace"),
    _T ("StringIsUpper"),
    _T ("StringIsXDigit"),
    _T ("StringLeft"),
    _T ("StringLen"),
    _T ("StringLower"),
    _T ("StringMid"),
    _T ("StringRegExp"),
    _T ("StringRegExpReplace"),
    _T ("StringReplace"),
    _T ("StringReverse"),
    _T ("StringRight"),
    _T ("StringSplit"),
    _T ("StringStripCR"),
    _T ("StringStripWS"),
    _T ("StringToASCIIArray"),
    _T ("StringToBinary"),
    _T ("StringTrimLeft"),
    _T ("StringTrimRight"),
    _T ("StringUpper"),
    _T ("Switch"),
    _T ("Tan"),
    _T ("TCPAccept"),
    _T ("TCPCloseSocket"),
    _T ("TCPConnect"),
    _T ("TCPListen"),
    _T ("TCPNameToIP"),
    _T ("TCPRecv"),
    _T ("TCPSend"),
    _T ("TCPShutdown"),
    _T ("TCPShutdownSocket"),
    _T ("TCPStartup"),
    _T ("Then"),
    _T ("TimerDiff"),
    _T ("TimerInit"),
    _T ("To"),
    _T ("ToolTip"),
    _T ("TrayCreateItem"),
    _T ("TrayCreateMenu"),
    _T ("TrayGetMsg"),
    _T ("TrayItemDelete"),
    _T ("TrayItemGetHandle"),
    _T ("TrayItemGetState"),
    _T ("TrayItemGetText"),
    _T ("TrayItemSetOnEvent"),
    _T ("TrayItemSetState"),
    _T ("TrayItemSetText"),
    _T ("TraySetClick"),
    _T ("TraySetIcon"),
    _T ("TraySetOnEvent"),
    _T ("TraySetPauseIcon"),
    _T ("TraySetState"),
    _T ("TraySetToolTip"),
    _T ("TrayTip"),
    _T ("True"),
    _T ("UBound"),
    _T ("UDPBind"),
    _T ("UDPCloseSocket"),
    _T ("UDPJoinMulticastGroup"),
    _T ("UDPOpen"),
    _T ("UDPRecv"),
    _T ("UDPSend"),
    _T ("UDPShutdown"),
    _T ("UDPStartup"),
    _T ("Until"),
    _T ("VarGetType"),
    _T ("Volatile"),
    _T ("WEnd"),
    _T ("While"),
    _T ("WinActivate"),
    _T ("WinActive"),
    _T ("WinClose"),
    _T ("WinExists"),
    _T ("WinFlash"),
    _T ("WinGetCaretPos"),
    _T ("WinGetClassList"),
    _T ("WinGetClientSize"),
    _T ("WinGetHandle"),
    _T ("WinGetPos"),
    _T ("WinGetProcess"),
    _T ("WinGetState"),
    _T ("WinGetText"),
    _T ("WinGetTitle"),
    _T ("WinKill"),
    _T ("WinList"),
    _T ("WinMenuSelectItem"),
    _T ("WinMinimizeAll"),
    _T ("WinMinimizeAllUndo"),
    _T ("WinMove"),
    _T ("WinSetOnTop"),
    _T ("WinSetState"),
    _T ("WinSetTitle"),
    _T ("WinSetTrans"),
    _T ("WinWait"),
    _T ("WinWaitActive"),
    _T ("WinWaitClose"),
    _T ("WinWaitNotActive"),
    _T ("With"),
  };

static bool
IsAutoItKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszAutoItKeywordList, pszChars, nLength);
}

static inline void
DefineIdentiferBlock(const tchar_t *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I)
{
  if (IsAutoItKeyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
    }
  else if (CrystalLineParser::IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
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

unsigned
CrystalLineParser::ParseLineAutoIt (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  bool bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars+I) - pszChars))
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
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK(nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & (COOKIE_USER1 ))
            {
              DEFINE_BLOCK(nPos, COLORINDEX_USER1);
            }
          else if (dwCookie & (COOKIE_VARIABLE))
            {
              DEFINE_BLOCK(nPos, COLORINDEX_USER2);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha (*tc::tcharnext(pszChars + nPos))))
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

      //  String constant '....'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'')
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
            }
          continue;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"')
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment #cs .... #ce
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if (bFirstChar && pszChars[I] == '#' &&
              ((I +  3 <= nLength && memcmp(&pszChars[I], _T("#ce"),            3 * sizeof(tchar_t)) == 0) ||
               (I +  3 <= nLength && memcmp(&pszChars[I], _T("#CE"),            3 * sizeof(tchar_t)) == 0) ||
               (I + 13 <= nLength && memcmp(&pszChars[I], _T("#comments-end"), 13 * sizeof(tchar_t)) == 0)))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              bFirstChar = false;
              I += ((pszChars[I + 2] == 'e') || (pszChars[I + 2] == 'E')) ? 2 : 12;
            }
          if (!xisspace (pszChars[I]))
            bFirstChar = false;
          continue;
        }

      if (pszChars[I] == ';')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Preprocessor directive #....
      if (dwCookie & COOKIE_PREPROCESSOR)
        {
          continue;
        }
      // Variable begins
      
      // Variable begins
      if (pszChars[I] == '@')
        {
          DEFINE_BLOCK(I, COLORINDEX_USER1);
          dwCookie |= COOKIE_USER1;
          continue;
        }

      // Variable ends
      if (dwCookie & COOKIE_USER1)
        {
          if (!xisalnum(pszChars[I]))
            {
              dwCookie &= ~COOKIE_USER1;
              bRedefineBlock = true;
              bDecIndex = true;
            }
          continue;
        }
      
      if (pszChars[I] == '$')
        {
          DEFINE_BLOCK(I, COLORINDEX_USER2);
          dwCookie |= COOKIE_VARIABLE;
          continue;
        }

      // Variable ends
      if (dwCookie & COOKIE_VARIABLE)
        {
          if (!xisalnum(pszChars[I]))
            {
              dwCookie &= ~COOKIE_VARIABLE;
              bRedefineBlock = true;
              bDecIndex = true;
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

      //  Normal text
      if (pszChars[I] == '\'')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_CHAR;
          continue;
        }

      if (bFirstChar)
        {
          if (pszChars[I] == '#')
            {
              if ((I +  3 <= nLength && memcmp(&pszChars[I], _T("#cs"),              3 * sizeof(tchar_t)) == 0) ||
                  (I +  3 <= nLength && memcmp(&pszChars[I], _T("#CS"),              3 * sizeof(tchar_t)) == 0) ||
                  (I + 15 <= nLength && memcmp(&pszChars[I], _T("#comments-start"), 15 * sizeof(tchar_t)) == 0))
                {
                  DEFINE_BLOCK (I, COLORINDEX_COMMENT);
                  dwCookie |= COOKIE_EXT_COMMENT;
                }
              else
                {
                  DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
                  dwCookie |= COOKIE_PREPROCESSOR;
                }
              bFirstChar = false;
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
              DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
