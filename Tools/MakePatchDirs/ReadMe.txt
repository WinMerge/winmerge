MakePatchDirs is a simple MFC AppWizard generated dialog-based utility which,
when given a directory with subdirectories "Applied" and "Original_all", will
diff them (naively using only modified date) and populate the two subdirectories
"Altered" and "Original". It first deletes any existing subdirectories by those
names ("Altered" and "Original").

It was created by Perry Rapp in the late summer of 2003 for the purpose of 
creating patches for winmerge (http://sourceforge.net/projects/winmerge).


