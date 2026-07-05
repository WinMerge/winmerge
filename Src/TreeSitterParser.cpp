////////////////////////////////////////////////////////////////////////////
//  File:       TreeSitterParser.cpp
//  Version:    1.0.1
//  Created:    2026-03-26
//
//  Tree-sitter based syntax highlighting bridge for CrystalEdit.
//
//  SPDX-License-Identifier: GPL-2.0-or-later
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <Windows.h>
#include "TreeSitterParser.h"
#include "ISyntaxParser.h"
#include "ITextBuffer.h"
#include "TextDefinition.h"
#include "unicoder.h"

#include <tree_sitter/api.h>

#include <fstream>
#include <algorithm>
#include <climits>
#include <cstring>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
bool HasCapturePrefix(const std::string& captureName, const char* prefix)
{
	const size_t prefixLen = strlen(prefix);
	return captureName == prefix ||
		(captureName.size() > prefixLen &&
		 captureName.compare(0, prefixLen, prefix) == 0 &&
		 captureName[prefixLen] == '.');
}

int CountCaptureSegments(const std::string& captureName)
{
	return static_cast<int>(std::count(captureName.begin(), captureName.end(), '.')) + 1;
}

int MakeCapturePriority(const std::string& captureName, uint32_t startByte, uint32_t endByte)
{
	const uint32_t span = (endByte > startByte) ? (endByte - startByte) : 0;
	const int specificityScore = CountCaptureSegments(captureName) * 200000;
	const int spanScore = 100000 - static_cast<int>(std::min<uint32_t>(span, 100000));
	return specificityScore + spanScore;
}

int GetColorPriority(int colorIndex)
{
	switch (colorIndex)
	{
	case COLORINDEX_FUNCNAME:     return 100;
	case COLORINDEX_USER1:        return 90;
	case COLORINDEX_KEYWORD:      return 80;
	case COLORINDEX_USER2:        return 70;
	case COLORINDEX_STRING:       return 60;
	case COLORINDEX_NUMBER:       return 50;
	case COLORINDEX_OPERATOR:     return 40;
	case COLORINDEX_COMMENT:      return 30;
	default:                      return 0;
	}
}

struct TSQueryCursorDeleter {
	void operator()(TSQueryCursor* p) const { ts_query_cursor_delete(p); }
};
using UniqueTSQueryCursor = std::unique_ptr<TSQueryCursor, TSQueryCursorDeleter>;

static constexpr uint64_t MakeByteRangeKey(uint32_t startByte, uint32_t endByte)
{
	return (static_cast<uint64_t>(startByte) << 32) | endByte;
}

}

// ============================================================================
// CTreeSitterColorMap
// ============================================================================

CTreeSitterColorMap::CTreeSitterColorMap()
{
	// Map standard tree-sitter highlight capture names to WinMerge COLORINDEX.
	//
	// Tree-sitter highlight queries use capture names like @keyword, @string,
	// @comment, etc. These names come from nvim-treesitter conventions.
	// We collapse them into WinMerge's available color categories.

	// Keywords: control flow, storage, operators as keywords
	m_map["keyword"]                = COLORINDEX_KEYWORD;
	m_map["keyword.function"]       = COLORINDEX_KEYWORD;
	m_map["keyword.operator"]       = COLORINDEX_KEYWORD;
	m_map["keyword.import"]         = COLORINDEX_KEYWORD;
	m_map["keyword.type"]           = COLORINDEX_KEYWORD;
	m_map["keyword.modifier"]       = COLORINDEX_KEYWORD;
	m_map["keyword.repeat"]         = COLORINDEX_KEYWORD;
	m_map["keyword.return"]         = COLORINDEX_KEYWORD;
	m_map["keyword.conditional"]    = COLORINDEX_KEYWORD;
	m_map["keyword.exception"]      = COLORINDEX_KEYWORD;
	m_map["keyword.directive"]      = COLORINDEX_PREPROCESSOR;
	m_map["keyword.coroutine"]      = COLORINDEX_KEYWORD;
	m_map["include"]                = COLORINDEX_KEYWORD;
	m_map["repeat"]                 = COLORINDEX_KEYWORD;
	m_map["conditional"]            = COLORINDEX_KEYWORD;
	m_map["exception"]              = COLORINDEX_KEYWORD;

	// Functions
	m_map["function"]               = COLORINDEX_FUNCNAME;
	m_map["function.call"]          = COLORINDEX_FUNCNAME;
	m_map["function.builtin"]       = COLORINDEX_FUNCNAME;
	m_map["function.macro"]         = COLORINDEX_FUNCNAME;
	m_map["method"]                 = COLORINDEX_FUNCNAME;
	m_map["method.call"]            = COLORINDEX_FUNCNAME;
	m_map["constructor"]            = COLORINDEX_NORMALTEXT;

	// Comments
	m_map["comment"]                = COLORINDEX_COMMENT;
	m_map["comment.documentation"]  = COLORINDEX_COMMENT;

	// Strings
	m_map["string"]                 = COLORINDEX_STRING;
	m_map["string.documentation"]   = COLORINDEX_STRING;
	m_map["string.regex"]           = COLORINDEX_STRING;
	m_map["string.escape"]          = COLORINDEX_STRING;
	m_map["string.special"]         = COLORINDEX_STRING;
	m_map["character"]              = COLORINDEX_STRING;
	m_map["character.special"]      = COLORINDEX_STRING;

	// Numbers
	m_map["number"]                 = COLORINDEX_NUMBER;
	m_map["number.float"]           = COLORINDEX_NUMBER;
	m_map["float"]                  = COLORINDEX_NUMBER;
	m_map["boolean"]                = COLORINDEX_NUMBER;

	// Operators and punctuation
	m_map["operator"]               = COLORINDEX_OPERATOR;
	m_map["punctuation"]            = COLORINDEX_OPERATOR;
	m_map["punctuation.bracket"]    = COLORINDEX_OPERATOR;
	m_map["punctuation.delimiter"]  = COLORINDEX_OPERATOR;
	m_map["punctuation.special"]    = COLORINDEX_OPERATOR;

	// Preprocessor / attributes
	m_map["preproc"]                = COLORINDEX_PREPROCESSOR;
	m_map["define"]                 = COLORINDEX_PREPROCESSOR;
	m_map["attribute"]              = COLORINDEX_PREPROCESSOR;
	m_map["attribute.builtin"]      = COLORINDEX_PREPROCESSOR;

	// Types -> USER1 (types are important in F# for merge understanding)
	m_map["type"]                   = COLORINDEX_USER1;
	m_map["type.builtin"]           = COLORINDEX_USER1;
	m_map["type.definition"]        = COLORINDEX_USER1;
	m_map["type.qualifier"]         = COLORINDEX_USER1;
	m_map["storageclass"]           = COLORINDEX_USER1;

	// Variables / properties / modules -> USER1, USER2 or NORMALTEXT
	m_map["variable"]               = COLORINDEX_NORMALTEXT;
	m_map["variable.builtin"]       = COLORINDEX_USER1;
	m_map["variable.parameter"]     = COLORINDEX_NORMALTEXT;
	m_map["variable.member"]        = COLORINDEX_USER1;
	m_map["property"]               = COLORINDEX_USER1;
	m_map["field"]                  = COLORINDEX_NORMALTEXT;
	m_map["constant"]               = COLORINDEX_NORMALTEXT;
	m_map["constant.builtin"]       = COLORINDEX_USER1;
	m_map["constant.macro"]         = COLORINDEX_USER1;
	m_map["module"]                 = COLORINDEX_USER1;
	m_map["namespace"]              = COLORINDEX_USER1;
	m_map["label"]                  = COLORINDEX_USER2;
	m_map["tag"]                    = COLORINDEX_KEYWORD;
	m_map["tag.attribute"]          = COLORINDEX_USER2;
	m_map["tag.delimiter"]          = COLORINDEX_OPERATOR;

	m_map["text"]                   = COLORINDEX_NORMALTEXT;
	m_map["text.title"]             = COLORINDEX_KEYWORD;
	m_map["text.uri"]               = COLORINDEX_USER2;
	m_map["text.reference"]         = COLORINDEX_USER1;
	m_map["text.literal"]           = COLORINDEX_STRING;
	m_map["text.escape"]            = COLORINDEX_OPERATOR;
}

int CTreeSitterColorMap::MapCapture(const std::string& sCaptureName) const
{
	// Try exact match first
	auto it = m_map.find(sCaptureName);
	if (it != m_map.end())
		return it->second;

	// Try prefix match: e.g. "keyword.control.fsharp" -> "keyword"
	std::string prefix = sCaptureName;
	while (true)
	{
		auto pos = prefix.rfind('.');
		if (pos == std::string::npos)
			break;
		prefix = prefix.substr(0, pos);
		it = m_map.find(prefix);
		if (it != m_map.end())
			return it->second;
	}

	return COLORINDEX_NORMALTEXT;
}


// ============================================================================
// CTreeSitterLanguage
// ============================================================================

CTreeSitterLanguage::CTreeSitterLanguage()
	: m_hDll(nullptr)
	, m_pLanguage(nullptr)
	, m_pHighlightQuery(nullptr)
	, m_pLocalsQuery(nullptr)
	, m_pTagsQuery(nullptr)
	, m_pInjectionQuery(nullptr)
{
}

CTreeSitterLanguage::~CTreeSitterLanguage()
{
	if (m_pHighlightQuery)
		ts_query_delete(m_pHighlightQuery);
	if (m_pLocalsQuery)
		ts_query_delete(m_pLocalsQuery);
	if (m_pTagsQuery)
		ts_query_delete(m_pTagsQuery);
	if (m_pInjectionQuery)
		ts_query_delete(m_pInjectionQuery);
	if (m_hDll)
		FreeLibrary(m_hDll);
}

TSQuery* CTreeSitterLanguage::LoadQuery(const std::wstring& sPath)
{
	std::ifstream file(sPath, std::ios::binary);
	if (!file.is_open())
		return nullptr;

	std::string source((std::istreambuf_iterator<char>(file)),
					    std::istreambuf_iterator<char>());

	uint32_t errorOffset = 0;
	TSQueryError errorType = TSQueryErrorNone;
	TSQuery* pQuery = ts_query_new(
		m_pLanguage,
		source.c_str(),
		static_cast<uint32_t>(source.size()),
		&errorOffset,
		&errorType);

	if (errorType != TSQueryErrorNone)
	{
		// Query compilation failed - log but continue without this query
		return nullptr;
	}

	return pQuery;
}

bool CTreeSitterLanguage::Load(const std::wstring& sGrammarDir, const std::wstring& sLanguage)
{
	m_sName = sLanguage;

	// Load the grammar DLL
	// Expected name: tree-sitter-<language>.dll (e.g. tree-sitter-fsharp.dll)
	std::wstring sDllPath = sGrammarDir + L"\\tree-sitter-" + sLanguage + L".dll";
	m_hDll = LoadLibraryW(sDllPath.c_str());
	if (!m_hDll)
		return false;

	// Get the language function
	// Expected export: tree_sitter_<language> (e.g. tree_sitter_fsharp)
	// Note: hyphens in language names are converted to underscores for the
	// C export name (e.g. "c-sharp" -> "tree_sitter_c_sharp")
	std::string sFuncName = "tree_sitter_";
	for (wchar_t ch : sLanguage)
	{
		if (ch == L'-')
			sFuncName += '_';
		else
			sFuncName += static_cast<char>(ch);  // ASCII language names only
	}

	typedef const TSLanguage* (*TSLanguageFunc)();
	TSLanguageFunc pfnLanguage = reinterpret_cast<TSLanguageFunc>(
		GetProcAddress(m_hDll, sFuncName.c_str()));
	if (!pfnLanguage)
	{
		FreeLibrary(m_hDll);
		m_hDll = nullptr;
		return false;
	}

	m_pLanguage = pfnLanguage();
	if (!m_pLanguage)
	{
		FreeLibrary(m_hDll);
		m_hDll = nullptr;
		return false;
	}

	// Load highlight query (.scm file) - required
	// Expected name: <language>-highlights.scm (e.g. fsharp-highlights.scm)
	std::wstring sHighlightPath = sGrammarDir + L"\\" + sLanguage + L"-highlights.scm";
	m_pHighlightQuery = LoadQuery(sHighlightPath);

	// Load locals query (.scm file) - optional
	// Expected name: <language>-locals.scm (e.g. fsharp-locals.scm)
	std::wstring sLocalsPath = sGrammarDir + L"\\" + sLanguage + L"-locals.scm";
	m_pLocalsQuery = LoadQuery(sLocalsPath);

	// Load tags query (.scm file) - optional
	// Expected name: <language>-tags.scm (e.g. python-tags.scm)
	std::wstring sTagsPath = sGrammarDir + L"\\" + sLanguage + L"-tags.scm";
	m_pTagsQuery = LoadQuery(sTagsPath);

	// Load injection query (.scm file) - optional
	// Expected name: <language>-injections.scm (e.g. html-injections.scm)
	std::wstring sInjectionPath = sGrammarDir + L"\\" + sLanguage + L"-injections.scm";
	m_pInjectionQuery = LoadQuery(sInjectionPath);

	return true;
}


// ============================================================================
// CTreeSitterParser
// ============================================================================

static const wchar_t* GetLanguageNameForId(LangServices::LanguageId id);

/**
 * @brief Construct a Tree-sitter parser adapter.
 */
CTreeSitterParser::CTreeSitterParser(LangServices::LanguageId textType)
	: m_pTextBuffer(nullptr)
	, m_pParser(nullptr)
	, m_pTree(nullptr)
	, m_pLang(nullptr)
	, m_bNeedsParse(false)
	, m_bTagsQueried(false)
	, m_nLineCount(0)
	, m_textType(textType)
	, m_bTreeSitterDisabled(false)
{
	auto& registry = TreeSitterRegistry::Instance();
	if (!registry.IsInitialized())
		registry.Initialize();
	auto* name = GetLanguageNameForId(textType);
	auto* pLanguage = registry.GetLanguageForName(name);
	SetLanguage(pLanguage);
}

CTreeSitterParser::~CTreeSitterParser()
{
	if (m_pTree)
		ts_tree_delete(m_pTree);
	if (m_pParser)
		ts_parser_delete(m_pParser);
}

/**
 * @brief Lazily create the TSParser instance on first use.
 *
 * This avoids calling ts_parser_new() in the constructor, which would
 * happen for every CMergeEditView even when tree-sitter isn't needed.
 */
void CTreeSitterParser::EnsureParser()
{
	if (!m_pParser)
		m_pParser = ts_parser_new();
}

void CTreeSitterParser::SetLanguage(const CTreeSitterLanguage* pLang)
{
	m_pLang = pLang;
	if (pLang && pLang->GetLanguage())
	{
		EnsureParser();
		if (m_pParser)
			ts_parser_set_language(m_pParser, pLang->GetLanguage());
	}
	Invalidate();
}

void CTreeSitterParser::Invalidate()
{
	if (m_pTree)
	{
		ts_tree_delete(m_pTree);
		m_pTree = nullptr;
	}
	m_lineBlocks.clear();
	m_localScopes.clear();
	m_localRefHighlights.clear();
	m_pendingRefs.clear();
	m_tagDefs.clear();
	m_tagRefs.clear();
	m_lineStartBytes.clear();
	m_cachedChunks.clear();
	m_nLineCount = 0;
	m_nextBlockOrder = 0;
	m_bNeedsParse = true;
	m_bTagsQueried = false;
}

void CTreeSitterParser::ParseDocument()
{
	if (!m_pTextBuffer)
		return;

	m_nLineCount = m_pTextBuffer->GetLineCount();
	if (m_nLineCount == 0)
		return;

	EnsureParser();
	if (!m_pParser || !m_pLang || !m_pLang->GetLanguage())
		return;

	m_lineStartBytes.resize(m_nLineCount + 1);
	uint32_t offset = 0;
	for (int i = 0; i < m_nLineCount; i++)
	{
		m_lineStartBytes[i] = offset;
		offset += static_cast<uint32_t>(m_pTextBuffer->GetLineLength(i) * sizeof(wchar_t));
		if (i < m_nLineCount - 1)
			offset += sizeof(wchar_t);  // '\n' separator
	}
	m_lineStartBytes[m_nLineCount] = offset;

	m_cachedChunks.assign((m_nLineCount + kCacheChunkSize - 1) / kCacheChunkSize, false);

	// Build per-line block arrays
	m_lineBlocks.clear();
	m_lineBlocks.resize(m_nLineCount);

	struct Payload
	{
		LangServices::ITextBuffer* pBuffer;
		int nLineCount;
		wchar_t newline;
	};
	Payload payload{ m_pTextBuffer, m_nLineCount, L'\n' };

	TSInput input;
	input.payload = &payload;
	input.encoding = TSInputEncodingUTF16LE;
	input.read = [](void* pPayload, uint32_t /*byteOffset*/,
		TSPoint position, uint32_t* bytesRead) -> const char*
		{
			auto* p = static_cast<Payload*>(pPayload);
			const int row = static_cast<int>(position.row);

			if (row >= p->nLineCount)
			{
				*bytesRead = 0;
				return nullptr;
			}

			const tchar_t* pszLine = p->pBuffer->GetLineChars(row);
			const int nLen = p->pBuffer->GetLineLength(row);
			const uint32_t colChars = position.column / sizeof(wchar_t);

			if (colChars < static_cast<uint32_t>(nLen))
			{
				*bytesRead = (static_cast<uint32_t>(nLen) - colChars) * sizeof(wchar_t);
				return reinterpret_cast<const char*>(pszLine + colChars);
			}
			else if (colChars == static_cast<uint32_t>(nLen))
			{
				if (row < p->nLineCount - 1)
				{
					*bytesRead = sizeof(wchar_t);
					return reinterpret_cast<const char*>(&p->newline);
				}
			}

			*bytesRead = 0;
			return nullptr;
		};

	TSTree* pOldTree = m_pTree;
	if (m_nLineCount < kMaxLinesForHighlight)
		m_pTree = ts_parser_parse(m_pParser, pOldTree, input);
	else
		m_pTree = nullptr;

	if (pOldTree)
		ts_tree_delete(pOldTree);

	if (m_pTree)
	{
		m_nextBlockOrder = 0;
		RunLocalsQuery();
	}

	// Cache is now fresh
	m_bNeedsParse = false;
}

/**
 * @brief Notify the parser of an edit for incremental reparsing.
 */
void CTreeSitterParser::NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType)
{
	if (!m_pTree || !m_pTextBuffer)
		return;

	TSInputEdit tsEdit{};

	uint32_t startByte = CharPosToByteOffset(ptStartPos.y, ptStartPos.x);
	tsEdit.start_byte = startByte;
	CharPosToTSPoint(ptStartPos.y, ptStartPos.x, tsEdit.start_point);

	if (bInsert)
	{
		tsEdit.old_end_byte = startByte;
		tsEdit.old_end_point = tsEdit.start_point;

		tsEdit.new_end_byte = CharPosToByteOffset(ptEndPos.y, ptEndPos.x, false);
		CharPosToTSPoint(ptEndPos.y, ptEndPos.x, tsEdit.new_end_point);
	}
	else
	{
		tsEdit.old_end_byte = CharPosToByteOffset(ptEndPos.y, ptEndPos.x);
		CharPosToTSPoint(ptEndPos.y, ptEndPos.x, tsEdit.old_end_point);

		tsEdit.new_end_byte = startByte;
		tsEdit.new_end_point = tsEdit.start_point;
	}

	ts_tree_edit(m_pTree, &tsEdit);
	m_bNeedsParse = true;
	m_bTagsQueried = false;
}

/**
 * @brief Ensure the document is parsed if the cache is dirty.
 *
 * Called lazily from ParseLine during the paint cycle. This means
 * we reparse at most once per paint, not once per keystroke.
 */
void CTreeSitterParser::EnsureParsed(int nLineIndex)
{
	if (m_bTreeSitterDisabled)
		return;

	if (m_bNeedsParse && m_pLang)
		ParseDocument();

	if (nLineIndex < 0 || nLineIndex >= m_nLineCount)
		return;

	const int chunk = nLineIndex / kCacheChunkSize;
	if (m_cachedChunks[chunk])
		return;

	const int nStartLine = chunk * kCacheChunkSize;
	const int nEndLine = (std::min)(m_nLineCount - 1, nStartLine + kCacheChunkSize - 1);

	for (int i = nStartLine; i <= nEndLine && i < static_cast<int>(m_lineBlocks.size()); i++)
		m_lineBlocks[i].clear();

	RunHighlightQuery(nStartLine, nEndLine);
	RunInjectionQuery(nStartLine, nEndLine);
	BuildLineCache(nStartLine, nEndLine);

	m_cachedChunks[chunk] = true;
}

void CTreeSitterParser::EnsureTagsQueried()
{
	if (!m_bTagsQueried)
		RunTagsQuery();
}

uint32_t CTreeSitterParser::CharPosToByteOffset(int nLine, int nCharPos, bool useCache) const
{
	if (!useCache)
	{
		if (!m_pTextBuffer)
			return 0;
		const int nLines = m_pTextBuffer->GetLineCount();
		uint32_t byteOffset = 0;
		for (int i = 0; i < nLine && i < nLines; i++)
		{
			byteOffset += static_cast<uint32_t>(
				m_pTextBuffer->GetLineLength(i) * sizeof(wchar_t));
			byteOffset += sizeof(wchar_t);  // '\n'
		}
		if (nLine >= nLines)
			return byteOffset;
		return byteOffset + static_cast<uint32_t>(nCharPos * sizeof(wchar_t));
	}

	if (m_lineStartBytes.empty())
		return 0;
	const int nLines = static_cast<int>(m_lineStartBytes.size()) - 1;
	if (nLine >= nLines)
		return m_lineStartBytes[nLines];  // end of document
	return m_lineStartBytes[nLine] + static_cast<uint32_t>(nCharPos * sizeof(wchar_t));
}

void CTreeSitterParser::CharPosToTSPoint(int line, int charPos, TSPoint& pt) const
{
	pt.row = static_cast<uint32_t>(line);
	pt.column = static_cast<uint32_t>(charPos * sizeof(wchar_t));
}

std::wstring CTreeSitterParser::GetUtf16Text(uint32_t startByte, uint32_t endByte) const
{
	std::wstring result;
	if (m_lineStartBytes.empty())
		return result;

	const int nLines = static_cast<int>(m_lineStartBytes.size()) - 1;

	// Binary search for the first line containing startByte
	int startLine = static_cast<int>(std::upper_bound(m_lineStartBytes.begin(), m_lineStartBytes.begin() + nLines + 1,
			startByte) - m_lineStartBytes.begin()) - 1;
	if (startLine < 0)
		startLine = 0;

	for (int i = startLine; i < nLines; i++)
	{
		const uint32_t lineStart = m_lineStartBytes[i];
		const uint32_t lineEnd = m_lineStartBytes[i + 1] - (i < nLines - 1 ? sizeof(wchar_t) : 0);

		if (startByte >= m_lineStartBytes[i + 1])
			continue;
		if (endByte <= lineStart)
			break;

		const uint32_t sChar = (startByte > lineStart ? startByte - lineStart : 0) / sizeof(wchar_t);
		const uint32_t eChar = ((std::min)(endByte, lineEnd) - lineStart) / sizeof(wchar_t);

		const tchar_t* pszLine = m_pTextBuffer->GetLineChars(i);
		if (pszLine && eChar > sChar)
			result.append(pszLine + sChar, eChar - sChar);

		if (endByte > lineEnd + sizeof(wchar_t) - 1 && i < nLines - 1)
			result += L'\n';
	}
	return result;
}

uint32_t CTreeSitterParser::GetTotalBytes() const
{
	if (!m_lineStartBytes.empty())
		return m_lineStartBytes.back();
	return 0;
}

std::vector<const CTreeSitterParser::LocalScope*>
CTreeSitterParser::FindEnclosingScopes(const std::vector<LocalScope>& localScopes, uint32_t startByte, uint32_t endByte)
{
	std::vector<const LocalScope*> result;
	for (const auto& scope : localScopes)
	{
		if (startByte >= scope.startByte && endByte <= scope.endByte)
			result.push_back(&scope);
	}
	std::sort(result.begin(), result.end(),
		[](const LocalScope* a, const LocalScope* b)
		{
			return (a->endByte - a->startByte) < (b->endByte - b->startByte);
		});
	return result;
}

const CTreeSitterParser::LocalDef*
CTreeSitterParser::FindDefinitionInScopes(const std::wstring& name, uint32_t refStartByte, const std::vector<const LocalScope*>& enclosingScopes)
{
	for (const auto* pScope : enclosingScopes)
	{
		for (const auto& def : pScope->defs)
		{
			if (def.name == name && def.startByte <= refStartByte)
				return &def;
		}
		if (!pScope->inherits)
			break;
	}
	return nullptr;
}

/**
 * @brief Extract a #set! predicate property value from a query pattern.
 *
 * Tree-sitter predicates like (#set! injection.language "javascript") are
 * encoded as sequences of TSQueryPredicateStep:
 *   [String "set!", String "injection.language", String "javascript", Done]
 *
 * @param pQuery       The query containing the pattern.
 * @param patternIndex The pattern index.
 * @param key          The property key to look for (e.g. "injection.language").
 * @return The property value, or empty string if not found.
 */
std::string CTreeSitterParser::GetSetProperty(const TSQuery* pQuery,
					                           uint32_t patternIndex,
					                           const std::string& key)
{
	uint32_t stepCount = 0;
	const TSQueryPredicateStep* steps =
		ts_query_predicates_for_pattern(pQuery, patternIndex, &stepCount);
	if (!steps || stepCount == 0)
		return std::string();

	// Walk through predicate steps looking for: "set!" <key> <value>
	for (uint32_t i = 0; i + 2 < stepCount; i++)
	{
		// Look for a string step containing "set!"
		if (steps[i].type != TSQueryPredicateStepTypeString)
			continue;

		uint32_t nameLen = 0;
		const char* name = ts_query_string_value_for_id(pQuery, steps[i].value_id, &nameLen);
		if (!name || strncmp(name, "set!", nameLen) != 0 || nameLen != 4)
			continue;

		// Next step should be the property key (string)
		if (i + 1 >= stepCount || steps[i + 1].type != TSQueryPredicateStepTypeString)
			continue;

		uint32_t keyLen = 0;
		const char* keyStr = ts_query_string_value_for_id(pQuery, steps[i + 1].value_id, &keyLen);
		if (!keyStr || strncmp(key.c_str(), keyStr, keyLen) != 0 || keyLen != key.length())
		{
			// Skip to the Done sentinel for this predicate
			while (i < stepCount && steps[i].type != TSQueryPredicateStepTypeDone)
				i++;
			continue;
		}

		// Next step should be the property value (string)
		if (i + 2 >= stepCount || steps[i + 2].type != TSQueryPredicateStepTypeString)
			continue;

		uint32_t valLen = 0;
		const char* valStr = ts_query_string_value_for_id(pQuery, steps[i + 2].value_id, &valLen);
		if (valStr)
			return std::string(valStr, valLen);
	}

	return std::string();
}

void CTreeSitterParser::RunTagsQuery()
{
	m_tagDefs.clear();
	m_tagRefs.clear();

	if (!m_pTree || !m_pLang || !m_pLang->GetTagsQuery())
		return;

	const TSQuery* pQuery = m_pLang->GetTagsQuery();
	TSNode rootNode = ts_tree_root_node(m_pTree);

	UniqueTSQueryCursor pCursor(ts_query_cursor_new());
	if (!pCursor)
		return;

	ts_query_cursor_exec(pCursor.get(), pQuery, rootNode);

	uint32_t totalBytes = GetTotalBytes();

	TSQueryMatch match;
	while (ts_query_cursor_next_match(pCursor.get(), &match))
	{
		std::wstring name;
		uint32_t nameStart = 0;
		uint32_t nameEnd = 0;
		bool hasName = false;

		struct Range
		{
			uint32_t startByte;
			uint32_t endByte;
		};
		std::vector<Range> defs;
		std::vector<Range> refs;

		for (uint16_t i = 0; i < match.capture_count; ++i)
		{
			TSQueryCapture capture = match.captures[i];
			TSNode node = capture.node;

			uint32_t captureNameLen = 0;
			const char* captureName = ts_query_capture_name_for_id(
				pQuery, capture.index, &captureNameLen);
			std::string sCapture(captureName, captureNameLen);

			const uint32_t nodeStart = ts_node_start_byte(node);
			const uint32_t nodeEnd = ts_node_end_byte(node);

			if (sCapture == "name")
			{
				if (nodeStart < totalBytes && nodeEnd <= totalBytes)
				{
					name = GetUtf16Text(nodeStart, nodeEnd);
					nameStart = nodeStart;
					nameEnd = nodeEnd;
					hasName = true;
				}
			}
			else if (HasCapturePrefix(sCapture, "definition"))
			{
				defs.push_back({ nodeStart, nodeEnd });
			}
			else if (HasCapturePrefix(sCapture, "reference"))
			{
				refs.push_back({ nodeStart, nodeEnd });
			}
		}

		if (!hasName)
			continue;

		if (defs.empty() && !refs.empty())
		{
			for (const auto& ref : refs)
			{
				const bool useNameRange =
					nameStart >= ref.startByte && nameEnd <= ref.endByte;
				m_tagRefs.push_back({
					name,
					useNameRange ? nameStart : ref.startByte,
					useNameRange ? nameEnd : ref.endByte,
				});
			}
		}

		for (const auto& def : defs)
		{
			const bool useNameRange =
				nameStart >= def.startByte && nameEnd <= def.endByte;
			m_tagDefs.push_back({
				name,
				useNameRange ? nameStart : def.startByte,
				useNameRange ? nameEnd : def.endByte,
			});
		}
	}

	m_bTagsQueried = true;
}

/**
 * @brief Run the locals query to build scope/definition/reference information.
 *
 * Processes locals.scm captures:
 *   @local.scope      - Defines a scope boundary
 *   @local.definition  - Defines a local variable/symbol
 *   @local.reference   - References a local variable/symbol
 *
 * The results are stored in m_localScopes and m_localRefHighlights,
 * which are used by RunHighlightQuery to provide scope-aware coloring.
 */
void CTreeSitterParser::RunLocalsQuery()
{
	m_localScopes.clear();
	m_localRefHighlights.clear();

	if (!m_pTree || !m_pLang || !m_pLang->GetLocalsQuery())
		return;

	const TSQuery* pQuery = m_pLang->GetLocalsQuery();
	TSNode rootNode = ts_tree_root_node(m_pTree);

	UniqueTSQueryCursor pCursor(ts_query_cursor_new());
	if (!pCursor)
		return;

	ts_query_cursor_exec(pCursor.get(), pQuery, rootNode);

	// Temporary storage for references (resolved after all defs are collected)
	std::vector<PendingRef> references;
	uint32_t totalBytes = GetTotalBytes();

	TSQueryMatch match;
	while (ts_query_cursor_next_match(pCursor.get(), &match))
	{
		for (uint16_t i = 0; i < match.capture_count; i++)
		{
			TSQueryCapture capture = match.captures[i];
			TSNode node = capture.node;

			uint32_t nameLen = 0;
			const char* captureName = ts_query_capture_name_for_id(
				pQuery, capture.index, &nameLen);
			std::string sCapture(captureName, nameLen);

			uint32_t nodeStart = ts_node_start_byte(node);
			uint32_t nodeEnd = ts_node_end_byte(node);

			if (HasCapturePrefix(sCapture, "local.scope"))
			{
				// Check for #set! local.scope-inherits predicate
				bool inherits = true;
				std::string inheritVal = GetSetProperty(pQuery, match.pattern_index, "local.scope-inherits");
				if (inheritVal == "false")
					inherits = false;

				LocalScope scope;
				scope.startByte = nodeStart;
				scope.endByte = nodeEnd;
				scope.inherits = inherits;
				m_localScopes.push_back(scope);
			}
			else if (HasCapturePrefix(sCapture, "local.definition"))
			{
				// Extract the text of the definition node as the symbol name
				if (nodeStart < totalBytes && nodeEnd <= totalBytes)
				{
					std::wstring defName = GetUtf16Text(nodeStart, nodeEnd);

					// Find the innermost enclosing scope and add this definition
					LocalScope* pBestScope = nullptr;
					for (auto& scope : m_localScopes)
					{
						if (nodeStart >= scope.startByte && nodeEnd <= scope.endByte)
						{
							if (!pBestScope ||
								(scope.endByte - scope.startByte) < (pBestScope->endByte - pBestScope->startByte))
							{
								pBestScope = &scope;
							}
						}
					}
					if (pBestScope)
					{
						LocalDef def;
						def.name = defName;
						def.startByte = nodeStart;
						def.endByte = nodeEnd;
						def.highlight = -1;  // Will be resolved during RunHighlightQuery
						pBestScope->defs.push_back(def);
					}
				}
			}
			else if (HasCapturePrefix(sCapture, "local.reference"))
			{
				if (nodeStart < totalBytes && nodeEnd <= totalBytes)
				{
					PendingRef ref;
					ref.name = GetUtf16Text(nodeStart, nodeEnd);
					ref.startByte = nodeStart;
					ref.endByte = nodeEnd;
					ref.scopeStartByte = nodeStart;
					references.push_back(ref);
				}
			}
		}
	}

	// Sort scopes by start byte, then by size (smallest/innermost first for lookup)
	std::sort(m_localScopes.begin(), m_localScopes.end(),
		[](const LocalScope& a, const LocalScope& b)
		{
			if (a.startByte != b.startByte)
				return a.startByte < b.startByte;
			return (a.endByte - a.startByte) < (b.endByte - b.startByte);
		});

	m_pendingRefs = std::move(references);
}

std::vector<CTreeSitterParser::HighlightCapture>
CTreeSitterParser::CollectCaptures(TSNode& rootNode, const TSQuery* pQuery, int nStartLine, int nEndLine)
{
	std::vector<HighlightCapture> captures;

	UniqueTSQueryCursor pCursor(ts_query_cursor_new());
	if (!pCursor)
		return captures;

	ts_query_cursor_set_point_range(pCursor.get(),
		{ static_cast<uint32_t>(nStartLine), 0 },
		{ static_cast<uint32_t>(nEndLine + 1), 0 });

	ts_query_cursor_exec(pCursor.get(), pQuery, rootNode);

	TSQueryMatch match;
	while (ts_query_cursor_next_match(pCursor.get(), &match))
	{
		for (uint16_t i = 0; i < match.capture_count; i++)
		{
			TSQueryCapture capture = match.captures[i];
			TSNode node = capture.node;

			uint32_t captureNameLen = 0;
			const char* captureName = ts_query_capture_name_for_id(
				pQuery, capture.index, &captureNameLen);


			HighlightCapture c;

			c.captureName.assign(captureName, captureNameLen);

			c.startByte = ts_node_start_byte(node);
			c.endByte = ts_node_end_byte(node);

			TSPoint start = ts_node_start_point(node);
			TSPoint end = ts_node_end_point(node);

			c.startRow = start.row;
			c.startCol = start.column;

			c.endRow = end.row;
			c.endCol = end.column;

			c.colorIndex = m_colorMap.MapCapture(c.captureName);

			captures.push_back(std::move(c));
		}
	}

	return captures;
}

std::vector<CTreeSitterParser::HighlightEntry>
CTreeSitterParser::BuildHighlightEntries(const std::vector<HighlightCapture>& captures)
{
	std::vector<HighlightEntry> entries;

	entries.reserve(captures.size());

	for (const auto& c : captures)
	{
		HighlightEntry entry;

		entry.startByte = c.startByte;
		entry.endByte = c.endByte;

		entry.startRow = c.startRow;
		entry.startCol = c.startCol;

		entry.endRow = c.endRow;
		entry.endCol = c.endCol;

		entry.colorIndex = c.colorIndex;

		entry.priority = MakeCapturePriority(c.captureName, c.startByte, c.endByte);

		entry.order = NextBlockOrder();

		entries.push_back(std::move(entry));
	}

	return entries;
}

void CTreeSitterParser::ResolveLocalReferences(std::vector<HighlightEntry>& highlights)
{
	if (m_localScopes.empty())
		return;

	std::unordered_map<uint64_t, int> nodeHighlightMap;

	for (const auto& e : highlights)
	{
		uint64_t key = MakeByteRangeKey(e.startByte, e.endByte);
		nodeHighlightMap[key] = e.colorIndex;
	}

	// Pass 1: Assign highlights to definitions based on their node's highlight
	for (auto& scope : m_localScopes)
	{
		for (auto& def : scope.defs)
		{
			uint64_t defKey = MakeByteRangeKey(def.startByte, def.endByte);
			auto it = nodeHighlightMap.find(defKey);
			if (it != nodeHighlightMap.end())
				def.highlight = it->second;
		}
	}

	// Pass 2: Resolve references - find matching definition in enclosing scopes
	for (const auto& ref : m_pendingRefs)
	{
		// Search scopes from innermost to outermost
		// Find all scopes that contain this reference
		auto enclosingScopes = FindEnclosingScopes(m_localScopes, ref.startByte, ref.endByte);

		// Search for a matching definition
		const LocalDef* pDef = FindDefinitionInScopes(ref.name, ref.startByte, enclosingScopes);
		if (pDef && pDef->highlight >= 0)
			m_localRefHighlights[MakeByteRangeKey(ref.startByte, ref.endByte)] = pDef->highlight;
	}

	// Pass 3: Apply resolved reference highlights to the highlight entries
	for (auto& h : highlights)
	{
		uint64_t key = MakeByteRangeKey(h.startByte, h.endByte);
		auto it = m_localRefHighlights.find(key);
		if (it != m_localRefHighlights.end())
			h.colorIndex = it->second;
	}
}

std::vector<CTreeSitterParser::InjectionRegion>
CTreeSitterParser::CollectInjectionRegions(TSNode& rootNode, const TSQuery* pQuery, int nStartLine, int nEndLine)
{
	std::vector<InjectionRegion> injections;

	UniqueTSQueryCursor pCursor(ts_query_cursor_new());
	if (!pCursor)
		return injections;

	ts_query_cursor_set_point_range(pCursor.get(),
		{ static_cast<uint32_t>(nStartLine), 0 },
		{ static_cast<uint32_t>(nEndLine + 1), 0 });

	ts_query_cursor_exec(pCursor.get(), pQuery, rootNode);

	uint32_t totalBytes = GetTotalBytes();

	TSQueryMatch match;
	while (ts_query_cursor_next_match(pCursor.get(), &match))
	{
		std::string language;
		TSNode contentNode = {};
		bool hasContent = false;

		// Check for #set! injection.language predicate first
		language = GetSetProperty(pQuery, match.pattern_index, "injection.language");

		// Also check for #set! injection.self
		if (language.empty())
		{
			std::string selfVal = GetSetProperty(pQuery, match.pattern_index, "injection.self");
			if (!selfVal.empty())
			{
				// Use the current language
				language = ucr::toUTF8(m_pLang->GetName());
			}
		}

		for (uint16_t i = 0; i < match.capture_count; i++)
		{
			TSQueryCapture capture = match.captures[i];

			uint32_t nameLen = 0;
			const char* captureName = ts_query_capture_name_for_id(
				pQuery, capture.index, &nameLen);
			std::string sCapture(captureName, nameLen);

			if (sCapture == "injection.content")
			{
				contentNode = capture.node;
				hasContent = true;
			}
			else if (sCapture == "injection.language" && language.empty())
			{
				// The captured node's text is the language name
				uint32_t start = ts_node_start_byte(capture.node);
				uint32_t end = ts_node_end_byte(capture.node);
				if (start < totalBytes && end <= totalBytes)
					language = ucr::toUTF8(GetUtf16Text(start, end));
			}
		}

		if (hasContent && !language.empty())
		{
			InjectionRegion region;
			region.language = language;
			region.contentStart = ts_node_start_byte(contentNode);
			region.contentEnd = ts_node_end_byte(contentNode);
			TSPoint startPoint = ts_node_start_point(contentNode);
			TSPoint endPoint = ts_node_end_point(contentNode);
			region.startRow = startPoint.row;
			region.startCol = startPoint.column;
			region.endRow = endPoint.row;
			region.endCol = endPoint.column;
			injections.push_back(region);
		}
	}

	return injections;
}

void CTreeSitterParser::TranslateCoordinates(std::vector<HighlightEntry>& entries, uint32_t injectionStartRow, uint32_t injectionStartCol)
{
	for (auto& e : entries)
	{
		e.startRow += injectionStartRow;
		e.endRow += injectionStartRow;

		if (e.startRow == injectionStartRow)
			e.startCol += injectionStartCol;

		if (e.endRow == injectionStartRow)
			e.endCol += injectionStartCol;
	}
}

void CTreeSitterParser::EmitLineBlocks(const std::vector<HighlightEntry>& entries, int layerPriority)
{
	for (const auto& h : entries)
	{
		for (uint32_t row = h.startRow; row <= h.endRow && row < static_cast<uint32_t>(m_nLineCount); ++row)
		{
			if (row == h.endRow && h.endCol == 0)
				break;

			uint32_t byteCol = (row == h.startRow) ? h.startCol : 0;

			TreeSitterLineBlock block;
			block.nCharPos = byteCol / sizeof(wchar_t);
			block.nColorIndex = h.colorIndex;
			block.nPriority = h.priority;
			block.nLayerPriority = layerPriority;
			block.nOrder = h.order;

			m_lineBlocks[row].push_back(block);
		}

		if (h.endRow < static_cast<uint32_t>(m_nLineCount))
		{
			TreeSitterLineBlock endBlock;

			endBlock.nCharPos = h.endCol / sizeof(wchar_t);
			endBlock.nColorIndex = COLORINDEX_NORMALTEXT;
			endBlock.nLayerPriority = layerPriority;
			endBlock.nPriority = INT_MIN;
			endBlock.nOrder = h.order;

			m_lineBlocks[h.endRow].push_back(endBlock);
		}
	}
}

/**
 * @brief Run the highlight query against the parsed tree and collect token ranges.
 *
 * This produces per-line block arrays by walking all highlight query matches
 * and mapping tree-sitter byte positions to WinMerge character positions.
 *
 * If locals information is available, definition nodes get their highlights
 * recorded, and references are resolved to use the same highlight as their
 * matching definition.
 */
void CTreeSitterParser::RunHighlightQuery(int nStartLine, int nEndLine)
{
	if (!m_pTree || !m_pLang || !m_pLang->GetHighlightQuery())
		return;

	const TSQuery* pQuery = m_pLang->GetHighlightQuery();
	TSNode rootNode = ts_tree_root_node(m_pTree);
	auto captures = CollectCaptures(rootNode, pQuery, nStartLine, nEndLine);
	auto entries = BuildHighlightEntries(captures);
	ResolveLocalReferences(entries);
	EmitLineBlocks(entries, 0);
}

/**
 * @brief Run the injection query to find embedded language regions.
 *
 * Processes injections.scm captures:
 *   @injection.content   - The node whose content should be parsed as another language
 *   @injection.language  - The node whose text specifies the language name
 *   (#set! injection.language "xxx") - Hard-coded language name
 *
 * For each injection region, this spawns a sub-parser with the appropriate
 * language grammar, runs highlights on the injected content, and merges
 * the results into the main highlight blocks.
 */
void CTreeSitterParser::RunInjectionQuery(int nStartLine, int nEndLine)
{
	if (!m_pTree || !m_pLang || !m_pLang->GetInjectionQuery())
		return;

	const TSQuery* pQuery = m_pLang->GetInjectionQuery();
	TSNode rootNode = ts_tree_root_node(m_pTree);

	auto injections = CollectInjectionRegions(rootNode, pQuery, nStartLine, nEndLine);
	if (injections.empty())
		return;

	uint32_t totalBytes = GetTotalBytes();

	// Process each injection: look up the language, parse the content, run highlights
	TreeSitterRegistry& registry = TreeSitterRegistry::Instance();

	for (const auto& inj : injections)
	{
		// Convert language name to wstring for registry lookup
		std::wstring wLangName = ucr::toTString(inj.language);

		// Try to find the language - look it up by name directly in the registry's
		// available languages (we need a way to get language by name, not just by ext).
		// For now, try common mappings. The language name from injections.scm
		// is typically the tree-sitter language name (e.g. "javascript", "css").
		// We can look it up as an extension since many languages use their name
		// as the extension.
		const CTreeSitterLanguage* pInjLang = registry.GetLanguageForName(wLangName);
		if (!pInjLang || !pInjLang->GetHighlightQuery())
			continue;

		// Extract the injection content
		if (inj.contentStart >= totalBytes || inj.contentEnd > totalBytes)
			continue;

		std::wstring injContent = GetUtf16Text(inj.contentStart, inj.contentEnd);
		if (injContent.empty())
			continue;

		// Create a temporary parser for the injected content
		TSParser* pInjParser = ts_parser_new();
		if (!pInjParser)
			continue;

		ts_parser_set_language(pInjParser, pInjLang->GetLanguage());

		TSInput injInput;
		injInput.payload = &injContent;
		injInput.encoding = TSInputEncodingUTF16LE;
		injInput.read = [](void* payload, uint32_t byte_offset, TSPoint position, uint32_t* bytes_read) -> const char* {
			auto* pStr = static_cast<std::wstring*>(payload);
			uint32_t char_offset = byte_offset / sizeof(wchar_t);
			if (char_offset >= pStr->size()) {
				*bytes_read = 0;
				return nullptr;
			}
			*bytes_read = (static_cast<uint32_t>(pStr->size()) - char_offset) * sizeof(wchar_t);
			return reinterpret_cast<const char*>(pStr->c_str() + char_offset);
		};

		TSTree* pInjTree = ts_parser_parse(pInjParser, nullptr, injInput);

		if (pInjTree)
		{
			// Run highlight query on the injected tree
			const TSQuery* pInjQuery = pInjLang->GetHighlightQuery();
			TSNode injRoot = ts_tree_root_node(pInjTree);

			auto captures = CollectCaptures(injRoot, pInjQuery, 0, INT_MAX);
			auto entries = BuildHighlightEntries(captures);
			TranslateCoordinates(entries, inj.startRow, inj.startCol);
			EmitLineBlocks(entries, 1);

			ts_tree_delete(pInjTree);
		}

		ts_parser_delete(pInjParser);
	}
}

void CTreeSitterParser::BuildLineCache(int nStartLine, int nEndLine)
{
	// Sort each line's blocks by character position and deterministic precedence.
	// For identical start positions, keep the most specific / shortest capture.
	for (int i = nStartLine; i <= nEndLine && i < static_cast<int>(m_lineBlocks.size()); i++)
	{
		auto& blocks = m_lineBlocks[i];
		std::sort(blocks.begin(), blocks.end(),
			[](const TreeSitterLineBlock& a, const TreeSitterLineBlock& b)
			{
				if (a.nCharPos != b.nCharPos)
					return a.nCharPos < b.nCharPos;
				if (a.nLayerPriority != b.nLayerPriority)
					return a.nLayerPriority < b.nLayerPriority;

				const int aColorPrio = GetColorPriority(a.nColorIndex);
				const int bColorPrio = GetColorPriority(b.nColorIndex);
				if (aColorPrio != bColorPrio)
					return aColorPrio < bColorPrio;

				if (a.nPriority != b.nPriority)
					return a.nPriority < b.nPriority;

				// Descending order: higher order (later pattern in highlights.scm)
				// sorts first, so the lower order (earlier/more specific pattern)
				// ends up last and is kept by the dedup step below.
				return a.nOrder > b.nOrder;
			});

		// Remove consecutive entries at the same position (keep the last one,
		// which is typically the more specific/inner match)
		if (blocks.size() > 1)
		{
			std::vector<TreeSitterLineBlock> deduped;
			deduped.reserve(blocks.size());
			for (size_t j = 0; j < blocks.size(); j++)
			{
				if (j + 1 < blocks.size() && blocks[j].nCharPos == blocks[j + 1].nCharPos)
					continue;  // skip, keep the later one
				deduped.push_back(blocks[j]);
			}
			blocks = std::move(deduped);
		}
	}
}

// ============================================================================
// TreeSitterRegistry
// ============================================================================

// Default extension -> language mappings.
// Users can extend this via configuration.
static struct
{
	LangServices::LanguageId id;
	const wchar_t* language;
} s_map[] =
{
	{ LangServices::LanguageId::SRC_C, L"cpp" },
	{ LangServices::LanguageId::SRC_CSHARP, L"c-sharp" },
	{ LangServices::LanguageId::SRC_CSS, L"css" },
	{ LangServices::LanguageId::SRC_FSHARP, L"fsharp" },
	{ LangServices::LanguageId::SRC_FSHARP_SIGNATURE, L"fsharp_signature" },
	{ LangServices::LanguageId::SRC_GO, L"go" },
	{ LangServices::LanguageId::SRC_HTML, L"html" },
	{ LangServices::LanguageId::SRC_INI, L"ini" },
	{ LangServices::LanguageId::SRC_JAVA, L"java" },
	{ LangServices::LanguageId::SRC_JAVASCRIPT, L"javascript" },
	{ LangServices::LanguageId::SRC_JSON, L"json" },
	{ LangServices::LanguageId::SRC_MARKDOWN, L"markdown" },
	{ LangServices::LanguageId::SRC_PHP, L"php" },
	{ LangServices::LanguageId::SRC_POWERSHELL, L"powershell" },
	{ LangServices::LanguageId::SRC_PROPERTIES, L"properties" },
	{ LangServices::LanguageId::SRC_PYTHON, L"python" },
	{ LangServices::LanguageId::SRC_RUBY, L"ruby" },
	{ LangServices::LanguageId::SRC_RUST, L"rust" },
	{ LangServices::LanguageId::SRC_SH, L"bash" },
	{ LangServices::LanguageId::SRC_TOML, L"toml" },
	{ LangServices::LanguageId::SRC_TSX, L"tsx" },
	{ LangServices::LanguageId::SRC_TYPESCRIPT, L"typescript" },
	{ LangServices::LanguageId::SRC_XML, L"xml" },
	{ LangServices::LanguageId::SRC_YAML, L"yaml" },
};

TreeSitterRegistry& TreeSitterRegistry::Instance()
{
	static TreeSitterRegistry instance;
	return instance;
}

void TreeSitterRegistry::Initialize(const std::wstring& sGrammarDir)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_bInitialized)
		return;

	// Determine grammar directory
	if (sGrammarDir.empty())
	{
		// Use <exe_dir>/TreeSitterGrammars/
		wchar_t szExePath[MAX_PATH] = {};
		GetModuleFileNameW(nullptr, szExePath, MAX_PATH);
		std::wstring sExeDir(szExePath);
		auto pos = sExeDir.rfind(L'\\');
		if (pos != std::wstring::npos)
			sExeDir = sExeDir.substr(0, pos);
		m_sGrammarDir = sExeDir + L"\\TreeSitterGrammars";
	}
	else
	{
		m_sGrammarDir = sGrammarDir;
	}

	// Check if the grammar directory exists
	DWORD dwAttrib = GetFileAttributesW(m_sGrammarDir.c_str());
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		// Directory doesn't exist - that's OK, just means no tree-sitter support
		m_bInitialized = true;
		return;
	}

	// Fix #3: Only discover available grammar DLLs, don't load them yet.
	// Grammars are loaded lazily on first request in GetLanguageForExt().
	WIN32_FIND_DATAW findData;
	std::wstring searchPattern = m_sGrammarDir + L"\\tree-sitter-*.dll";
	HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Extract language name from "tree-sitter-<language>.dll"
			std::wstring sFileName(findData.cFileName);
			const std::wstring prefix = L"tree-sitter-";
			const std::wstring suffix = L".dll";
			if (sFileName.size() > prefix.size() + suffix.size())
			{
				std::wstring sLangName = sFileName.substr(
					prefix.size(),
					sFileName.size() - prefix.size() - suffix.size());

				// Just record that this language is available
				m_availableLanguages.insert(sLangName);
			}
		} while (FindNextFileW(hFind, &findData));
		FindClose(hFind);
	}

	m_bInitialized = true;
}

const CTreeSitterLanguage* TreeSitterRegistry::GetLanguageForName(const std::wstring& sLangName)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Check if already loaded
	auto itLang = m_languages.find(sLangName);
	if (itLang != m_languages.end())
	{
		if (!itLang->second->GetHighlightQuery())
			return nullptr;
		return itLang->second.get();
	}

	// Check if this language previously failed to load
	if (m_failedLanguages.count(sLangName) > 0)
		return nullptr;

	// Check if a DLL is available for this language
	if (m_availableLanguages.count(sLangName) == 0)
		return nullptr;

	// Lazy load: load the grammar DLL now
	auto pLang = std::make_unique<CTreeSitterLanguage>();
	if (!pLang->Load(m_sGrammarDir, sLangName))
	{
		m_failedLanguages.insert(sLangName);
		return nullptr;
	}

	if (!pLang->GetHighlightQuery())
	{
		m_failedLanguages.insert(sLangName);
		return nullptr;
	}

	const CTreeSitterLanguage* pResult = pLang.get();
	m_languages[sLangName] = std::move(pLang);
	return pResult;
}

// ============================================================================
// CTreeSitterParser - Additional Methods
// ============================================================================

/**
 * @brief Get the node type name at a specific position.
 * 
 * This is used for comment filtering and other syntax-aware operations.
 */
String CTreeSitterParser::GetNodeTypeAt(int nLineIndex, int nCharPos)
{
	EnsureParsed(nLineIndex);

	if (!m_pTree || nLineIndex < 0 || nLineIndex >= m_nLineCount)
		return _T("");

	// Calculate byte offset
	const uint32_t byteOffset = CharPosToByteOffset(nLineIndex, nCharPos);

	// Get the tree-sitter node at this byte position
	TSNode rootNode = ts_tree_root_node(m_pTree);
	TSNode node = ts_node_descendant_for_byte_range(rootNode, byteOffset, byteOffset);

	if (ts_node_is_null(node))
		return _T("");

	// Get node type
	const char* pszType = ts_node_type(node);
	if (!pszType)
		return _T("");

	// Convert from UTF-8 to String
	return ucr::toTString(pszType);
}

bool CTreeSitterParser::FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos)
{
	EnsureParsed(nLineIndex);

	if (m_pTree == nullptr || m_pTextBuffer == nullptr)
	{
		return false;
	}

	// Convert line/char position to byte offset
	if (nLineIndex < 0 || nLineIndex >= m_nLineCount)
	{
		return false;
	}

	uint32_t byteOffset = CharPosToByteOffset(nLineIndex, nCharPos);

	// Get the root node
	TSNode rootNode = ts_tree_root_node(m_pTree);

	// Get the smallest node at this position
	TSNode node = ts_node_descendant_for_byte_range(rootNode, byteOffset, byteOffset);

	if (ts_node_is_null(node))
	{
		return false;
	}

	// Check if this node represents a brace/bracket
	const char* pszType = ts_node_type(node);
	if (pszType == nullptr)
	{
		return false;
	}

	// Common brace types in various languages
	std::string sType(pszType);
	if (sType == "(" || sType == ")" ||
		sType == "[" || sType == "]" ||
		sType == "{" || sType == "}" ||
		sType == "<" || sType == ">" ||
		sType == "\"(\"" || sType == "\")\"" ||
		sType == "\"[\"" || sType == "\"]\"" ||
		sType == "\"{\"" || sType == "\"}\"" ||
		sType == "\"<\"" || sType == "\">\"")
	{
		// Get the parent node - it should contain both opening and closing
		TSNode parent = ts_node_parent(node);
		if (ts_node_is_null(parent))
		{
			return false;
		}

		// Find the matching sibling
		uint32_t childCount = ts_node_child_count(parent);
		for (uint32_t i = 0; i < childCount; i++)
		{
			TSNode child = ts_node_child(parent, i);
			if (ts_node_is_null(child))
				continue;

			// Skip the current node itself
			if (ts_node_start_byte(child) == ts_node_start_byte(node))
				continue;

			const char* pszChildType = ts_node_type(child);
			if (pszChildType == nullptr)
				continue;

			std::string sChildType(pszChildType);

			// Check if this is the matching brace
			bool bIsMatch = false;
			if (sType == "(" || sType == "\"(\"") bIsMatch = (sChildType == ")" || sChildType == "\")\"");
			else if (sType == ")" || sType == "\")\"") bIsMatch = (sChildType == "(" || sChildType == "\"(\"");
			else if (sType == "[" || sType == "\"[\"") bIsMatch = (sChildType == "]" || sChildType == "\"]\"");
			else if (sType == "]" || sType == "\"]\"") bIsMatch = (sChildType == "[" || sChildType == "\"[\"");
			else if (sType == "{" || sType == "\"{\"") bIsMatch = (sChildType == "}" || sChildType == "\"}\"");
			else if (sType == "}" || sType == "\"}\"") bIsMatch = (sChildType == "{" || sChildType == "\"{\"");
			else if (sType == "<" || sType == "\"<\"") bIsMatch = (sChildType == ">" || sChildType == "\">\"");
			else if (sType == ">" || sType == "\">\"") bIsMatch = (sChildType == "<" || sChildType == "\"<\"");

			if (bIsMatch)
			{
				// Convert byte offset back to line/char
				uint32_t matchByteOffset = ts_node_start_byte(child);
				return ByteOffsetToLineChar(matchByteOffset, outLineIndex, outCharPos);
			}
		}
	}

	return false;
}

bool CTreeSitterParser::TryGetDefinitionByteRangeAt(uint32_t byteOffset, uint32_t& defStartByte, uint32_t& defEndByte) const
{
	for (const auto& scope : m_localScopes)
	{
		for (const auto& def : scope.defs)
		{
			if (byteOffset >= def.startByte && byteOffset < def.endByte)
			{
				defStartByte = def.startByte;
				defEndByte = def.endByte;
				return true;
			}
		}
	}

	for (const auto& ref : m_pendingRefs)
	{
		if (byteOffset < ref.startByte || byteOffset >= ref.endByte)
			continue;

		auto enclosingScopes = FindEnclosingScopes(m_localScopes, ref.startByte, ref.endByte);

		const LocalDef* pDef = FindDefinitionInScopes(ref.name, ref.startByte, enclosingScopes);
		if (pDef)
		{
			defStartByte = pDef->startByte;
			defEndByte = pDef->endByte;
			return true;
		}

		return false;
	}

	for (const auto& def : m_tagDefs)
	{
		if (byteOffset >= def.startByte && byteOffset < def.endByte)
		{
			defStartByte = def.startByte;
			defEndByte = def.endByte;
			return true;
		}
	}

	for (const auto& ref : m_tagRefs)
	{
		if (byteOffset < ref.startByte || byteOffset >= ref.endByte)
			continue;

		const TagDef* pBestDef = nullptr;
		for (const auto& def : m_tagDefs)
		{
			if (def.name != ref.name)
				continue;
			if (!pBestDef || def.startByte < pBestDef->startByte)
				pBestDef = &def;
		}

		if (pBestDef)
		{
			defStartByte = pBestDef->startByte;
			defEndByte = pBestDef->endByte;
			return true;
		}

		return false;
	}

	return false;
}

bool CTreeSitterParser::ByteOffsetToLineChar(uint32_t byteOffset, int& nLineIndex, int& nCharPos) const
{
	if (m_lineStartBytes.empty())
		return false;

	const int nLines = static_cast<int>(m_lineStartBytes.size()) - 1;
	if (nLines == 0)
		return false;

	auto it = std::upper_bound(m_lineStartBytes.begin(), m_lineStartBytes.begin() + nLines, byteOffset);
	int line = static_cast<int>(it - m_lineStartBytes.begin()) - 1;
	if (line < 0)
		line = 0;
	if (line >= nLines)
		line = nLines - 1;

	nLineIndex = line;
	nCharPos = static_cast<int>((byteOffset - m_lineStartBytes[line]) / sizeof(wchar_t));

	const int lineLen = m_pTextBuffer->GetLineLength(line);
	if (nCharPos > lineLen)
		nCharPos = lineLen;

	return true;
}

bool CTreeSitterParser::TryGetTagDefinitionByNameAt(int nLineIndex, int nCharPos, uint32_t& defStartByte, uint32_t& defEndByte) const
{
	if (!m_pTextBuffer || nLineIndex < 0 || nLineIndex >= m_pTextBuffer->GetLineCount())
		return false;

	const tchar_t* pszLine = m_pTextBuffer->GetLineChars(nLineIndex);
	const int nLineLength = m_pTextBuffer->GetLineLength(nLineIndex);
	if (!pszLine || nLineLength <= 0)
		return false;

	auto IsIdentChar = [](tchar_t ch)
	{
		return tc::istalnum(ch) || ch == _T('_') || ch == _T('`') || ch == _T('\'');
	};

	int nIndex = nCharPos;
	if (nIndex >= nLineLength)
		nIndex = nLineLength - 1;
	if (nIndex < 0)
		return false;

	if (!IsIdentChar(pszLine[nIndex]) && nIndex > 0 && IsIdentChar(pszLine[nIndex - 1]))
		--nIndex;
	if (!IsIdentChar(pszLine[nIndex]))
		return false;

	int nStart = nIndex;
	while (nStart > 0 && IsIdentChar(pszLine[nStart - 1]))
		--nStart;

	int nEnd = nIndex + 1;
	while (nEnd < nLineLength && IsIdentChar(pszLine[nEnd]))
		++nEnd;

	std::wstring symbol(pszLine + nStart, nEnd - nStart);

	const TagDef* pBestDef = nullptr;
	for (const auto& def : m_tagDefs)
	{
		if (def.name != symbol)
			continue;
		if (!pBestDef || def.startByte < pBestDef->startByte)
			pBestDef = &def;
	}

	if (!pBestDef)
		return false;

	defStartByte = pBestDef->startByte;
	defEndByte = pBestDef->endByte;
	return true;
}

bool CTreeSitterParser::FindDefinition(int nLineIndex, int nCharPos, int& nDefLine, int& nDefChar)
{
	EnsureParsed(nLineIndex);

	if (!m_pTree || nLineIndex < 0 || nLineIndex >= m_nLineCount)
		return false;

	EnsureTagsQueried();

	const uint32_t byteOffset = CharPosToByteOffset(nLineIndex, nCharPos);

	uint32_t defStartByte = 0;
	uint32_t defEndByte = 0;
	const bool foundAtPosition = TryGetDefinitionByteRangeAt(byteOffset, defStartByte, defEndByte);

	uint32_t tagDefStartByte = 0;
	uint32_t tagDefEndByte = 0;
	if (!foundAtPosition)
	{
		if (!TryGetTagDefinitionByNameAt(
				nLineIndex, nCharPos, tagDefStartByte, tagDefEndByte))
			return false;

		defStartByte = tagDefStartByte;
		defEndByte = tagDefEndByte;
	}
	else
	{
		int posDefLine = 0;
		int posDefChar = 0;
		const bool positionResolved =
			ByteOffsetToLineChar(defStartByte, posDefLine, posDefChar);

		// If the position-based lookup resolves back to the clicked symbol,
		// prefer the tag-definition target when it points somewhere else.
		// This avoids getting stuck on the clicked type reference after the
		// context-menu click has moved the caret onto the symbol.
		if (!positionResolved || (posDefLine == nLineIndex && posDefChar == nCharPos))
		{
			if (TryGetTagDefinitionByNameAt(
					nLineIndex, nCharPos, tagDefStartByte, tagDefEndByte) &&
				(!positionResolved || tagDefStartByte != defStartByte))
			{
				defStartByte = tagDefStartByte;
				defEndByte = tagDefEndByte;
			}
		}
	}

	return ByteOffsetToLineChar(defStartByte, nDefLine, nDefChar);
}

static const wchar_t* GetLanguageNameForId(LangServices::LanguageId id)
{
	for (const auto& mapping : s_map)
	{
		if (mapping.id == id)
			return mapping.language;
	}
	return nullptr;
}

/**
 * @brief Set the text buffer that this parser will operate on.
 */
void CTreeSitterParser::SetTextBuffer(LangServices::ITextBuffer* pTextBuffer)
{
	m_pTextBuffer = pTextBuffer;
	Invalidate();
}

/**
 * @brief Parse a single line and return syntax highlighting information.
 */
std::vector<LangServices::TEXTBLOCK> CTreeSitterParser::ParseLine(int nLineIndex)
{
	if (m_bTreeSitterDisabled)
		return {};

	// Ensure the document is parsed (handles lazy reparsing if dirty)
	EnsureParsed(nLineIndex);

	std::vector<LangServices::TEXTBLOCK> newBlocks;
	newBlocks.push_back({0, COLORINDEX_NORMALTEXT, COLORINDEX_BKGND});

	if (nLineIndex < 0 || nLineIndex >= static_cast<int>(m_lineBlocks.size()))
		return newBlocks;

	const auto& blocks = m_lineBlocks[nLineIndex];

	for (const auto& block : blocks)
	{
		// If the caller's last block is at the same position, overwrite it
		// (same logic as DEFINE_BLOCK macro in crystallineparser.h)
		if (!newBlocks.empty() && newBlocks.back().m_nCharPos == block.nCharPos)
		{
			newBlocks.back().m_nColorIndex = block.nColorIndex;
			newBlocks.back().m_nBgColorIndex = COLORINDEX_BKGND;
			continue;
		}

		// Skip if same color as previous block (no visible change)
		if (!newBlocks.empty() && newBlocks.back().m_nColorIndex == block.nColorIndex)
			continue;

		newBlocks.push_back({block.nCharPos, block.nColorIndex, COLORINDEX_BKGND});
	}

	return newBlocks;
}

/**
 * @brief Check if the given language type is supported by this parser factory.
 */
bool TreeSitterSyntaxParserFactory::IsSupported(LangServices::LanguageId type) const
{
	const auto* name = GetLanguageNameForId(type);
	if (!name)
		return false;
	auto& registry = TreeSitterRegistry::Instance();
	if (!registry.IsInitialized())
		registry.Initialize();
	if (registry.GetLanguageForName(name))
		return true;
	return false;
}
