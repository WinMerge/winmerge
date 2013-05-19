#include "DragDrop.h"
#include "paths.h"
#include <boost/scoped_array.hpp>

//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//

bool GetDroppedFiles(HDROP dropInfo, std::vector<String>& files)
{
// Get the number of pathnames that have been dropped
	UINT wNumFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
	UINT fileCount = 0;

	// get all file names. but we'll only need the first one.
	for (WORD x = 0 ; x < wNumFilesDropped; x++)
	{
		// Get the number of bytes required by the file's full pathname
		UINT wPathnameSize = DragQueryFile(dropInfo, x, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize += 1;
		boost::scoped_array<TCHAR> npszFile(new TCHAR[wPathnameSize]);

		// Copy the pathname into the buffer
		DragQueryFile(dropInfo, x, npszFile.get(), wPathnameSize);

		if (x < 3)
		{
			files.resize(x + 1);
			files[x] = npszFile.get();
			fileCount++;
		}
	}

	// Free the memory block containing the dropped-file information
	DragFinish(dropInfo);

	for (UINT i = 0; i < fileCount; i++)
	{
		if (paths_IsShortcut(files[i]))
		{
			// if this was a shortcut, we need to expand it to the target path
			String expandedFile = ExpandShortcut(files[i]);

			// if that worked, we should have a real file name
			if (!expandedFile.empty())
				files[i] = expandedFile;
		}
	}
	return true;
}
