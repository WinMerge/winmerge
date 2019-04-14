Some useful scripts:


BuildManualChm.bat
------------------
Batch file for creating HTML help (.chm) manual.


BuildManualWeb.bat
------------------
Batch file for creating manual as separate HTML files.


CheckMenuResources.vbs
----------------------
Script that checking the menu resources for missing mnemonic chars and description texts.


CheckUnusedResources.vbs
------------------------
Script for creating a list of (hopefully) unused resource IDs.


SetVersions.py
--------------
 A script for setting version numbers of different components.
 See the file SetVersions.txt for documentation of the script.


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
