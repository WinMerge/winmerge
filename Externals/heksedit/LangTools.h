class LangArray;
extern LangArray langArray;

void NTAPI TranslateDialog(HWND);
INT_PTR NTAPI ShowModalDialog(UINT, HWND, DLGPROC, LPVOID);
/*void NTAPI SetWindowText(HWND, PCWSTR);
void NTAPI SetDlgItemText(HWND, int, PCWSTR);
int NTAPI GetDlgItemText(HWND, int, PWSTR, int);
LRESULT NTAPI SendTextMsg(HWND, UINT, WPARAM, LPCWSTR);
HINSTANCE NTAPI ShellExecute(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);

#define MAKETEXTMSG(WM) ((UINT)MAKELONG(WM##W, WM##A))*/