///////////////////////////////////////////////////////////////////////////
//  File:    registry.h
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

#ifndef __REGISTRY_H__INCLUDED__
#define __REGISTRY_H__INCLUDED__

/*////////////////////////////////////////////////////////////////////////////*/

#if (defined (__AFXWIN_H__) || defined (ARX_WITH_MFC)) && !defined (REG_WITH_MFC)
#define REG_WITH_MFC
#endif

struct RegVal
  {
    DWORD dwType;

    union
      {
        DWORD dwNumber;
        LPBYTE pbyteData;
        LPTSTR pszString;
      };
    union
      {
        DWORD dwSize;
        DWORD dwLength;
      };
  };

/* initializes registry value data */
void EDITPADC_CLASS RegValInit (RegVal *pValData);

/* frees registry value data */
void EDITPADC_CLASS RegValFree (RegVal *pValData);

/* get a number */
bool EDITPADC_CLASS RegValGetNumber (const RegVal *pValData, DWORD *pdwNumber);

/* get binary data */
bool EDITPADC_CLASS RegValGetBinary (const RegVal *pValData, LPBYTE pbyteData, DWORD dwSize);

/* get new binary data */
bool EDITPADC_CLASS RegValGetNewBinary (const RegVal *pValData, LPBYTE *pbyteData, DWORD *pdwSize);

/* get a string */
bool EDITPADC_CLASS RegValGetString (const RegVal *pValData, LPTSTR pszString, DWORD dwLength);

/* get a new string */
bool EDITPADC_CLASS RegValGetNewString (const RegVal *pValData, LPTSTR *pszString, DWORD *pdwLength);

/* get an array of strings */
bool EDITPADC_CLASS RegValGetStringArr (const RegVal *pValData, LPTSTR pszStrings[], DWORD dwCount);

/* get a new array of strings */
bool EDITPADC_CLASS RegValGetNewStringArr (const RegVal *pValData, LPTSTR **pszStrings, DWORD *pdwCount);

#ifdef REG_WITH_MFC

/* get a string */
bool EDITPADC_CLASS RegValGetString (const RegVal *pValData, CString &sString);

/* get an array of strings */
bool EDITPADC_CLASS RegValGetStringArr (const RegVal *pValData, CStringArray &arrString);

#endif /* REG_WITH_MFC */

/* set a number */
void EDITPADC_CLASS RegValSetNumber (RegVal *pValData, DWORD dwNumber);

/* set binary data */
bool EDITPADC_CLASS RegValSetBinary (RegVal *pValData, const LPBYTE pbyteData, DWORD dwSize);

/* set a string */
bool EDITPADC_CLASS RegValSetString (RegVal *pValData, LPCTSTR pszString);

/* set an array of strings */
bool EDITPADC_CLASS RegValSetStringArr (RegVal *pValData, const LPCTSTR pszStrings[], DWORD dwCount);

#ifdef REG_WITH_MFC

/* set an array of strings */
bool EDITPADC_CLASS RegValSetStringArr (RegVal *pValData, const CStringArray &arrString);

#endif /* REG_WITH_MFC */

/* connect to remote computer registry */
HKEY EDITPADC_CLASS RegConnect (HKEY hKey, LPCTSTR pszRemote);

/* open computer registry */
HKEY EDITPADC_CLASS RegOpen (HKEY hKey, LPCTSTR pszSubKey, DWORD dwRights);

/* close computer registry */
void EDITPADC_CLASS RegClose (HKEY hKey);

/* create computer registry */
HKEY EDITPADC_CLASS RegCreate (HKEY hKey, LPCTSTR pszSubKey, DWORD dwRights);

/* load data of any type */
bool EDITPADC_CLASS RegLoadVal (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, RegVal *pValData);

/* load a number */
bool EDITPADC_CLASS RegLoadNumber (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD *pdwNumber);

/* load binary data */
bool EDITPADC_CLASS RegLoadBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize);

/* load new binary data */
bool EDITPADC_CLASS RegLoadNewBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize);

/* load a string */
bool EDITPADC_CLASS RegLoadString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength);

/* load a new string */
bool EDITPADC_CLASS RegLoadNewString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength);

/* load an array of strings */
bool EDITPADC_CLASS RegLoadStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount);

/* load a new array of strings */
bool EDITPADC_CLASS RegLoadNewStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount);

#ifdef REG_WITH_MFC

/* load a string */
bool EDITPADC_CLASS RegLoadString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, CString &sString);

/* load an array of strings */
bool EDITPADC_CLASS RegLoadStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, CStringArray &arrString);

#endif /* REG_WITH_MFC */

/* store data of any type */
bool EDITPADC_CLASS RegSaveVal (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const RegVal *pValData);

/* store a number */
bool EDITPADC_CLASS RegSaveNumber (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD dwNumber);

/* store binary data */
bool EDITPADC_CLASS RegSaveBinary (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize);

/* store a string */
bool EDITPADC_CLASS RegSaveString (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, LPCTSTR pszString);

/* store an array of strings */
bool EDITPADC_CLASS RegSaveStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount);

#ifdef REG_WITH_MFC

/* store an array of strings */
bool EDITPADC_CLASS RegSaveStringArr (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName, const CStringArray &arrString);

#endif /* REG_WITH_MFC */

/* delete the given value or key in the registry with all of its subkeys */
bool EDITPADC_CLASS RegDeleteKey (HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValName);

/* check wether the given key has other subkeys and/or values */
bool EDITPADC_CLASS RegHasEntries (HKEY hKey, LPCTSTR pszSubKey, DWORD *pdwSubKeyCount, DWORD *pdwValueCount);

/* walks to the first value */
bool RegFindFirstValue (HKEY hKey, LPCTSTR *ppszValue, RegVal *pValData);

/* walks to the next value */
bool RegFindNextValue (HKEY hKey, LPCTSTR *ppszValue, RegVal *pValData);

/* closes registry walking */
void RegFindClose ();

/*////////////////////////////////////////////////////////////////////////////*/

#ifdef __cplusplus

struct EDITPADC_CLASS CRegVal : public RegVal
  {
    /* constructor - automatically initializes registry value data */
    CRegVal ();

    /* destructor - automatically frees registry value data */
    ~CRegVal ();
    
    /* initializes registry value data */
    void Init ();
    
    /* frees registry value data */
    void Free ();
    
    /* get a number */
    bool GetNumber (DWORD *pdwNumber) const;
    
    /* get binary data */
    bool GetBinary (LPBYTE pbyteData, DWORD dwSize) const;
    
    /* get new binary data */
    bool GetNewBinary (LPBYTE *pbyteData, DWORD *pdwSize) const;
    
    /* get a string */
    bool GetString (LPTSTR pszString, DWORD dwLength) const;
    
    /* get a new string */
    bool GetNewString (LPTSTR *pszString, DWORD *pdwLength) const;
    
    /* get an array of strings */
    bool GetStringArr (LPTSTR pszStrings[], DWORD dwCount) const;
    
    /* get a new array of strings */
    bool GetNewStringArr (LPTSTR **pszStrings, DWORD *pdwCount) const;
    
    #ifdef REG_WITH_MFC
    
    /* get a string */
    bool GetString (CString &sString) const;
    
    /* get an array of strings */
    bool GetStringArr (CStringArray &arrString) const;
    
    #endif /* REG_WITH_MFC */
    
    /* set a number */
    void SetNumber (DWORD dwNumber);
    
    /* set binary data */
    bool SetBinary (const LPBYTE pbyteData, DWORD dwSize);
    
    /* set a string */
    bool SetString (LPCTSTR pszString);
    
    /* set an array of strings */
    bool SetStringArr (const LPCTSTR pszStrings[], DWORD dwCount);
    
    #ifdef REG_WITH_MFC
    
    /* set an array of strings */
    bool SetStringArr (const CStringArray &arrString);
    
    #endif /* REG_WITH_MFC */
  };

struct EDITPADC_CLASS CReg
  {
    HKEY hKey;
    
    /* constructor - automatically initializes registry data */
    CReg ();

    /* destructor - automatically frees registry data */
    ~CReg ();
    
    /* connect to remote computer registry */
    HKEY Connect (HKEY hNewKey, LPCTSTR pszRemote);
    
    /* connect to registry key */
    HKEY Open (HKEY hNewKey = NULL);
    
    /* open computer registry */
    HKEY Open (HKEY hNewKey, LPCTSTR pszSubKey, DWORD dwRights);
    
    /* close computer registry */
    void Close ();
    
    /* create computer registry */
    HKEY Create (HKEY hNewKey, LPCTSTR pszSubKey, DWORD dwRights);
    
    /* load data of any type */
    bool LoadVal (LPCTSTR pszValName, RegVal *pValData);
    
    /* load data of any type from subkey */
    bool LoadVal (LPCTSTR pszSubKey, LPCTSTR pszValName, RegVal *pValData);
    
    /* load a number */
    bool LoadNumber (LPCTSTR pszValName, DWORD *pdwNumber);
    
    /* load a number from subkey */
    bool LoadNumber (LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD *pdwNumber);
    
    /* load binary data */
    bool LoadBinary (LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize);
    
    /* load binary data from subkey */
    bool LoadBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE pbyteData, DWORD dwSize);
    
    /* load new binary data */
    bool LoadNewBinary (LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize);
    
    /* load new binary data from subkey */
    bool LoadNewBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, LPBYTE *pbyteData, DWORD *pdwSize);
    
    /* load a string */
    bool LoadString (LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength);
    
    /* load a string from subkey */
    bool LoadString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszString, DWORD dwLength);
    
    /* load a new string */
    bool LoadNewString (LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength);
    
    /* load a new string from subkey */
    bool LoadNewString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR *pszString, DWORD *pdwLength);
    
    /* load an array of strings */
    bool LoadStringArr (LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount);
    
    /* load an array of strings from subkey */
    bool LoadStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR pszStrings[], DWORD dwCount);
    
    /* load a new array of strings */
    bool LoadNewStringArr (LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount);
    
    /* load a new array of strings from subkey */
    bool LoadNewStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, LPTSTR **pszStrings, DWORD *pdwCount);
    
    #ifdef REG_WITH_MFC
    
    /* load a string */
    bool LoadString (LPCTSTR pszValName, CString &sString);
    
    /* load a string from subkey */
    bool LoadString (LPCTSTR pszSubKey, LPCTSTR pszValName, CString &sString);
    
    /* load an array of strings */
    bool LoadStringArr (LPCTSTR pszValName, CStringArray &arrString);
    
    /* load an array of strings from subkey */
    bool LoadStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, CStringArray &arrString);
    
    #endif /* REG_WITH_MFC */
    
    /* store data of any type */
    bool SaveVal (LPCTSTR pszValName, const RegVal *pValData);
    
    /* store data of any type to subkey */
    bool SaveVal (LPCTSTR pszSubKey, LPCTSTR pszValName, const RegVal *pValData);
    
    /* store a number */
    bool SaveNumber (LPCTSTR pszValName, DWORD dwNumber);
    
    /* store a number to subkey */
    bool SaveNumber (LPCTSTR pszSubKey, LPCTSTR pszValName, DWORD dwNumber);
    
    /* store binary data */
    bool SaveBinary (LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize);
    
    /* store binary data to subkey */
    bool SaveBinary (LPCTSTR pszSubKey, LPCTSTR pszValName, const LPBYTE pbyteData, DWORD dwSize);
    
    /* store a string */
    bool SaveString (LPCTSTR pszValName, LPCTSTR pszString);
    
    /* store a string to subkey */
    bool SaveString (LPCTSTR pszSubKey, LPCTSTR pszValName, LPCTSTR pszString);
    
    /* store an array of strings */
    bool SaveStringArr (LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount);
    
    /* store an array of strings to subkey */
    bool SaveStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, const LPCTSTR pszStrings[], DWORD dwCount);
    
    #ifdef REG_WITH_MFC
    
    /* store an array of strings */
    bool SaveStringArr (LPCTSTR pszValName, const CStringArray &arrString);
    
    /* store an array of strings to subkey */
    bool SaveStringArr (LPCTSTR pszSubKey, LPCTSTR pszValName, const CStringArray &arrString);
    
    #endif /* REG_WITH_MFC */
    
    /* delete the given value or key in the registry with all of its subkeys */
    bool DeleteKey (LPCTSTR pszValName);
    
    /* delete the given value or key in the registry with all of its subkeys in subkey */
    bool DeleteKey (LPCTSTR pszSubKey, LPCTSTR pszValName);
    
    /* delete all of subkeys in the key */
    bool DeleteSubKeys ();
    
    /* check wether the given key has other subkeys and/or values */
    bool HasEntries (DWORD *pdwSubKeyCount, DWORD *pdwValueCount);
    
    /* check wether the given key has other subkeys and/or values in subkey */
    bool HasEntries (LPCTSTR pszSubKey, DWORD *pdwSubKeyCount, DWORD *pdwValueCount);
    
    /* walks to the first value */
    bool FindFirstValue (LPCTSTR &ppszValue, RegVal *pValData);
    
    /* walks to the next value */
    bool FindNextValue (LPCTSTR &ppszValue, RegVal *pValData);
    
    /* closes registry walking */
    void FindClose ();
  };

#endif /* cplusplus */

bool EDITPADC_CLASS RegDeleteSubKeys (HKEY hKey);

/*////////////////////////////////////////////////////////////////////////////*/

#endif /* __REGISTRY_H__INCLUDED__ */
