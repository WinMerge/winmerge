/**
 * @file ToolbarButtons.cpp
 * @brief Implementation file for toolbar button management.
 */
#include "StdAfx.h"
#include "ToolbarButtons.h"
#include "resource.h"

static constexpr UINT g_toolbarImageOrder[] =
{
	ID_FILE_NEW, ID_FILE_OPEN,
	ID_FILE_SAVE, ID_EDIT_UNDO, ID_EDIT_REDO,
	ID_SELECTLINEDIFF,
	ID_NEXTDIFF, ID_PREVDIFF,
	ID_NEXTCONFLICT, ID_PREVCONFLICT,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF,
	ID_L2R, ID_R2L, ID_L2RNEXT, ID_R2LNEXT,
	ID_ALL_RIGHT, ID_ALL_LEFT,
	ID_AUTO_MERGE,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE,
	ID_OPTIONS, ID_REFRESH,
	ID_MERGE_CHOOSE_LEFT, ID_MERGE_CHOOSE_MIDDLE, ID_MERGE_CHOOSE_RIGHT
};

static constexpr int TOOLBAR_IMAGE_COUNT = static_cast<int>(std::size(g_toolbarImageOrder));

static constexpr UINT g_toolbarEmpty[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, 0, ID_OPTIONS,
};

static constexpr UINT g_toolbarOpenView[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, 0,
	ID_FILE_SAVE, 0, ID_EDIT_UNDO, 0, ID_OPTIONS,
};

static constexpr UINT g_toolbarFileCmp2[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarFileCmp2WithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarFileCmp3[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarFileCmp3WithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarFileCmp4[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_OPTIONS, 0, ID_REFRESH, 0,
	ID_MERGE_CHOOSE_LEFT, ID_MERGE_CHOOSE_MIDDLE, ID_MERGE_CHOOSE_RIGHT
};

static constexpr UINT g_toolbarFileCmp4WithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_SELECTLINEDIFF, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH, 0,
	ID_MERGE_CHOOSE_LEFT, ID_MERGE_CHOOSE_MIDDLE, ID_MERGE_CHOOSE_RIGHT
};

static constexpr UINT g_toolbarBinaryImageCmp2[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarBinaryImageCmp2WithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarBinaryImageCmp3[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarBinaryImageCmp3WithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, ID_EDIT_REDO, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_NEXTCONFLICT, ID_PREVCONFLICT, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0, ID_L2RNEXT, ID_R2LNEXT, 0,
	ID_ALL_RIGHT, ID_ALL_LEFT, 0,
	ID_AUTO_MERGE, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarWebpageCmp[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarWebpageCmpWithDirDoc[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_LASTDIFF, 0,
	ID_FIRSTFILE, ID_PREVFILE, ID_NEXTFILE, ID_LASTFILE, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

static constexpr UINT g_toolbarDirCmp[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, 0,
	ID_EDIT_UNDO, 0,
	ID_NEXTDIFF, ID_PREVDIFF, 0,
	ID_FIRSTDIFF, ID_CURDIFF, ID_LASTDIFF, 0,
	ID_L2R, ID_R2L, 0,
	ID_OPTIONS, 0, ID_REFRESH
};

namespace ToolbarButtons
{

std::vector<UINT> GetToolbarButtons(FRAMETYPE frameType, int nFiles, bool hasDirDoc, bool hasMergeResultPane)
{
	const UINT* toolbarIcons = nullptr;
	size_t toolbarIconCount = 0;
	if (frameType == FRAME_FOLDER)
	{
		toolbarIcons = g_toolbarDirCmp;
		toolbarIconCount = _countof(g_toolbarDirCmp);
	}
	else if (frameType == FRAME_OTHER)
	{
		toolbarIcons = g_toolbarOpenView;
		toolbarIconCount = _countof(g_toolbarOpenView);
	}
	else if (frameType == FRAME_FILE)
	{
		if (nFiles < 3)
		{
			toolbarIcons = hasDirDoc ? g_toolbarFileCmp2WithDirDoc : g_toolbarFileCmp2;
			toolbarIconCount = hasDirDoc ? _countof(g_toolbarFileCmp2WithDirDoc) : _countof(g_toolbarFileCmp2);
		}
		else if (!hasMergeResultPane)
		{
			toolbarIcons = hasDirDoc ? g_toolbarFileCmp3WithDirDoc : g_toolbarFileCmp3;
			toolbarIconCount = hasDirDoc ? _countof(g_toolbarFileCmp3WithDirDoc) : _countof(g_toolbarFileCmp3);
		}
		else
		{
			toolbarIcons = hasDirDoc ? g_toolbarFileCmp4WithDirDoc : g_toolbarFileCmp4;
			toolbarIconCount = hasDirDoc ? _countof(g_toolbarFileCmp4WithDirDoc) : _countof(g_toolbarFileCmp4);
		}
	}
	else if (frameType == FRAME_HEXFILE || frameType == FRAME_IMGFILE)
	{
		if (nFiles < 3)
		{
			toolbarIcons = hasDirDoc ? g_toolbarBinaryImageCmp2WithDirDoc : g_toolbarBinaryImageCmp2;
			toolbarIconCount = hasDirDoc ? _countof(g_toolbarBinaryImageCmp2WithDirDoc) : _countof(g_toolbarBinaryImageCmp2);
		}
		else
		{
			toolbarIcons = hasDirDoc ? g_toolbarBinaryImageCmp3WithDirDoc : g_toolbarBinaryImageCmp3;
			toolbarIconCount = hasDirDoc ? _countof(g_toolbarBinaryImageCmp3WithDirDoc) : _countof(g_toolbarBinaryImageCmp3);
		}
	}
	else if (frameType == FRAME_WEBPAGE)
	{
		toolbarIcons = hasDirDoc ? g_toolbarWebpageCmpWithDirDoc : g_toolbarWebpageCmp;
		toolbarIconCount = hasDirDoc ? _countof(g_toolbarWebpageCmpWithDirDoc) : _countof(g_toolbarWebpageCmp);
	}
	else
	{
		toolbarIcons = g_toolbarEmpty;
		toolbarIconCount = _countof(g_toolbarEmpty);
	}
	return std::vector<UINT>(toolbarIcons, toolbarIcons + toolbarIconCount);
}

int GetToolbarImageIndex(UINT nID)
{
	int iImage = 0;
	auto it = std::find(std::begin(g_toolbarImageOrder), std::end(g_toolbarImageOrder), nID);
	if (it != std::end(g_toolbarImageOrder))
		iImage = static_cast<int>(std::distance(std::begin(g_toolbarImageOrder), it));
	return iImage;
}

int GetToolbarImageCount()
{
	return TOOLBAR_IMAGE_COUNT;
}

}