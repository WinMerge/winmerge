///////////////////////////////////////////////////////////////////////////
//  File:    registry.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Some handy stuff to deal with Windows registry
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  14-Sep-99
//  + FIX: closing null handle (Michael A. Barnhart)
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*////////////////////////////////////////////////////////////////////////////*/

/* initializes registry value data */
void
RegValInit (RegVal *pValData)
{
  ASSERT (pValData != nullptr);
  pValData->dwType = REG_NONE;
}

/* frees registry value data */
void
RegValFree (RegVal *pValData)
{
  ASSERT (pValData != nullptr);
  if (pValData->dwType == REG_SZ || pValData->dwType == REG_EXPAND_SZ
        || pValData->dwType == REG_LINK || pValData->dwType == REG_MULTI_SZ
        || pValData->dwType == REG_BINARY)
    {
      free (pValData->pbyteData);
      pValData->dwType = REG_NONE;
    }
}

/* get a number */
bool
RegValGetNumber (const RegVal *pValData, DWORD *pdwNumber)
{
  ASSERT (pValData != nullptr && pdwNumber != nullptr);
  if (pValData->dwType == REG_DWORD)
    {
      *pdwNumber = pValData->dwNumber;
      return true;
    }
  return false;
}

/* get binary data */
bool
RegValGetBinary (const RegVal *pValData, LPBYTE pbyteData, DWORD dwSize)
{
  ASSERT (pValData != nullptr && pbyteData != nullptr);
  if (pValData->dwType == REG_BINARY &&dwSize >= pValData->dwSize)
    {
      memcpy (pbyteData, pValData->pbyteData, pValData->dwSize);
      return true;
    }
  return false;
}

/* get new binary data */
bool
RegValGetNewBinary (const RegVal *pValData, LPBYTE *pbyteData, DWORD *pdwSize)
{
  ASSERT (pValData != nullptr && pbyteData != nullptr);
  if (pValData->dwType == REG_BINARY)
    {
      LPBYTE pbyteNewData = (LPBYTE) malloc (pValData->dwSize);
      if (pbyteNewData != nullptr)
        {
          *pbyteData = pbyteNewData;
          *pdwSize = pValData->dwSize;
          memcpy (pbyteNewData, pValData->pbyteData, pValData->dwSize);
          return true;
        }
    }
  return false;
}

/* get a new string */
bool
RegValGetNewString (const RegVal *pValData, LPTSTR *pszString, DWORD *pdwLength)
{
  ASSERT (pValData != nullptr && pszString != nullptr);
  if (pValData->dwType == REG_SZ || pValData->dwType == REG_EXPAND_SZ
        || pValData->dwType == REG_LINK || pValData->dwType == REG_MULTI_SZ)
    {
      LPTSTR pszNewString = (LPTSTR) malloc (pValData->dwLength + 1);
      if (pszNewString != nullptr)
        {
          *pszString = pszNewString;
          if (pdwLength != nullptr)
            {
              *pdwLength = pValData->dwLength;
            }
          memcpy (pszNewString, pValData->pszString, pValData->dwLength);
          pszNewString [pValData->dwLength] = _T ('\0');
          return true;
        }
    }
  return false;
}

/* get a string */
bool
RegValGetString (const RegVal *pValData, LPTSTR pszString, DWORD dwLength)
{
  ASSERT (pValData != nullptr && pszString != nullptr);
  if ((pValData->dwType == REG_SZ || pValData->dwType == REG_EXPAND_SZ
         || pValData->dwType == REG_LINK || pValData->dwType == REG_MULTI_SZ)
        && dwLength >= pValData->dwLength)
    {
      memcpy (pszString, pValData->pszString, pValData->dwLength);
      pszString [pValData->dwLength] = _T ('\0');
      return true;
    }
  return false;
}

/* get an array of strings */
bool
RegValGetStringArr (const RegVal *pValData, LPTSTR pszStrings[], DWORD dwCount)
{
  ASSERT (pValData != nullptr);
  if (pValData->dwType == REG_MULTI_SZ)
    {
      LPCTSTR pszString0;
      DWORD dwRealCount = 0, dwLength;
      for (pszString0 = pValData->pszString; *pszString0; pszString0 += dwLength)
        {
          dwLength = (DWORD) _tcslen (pszString0) + 1;
          dwRealCount++;
        }
      if (dwCount >= dwRealCount)
        {
          LPTSTR *pszDstString = pszStrings;
          for (LPCTSTR pszString1 = pValData->pszString; *pszString1; pszString1 += dwLength, pszDstString++)
            {
              dwLength = (DWORD) _tcslen (pszString1) + 1;
              LPTSTR pszNewString = (LPTSTR) malloc (dwLength);
              *pszDstString = pszNewString;
              if (pszNewString != nullptr)
                {
                  while ((*pszNewString = (BYTE) *pszString1) != _T ('\0'))
                    {
                      pszNewString++;
                      pszString1++;
                    }
                }
              else
                {
                  while (*pszString1)
                    {
                      pszString1++;
                    }
                }
            }
          return true;
        }
    }
  return false;
}

/* get a new array of strings */
bool
RegValGetNewStringArr (const RegVal *pValData, LPTSTR **pszStrings, DWORD *pdwCount)
{
  ASSERT (pValData != nullptr);
  if (pValData->dwType == REG_MULTI_SZ)
    {
      LPTSTR pszString;
      DWORD dwRealCount = 0, dwLength;
      for (pszString = pValData->pszString; *pszString; pszString += dwLength)
        {
          dwLength = (DWORD) _tcslen (pszString) + 1;
          dwRealCount++;
        }
      LPTSTR *pszNewStrings = (LPTSTR *) malloc (dwRealCount *sizeof (LPTSTR));
      if (pszNewStrings != nullptr)
        {
          *pszStrings = pszNewStrings;
          *pdwCount = dwRealCount;
          for (pszString = pValData->pszString; *pszString; pszString += dwLength, pszNewStrings++)
            {
              dwLength = (DWORD) _tcslen (pszString) + 1;
              LPTSTR pszNewString = (LPTSTR) malloc (dwLength);
              *pszNewStrings = pszNewString;
              if (pszNewString != nullptr)
                {
                  while ((*pszNewString = (BYTE) *pszString) != _T ('\0'))
                    {
                      pszNewString++;
                      pszString++;
                    }
                }
              else
                {
                  while (*pszString)
                    {
                      pszString++;
                    }
                }
            }
          return true;
        }
    }
  return false;
}

#ifdef REG_WITH_MFC

/* get a string */
bool
RegValGetString (const RegVal *pValData, CString &sString)
{
  ASSERT (pValData != nullptr);
  if (pValData->dwType == REG_SZ || pValData->dwType == REG_EXPAND_SZ
        || pValData->dwType == REG_LINK || pValData->dwType == REG_MULTI_SZ)
    {
      LPTSTR pszString = sString.GetBuffer (pValData->dwLength + 1);
      CopyMemory (pszString, pValData->pszString, pValData->dwLength);
      sString.ReleaseBuffer (pValData->dwLength);
      return true;
    }
  return false;
}

/* get an array of strings */
bool
RegValGetStringArr (const RegVal *pValData, CStringArray &arrString)
{
  ASSERT (pValData != nullptr);
  if (pValData->dwType == REG_MULTI_SZ)
    {
      arrString.RemoveAll ();
      for (LPCTSTR pszString = pValData->pszString; *pszString; pszString += _tcslen (pszString) + 1)
        {
          arrString.Add (pszString);
        }
      return true;
    }
  return false;
}

#endif /* REG_WITH_MFC */

/* set a number */
void
RegValSetNumber (RegVal *pValData, DWORD dwNumber)
{
  ASSERT (pValData != nullptr);
  pValData->dwType = REG_DWORD;
  pValData->dwNumber = dwNumber;
}

/* set binary data */
bool
RegValSetBinary (RegVal *pValData, const LPBYTE pbyteData, DWORD dwSize)
{
  ASSERT (pValData != nullptr && pbyteData != nullptr);
  pValData->pbyteData = (LPBYTE) malloc (dwSize);
  if (pValData->pbyteData != nullptr)
    {
      pValData->dwSize = dwSize;
      pValData->dwType = REG_BINARY;
      memcpy (pValData->pbyteData, pbyteData, dwSize);
      return true;
    }
  pValData->dwType = REG_NONE;
  return false;
}

/* set a string */
bool
RegValSetString (RegVal *pValData, LPCTSTR pszString)
{
  ASSERT (pValData != nullptr && pszString != nullptr);
  DWORD dwLength = (DWORD) _tcslen (pszString) + 1;
  pValData->pszString = (LPTSTR) malloc (dwLength);
  if (pValData->pszString != nullptr)
    {
      pValData->dwLength = dwLength;
      pValData->dwType = REG_SZ;
      memcpy (pValData->pbyteData, pszString, dwLength);
      return true;
    }
  pValData->dwType = REG_NONE;
  return false;
}

bool
RegValSetStringArr (RegVal *pValData, const LPCTSTR pszStrings[], DWORD dwCount)
{
  ASSERT (pValData != nullptr && pszStrings != nullptr);
  DWORD i, dwSize = 1;
  if (dwCount != 0)
    {
      for (i = 0; i < dwCount; i++)
        {
          dwSize += (DWORD) _tcslen (pszStrings[i]) + 1;
        }
    }
  else
    {
      dwSize++;
    }
  pValData->pbyteData = (LPBYTE) malloc (dwSize);
  if (pValData->pbyteData != nullptr)
    {
      pValData->dwSize = dwSize;
      pValData->dwType = REG_MULTI_SZ;
      LPBYTE pbyteData = pValData->pbyteData;
      if (dwCount != 0)
        {
          for (i = 0; i < dwCount; i++)
            {
              LPCTSTR pszString = pszStrings[i];
              while ((*pbyteData++ = (BYTE) *pszString) != _T ('\0'))
                {
                  pszString++;
                }
            }
        }
      else
        {
          *pbyteData++ = _T ('\0');
        }
      *pbyteData = _T ('\0');
      return true;
    }
  pValData->dwType = REG_NONE;
  return false;
}

#ifdef REG_WITH_MFC

/* set an array of strings */
bool
RegValSetStringArr (RegVal *pValData, const CStringArray &arrString)
{
  ASSERT (pValData != nullptr);
  DWORD i, dwSize = 1, dwCount = (DWORD) arrString.GetSize ();
  if (dwCount != 0)
    {
      for (i = 0; i < dwCount; i++)
        {
          dwSize += arrString[i].GetLength () + 1;
        }
    }
  else
    {
      dwSize++;
    }
  pValData->pbyteData = (LPBYTE) malloc (dwSize);
  if (pValData->pbyteData != nullptr)
    {
      pValData->dwSize = dwSize;
      pValData->dwType = REG_MULTI_SZ;
      LPBYTE pbyteData = pValData->pbyteData;
      if (dwCount != 0)
        {
          for (i = 0; i < dwCount; i++)
            {
              LPCTSTR pszString = arrString[i];
              while ((*pbyteData++ = (BYTE) *pszString) != _T ('\0'))
                {
                  pszString++;
                }
            }
        }
      else
        {
          *pbyteData++ = _T ('\0');
        }
      *pbyteData = _T ('\0');
      return true;
    }
  pValData->dwType = REG_NONE;
  return false;
}

#endif /* REG_WITH_MFC */

/* connect to remote computer registry */
HKEY
RegConnect (HKEY hKey, LPCTSTR pszRemote)
{
  HKEY hSubKey;
  if (RegConnectRegistry (pszRemote, hKey, &hSubKey) == ERROR_SUCCESS)
    {
      return hSubKey;
    }
  return nullptr;
}

/* open computer registry */
HKEY
RegOpen (HKEY hKey, LPCTSTR pszSubKey, DWORD dwRights)
{
  HKEY hSubKey;
  if (RegOpenKeyEx (hKey, pszSubKey, 0, dwRights, &hSubKey) == ERROR_SUCCESS)
    {
      return hSubKey;
    }
  return nullptr;
}

/* create computer registry */
HKEY
RegCreate (HKEY hKey, LPCTSTR pszSubKey, DWORD dwRights)
{
  HKEY hSubKey;
  DWORD dwDisposition;
  if (RegCreateKeyEx (hKey, pszSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE, dwRights,
                        nullptr, &hSubKey, &dwDisposition) == ERROR_SUCCESS)
    {
      return hSubKey;
    }
  return nullptr;
}

/* close computer registry */
void
RegClose (HKEY hKey)
{
  if (hKey != nullptr)
    RegCloseKey (hKey);
}

/* load data of any type */
bool
RegLoadVal (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, RegVal *pValData)
{
  ASSERT (pValData != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwSize;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwSize) == ERROR_SUCCESS)
        {
          if (dwType == REG_DWORD)
            {
              ASSERT (dwSize == sizeof (DWORD));
              DWORD dwNumber;
              if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, (LPBYTE) &dwNumber, &dwSize) == ERROR_SUCCESS)
                {
                  ASSERT (dwSize == sizeof (DWORD));
                  RegValFree (pValData);
                  pValData->dwType = dwType;
                  pValData->dwNumber = dwNumber;
                  if (hSubKey != hKey)
                    RegClose (hSubKey);
                  return true;
                }
            }
          else if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK
                || dwType == REG_MULTI_SZ || dwType == REG_BINARY)
            {
              LPBYTE pbyteData = (LPBYTE) malloc (dwSize);
              if (pbyteData != nullptr)
                {
                  if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, pbyteData, &dwSize) == ERROR_SUCCESS)
                    {
                      RegValFree (pValData);
                      pValData->dwType = dwType;
                      pValData->pbyteData = pbyteData;
                      pValData->dwLength = dwSize;
                      if (hSubKey != hKey)
                        RegClose (hSubKey);
                      return true;
                    }
                  free (pbyteData);
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load a number */
bool
RegLoadNumber (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD *pdwNumber)
{
  ASSERT (pdwNumber != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwSize;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwSize) == ERROR_SUCCESS)
        {
          if (dwType == REG_DWORD)
            {
              ASSERT (dwSize == sizeof (DWORD));
              if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, (LPBYTE) pdwNumber, &dwSize) == ERROR_SUCCESS)
                {
                  ASSERT (dwSize == sizeof (DWORD));
                  if (hSubKey != hKey)
                    RegClose (hSubKey);
                  return true;
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load binary data */
bool
RegLoadBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize)
{
  ASSERT (pbyteData != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwRealSize;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwRealSize) == ERROR_SUCCESS)
        {
          if (dwType == REG_BINARY &&dwSize >= dwRealSize)
            {
              if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, pbyteData, &dwRealSize) == ERROR_SUCCESS)
                {
                  if (hSubKey != hKey)
                    RegClose (hSubKey);
                  return true;
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load new binary data */
bool
RegLoadNewBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize)
{
  ASSERT (pbyteData != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwRealSize;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwRealSize) == ERROR_SUCCESS)
        {
          if (dwType == REG_BINARY)
            {
              LPBYTE pbyteNewData = (LPBYTE) malloc (dwRealSize);
              if (pbyteNewData != nullptr)
                {
                  if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, pbyteNewData, &dwRealSize) == ERROR_SUCCESS)
                    {
                      *pbyteData = pbyteNewData;
                      *pdwSize = dwRealSize;
                      if (hSubKey != hKey)
                        RegClose (hSubKey);
                      return true;
                    }
                  free (pbyteNewData);
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load a string */
bool
RegLoadString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength)
{
  ASSERT (pszString != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwRealLength;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwRealLength) == ERROR_SUCCESS)
        {
          if ((dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK
                 || dwType == REG_MULTI_SZ) &&dwLength >= dwRealLength)
            {
              if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, (LPBYTE) pszString, &dwRealLength) == ERROR_SUCCESS)
                {
                  if (hSubKey != hKey)
                    RegClose (hSubKey);
                  return true;
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load a new string */
bool
RegLoadNewString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength)
{
  ASSERT (pszString != nullptr);
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwRealLength;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwRealLength) == ERROR_SUCCESS)
        {
          if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK
                || dwType == REG_MULTI_SZ)
            {
              LPTSTR pszNewString = (LPTSTR) malloc (dwRealLength);
              if (pszNewString != nullptr)
                {
                  if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, (LPBYTE) pszNewString, &dwRealLength) == ERROR_SUCCESS)
                    {
                      *pszString = pszNewString;
                      if (pdwLength != nullptr)
                        {
                          *pdwLength = dwRealLength;
                        }
                      if (hSubKey != hKey)
                        RegClose (hSubKey);
                      return true;
                    }
                  free (pszNewString);
                }
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load an array of strings */
bool
RegLoadStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount)
{
  RegVal Value;
  if (RegLoadVal (hKey, pszSubKey, pszValName, &Value))
    {
      if (RegValGetStringArr (&Value, pszStrings, dwCount))
        {
          RegValFree (&Value);
          return true;
        }
      RegValFree (&Value);
    }
  return false;
}

/* load a new array of strings */
bool
RegLoadNewStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount)
{
  RegVal Value;
  if (RegLoadVal (hKey, pszSubKey, pszValName, &Value))
    {
      if (RegValGetNewStringArr (&Value, pszStrings, pdwCount))
        {
          RegValFree (&Value);
          return true;
        }
      RegValFree (&Value);
    }
  return false;
}

#ifdef REG_WITH_MFC

/* load a string */
bool
RegLoadString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, CString &sString)
{
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      DWORD dwType, dwRealLength;
      if (RegQueryValueEx (hSubKey, pszValName, 0, &dwType, nullptr, &dwRealLength) == ERROR_SUCCESS)
        {
          if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_LINK
                || dwType == REG_MULTI_SZ)
            {
              LPTSTR pszString = sString.GetBuffer (dwRealLength);
              if (RegQueryValueEx (hSubKey, pszValName, 0, nullptr, (LPBYTE) pszString, &dwRealLength) == ERROR_SUCCESS)
                {
                  sString.ReleaseBuffer (dwRealLength);
                  if (hSubKey != hKey)
                    RegClose (hSubKey);
                  return true;
                }
              sString.ReleaseBuffer (0);
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* load an array of strings */
bool
RegLoadStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, CStringArray &arrString)
{
  RegVal Value;
  if (RegLoadVal (hKey, pszSubKey, pszValName, &Value))
    {
      if (RegValGetStringArr (&Value, arrString))
        {
          RegValFree (&Value);
          return true;
        }
      RegValFree (&Value);
    }
  return false;
}

#endif /* REG_WITH_MFC */

/* store data of any type */
bool
RegSaveVal (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const RegVal *pValData)
{
  ASSERT (pValData != nullptr);
  HKEY hSubKey = pszSubKey ? RegCreate (hKey, pszSubKey, KEY_WRITE) : hKey;
  if (hSubKey != nullptr)
    {
      LONG lResult;
      if (pValData->dwType == REG_DWORD)
        {
          lResult = RegSetValueEx (hSubKey, pszValName, 0, pValData->dwType, (LPBYTE) &pValData->dwNumber, sizeof (DWORD));
        }
      else if (pValData->dwType == REG_SZ || pValData->dwType == REG_EXPAND_SZ
            || pValData->dwType == REG_LINK || pValData->dwType == REG_MULTI_SZ
            || pValData->dwType == REG_BINARY)
        {
          lResult = RegSetValueEx (hSubKey, pszValName, 0, pValData->dwType, pValData->pbyteData, pValData->dwSize);
        }
      else
        {
          lResult = ERROR_BAD_FORMAT;
        }
      if (lResult == ERROR_SUCCESS)
        {
          if (hSubKey != hKey)
            RegClose (hSubKey);
          return true;
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* store a number */
bool
RegSaveNumber (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD dwNumber)
{
  HKEY hSubKey = pszSubKey ? RegCreate (hKey, pszSubKey, KEY_WRITE) : hKey;
  if (hSubKey != nullptr)
    {
      if (RegSetValueEx (hSubKey, pszValName, 0, REG_DWORD, (LPBYTE) &dwNumber, sizeof (DWORD)) == ERROR_SUCCESS)
        {
          if (hSubKey != hKey)
            RegClose (hSubKey);
          return true;
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* store binary data */
bool
RegSaveBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize)
{
  HKEY hSubKey = pszSubKey ? RegCreate (hKey, pszSubKey, KEY_WRITE) : hKey;
  if (hSubKey != nullptr)
    {
      if (RegSetValueEx (hSubKey, pszValName, 0, REG_BINARY, pbyteData, dwSize) == ERROR_SUCCESS)
        {
          if (hSubKey != hKey)
            RegClose (hSubKey);
          return true;
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* store a string */
bool
RegSaveString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPCTSTR pszString)
{
  HKEY hSubKey = pszSubKey ? RegCreate (hKey, pszSubKey, KEY_WRITE) : hKey;
  if (hSubKey != nullptr)
    {
      if (RegSetValueEx (hSubKey, pszValName, 0, REG_SZ, (LPBYTE) pszString, (DWORD)(_tcslen (pszString) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
        {
          if (hSubKey != hKey)
            RegClose (hSubKey);
          return true;
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* store an array of strings */
bool
RegSaveStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount)
{
  RegVal Value;
  if (RegValSetStringArr (&Value, pszStrings, dwCount))
    {
      if (RegSaveVal (hKey, pszSubKey, pszValName, &Value))
        {
          RegValFree (&Value);
          return true;
        }
      RegValFree (&Value);
    }
  return false;
}

#ifdef REG_WITH_MFC

/* store an array of strings */
bool
RegSaveStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const CStringArray &arrString)
{
  RegVal Value;
  if (RegValSetStringArr (&Value, arrString))
    {
      if (RegSaveVal (hKey, pszSubKey, pszValName, &Value))
        {
          RegValFree (&Value);
          return true;
        }
      RegValFree (&Value);
    }
  return false;
}

#endif /* REG_WITH_MFC */

/* delete all subkeys in the given key */
bool
RegDeleteSubKeys (HKEY hKey)
{
  DWORD dwSubKeyCnt, dwMaxSubKey;
  if (RegQueryInfoKey (hKey, nullptr, nullptr, 0, &dwSubKeyCnt, &dwMaxSubKey,
                         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
    {
      if (dwSubKeyCnt)
        {
          LPTSTR pszKeyName = (LPTSTR) malloc (dwMaxSubKey += 1);
          if (pszKeyName != nullptr)
            {
              do
                {
                  if (RegEnumKey (hKey, --dwSubKeyCnt, pszKeyName, dwMaxSubKey) == ERROR_SUCCESS)
                    {
                      HKEY hSubKey = RegOpen (hKey, pszKeyName, KEY_READ | KEY_WRITE);
                      if (hSubKey != nullptr)
                        {
                          if (RegDeleteSubKeys (hSubKey))
                            {
                              RegClose (hSubKey);
                              if (RegDeleteKey (hKey, pszKeyName) != ERROR_SUCCESS)
                                {
                                  free (pszKeyName);
                                  return false;
                                }
                            }
                          else
                            {
                              RegClose (hSubKey);
                              free (pszKeyName);
                              return false;
                            }
                        }
                      else
                        {
                          free (pszKeyName);
                          return false;
                        }
                    }
                  else
                    {
                      free (pszKeyName);
                      return false;
                    }
                }
              while (dwSubKeyCnt);
              free (pszKeyName);
            }
          else
            {
              return false;
            }
        }
      return true;
    }
  return false;
}

/* delete the given value or key in the registry with all of its subkeys */
bool
RegDeleteKey (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName)
{
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ | KEY_WRITE) : hKey;
  if (hSubKey != nullptr)
    {
      if (pszValName != nullptr)
        {
          if (RegDeleteValue (hSubKey, pszValName) == ERROR_SUCCESS)
            {
              if (hSubKey != hKey)
                RegClose (hSubKey);
              return true;
            }
        }
      else
        {
          if (RegDeleteSubKeys (hSubKey))
            {
              if (hSubKey != hKey)
                RegClose (hSubKey);
              return pszSubKey ? (RegDeleteKey (hKey, pszSubKey) == ERROR_SUCCESS) : true;
            }
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  return false;
}

/* check wether the given key has other subkeys and/or values */
bool
RegHasEntries (HKEY hKey, LPCTSTR pszSubKey, DWORD *pdwSubKeyCount, DWORD *pdwValueCount)
{
  HKEY hSubKey = pszSubKey ? RegOpen (hKey, pszSubKey, KEY_READ) : hKey;
  if (hSubKey != nullptr)
    {
      if (RegQueryInfoKey (hSubKey, nullptr, nullptr, 0, pdwSubKeyCount, nullptr, nullptr, pdwValueCount, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        {
          if (hSubKey != hKey)
            RegClose (hSubKey);
          return true;
        }
      if (hSubKey != hKey)
        RegClose (hSubKey);
    }
  *pdwSubKeyCount = *pdwValueCount = 0;
  return false;
}

static LPTSTR g_pszValue = nullptr;
static DWORD g_dwValueCnt, g_dwValue, g_dwValueMax;

bool RegFindFirstValue (HKEY hKey, LPCTSTR *ppszValue, RegVal *pValData);

/* walks to the first value */
bool
RegFindFirstValue (HKEY hKey, LPCTSTR *ppszValue, RegVal *pValData)
{
  if (RegQueryInfoKey (hKey, nullptr, nullptr, 0, nullptr, nullptr, nullptr,
    &g_dwValueCnt, &g_dwValueMax, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
    {
      if (g_dwValueCnt)
        {
          if (g_pszValue != nullptr)
            {
              free (g_pszValue);
              g_pszValue = nullptr;
            }
          g_pszValue = (LPTSTR) malloc (g_dwValueMax += 1);
          if (g_pszValue)
            {
              DWORD dwMaxValue = g_dwValueMax;
              g_dwValue = 0;
              if (RegEnumValue (hKey, g_dwValue++, g_pszValue, &dwMaxValue, 0, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
                {
                  *ppszValue = g_pszValue;
                  return RegLoadVal (hKey, nullptr, g_pszValue, pValData);
                }
            }
        }
    }
  return false;
}

/* walks to the next value */
bool RegFindNextValue (HKEY hKey, LPCTSTR *ppszValue, RegVal *pValData)
{
  DWORD dwMaxValue = g_dwValueMax;
  if (g_dwValue < g_dwValueCnt && RegEnumValue (hKey, g_dwValue++, g_pszValue, &dwMaxValue, 0, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
    {
      *ppszValue = g_pszValue;
      return RegLoadVal (hKey, nullptr, g_pszValue, pValData);
    }
  return false;
}

/* closes registry walking */
void RegFindClose ()
{
  if (g_pszValue != nullptr)
    {
      free (g_pszValue);
      g_pszValue = nullptr;
    }
}

/*////////////////////////////////////////////////////////////////////////////*/

#ifdef __cplusplus

/* constructor - automatically initializes registry value data */
CRegVal::CRegVal ()
{
  Init ();
}

/* destructor - automatically frees registry value data */
CRegVal::~CRegVal ()
{
  Free ();
}

/* initializes registry value data */
void CRegVal::Init ()
{
  RegValInit (this);
}

/* frees registry value data */
void CRegVal::Free ()
{
  RegValFree (this);
}

/* get a number */
bool CRegVal::GetNumber (DWORD *pdwNumber) const
{
  return RegValGetNumber (this, pdwNumber);
}

/* get binary data */
bool CRegVal::GetBinary (LPBYTE _pbyteData, DWORD _dwSize) const
{
  return RegValGetBinary (this, _pbyteData, _dwSize);
}

/* get new binary data */
bool CRegVal::GetNewBinary (LPBYTE *_pbyteData, DWORD *_pdwSize) const
{
  return RegValGetNewBinary (this, _pbyteData, _pdwSize);
}

/* get a string */
bool CRegVal::GetString (LPTSTR _pszString, DWORD _dwLength) const
{
  return RegValGetString (this, _pszString, _dwLength);
}

/* get a new string */
bool CRegVal::GetNewString (LPTSTR *_pszString, DWORD *_pdwLength) const
{
  return RegValGetNewString (this, _pszString, _pdwLength);
}

/* get an array of strings */
bool CRegVal::GetStringArr (LPTSTR pszStrings[], DWORD dwCount) const
{
  return RegValGetStringArr (this, pszStrings, dwCount);
}

/* get a new array of strings */
bool CRegVal::GetNewStringArr (LPTSTR **pszStrings, DWORD *pdwCount) const
{
  return RegValGetNewStringArr (this, pszStrings, pdwCount);
}

#ifdef REG_WITH_MFC

/* get a string */
bool CRegVal::GetString (CString &sString) const
{
  return RegValGetString (this, sString);
}

/* get an array of strings */
bool CRegVal::GetStringArr (CStringArray &arrString) const
{
  return RegValGetStringArr (this, arrString);
}

#endif /* REG_WITH_MFC */

/* set a number */
void CRegVal::SetNumber (DWORD _dwNumber)
{
  RegValSetNumber (this, _dwNumber);
}

/* set binary data */
bool CRegVal::SetBinary (const LPBYTE _pbyteData, DWORD _dwSize)
{
  return RegValSetBinary (this, _pbyteData, _dwSize);
}

/* set a string */
bool CRegVal::SetString (LPCTSTR _pszString)
{
  return RegValSetString (this, _pszString);
}

/* set an array of strings */
bool CRegVal::SetStringArr (const LPCTSTR pszStrings[], DWORD dwCount)
{
  return RegValSetStringArr (this, pszStrings, dwCount);
}

#ifdef REG_WITH_MFC

/* set an array of strings */
bool CRegVal::SetStringArr (const CStringArray &arrString)
{
  return RegValSetStringArr (this, arrString);
}

#endif /* REG_WITH_MFC */

/* constructor - automatically initializes registry data */
CReg::CReg ()
{
  Open ();
}

/* destructor - automatically frees registry data */
CReg::~CReg ()
{
  Close ();
}

/* connect to remote computer registry */
HKEY CReg::Connect (HKEY hNewKey, LPCTSTR pszRemote)
{
  return hKey = RegConnect (hNewKey, pszRemote);
}

/* connect to registry key */
HKEY CReg::Open (HKEY hNewKey /*= nullptr*/)
{
  return hKey = hNewKey;
}

/* open computer registry */
HKEY CReg::Open (HKEY hNewKey, LPCTSTR pszSubKey, DWORD dwRights)
{
  return hKey = RegOpen (hNewKey, pszSubKey, dwRights);
}

/* close computer registry */
void CReg::Close ()
{
  if (hKey != nullptr) // MAB 8 Nov 1999 - added NULL test
    {
      RegClose (hKey);
      //*** MIPO 07-12-1999 - After Closing the Key, hKey must by NULL ****
      // RegClose - dont do that - it must be done manualy
      hKey = nullptr;
    }
}

/* create computer registry */
HKEY CReg::Create (HKEY hNewKey, LPCTSTR pszSubKey, DWORD dwRights)
{
  return hKey = RegCreate (hNewKey, pszSubKey, dwRights);
}

/* load data of any type */
bool CReg::LoadVal (LPCTSTR pszValName, RegVal *pValData)
{
  return RegLoadVal (hKey, nullptr, pszValName, pValData);
}

/* load data of any type from subkey */
bool CReg::LoadVal (LPCTSTR pszSubKey, LPCTSTR pszValName, RegVal *pValData)
{
  return RegLoadVal (hKey, pszSubKey, pszValName, pValData);
}

/* load a number */
bool CReg::LoadNumber (LPCTSTR pszValName, DWORD *pdwNumber)
{
  return RegLoadNumber (hKey, nullptr, pszValName, pdwNumber);
}

/* load a number from subkey */
bool CReg::LoadNumber (LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD *pdwNumber)
{
  return RegLoadNumber (hKey, pszSubKey, pszValName, pdwNumber);
}

/* load binary data */
bool CReg::LoadBinary (LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize)
{
  return RegLoadBinary (hKey, nullptr, pszValName, pbyteData, dwSize);
}

/* load binary data from subkey */
bool CReg::LoadBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize)
{
  return RegLoadBinary (hKey, pszSubKey, pszValName, pbyteData, dwSize);
}

/* load new binary data */
bool CReg::LoadNewBinary (LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize)
{
  return RegLoadNewBinary (hKey, nullptr, pszValName, pbyteData, pdwSize);
}

/* load new binary data from subkey */
bool CReg::LoadNewBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize)
{
  return RegLoadNewBinary (hKey, pszSubKey, pszValName, pbyteData, pdwSize);
}

/* load a string */
bool CReg::LoadString (LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength)
{
  return RegLoadString (hKey, nullptr, pszValName, pszString, dwLength);
}

/* load a string from subkey */
bool CReg::LoadString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength)
{
  return RegLoadString (hKey, pszSubKey, pszValName, pszString, dwLength);
}

/* load a new string */
bool CReg::LoadNewString (LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength)
{
  return RegLoadNewString (hKey, nullptr, pszValName, pszString, pdwLength);
}

/* load a new string from subkey */
bool CReg::LoadNewString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength)
{
  return RegLoadNewString (hKey, pszSubKey, pszValName, pszString, pdwLength);
}

/* load an array of strings */
bool CReg::LoadStringArr (LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount)
{
  return RegLoadStringArr (hKey, nullptr, pszValName, pszStrings, dwCount);
}

/* load an array of strings from subkey */
bool CReg::LoadStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount)
{
  return RegLoadStringArr (hKey, pszSubKey, pszValName, pszStrings, dwCount);
}

/* load a new array of strings */
bool CReg::LoadNewStringArr (LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount)
{
  return RegLoadNewStringArr (hKey, nullptr, pszValName, pszStrings, pdwCount);
}

/* load a new array of strings from subkey */
bool CReg::LoadNewStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount)
{
  return RegLoadNewStringArr (hKey, pszSubKey, pszValName, pszStrings, pdwCount);
}

#ifdef REG_WITH_MFC

/* load a string */
bool CReg::LoadString (LPCTSTR pszValName, CString &sString)
{
  return RegLoadString (hKey, nullptr, pszValName, sString);
}

/* load a string from subkey */
bool CReg::LoadString (LPCTSTR pszSubKey, LPCTSTR pszValName, CString &sString)
{
  return RegLoadString (hKey, pszSubKey, pszValName, sString);
}

/* load an array of strings */
bool CReg::LoadStringArr (LPCTSTR pszValName, CStringArray &arrString)
{
  return RegLoadStringArr (hKey, nullptr, pszValName, arrString);
}

/* load an array of strings from subkey */
bool CReg::LoadStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, CStringArray &arrString)
{
  return RegLoadStringArr (hKey, pszSubKey, pszValName, arrString);
}

#endif /* REG_WITH_MFC */

/* store data of any type */
bool CReg::SaveVal (LPCTSTR pszValName, const RegVal *pValData)
{
  return RegSaveVal (hKey, nullptr, pszValName, pValData);
}

/* store data of any type to subkey */
bool CReg::SaveVal (LPCTSTR pszSubKey, LPCTSTR pszValName, const RegVal *pValData)
{
  return RegSaveVal (hKey, pszSubKey, pszValName, pValData);
}

/* store a number */
bool CReg::SaveNumber (LPCTSTR pszValName, DWORD dwNumber)
{
  return RegSaveNumber (hKey, nullptr, pszValName, dwNumber);
}

/* store a number to subkey */
bool CReg::SaveNumber (LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD dwNumber)
{
  return RegSaveNumber (hKey, pszSubKey, pszValName, dwNumber);
}

/* store binary data */
bool CReg::SaveBinary (LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize)
{
  return RegSaveBinary (hKey, nullptr, pszValName, pbyteData, dwSize);
}

/* store binary data to subkey */
bool CReg::SaveBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize)
{
  return RegSaveBinary (hKey, pszSubKey, pszValName, pbyteData, dwSize);
}

/* store a string */
bool CReg::SaveString (LPCTSTR pszValName, LPCTSTR pszString)
{
  return RegSaveString (hKey, nullptr, pszValName, pszString);
}

/* store a string to subkey */
bool CReg::SaveString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPCTSTR pszString)
{
  return RegSaveString (hKey, pszSubKey, pszValName, pszString);
}

/* store an array of strings */
bool CReg::SaveStringArr (LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount)
{
  return RegSaveStringArr (hKey, nullptr, pszValName, pszStrings, dwCount);
}

/* store an array of strings to subkey */
bool CReg::SaveStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount)
{
  return RegSaveStringArr (hKey, pszSubKey, pszValName, pszStrings, dwCount);
}

#ifdef REG_WITH_MFC

/* store an array of strings */
bool CReg::SaveStringArr (LPCTSTR pszValName, const CStringArray &arrString)
{
  return RegSaveStringArr (hKey, nullptr, pszValName, arrString);
}

/* store an array of strings to subkey */
bool CReg::SaveStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, const CStringArray &arrString)
{
  return RegSaveStringArr (hKey, pszSubKey, pszValName, arrString);
}

#endif /* REG_WITH_MFC */

/* delete the given value or key in the registry with all of its subkeys */
bool CReg::DeleteKey (LPCTSTR pszValName)
{
  return RegDeleteKey (hKey, nullptr, pszValName);
}

/* delete the given value or key in the registry with all of its subkeys in subkey */
bool CReg::DeleteKey (LPCTSTR pszSubKey, LPCTSTR pszValName)
{
  return RegDeleteKey (hKey, pszSubKey, pszValName);
}

/* delete all of subkeys in the key */
bool CReg::DeleteSubKeys ()
{
  return RegDeleteSubKeys (hKey);
}

/* check wether the given key has other subkeys and/or values */
bool CReg::HasEntries (DWORD *pdwSubKeyCount, DWORD *pdwValueCount)
{
  return RegHasEntries (hKey, nullptr, pdwSubKeyCount, pdwValueCount);
}

/* check wether the given key has other subkeys and/or values in subkey */
bool CReg::HasEntries (LPCTSTR pszSubKey, DWORD *pdwSubKeyCount, DWORD *pdwValueCount)
{
  return RegHasEntries (hKey, pszSubKey, pdwSubKeyCount, pdwValueCount);
}

/* walks to the first value */
bool CReg::FindFirstValue (LPCTSTR &ppszValue, RegVal *pValData)
{
  return RegFindFirstValue (hKey, &ppszValue, pValData);
}

/* walks to the next value */
bool CReg::FindNextValue (LPCTSTR &ppszValue, RegVal *pValData)
{
  return RegFindNextValue (hKey, &ppszValue, pValData);
}

/* closes registry walking */
void CReg::FindClose ()
{
  RegFindClose ();
}

#endif /* cplusplus */

/*////////////////////////////////////////////////////////////////////////////*/
