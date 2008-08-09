/*Recursively delete key for WinNT
Don't use this under Win9x
Don't use this to delete keys you know will have no subkeys or should not have subkeys
This recursively deletes subkeys of the key and then
returns the return value of RegDeleteKey(basekey,keynam)*/
LONG RegDeleteWinNTKey(HKEY basekey, const char *keynam);
LONG RegCopyValues(HKEY src, const char *skey, HKEY dst, const char *dkey);

