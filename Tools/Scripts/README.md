Some useful scripts:


CheckMenuResources.vbs
----------------------
Script that checking the menu resources for missing mnemonic chars and description texts.


CheckUnusedResources.vbs
------------------------
Script for creating a list of (hopefully) unused resource IDs.


tsvn_patch.py
-------------
 Script for cleaning up TortoiseSVN created patch files to use with GNU patch.

```
 Usage: python tsvn_patch.py [-b] patchfile
  where:
   -b, --nobak skip creating a backup file of the original patch file
```


UpdateTranslations.bat
----------------------
Batch file for updating PO template (.pot) file and merging changes to
all PO files.

Remember to call this batch file after every Merge.rc file change!
