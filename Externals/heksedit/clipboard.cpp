#include "precomp.h"
#include "clipboard.h"

//============================================================================================

void TextToClipboard(HWND hwnd, char *text, int len)
{
	// Changed for pabs's patch to compare files command.
	if (HGLOBAL hGlobal = GlobalAlloc(GHND, len))
	{
		SetCursor (LoadCursor (NULL, IDC_WAIT)); //tell user to wait
		if (char *pd = (char *)GlobalLock(hGlobal)) // get pointer to clip data
		{
			//succesfuly got pointer
			lstrcpyn(pd, text, len);//copy Text into global mem
			GlobalUnlock(hGlobal);//unlock global mem
			if (OpenClipboard(hwnd))
			{
				// open clip
				EmptyClipboard(); //empty clip
				SetClipboardData(CF_TEXT, hGlobal);//copy to clip
				CloseClipboard(); //close clip
			}
			else //failed to open clip
				MessageBox(hwnd,"Cannot get access to clipboard.", "Copy", MB_ICONERROR);
		}
		else
		{//failed to get pointer to global mem
			GlobalFree(hGlobal);
			MessageBox(hwnd,"Cannot lock clipboard.", "Copy", MB_ICONERROR);
		}
		SetCursor (LoadCursor (NULL, IDC_ARROW));//user can stop waiting
	}
	else// failed to allocate global mem
		MessageBox(hwnd, "Not enough memory for copying.", "Copy", MB_ICONERROR);
}

//Pabs changed - mutated TextToClipboard into two functions
void TextToClipboard(HWND hwnd, char *text)
{
	int len = 1 + strlen(text);
	TextToClipboard(hwnd, text, len);
}

void MessageCopyBox(HWND hwnd, LPTSTR text, LPCTSTR caption, UINT type)
{
	int len = lstrlen(text);//get the # bytes needed to store the string (not counting '\0')
	//& get where we have to put a '\0' character later
	// RK: Added "?" to end of string.
	lstrcat(text, "\nDo you want the above output to be copied to the clipboard?\n");
	if (IDYES == MessageBox(hwnd, text, caption, MB_YESNO | type))
	{
		//user wants to copy output
		text[len] = '\0';//Remove the line added above
		//Pabs removed & replaced with TextToClipboard
		TextToClipboard(hwnd, text, len + 1);
	}
//user doesn't want to copy output
}
