#include "pch.h"
#include "FileContentRef.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "UniFile.h"
#include "codepage_detect.h"
#include "paths.h"
#include "MergeApp.h"

bool FileContentRef::operator==(const FileContentRef& other) const
{
	return path == other.path;
}

bool FileContentRef::Contains(const std::string& str) const
{
	UniMemFile file;
	if (!file.OpenReadOnly(path))
		return false;
	file.ReadBom();
	if (!file.HasBom())
	{
		int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
		int64_t fileSize = file.GetFileSize();
		FileTextEncoding encoding = codepage_detect::Guess(
			paths::FindExtension(path), file.GetBase(), static_cast<size_t>(
				fileSize < static_cast<int64_t>(codepage_detect::BufSize) ?
				fileSize : static_cast<int64_t>(codepage_detect::BufSize)),
			iGuessEncodingType);
		file.SetCodepage(encoding.m_codepage);
	}
	String line, eol;
	bool lossy = false;
	//file.ReadString(line, eol, lossy);
	file.Close();
	return true;
}

bool FileContentRef::REContains(const Poco::RegularExpression& regexp) const
{
	return true;
}

