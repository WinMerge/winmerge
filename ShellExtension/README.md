# Shell Extension

Source code from the Windows Explorer shell integration.

## Testing

WinMerge sets executable path (`WinMergeU.exe`) which ShellExtension starts every time when options-dialog is closed. This can cause problems when testing several versions of WinMerge. There is registry value to overwrite path to WinMerge executable:

```
HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\PriExecutable
```

It does not exist by default, so create `PriExecutable` as string value and type path of `WinMergeU.exe` you mainly use as its value. If this value exists ShellExtension does not care about another path value.
