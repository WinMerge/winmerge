Some useful scripts:

BuildManualChm.bat
------------------
Batch file for creating HTML help (.chm) manual.


BuildManualWeb.bat
------------------
Batch file for creating manual as separate HTML files.


changelog.py
------------
 Python script creating a changelog from SVN. The script reads a commit log
 from SVN and outputs formatted changelog.

 Usage: python changelog.py [-h] [--help] [-r:n] [--revisions:n]
  Where:
   -h, --help print usage help
   -r:n, --revisions:n output log for last n revisions (default is 100)


CheckUnusedResources.vbs
------------------------
Script for creating a list of (hopefully) unused resource IDs.


create_release.py
-----------------
 Script for creating a release. Builds WinMerge components, exports sources,
 creates binary zip folder.

 Run from root folder (Src, Filters etc are subfolders).

 Usage: python create_release.py [-h] [--help] [-v n.n.n.n] [-version n.n.n.n]
  Where:
   -h, --help print usage help
   -v, --version n.n.n.n sets the version number
  For example:
   create_release -v 2.7.7.1


UpdateTranslations.bat
----------------------
Batch file for updating PO template (.pot) file and merging changes to
all PO files.

Remember to call this batch file after every Merge.rc file change!
