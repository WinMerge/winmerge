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
#include "ITextBuffer.h"
#include "utils/ctchar.h"

#include <tree_sitter/api.h>

#include <fstream>
#include <sstream>
#include <cassert>
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
    m_map["constructor"]            = COLORINDEX_FUNCNAME;

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

    // Variables / properties / modules -> USER2 or NORMALTEXT
    m_map["variable"]               = COLORINDEX_NORMALTEXT;
    m_map["variable.builtin"]       = COLORINDEX_USER2;
    m_map["variable.parameter"]     = COLORINDEX_NORMALTEXT;
    m_map["variable.member"]        = COLORINDEX_USER2;
    m_map["property"]               = COLORINDEX_USER2;
    m_map["field"]                  = COLORINDEX_USER2;
    m_map["constant"]               = COLORINDEX_USER2;
    m_map["constant.builtin"]       = COLORINDEX_USER2;
    m_map["constant.macro"]         = COLORINDEX_USER2;
    m_map["module"]                 = COLORINDEX_USER1;
    m_map["namespace"]              = COLORINDEX_USER1;
    m_map["label"]                  = COLORINDEX_USER2;
    m_map["tag"]                    = COLORINDEX_KEYWORD;
    m_map["tag.attribute"]          = COLORINDEX_USER2;
    m_map["tag.delimiter"]          = COLORINDEX_OPERATOR;
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

CTreeSitterParser::CTreeSitterParser()
    : m_pParser(nullptr)   // Fix #2: lazy-init, don't call ts_parser_new() here
    , m_pTree(nullptr)
    , m_pLang(nullptr)
    , m_bDirty(false)
    , m_nLineCount(0)
{
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
    m_lineUtf8.clear();
    m_documentText.clear();
    m_localScopes.clear();
    m_localRefHighlights.clear();
    m_pendingRefs.clear();
    m_tagDefs.clear();
    m_tagRefs.clear();
    m_nLineCount = 0;
    m_nextBlockOrder = 0;
    m_bDirty = false;
}

void CTreeSitterParser::ParseDocument(const tchar_t* const* ppszLines,
                                       const int* pnLineLengths,
                                       int nLineCount)
{
    EnsureParser();
    if (!m_pParser || !m_pLang || !m_pLang->GetLanguage())
        return;

    // Build contiguous document text from line pointers.
    // Tree-sitter requires UTF-8 input, so we convert from tchar_t (wchar_t on Windows).
    m_documentText.clear();
    m_lineUtf8.clear();
    m_nLineCount = nLineCount;

    // Pre-calculate total size estimate
    size_t totalEstimate = 0;
    for (int i = 0; i < nLineCount; i++)
        totalEstimate += static_cast<size_t>(pnLineLengths[i]) * 3 + 1; // worst case UTF-8
    m_documentText.reserve(totalEstimate);
    m_lineUtf8.resize(nLineCount);

    for (int i = 0; i < nLineCount; i++)
    {
        if (ppszLines[i] && pnLineLengths[i] > 0)
        {
#ifdef _UNICODE
            // Convert UTF-16 to UTF-8
            int nLen = WideCharToMultiByte(CP_UTF8, 0,
                ppszLines[i], pnLineLengths[i],
                nullptr, 0, nullptr, nullptr);
            if (nLen > 0)
            {
                std::string lineUtf8(nLen, '\0');
                WideCharToMultiByte(CP_UTF8, 0,
                    ppszLines[i], pnLineLengths[i],
                    &lineUtf8[0], nLen,
                    nullptr, nullptr);
                m_lineUtf8[i] = lineUtf8;
                m_documentText.append(lineUtf8);
            }
#else
            m_lineUtf8[i].assign(ppszLines[i], pnLineLengths[i]);
            m_documentText.append(ppszLines[i], pnLineLengths[i]);
#endif
        }
        if (i < nLineCount - 1)
            m_documentText += '\n';
    }

    // Parse the document.
    // Pass the old tree if available -- tree-sitter can reuse unchanged subtrees
    // for faster re-parsing. (For full incremental support, ts_tree_edit() should
    // be called on the old tree before re-parsing, but even without edit info
    // tree-sitter benefits from having the previous tree as a reference.)
    TSTree* pOldTree = m_pTree;
    m_pTree = ts_parser_parse_string(
        m_pParser,
        pOldTree,
        m_documentText.c_str(),
        static_cast<uint32_t>(m_documentText.size()));

    if (pOldTree)
        ts_tree_delete(pOldTree);

    if (m_pTree)
    {
        m_nextBlockOrder = 0;
        // 1. Run locals query first to build scope/def/ref information
        RunLocalsQuery();
        // 2. Run tags query for same-file symbol definitions/references
        RunTagsQuery();
        // 3. Run highlight query (uses locals info for scope-aware coloring)
        RunHighlightQuery();
        // 4. Run injection query to handle embedded languages
        RunInjectionQuery();
        BuildLineCache(nLineCount);
    }

    // Cache is now fresh
    m_bDirty = false;
}

/**
 * @brief Notify the parser of an edit for incremental reparsing.
 *
 * Reads the last UndoRecord from the buffer to get the edit position,
 * then calls ts_tree_edit() on the existing tree. This allows tree-sitter
 * to reuse unchanged subtrees during the next reparse, which is
 * significantly faster for large documents.
 *
 * Falls back to a simple MarkDirty() if the tree or undo info is unavailable.
 */
void CTreeSitterParser::NotifyEdit(const TextEdit& textEdit)
{
    m_bDirty = true;

    // If we don't have a tree, there's nothing to edit incrementally
    if (!m_pTree)
        return;

    // Convert CEPoint (char-based, line/col) to UTF-8 byte offsets.
    // m_lineUtf8 holds the per-line UTF-8 from the previous parse.
    // We need:
    //   start_byte:     absolute byte offset of the edit start in old doc
    //   old_end_byte:   absolute byte offset of the old content end
    //   new_end_byte:   absolute byte offset of the new content end

    // Helper: compute absolute byte offset from (line, charPos) using old m_lineUtf8
    // Each line in m_documentText is followed by '\n' (except the last)
    auto charPosToByteOffset = [this](int line, int charPos) -> uint32_t
    {
        uint32_t byteOffset = 0;
        int nLines = static_cast<int>(m_lineUtf8.size());

        // Sum up all lines before 'line'
        for (int i = 0; i < line && i < nLines; i++)
        {
            byteOffset += static_cast<uint32_t>(m_lineUtf8[i].size());
            byteOffset += 1; // for '\n' separator
        }

        // Add the byte offset within the target line
        if (line >= 0 && line < nLines && charPos > 0)
        {
#ifdef _UNICODE
            const std::string& utf8Line = m_lineUtf8[line];
            // Convert charPos (UTF-16 units) to UTF-8 byte count
            // We need the original line text to do this properly.
            // Use the stored UTF-8 line and convert back to count bytes.
            int nUtf16Len = MultiByteToWideChar(CP_UTF8, 0,
                utf8Line.c_str(), static_cast<int>(utf8Line.size()),
                nullptr, 0);
            if (charPos >= nUtf16Len)
            {
                byteOffset += static_cast<uint32_t>(utf8Line.size());
            }
            else
            {
                // Walk the UTF-8 bytes counting UTF-16 chars until we reach charPos
                int utf16Count = 0;
                uint32_t byteIdx = 0;
                const uint8_t* p = reinterpret_cast<const uint8_t*>(utf8Line.c_str());
                uint32_t lineLen = static_cast<uint32_t>(utf8Line.size());
                while (byteIdx < lineLen && utf16Count < charPos)
                {
                    uint8_t ch = p[byteIdx];
                    uint32_t seqLen;
                    if (ch < 0x80)
                        seqLen = 1;
                    else if (ch < 0xE0)
                        seqLen = 2;
                    else if (ch < 0xF0)
                        seqLen = 3;
                    else
                        seqLen = 4;

                    byteIdx += seqLen;
                    // A 4-byte UTF-8 sequence produces a surrogate pair (2 UTF-16 units)
                    utf16Count += (seqLen == 4) ? 2 : 1;
                }
                byteOffset += byteIdx;
            }
#else
            byteOffset += static_cast<uint32_t>(charPos);
#endif
        }
        else if (line >= nLines && line > 0)
        {
            // Line is beyond our cached data -- use end of document
            byteOffset = static_cast<uint32_t>(m_documentText.size());
        }

        return byteOffset;
    };

    // Helper: compute TSPoint (row, column in bytes) from (line, charPos)
    auto charPosToTSPoint = [this](int line, int charPos) -> TSPoint
    {
        TSPoint pt;
        pt.row = static_cast<uint32_t>(line);
        pt.column = 0;

        if (charPos > 0 && line >= 0 && line < static_cast<int>(m_lineUtf8.size()))
        {
            const std::string& utf8Line = m_lineUtf8[line];
#ifdef _UNICODE
            int nUtf16Len = MultiByteToWideChar(CP_UTF8, 0,
                utf8Line.c_str(), static_cast<int>(utf8Line.size()),
                nullptr, 0);
            if (charPos >= nUtf16Len)
            {
                pt.column = static_cast<uint32_t>(utf8Line.size());
            }
            else
            {
                int utf16Count = 0;
                uint32_t byteIdx = 0;
                const uint8_t* p = reinterpret_cast<const uint8_t*>(utf8Line.c_str());
                uint32_t lineLen = static_cast<uint32_t>(utf8Line.size());
                while (byteIdx < lineLen && utf16Count < charPos)
                {
                    uint8_t ch = p[byteIdx];
                    uint32_t seqLen;
                    if (ch < 0x80)
                        seqLen = 1;
                    else if (ch < 0xE0)
                        seqLen = 2;
                    else if (ch < 0xF0)
                        seqLen = 3;
                    else
                        seqLen = 4;
                    byteIdx += seqLen;
                    utf16Count += (seqLen == 4) ? 2 : 1;
                }
                pt.column = byteIdx;
            }
#else
            pt.column = static_cast<uint32_t>(charPos);
#endif
        }
        return pt;
    };

    TSInputEdit tsEdit;
    memset(&tsEdit, 0, sizeof(tsEdit));

    if (textEdit.bInsert)
    {
        // Insert: old range is empty (start == old_end), new range is the inserted text
        tsEdit.start_byte = charPosToByteOffset(textEdit.ptStartPos.y, textEdit.ptStartPos.x);
        tsEdit.old_end_byte = tsEdit.start_byte;
        tsEdit.start_point = charPosToTSPoint(textEdit.ptStartPos.y, textEdit.ptStartPos.x);
        tsEdit.old_end_point = tsEdit.start_point;

        // For new_end, we need the end position after insert.
        // The TextEdit's ptEndPos gives us the end position in the *new* document.
        // But our m_lineUtf8 is from the *old* document, so we can't use
        // charPosToByteOffset for the end position directly.
        // Instead, compute new_end_byte = start_byte + utf8_length_of_inserted_text.
        // Normalize to LF-only: ParseDocument() concatenates lines with '\n' (no '\r'),
        // so '\r' bytes must be excluded from all byte-offset calculations.
        const tchar_t* pInsText = textEdit.pszText;
        size_t nInsLen = textEdit.nTextLength;
#ifdef _UNICODE
        // Convert to UTF-8 and strip '\r' to match ParseDocument()'s representation.
        int nRawUtf8Len = WideCharToMultiByte(CP_UTF8, 0,
            pInsText, static_cast<int>(nInsLen),
            nullptr, 0, nullptr, nullptr);
        std::string insUtf8;
        if (nRawUtf8Len > 0)
        {
            insUtf8.resize(nRawUtf8Len);
            WideCharToMultiByte(CP_UTF8, 0, pInsText, static_cast<int>(nInsLen),
                &insUtf8[0], nRawUtf8Len, nullptr, nullptr);
            insUtf8.erase(std::remove(insUtf8.begin(), insUtf8.end(), '\r'), insUtf8.end());
        }
        int nUtf8Len = static_cast<int>(insUtf8.size());
        tsEdit.new_end_byte = tsEdit.start_byte + static_cast<uint32_t>(nUtf8Len);
#else
        // Exclude '\r' characters to match ParseDocument()'s LF-only representation.
        int nCrCount = static_cast<int>(std::count(pInsText, pInsText + nInsLen, static_cast<char>('\r')));
        tsEdit.new_end_byte = tsEdit.start_byte + static_cast<uint32_t>(nInsLen - nCrCount);
#endif
        tsEdit.new_end_point.row = static_cast<uint32_t>(textEdit.ptEndPos.y);
        // For the column, we can compute it from the text: count bytes after last newline
        uint32_t lastNewlineBytes = 0;
        bool foundNewline = false;
#ifdef _UNICODE
        // Use the already-normalized UTF-8 string to find the last newline position.
        if (nUtf8Len > 0)
        {
            auto lastNL = insUtf8.rfind('\n');
            if (lastNL != std::string::npos)
            {
                foundNewline = true;
                lastNewlineBytes = static_cast<uint32_t>(nUtf8Len - lastNL - 1);
            }
        }
#else
        {
            const char* pText = pInsText;
            for (int i = static_cast<int>(nInsLen) - 1; i >= 0; i--)
            {
                if (pText[i] == '\n') { foundNewline = true; lastNewlineBytes = nInsLen - i - 1; break; }
            }
        }
#endif
        if (foundNewline)
        {
            tsEdit.new_end_point.column = lastNewlineBytes;
        }
        else
        {
            // No newline in inserted text: column = start column + inserted byte length
            tsEdit.new_end_point.column = tsEdit.start_point.column +
                (tsEdit.new_end_byte - tsEdit.start_byte);
        }
    }
    else
    {
        // Delete: old range is the deleted text, new range is empty (start == new_end)
        tsEdit.start_byte = charPosToByteOffset(textEdit.ptStartPos.y, textEdit.ptStartPos.x);
        tsEdit.start_point = charPosToTSPoint(textEdit.ptStartPos.y, textEdit.ptStartPos.x);

        // For old_end, we use the old document positions
        tsEdit.old_end_byte = charPosToByteOffset(textEdit.ptEndPos.y, textEdit.ptEndPos.x);
        tsEdit.old_end_point = charPosToTSPoint(textEdit.ptEndPos.y, textEdit.ptEndPos.x);

        // After deletion, the cursor is at start
        tsEdit.new_end_byte = tsEdit.start_byte;
        tsEdit.new_end_point = tsEdit.start_point;
    }

    ts_tree_edit(m_pTree, &tsEdit);
}

/**
 * @brief Ensure the document is parsed if the cache is dirty.
 *
 * Called lazily from ParseLine during the paint cycle. This means
 * we reparse at most once per paint, not once per keystroke.
 */
void CTreeSitterParser::EnsureParsed(ITextBuffer* pBuffer)
{
    if (m_bDirty && m_pLang)
    {
        ParseFromBuffer(pBuffer);
        // ParseFromBuffer sets m_bDirty = false via ParseDocument
    }
}

/**
 * @brief Convert a UTF-8 byte offset within a line to a UTF-16 character position.
 *
 * Tree-sitter reports column positions as byte offsets in the UTF-8 text.
 * WinMerge's TEXTBLOCK.m_nCharPos expects tchar_t (wchar_t) character indices.
 * For pure ASCII, these are identical. For multi-byte UTF-8 / surrogate pairs,
 * we need to walk the UTF-8 bytes and count the corresponding UTF-16 code units.
 *
 * @param nLine    Zero-based line index.
 * @param byteCol  Byte offset within the line's UTF-8 representation.
 * @return Character position (index into the wchar_t line).
 */
int CTreeSitterParser::Utf8ByteOffsetToCharPos(int nLine, uint32_t byteCol) const
{
    if (nLine < 0 || nLine >= static_cast<int>(m_lineUtf8.size()))
        return static_cast<int>(byteCol);

    const std::string& utf8Line = m_lineUtf8[nLine];
    if (utf8Line.empty() || byteCol == 0)
        return 0;

    // Clamp to line length
    uint32_t maxByte = static_cast<uint32_t>(utf8Line.size());
    if (byteCol > maxByte)
        byteCol = maxByte;

#ifdef _UNICODE
    // Convert the prefix [0..byteCol) from UTF-8 to UTF-16 and count chars
    int nChars = MultiByteToWideChar(CP_UTF8, 0,
        utf8Line.c_str(), static_cast<int>(byteCol),
        nullptr, 0);
    return nChars;
#else
    return static_cast<int>(byteCol);
#endif
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
        if (!name || std::string(name, nameLen) != "set!")
            continue;

        // Next step should be the property key (string)
        if (i + 1 >= stepCount || steps[i + 1].type != TSQueryPredicateStepTypeString)
            continue;

        uint32_t keyLen = 0;
        const char* keyStr = ts_query_string_value_for_id(pQuery, steps[i + 1].value_id, &keyLen);
        if (!keyStr || std::string(keyStr, keyLen) != key)
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

    TSQueryCursor* pCursor = ts_query_cursor_new();
    if (!pCursor)
        return;

    ts_query_cursor_exec(pCursor, pQuery, rootNode);

    TSQueryMatch match;
    while (ts_query_cursor_next_match(pCursor, &match))
    {
        std::string name;
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
                if (nodeStart < m_documentText.size() && nodeEnd <= m_documentText.size())
                {
                    name = m_documentText.substr(nodeStart, nodeEnd - nodeStart);
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

    ts_query_cursor_delete(pCursor);
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

    TSQueryCursor* pCursor = ts_query_cursor_new();
    if (!pCursor)
        return;

    ts_query_cursor_exec(pCursor, pQuery, rootNode);

    // Temporary storage for references (resolved after all defs are collected)
    std::vector<PendingRef> references;

    TSQueryMatch match;
    while (ts_query_cursor_next_match(pCursor, &match))
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
                if (nodeStart < m_documentText.size() && nodeEnd <= m_documentText.size())
                {
                    std::string defName = m_documentText.substr(nodeStart, nodeEnd - nodeStart);

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
                if (nodeStart < m_documentText.size() && nodeEnd <= m_documentText.size())
                {
                    PendingRef ref;
                    ref.name = m_documentText.substr(nodeStart, nodeEnd - nodeStart);
                    ref.startByte = nodeStart;
                    ref.endByte = nodeEnd;
                    ref.scopeStartByte = nodeStart;
                    references.push_back(ref);
                }
            }
        }
    }

    ts_query_cursor_delete(pCursor);

    // Sort scopes by start byte, then by size (smallest/innermost first for lookup)
    std::sort(m_localScopes.begin(), m_localScopes.end(),
        [](const LocalScope& a, const LocalScope& b)
        {
            if (a.startByte != b.startByte)
                return a.startByte < b.startByte;
            return (a.endByte - a.startByte) < (b.endByte - b.startByte);
        });

    // Store references for later resolution in RunHighlightQuery.
    // We can't resolve them yet because definition highlights haven't been
    // determined. Instead, store them and resolve after RunHighlightQuery
    // has assigned highlights to definitions.
    //
    // Actually, we'll store the reference info and do a two-pass approach:
    // RunHighlightQuery will first assign highlights to definition nodes,
    // then we resolve references.
    //
    // For now, store references as pending. The key is (startByte << 32 | endByte).
    // We'll store reference name -> node range for later lookup.
    m_pendingRefs = std::move(references);
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
void CTreeSitterParser::RunHighlightQuery()
{
    if (!m_pTree || !m_pLang || !m_pLang->GetHighlightQuery())
        return;

    const TSQuery* pQuery = m_pLang->GetHighlightQuery();
    TSNode rootNode = ts_tree_root_node(m_pTree);

    TSQueryCursor* pCursor = ts_query_cursor_new();
    if (!pCursor)
        return;

    ts_query_cursor_exec(pCursor, pQuery, rootNode);

    // Temporary structure to collect all highlights
    struct HighlightEntry
    {
        uint32_t startRow;
        uint32_t startCol;   // UTF-8 byte offset in line
        uint32_t endRow;
        uint32_t endCol;     // UTF-8 byte offset in line
        uint32_t startByte;
        uint32_t endByte;
        int colorIndex;
        int priority;
        uint32_t order;
    };
    std::vector<HighlightEntry> highlights;

    // Map from (startByte, endByte) to colorIndex for definition resolution.
    // Key: (startByte << 32 | endByte). This assumes files < 4GB and that
    // nodes with identical byte ranges should share the same highlight.
    std::unordered_map<uint64_t, int> nodeHighlightMap;

    TSQueryMatch match;
    while (ts_query_cursor_next_match(pCursor, &match))
    {
        for (uint16_t i = 0; i < match.capture_count; i++)
        {
            TSQueryCapture capture = match.captures[i];
            TSNode node = capture.node;

            uint32_t captureNameLen = 0;
            const char* captureName = ts_query_capture_name_for_id(
                pQuery, capture.index, &captureNameLen);

            std::string sName(captureName, captureNameLen);
            int colorIndex = m_colorMap.MapCapture(sName);

            uint32_t nodeStartByte = ts_node_start_byte(node);
            uint32_t nodeEndByte = ts_node_end_byte(node);
            TSPoint startPoint = ts_node_start_point(node);
            TSPoint endPoint = ts_node_end_point(node);

            HighlightEntry entry;
            entry.startRow = startPoint.row;
            entry.startCol = startPoint.column;
            entry.endRow = endPoint.row;
            entry.endCol = endPoint.column;
            entry.startByte = nodeStartByte;
            entry.endByte = nodeEndByte;
            entry.colorIndex = colorIndex;
            entry.priority = MakeCapturePriority(sName, nodeStartByte, nodeEndByte);
            entry.order = NextBlockOrder();

            highlights.push_back(entry);

            // Record this node's highlight for locals resolution
            uint64_t nodeKey = (static_cast<uint64_t>(nodeStartByte) << 32) |
                               static_cast<uint64_t>(nodeEndByte);
            nodeHighlightMap[nodeKey] = colorIndex;
        }
    }

    ts_query_cursor_delete(pCursor);

    // --- Locals resolution ---
    // Now that we know the highlight for each node, assign highlights to
    // definitions and resolve references.
    if (!m_localScopes.empty())
    {
        // Pass 1: Assign highlights to definitions based on their node's highlight
        for (auto& scope : m_localScopes)
        {
            for (auto& def : scope.defs)
            {
                uint64_t defKey = (static_cast<uint64_t>(def.startByte) << 32) |
                                  static_cast<uint64_t>(def.endByte);
                auto it = nodeHighlightMap.find(defKey);
                if (it != nodeHighlightMap.end())
                    def.highlight = it->second;
            }
        }

        // Pass 2: Resolve references — find matching definition in enclosing scopes
        for (const auto& ref : m_pendingRefs)
        {
            int resolvedHighlight = -1;

            // Search scopes from innermost to outermost
            // Find all scopes that contain this reference
            std::vector<const LocalScope*> enclosingScopes;
            for (const auto& scope : m_localScopes)
            {
                if (ref.startByte >= scope.startByte && ref.endByte <= scope.endByte)
                    enclosingScopes.push_back(&scope);
            }

            // Sort by size (smallest = innermost first)
            std::sort(enclosingScopes.begin(), enclosingScopes.end(),
                [](const LocalScope* a, const LocalScope* b)
                {
                    return (a->endByte - a->startByte) < (b->endByte - b->startByte);
                });

            // Search for a matching definition
            for (const auto* pScope : enclosingScopes)
            {
                for (const auto& def : pScope->defs)
                {
                    // Match by name and ensure the definition appears before the reference
                    if (def.name == ref.name && def.startByte <= ref.startByte && def.highlight >= 0)
                    {
                        resolvedHighlight = def.highlight;
                        break;
                    }
                }
                if (resolvedHighlight >= 0)
                    break;
                // If this scope doesn't inherit, stop searching
                if (!pScope->inherits)
                    break;
            }

            if (resolvedHighlight >= 0)
            {
                uint64_t refKey = (static_cast<uint64_t>(ref.startByte) << 32) |
                                  static_cast<uint64_t>(ref.endByte);
                m_localRefHighlights[refKey] = resolvedHighlight;
            }
        }

        // Pass 3: Apply resolved reference highlights to the highlight entries
        for (auto& h : highlights)
        {
            uint64_t key = (static_cast<uint64_t>(h.startByte) << 32) |
                           static_cast<uint64_t>(h.endByte);
            auto it = m_localRefHighlights.find(key);
            if (it != m_localRefHighlights.end())
                h.colorIndex = it->second;
        }
    }

    // Sort by start position (row, then column)
    std::sort(highlights.begin(), highlights.end(),
        [](const HighlightEntry& a, const HighlightEntry& b)
        {
            if (a.startRow != b.startRow)
                return a.startRow < b.startRow;
            return a.startCol < b.startCol;
        });

    // Build per-line block arrays
    m_lineBlocks.clear();
    m_lineBlocks.resize(m_nLineCount);

    for (const auto& h : highlights)
    {
        // Handle tokens that span multiple lines (e.g. multi-line strings/comments)
        for (uint32_t row = h.startRow; row <= h.endRow && row < static_cast<uint32_t>(m_nLineCount); row++)
        {
            uint32_t byteCol = (row == h.startRow) ? h.startCol : 0;

            // Convert UTF-8 byte offset to UTF-16 character position
            int charPos = Utf8ByteOffsetToCharPos(static_cast<int>(row), byteCol);

            TreeSitterLineBlock block;
            block.nCharPos = charPos;
            block.nColorIndex = h.colorIndex;
            block.nPriority = h.priority;
            block.nOrder = h.order;
            m_lineBlocks[row].push_back(block);
        }

        // Emit a block at the end of the capture to restore normal color.
        // This prevents the token's color from "bleeding" past its end.
        if (h.endRow < static_cast<uint32_t>(m_nLineCount))
        {
            uint32_t endByteCol = h.endCol;

            // Convert UTF-8 byte offset at token end to UTF-16 character position
            int endCharPos = Utf8ByteOffsetToCharPos(static_cast<int>(h.endRow), endByteCol);

            TreeSitterLineBlock endBlock;
            endBlock.nCharPos = endCharPos;
            endBlock.nColorIndex = COLORINDEX_NORMALTEXT;
            endBlock.nPriority = INT_MIN;
            endBlock.nOrder = h.order;
            m_lineBlocks[h.endRow].push_back(endBlock);
        }
    }
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
void CTreeSitterParser::RunInjectionQuery()
{
    if (!m_pTree || !m_pLang || !m_pLang->GetInjectionQuery())
        return;

    const TSQuery* pQuery = m_pLang->GetInjectionQuery();
    TSNode rootNode = ts_tree_root_node(m_pTree);

    TSQueryCursor* pCursor = ts_query_cursor_new();
    if (!pCursor)
        return;

    ts_query_cursor_exec(pCursor, pQuery, rootNode);

    // Collect injection regions
    struct InjectionRegion
    {
        std::string language;       // Target language name
        uint32_t    contentStart;   // Byte offset in document
        uint32_t    contentEnd;
        TSPoint     startPoint;
        TSPoint     endPoint;
    };
    std::vector<InjectionRegion> injections;

    TSQueryMatch match;
    while (ts_query_cursor_next_match(pCursor, &match))
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
                const std::wstring& wname = m_pLang->GetName();
                for (wchar_t ch : wname)
                    language += static_cast<char>(ch);  // ASCII only
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
                if (start < m_documentText.size() && end <= m_documentText.size())
                    language = m_documentText.substr(start, end - start);
            }
        }

        if (hasContent && !language.empty())
        {
            InjectionRegion region;
            region.language = language;
            region.contentStart = ts_node_start_byte(contentNode);
            region.contentEnd = ts_node_end_byte(contentNode);
            region.startPoint = ts_node_start_point(contentNode);
            region.endPoint = ts_node_end_point(contentNode);
            injections.push_back(region);
        }
    }

    ts_query_cursor_delete(pCursor);

    if (injections.empty())
        return;

    // Process each injection: look up the language, parse the content, run highlights
    TreeSitterRegistry& registry = TreeSitterRegistry::Instance();

    for (const auto& inj : injections)
    {
        // Convert language name to wstring for registry lookup
        std::wstring wLangName;
        for (char ch : inj.language)
            wLangName += static_cast<wchar_t>(ch);

        // Try to find the language — look it up by name directly in the registry's
        // available languages (we need a way to get language by name, not just by ext).
        // For now, try common mappings. The language name from injections.scm
        // is typically the tree-sitter language name (e.g. "javascript", "css").
        // We can look it up as an extension since many languages use their name
        // as the extension.
        const CTreeSitterLanguage* pInjLang = registry.GetLanguageForName(wLangName);
        if (!pInjLang || !pInjLang->GetHighlightQuery())
            continue;

        // Extract the injection content
        if (inj.contentStart >= m_documentText.size() || inj.contentEnd > m_documentText.size())
            continue;

        std::string injContent = m_documentText.substr(inj.contentStart, inj.contentEnd - inj.contentStart);
        if (injContent.empty())
            continue;

        // Create a temporary parser for the injected content
        TSParser* pInjParser = ts_parser_new();
        if (!pInjParser)
            continue;

        ts_parser_set_language(pInjParser, pInjLang->GetLanguage());

        TSTree* pInjTree = ts_parser_parse_string(
            pInjParser, nullptr,
            injContent.c_str(),
            static_cast<uint32_t>(injContent.size()));

        if (pInjTree)
        {
            // Run highlight query on the injected tree
            const TSQuery* pInjQuery = pInjLang->GetHighlightQuery();
            TSNode injRoot = ts_tree_root_node(pInjTree);
            TSQueryCursor* pInjCursor = ts_query_cursor_new();

            if (pInjCursor)
            {
                ts_query_cursor_exec(pInjCursor, pInjQuery, injRoot);

                TSQueryMatch injMatch;
                while (ts_query_cursor_next_match(pInjCursor, &injMatch))
                {
                    for (uint16_t ci = 0; ci < injMatch.capture_count; ci++)
                    {
                        TSQueryCapture cap = injMatch.captures[ci];
                        TSNode capNode = cap.node;

                        uint32_t capNameLen = 0;
                        const char* capName = ts_query_capture_name_for_id(
                            pInjQuery, cap.index, &capNameLen);
                        std::string sCapName(capName, capNameLen);
                        int colorIndex = m_colorMap.MapCapture(sCapName);

                        // Map the injected node's position back to the parent document.
                        // The injection content starts at inj.startPoint in the parent doc.
                        TSPoint capStart = ts_node_start_point(capNode);
                        TSPoint capEnd = ts_node_end_point(capNode);

                        // Translate rows/columns to parent document coordinates.
                        // Since the injection content is a contiguous substring of
                        // m_documentText, continuation line columns in the sub-parse
                        // are already correct relative to the parent document lines.
                        // NOTE: This does NOT handle injection.combined (multiple
                        // disjoint ranges concatenated into one parse), but we don't
                        // support that feature currently.
                        uint32_t parentStartRow = inj.startPoint.row + capStart.row;
                        uint32_t parentEndRow = inj.startPoint.row + capEnd.row;
                        uint32_t parentStartCol = (capStart.row == 0)
                            ? inj.startPoint.column + capStart.column
                            : capStart.column;

                        // Add to parent's line blocks
                        for (uint32_t row = parentStartRow;
                             row <= parentEndRow && row < static_cast<uint32_t>(m_nLineCount);
                             row++)
                        {
                            uint32_t byteCol = (row == parentStartRow) ? parentStartCol : 0;
                            int charPos = Utf8ByteOffsetToCharPos(static_cast<int>(row), byteCol);

                            TreeSitterLineBlock block;
                            block.nCharPos = charPos;
                            block.nColorIndex = colorIndex;
                            block.nPriority = MakeCapturePriority(sCapName,
                                ts_node_start_byte(capNode), ts_node_end_byte(capNode));
                            block.nOrder = NextBlockOrder();
                            m_lineBlocks[row].push_back(block);
                        }
                    }
                }

                ts_query_cursor_delete(pInjCursor);
            }

            ts_tree_delete(pInjTree);
        }

        ts_parser_delete(pInjParser);
    }
}

void CTreeSitterParser::BuildLineCache(int nLineCount)
{
    // Sort each line's blocks by character position and deterministic precedence.
    // For identical start positions, keep the most specific / shortest capture.
    for (int i = 0; i < nLineCount && i < static_cast<int>(m_lineBlocks.size()); i++)
    {
        auto& blocks = m_lineBlocks[i];
        std::sort(blocks.begin(), blocks.end(),
            [](const TreeSitterLineBlock& a, const TreeSitterLineBlock& b)
            {
                if (a.nCharPos != b.nCharPos)
                    return a.nCharPos < b.nCharPos;
                if (a.nPriority != b.nPriority)
                    return a.nPriority < b.nPriority;
                return a.nOrder < b.nOrder;
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

void CTreeSitterParser::GetLineBlocks(int nLineIndex,
                                           CrystalLineParser::TEXTBLOCK* pBuf,
                                           int& nActualItems,
                                           int nMaxBlocks) const
{
    // Cookie-only mode (pBuf == nullptr): caller just wants the cookie.
    // Tree-sitter doesn't use cookies, so nothing to do.
    if (!pBuf)
        return;

    // Fix #4: bounds check against cached line count
    if (nLineIndex < 0 || nLineIndex >= static_cast<int>(m_lineBlocks.size()))
        return;

    const auto& blocks = m_lineBlocks[nLineIndex];

    // The caller (GetTextBlocks) pre-inserts a NORMALTEXT block at position 0
    // and sets nActualItems = 1 before calling ParseLine. We follow the same
    // convention as existing parsers: append our blocks starting at the current
    // nActualItems, but overwrite the caller's default block if we have our own
    // block at position 0.

    for (const auto& block : blocks)
    {
        // If the caller's last block is at the same position, overwrite it
        // (same logic as DEFINE_BLOCK macro in crystallineparser.h)
        if (nActualItems > 0 && pBuf[nActualItems - 1].m_nCharPos == block.nCharPos)
        {
            pBuf[nActualItems - 1].m_nColorIndex = block.nColorIndex;
            pBuf[nActualItems - 1].m_nBgColorIndex = COLORINDEX_BKGND;
            continue;
        }

        // Skip if same color as previous block (no visible change)
        if (nActualItems > 0 && pBuf[nActualItems - 1].m_nColorIndex == block.nColorIndex)
            continue;

        // Bounds check: stop if we'd overflow the buffer
        if (nMaxBlocks > 0 && nActualItems >= nMaxBlocks)
            break;

        pBuf[nActualItems].m_nCharPos = block.nCharPos;
        pBuf[nActualItems].m_nColorIndex = block.nColorIndex;
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;
        nActualItems++;
    }
}


// ============================================================================
// TreeSitterRegistry
// ============================================================================

// Default extension -> language mappings.
// Users can extend this via configuration.
static const struct
{
    const wchar_t* ext;
    const wchar_t* language;
} s_defaultExtMap[] =
{
    // F# (primary target)
    { L"fs",    L"fsharp" },
    { L"fsx",   L"fsharp" },
    { L"fsi",   L"fsharp_signature" },

    // Common languages
    { L"c",     L"c" },
    { L"h",     L"c" },
    { L"cpp",   L"cpp" },
    { L"cxx",   L"cpp" },
    { L"cc",    L"cpp" },
    { L"hpp",   L"cpp" },
    { L"hxx",   L"cpp" },
    { L"cs",    L"c-sharp" },
    { L"py",    L"python" },
    { L"js",    L"javascript" },
    { L"ts",    L"typescript" },
    { L"tsx",   L"tsx" },
    { L"jsx",   L"javascript" },
    { L"java",  L"java" },
    { L"go",    L"go" },
    { L"rs",    L"rust" },
    { L"rb",    L"ruby" },
    { L"lua",   L"lua" },
    { L"sh",    L"bash" },
    { L"bash",  L"bash" },
    { L"json",  L"json" },
    { L"yaml",  L"yaml" },
    { L"yml",   L"yaml" },
    { L"xml",   L"xml" },
    { L"html",  L"html" },
    { L"htm",   L"html" },
    { L"css",   L"css" },
    { L"sql",   L"sql" },
    { L"ps1",   L"powershell" },
    { L"psm1",  L"powershell" },
    { L"php",   L"php" },
    { L"pl",    L"perl" },
    { L"swift", L"swift" },
    { L"kt",    L"kotlin" },
    { L"scala", L"scala" },
    { L"hs",    L"haskell" },
    { L"ml",    L"ocaml" },
    { L"mli",   L"ocaml" },
    { L"ex",    L"elixir" },
    { L"exs",   L"elixir" },
    { L"zig",   L"zig" },
    { L"nim",   L"nim" },
    { L"toml",  L"toml" },
    { L"md",    L"markdown" },
};

TreeSitterRegistry& TreeSitterRegistry::Instance()
{
    static TreeSitterRegistry instance;
    return instance;
}

void TreeSitterRegistry::Initialize(const std::wstring& sGrammarDir)
{
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

    // Register default extension mappings
    for (const auto& mapping : s_defaultExtMap)
    {
        m_extMap[mapping.ext] = mapping.language;
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

const CTreeSitterLanguage* TreeSitterRegistry::GetLanguageForExt(const std::wstring& sExt)
{
    // Look up extension -> language name
    auto itExt = m_extMap.find(sExt);
    if (itExt == m_extMap.end())
        return nullptr;

    const std::wstring& sLangName = itExt->second;

    // Check if already loaded
    auto itLang = m_languages.find(sLangName);
    if (itLang != m_languages.end())
    {
        // Already loaded - return if it has a valid highlight query
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
        // Record failure so we don't retry
        m_failedLanguages.insert(sLangName);
        return nullptr;
    }

    // Check if the loaded grammar has a highlight query
    if (!pLang->GetHighlightQuery())
    {
        m_failedLanguages.insert(sLangName);
        return nullptr;
    }

    const CTreeSitterLanguage* pResult = pLang.get();
    m_languages[sLangName] = std::move(pLang);
    return pResult;
}

const CTreeSitterLanguage* TreeSitterRegistry::GetLanguageForName(const std::wstring& sLangName)
{
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

void TreeSitterRegistry::RegisterExtension(const std::wstring& sExt, const std::wstring& sLanguage)
{
    m_extMap[sExt] = sLanguage;
}

// ============================================================================
// CTreeSitterParser - Additional Methods
// ============================================================================

/**
 * @brief Convenience: parse document from a text buffer.
 */
void CTreeSitterParser::ParseFromBuffer(ITextBuffer* pBuffer)
{
    if (!pBuffer)
        return;

    const int nLineCount = pBuffer->GetLineCount();
    if (nLineCount == 0)
        return;

    // Collect line pointers and lengths
    std::vector<const tchar_t*> ppszLines(nLineCount);
    std::vector<int> pnLineLengths(nLineCount);

    for (int i = 0; i < nLineCount; i++)
    {
        ppszLines[i] = pBuffer->GetLineChars(i);
        pnLineLengths[i] = pBuffer->GetLineLength(i);
    }

    ParseDocument(ppszLines.data(), pnLineLengths.data(), nLineCount);
}

/**
 * @brief Get the node type name at a specific position.
 * 
 * This is used for comment filtering and other syntax-aware operations.
 */
std::wstring CTreeSitterParser::GetNodeTypeAt(int nLineIndex, int nCharPos) const
{
    if (!m_pTree || nLineIndex < 0 || nLineIndex >= m_nLineCount)
        return _T("");

    // Convert line + character position to byte offset
    if (nLineIndex >= static_cast<int>(m_lineUtf8.size()))
        return _T("");

    // Calculate byte offset
    uint32_t byteOffset = 0;

    // Add bytes from all previous lines
    for (int i = 0; i < nLineIndex; i++)
    {
        if (i >= static_cast<int>(m_lineUtf8.size()))
            return _T("");
        byteOffset += static_cast<uint32_t>(m_lineUtf8[i].size());
        byteOffset++;  // newline character
    }

    // Add bytes from current line up to nCharPos
    const std::string& lineUtf8 = m_lineUtf8[nLineIndex];
    int charCount = 0;
    for (size_t i = 0; i < lineUtf8.size() && charCount < nCharPos; )
    {
        unsigned char byte = lineUtf8[i];

        // UTF-8 character length determination
        int charLen = 1;
        if ((byte & 0x80) == 0x00)
            charLen = 1;  // ASCII
        else if ((byte & 0xE0) == 0xC0)
            charLen = 2;
        else if ((byte & 0xF0) == 0xE0)
            charLen = 3;
        else if ((byte & 0xF8) == 0xF0)
            charLen = 4;

        i += charLen;
        byteOffset += charLen;
        charCount++;
    }

    // Get the tree-sitter node at this byte position
    TSNode rootNode = ts_tree_root_node(m_pTree);
    TSNode node = ts_node_descendant_for_byte_range(rootNode, byteOffset, byteOffset);

    if (ts_node_is_null(node))
        return _T("");

    // Get node type
    const char* pszType = ts_node_type(node);
    if (!pszType)
        return _T("");

    // Convert from UTF-8 to wstring/String
#ifdef UNICODE
    int nLen = MultiByteToWideChar(CP_UTF8, 0, pszType, -1, nullptr, 0);
    if (nLen == 0)
        return _T("");

    std::vector<wchar_t> buffer(nLen);
    MultiByteToWideChar(CP_UTF8, 0, pszType, -1, buffer.data(), nLen);
    return std::wstring(buffer.data());
#else
    return String(pszType);
#endif
}

bool CTreeSitterParser::IsCommentPosition(int nLineIndex, int nCharPos) const
{
    const std::wstring sNodeType = GetNodeTypeAt(nLineIndex, nCharPos);
    return sNodeType.find(L"comment") != std::wstring::npos;
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

        std::vector<const LocalScope*> enclosingScopes;
        for (const auto& scope : m_localScopes)
        {
            if (ref.startByte >= scope.startByte && ref.endByte <= scope.endByte)
                enclosingScopes.push_back(&scope);
        }

        std::sort(enclosingScopes.begin(), enclosingScopes.end(),
            [](const LocalScope* a, const LocalScope* b)
            {
                return (a->endByte - a->startByte) < (b->endByte - b->startByte);
            });

        for (const auto* pScope : enclosingScopes)
        {
            for (const auto& def : pScope->defs)
            {
                if (def.name == ref.name && def.startByte <= ref.startByte)
                {
                    defStartByte = def.startByte;
                    defEndByte = def.endByte;
                    return true;
                }
            }
            if (!pScope->inherits)
                break;
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
    uint32_t currentOffset = 0;
    for (int i = 0; i < static_cast<int>(m_lineUtf8.size()); ++i)
    {
        const std::string& utf8Line = m_lineUtf8[i];
        const uint32_t lineStart = currentOffset;
        const uint32_t lineEnd = lineStart + static_cast<uint32_t>(utf8Line.size());
        if (byteOffset <= lineEnd)
        {
            nLineIndex = i;
            nCharPos = Utf8ByteOffsetToCharPos(i, byteOffset - lineStart);
            return true;
        }
        currentOffset = lineEnd + 1;
    }

    if (!m_lineUtf8.empty() && byteOffset == currentOffset - 1)
    {
        nLineIndex = static_cast<int>(m_lineUtf8.size()) - 1;
        nCharPos = static_cast<int>(m_lineUtf8.back().size());
        return true;
    }

    return false;
}

bool CTreeSitterParser::TryGetTagDefinitionByNameAt(int nLineIndex, int nCharPos, uint32_t& defStartByte, uint32_t& defEndByte) const
{
    if (!m_pBuffer || nLineIndex < 0 || nLineIndex >= m_pBuffer->GetLineCount())
        return false;

    const tchar_t* pszLine = m_pBuffer->GetLineChars(nLineIndex);
    const int nLineLength = m_pBuffer->GetLineLength(nLineIndex);
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

    std::wstring symbolW(pszLine + nStart, nEnd - nStart);
#ifdef _UNICODE
    int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, symbolW.c_str(), static_cast<int>(symbolW.size()), nullptr, 0, nullptr, nullptr);
    if (nUtf8Len <= 0)
        return false;
    std::string symbol(nUtf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, symbolW.c_str(), static_cast<int>(symbolW.size()), symbol.data(), nUtf8Len, nullptr, nullptr);
#else
    std::string symbol(symbolW.begin(), symbolW.end());
#endif

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

bool CTreeSitterParser::FindDefinition(int nLineIndex, int nCharPos, int& nDefLine, int& nDefChar) const
{
    if (!m_pTree || nLineIndex < 0 || nLineIndex >= m_nLineCount)
        return false;

    uint32_t byteOffset = 0;
    for (int i = 0; i < nLineIndex; ++i)
        byteOffset += static_cast<uint32_t>(m_lineUtf8[i].size()) + 1;

    const std::string& lineUtf8 = m_lineUtf8[nLineIndex];
    int charCount = 0;
    for (size_t i = 0; i < lineUtf8.size() && charCount < nCharPos; )
    {
        const unsigned char byte = static_cast<unsigned char>(lineUtf8[i]);
        int charLen = 1;
        if ((byte & 0x80) == 0x00)
            charLen = 1;
        else if ((byte & 0xE0) == 0xC0)
            charLen = 2;
        else if ((byte & 0xF0) == 0xE0)
            charLen = 3;
        else if ((byte & 0xF8) == 0xF0)
            charLen = 4;
        i += charLen;
        byteOffset += charLen;
        charCount++;
    }

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
