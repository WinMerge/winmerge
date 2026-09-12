#pragma once

enum FRAMETYPE
{
	FRAME_NONE, /**< No frame. */
	FRAME_FOLDER, /**< Folder compare frame. */
	FRAME_FILE, /**< File compare frame. */
	FRAME_HEXFILE, /**< Hex file compare frame. */
	FRAME_IMGFILE, /**< Image file compare frame. */
	FRAME_WEBPAGE, /**< Web page compare frame. */
	FRAME_OTHER, /**< No frame? */
};
