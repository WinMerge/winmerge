#include "pch.h"
#include "TableProps.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "FileFilterHelper.h"
#include "MergeApp.h"

TableProps MakeTablePropertiesByFileName(const String& path)
{
	const tchar_t quote = strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_TBL_QUOTE_CHAR));
	FileFilterHelper filterCSV, filterTSV, filterDSV;
	bool allowNewlineIQuotes = GetOptionsMgr()->GetBool(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES);
	const String& csvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_CSV_FILEPATTERNS);
	if (!csvFilePattern.empty())
	{
		filterCSV.SetMaskOrExpression(csvFilePattern);
		if (filterCSV.includeFile(path))
			return { true, strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_CSV_DELIM_CHAR)), quote, allowNewlineIQuotes };
	}
	const String& tsvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_TSV_FILEPATTERNS);
	if (!tsvFilePattern.empty())
	{
		filterTSV.SetMaskOrExpression(tsvFilePattern);
		if (filterTSV.includeFile(path))
			return { true, '\t', quote, allowNewlineIQuotes };
	}
	const String& dsvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_DSV_FILEPATTERNS);
	if (!dsvFilePattern.empty())
	{
		filterDSV.SetMaskOrExpression(dsvFilePattern);
		if (filterDSV.includeFile(path))
			return { true, strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_DSV_DELIM_CHAR)), quote };
	}
	return { false, 0, 0, false };
}
