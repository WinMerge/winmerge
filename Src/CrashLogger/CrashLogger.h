/**
 * @file  CrashLogger.h
 *
 * @brief Lightweight crash information logger for WinMerge
 */
#pragma once

namespace CrashLogger
{
	/**
	 * @brief Install the unhandled exception filter
	 * @return true if successful
	 */
	bool Install();

	/**
	 * @brief Disable crash logging
	 * @note Call this before application exit to avoid logging shutdown crashes
	 */
	void Disable();

	/**
	 * @brief Check if previous crash exists without reporting
	 * @return true if crash info file exists
	 */
	bool HasPreviousCrash();

	/**
	 * @brief Check for previous crash and report to output pane
	 * @note Call this during application startup or idle
	 */
	void CheckAndReportPreviousCrash();
}
