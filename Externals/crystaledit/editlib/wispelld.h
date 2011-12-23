/*
 *  The following ifdef block is the standard way of creating macros which make exporting 
 *  from a DLL simpler. All files within this DLL are compiled with the WISPELL_EXPORTS
 *  symbol defined on the command line. this symbol should not be defined on any project
 *  that uses this DLL. This way any other project whose source files include this file see 
 *  WISPELL_API functions as being imported from a DLL, wheras this DLL sees symbols
 *  defined with this macro as being exported.
 */

#ifndef __WISPELLD_H__
#define __WISPELLD_H__

/*#ifdef WISPELL_EXPORTS
#define WISPELL_API __declspec (dllexport)
#else
#define WISPELL_API __declspec (dllimport)
#endif*/

#define SN_FOUND       1 /* possibly incorrect word was found */
#define SN_REPLACED    2 /* possibly incorrect word was commanded to be replaced */
#define SN_FINISHED    3 /* spell checking finished */
#define SN_HELP_SPELL  4 /* help button of the checking dialog was pressed */
#define SN_HELP_CONFIG 5 /* help button of the configuration dialog was pressed */

#define SF_TEX         0x00001 /* -t/-n */  /* TeX mode on */
#define SF_HTML        0x00002 /* -h    */  /* HTML mode on */
#define SF_RUNTOGETHER 0x00004 /* -C/-B */  /* run-together words to be legal compounds */
#define SF_EXTRAAFFIX  0x00008 /* -m/-P */  /* generate extra root/affix combinations */
#define SF_SORTPROBLY  0x00010 /* -S    */  /* sort the list of suggested words by probable correctness */
#define SF_HELP        0x10000              /* to enable the Help button in checking and configuration dialogs */
#define SF_ISPELLPATH  0x20000              /* to enable setting of the directory where ispell resides */

#define SF_TOSET (SF_TEX|SF_HTML|SF_RUNTOGETHER|SF_EXTRAAFFIX|SF_SORTPROBLY)

#define BUFSIZE 4096
#define INPUTWORDLEN 100
#define WAITTIME 50

typedef struct SpellData_t
  {
    DWORD dwFlags;                            /* combination of SF_ flags */
    TCHAR szIspell[MAX_PATH],                 /* the directory where ispell resides */
          szUserDict[MAX_PATH],               /* path to the user dictionary */
          szMainDict[INPUTWORDLEN],           /* name of the file with the main dictionary */
          szWordChars[INPUTWORDLEN],          /* extra characters to be allowed to build the word */
          szFormatter[INPUTWORDLEN];          /* alternate form for the character set */
    HWND hParent;                             /* parent window for the calling of the current routine */
    LPCTSTR pszWord;                          /* currently processed word - found or to be replaced */
    LPTSTR pszBuffer,                         /* buffer from which the engine reads the input line */
           pszCurrent;                        /* current position in the buffer - for internal use only */
    int nColumn,                              /* character position in currently processed line */
        nRow,                                 /* currently processed line */
        nDelta,                               /* gap between original column and another one modified by replacing - for internal use only */
        nLegalLen;                            /* maximal length of the word considered to be always correct */
    LPVOID pUserData;                         /* for external use only */
    int (*GetLine) (struct SpellData_t*);     /* pointer to routine which has to fill in the input buffer */
    int (*Notify) (int, struct SpellData_t*); /* pointer to routine which receives SN_ notifications */
  } SpellData;

/* WISPELL_API int SpellInit (SpellData*); */   /* initializes the control structure */
/* WISPELL_API int SpellCheck (SpellData*); */  /* performs spell checking (the dialog may appear) */
/* WISPELL_API int SpellConfig (SpellData*); */ /* performs spell checking configuration (the dialog appears) */

#endif /* __WISPELLD_H__ */
