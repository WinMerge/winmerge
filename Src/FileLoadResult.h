// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileLoadResult.h
 *
 * @brief Declaration file for file routines
 */
#pragma once

/**
 * @brief File-operation return-statuses
 * Note that FileLoadResult class has no instance data or methods.
 * It is only a namespace for static methods & constants.
 * Everything is public.
 */
class FileLoadResult
{
public:
	/** @brief Return values for functions. */
	typedef unsigned flags_t;
	enum FILES_RESULT : flags_t
	{
		/**
		 * Mask for the main return values.
		 * This mask defines bits used for main return values, separated from
		 * modifier flags.
		 */
		FRESULT_MAIN_MASK = 0xF,

		/**
		 * Error.
		 * This defines general error return value.
		 */
		FRESULT_ERROR = 0x0,
		/**
		 * Success.
		 * This defines general success return value.
		 */
		FRESULT_OK = 0x1,
		/**
		 * Success, but impure file.
		 * The file operation was successful. But the files was detected
		 * to be impure file. Impure file is a file with two or three
		 * different EOL styles.
		 */
		FRESULT_OK_IMPURE = 0x2,
		/**
		 * Binary file.
		 * The file was loaded OK, and was detected to be a binary file.
		 */
		FRESULT_BINARY = 0x3,
		/**
		 * Unpacking plugin failed.
		 * The file was loaded OK, but the unpacking plugin failed.
		 */
		FRESULT_ERROR_UNPACK = 0x4,

		/**
		 * Lossy conversions done.
		 * Unicode / codepage conversions caused lossy conversions.
		 */
		FRESULT_LOSSY = 0x10000,
	};

// Checking results
	/**
	 * Is result an error?
	 * @param [in] flr Return value to test.
	 * @return true if return value is an error value.
	 */
	static bool IsError(flags_t flr) { return Main(flr) == FRESULT_ERROR; }
	/**
	 * Is result a success?
	 * @param [in] flr Return value to test.
	 * @return true if return value is an success value.
	 */
	static bool IsOk(flags_t flr) { return Main(flr) == FRESULT_OK; }
	/**
	 * Is result OK but file is impure?
	 * @param [in] flr Return value to test.
	 * @return true if return value is success and file is impure.
	 */
	static bool IsOkImpure(flags_t flr) { return Main(flr) == FRESULT_OK_IMPURE; }
	/**
	 * Is result binary file?
	 * @param [in] flr Return value to test.
	 * @return true if return value determines binary file.
	 */
	static bool IsBinary(flags_t flr) { return Main(flr) == FRESULT_BINARY; }
	/**
	 * Is result unpack error?
	 * @param [in] flr Return value to test.
	 * @return true if return value determines unpacking error.
	 */
	static bool IsErrorUnpack(flags_t flr) { return Main(flr) == FRESULT_ERROR_UNPACK; }
	/**
	 * Was there lossy conversion involved?
	 * @param [in] flr Return value to test.
	 * @return true if return value determines lossy conversion(s) were done.
	 */
	static bool IsLossy(flags_t flr) { return IsModifier(flr, FRESULT_LOSSY); }

// Assigning results
	// main results
	static void SetMainOk(flags_t & flr) { SetMain(flr, FRESULT_OK); }
	// modifiers
	static void AddModifier(flags_t& flr, flags_t modifier) { flr = (flr | modifier); }

	// bit manipulations
	static void SetMain(flags_t & flr, flags_t newmain) { flr = flr & ~FRESULT_MAIN_MASK; flr = flr | newmain; }
	static flags_t Main(flags_t flr) { return flr & FRESULT_MAIN_MASK; }
	static bool IsModifier(flags_t flr, flags_t modifier) { return (flr & modifier) != 0; }
};
