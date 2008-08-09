#include "precomp.h"


#include <htmlhelp.h>

typedef struct tagHH_LAST_ERROR
{
	int cbStruct;
	HRESULT hr;
	BSTR description;
} HH_LAST_ERROR;


typedef HWND (WINAPI* LPFNHtmlHelpA)(
	HWND hwndCaller,
	LPCSTR pszFile,
	UINT uCommand,
	DWORD_PTR dwData
	);

static LPFNHtmlHelpA pfnHtmlHelpA;
static HMODULE hHtmlHelpLibrary = 0;

#define HELPFILENAME "frhed.chm"

BOOL ShowHtmlHelp( UINT uCommand, DWORD dwData, HWND hParentWindow )
{
	if( !hHtmlHelpLibrary )
	{
		hHtmlHelpLibrary = LoadLibrary("hhctrl.ocx");
		if( hHtmlHelpLibrary )
		{
			pfnHtmlHelpA = (LPFNHtmlHelpA)GetProcAddress(hHtmlHelpLibrary,"HtmlHelpA");
			if( !pfnHtmlHelpA )
			{
				CloseHandle(hHtmlHelpLibrary);
				hHtmlHelpLibrary = 0;
			}
		}
	}
	if( pfnHtmlHelpA )
	{
		HWND ret = NULL;
		// Annahme: Aufruf erfolgt mit WinHelp-Befehlen !!!!!
		if( uCommand == HELP_CONTEXT )
		{
			CHAR szContext[256];
			sprintf( szContext, HELPFILENAME "::/%d.htm", dwData );
			ret = pfnHtmlHelpA( hParentWindow, HELPFILENAME, HH_DISPLAY_TOPIC, 0 );
		}
		else if( uCommand == HELP_FINDER )
		{
			ret = pfnHtmlHelpA( hParentWindow, HELPFILENAME, HH_DISPLAY_SEARCH, dwData );
		}
		else if( uCommand == HELP_CONTENTS )
		{
			ret = pfnHtmlHelpA( hParentWindow, HELPFILENAME, HH_DISPLAY_TOC, dwData );
		}
		else if( uCommand == HELP_QUIT )
		{
			pfnHtmlHelpA( NULL, NULL, HH_CLOSE_ALL, 0 );
			CloseHandle(hHtmlHelpLibrary);
			hHtmlHelpLibrary = 0;
			ret = (HWND)1;//We don't care
		}
		if( ret == NULL ){
			HH_LAST_ERROR lasterror;
			lasterror.cbStruct = sizeof(lasterror);

			ret = pfnHtmlHelpA( hParentWindow, NULL, HH_GET_LAST_ERROR, reinterpret_cast<DWORD>(&lasterror)) ;

			// Make sure that HH_GET_LAST_ERROR succeeded.
			if ( ret != 0 )
			{
				// Only report an error if we found one:
				if (FAILED(lasterror.hr))
				{
					// Is there a text message to display...
					if (lasterror.description)
					{
						// Display
						MessageBoxW( hParentWindow, lasterror.description, L"Help Error", MB_OK) ;
						SysFreeString( lasterror.description );
					}
				}
			}
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


