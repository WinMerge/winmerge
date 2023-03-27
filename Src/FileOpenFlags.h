#pragma once

#include <cstdint>

/**
 * @brief Flags used when opening files
 */
typedef uint32_t fileopenflags_t;
enum : fileopenflags_t
{
	FFILEOPEN_NONE = 0x0000,
	FFILEOPEN_NOMRU = 0x0001, /**< Do not add this path to MRU list */
	FFILEOPEN_READONLY = 0x0002, /**< Open this path as read-only */
	FFILEOPEN_MODIFIED = 0x0004, /**< Mark file modified after opening. */
	FFILEOPEN_CMDLINE = 0x0010, /**< Path is read from commandline */
	FFILEOPEN_PROJECT = 0x0020, /**< Path is read from project-file */
	FFILEOPEN_SETFOCUS = 0x0040, /**< Set focus to this pane */
	FFILEOPEN_AUTOMERGE = 0x0080, /**< auto-merge at this pane */
};
