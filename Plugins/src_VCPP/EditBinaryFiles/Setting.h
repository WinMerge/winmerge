#ifndef Setting_h_included
#define Setting_h_included

bool ReadSetting(LPCTSTR setting, LPCTSTR name, LPTSTR buffer, int maxlen);
bool ReadSetting(HKEY hKeyRoot, LPCTSTR setting, LPCTSTR name, LPTSTR buffer, int maxlen);

#endif // Setting_h_included
