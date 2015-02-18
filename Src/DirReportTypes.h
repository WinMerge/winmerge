#pragma once

/**
 * @brief Report types in selection list.
 */
typedef enum
{
	REPORT_TYPE_COMMALIST = 0, /**< Comma-separated list */
	REPORT_TYPE_TABLIST, /**< Tab-separated list */
	REPORT_TYPE_SIMPLEHTML, /**< Simple html table */
	REPORT_TYPE_SIMPLEXML, /**< Simple xml */
} REPORT_TYPE;
