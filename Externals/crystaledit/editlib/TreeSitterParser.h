////////////////////////////////////////////////////////////////////////////
//  File:       TreeSitterParser.h
//  Version:    1.0.1
//  Created:    2026-03-26
//
//  Tree-sitter based syntax highlighting bridge for CrystalEdit.
//
//  Loads tree-sitter grammar DLLs and highlight query (.scm) files
//  at runtime. Parses full documents into ASTs and extracts per-line
//  color blocks mapped to WinMerge's COLORINDEX values.
//
//  Grammar DLLs are loaded from a "TreeSitterGrammars" directory
//  alongside the WinMerge executable. Each DLL exports a function
//  named tree_sitter_<language>() returning const TSLanguage*.
//
//  Highlight queries are loaded from .scm files in the same directory.
//
//  SPDX-License-Identifier: GPL-2.0-or-later
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "crystalparser.h"
#include "SyntaxColors.h"
#include "parsers/crystallineparser.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

// Forward declarations for tree-sitter C API types.
// The actual tree_sitter/api.h header is included only in the .cpp file.
typedef struct TSParser TSParser;
typedef struct TSTree TSTree;
typedef struct TSQuery TSQuery;
typedef struct TSLanguage TSLanguage;

class CCrystalTextBuffer;

/**
 * @brief Manages a tree-sitter grammar loaded from a DLL.
 *
 * Each instance holds a loaded grammar DLL, the TSLanguage pointer,
 * and the compiled highlight, locals, and injection queries for that language.
 */
class CTreeSitterLanguage
{
public:
    CTreeSitterLanguage();
    ~CTreeSitterLanguage();

    CTreeSitterLanguage(const CTreeSitterLanguage&) = delete;
    CTreeSitterLanguage& operator=(const CTreeSitterLanguage&) = delete;

    /**
     * @brief Load a grammar DLL and its query files.
     * @param sGrammarDir  Directory containing grammar DLLs and .scm files.
     * @param sLanguage    Language name (e.g. "fsharp", "python", "cpp").
     * @return true if both the DLL and highlight query loaded successfully.
     *
     * Also attempts to load locals.scm and injections.scm if present.
     * Failure to load locals or injections is not fatal.
     */
    bool Load(const std::wstring& sGrammarDir, const std::wstring& sLanguage);

    /** @brief Get the loaded TSLanguage pointer (or nullptr). */
    const TSLanguage* GetLanguage() const { return m_pLanguage; }

    /** @brief Get the compiled highlight TSQuery (or nullptr). */
    const TSQuery* GetHighlightQuery() const { return m_pHighlightQuery; }

    /** @brief Get the compiled locals TSQuery (or nullptr). */
    const TSQuery* GetLocalsQuery() const { return m_pLocalsQuery; }

    /** @brief Get the compiled injection TSQuery (or nullptr). */
    const TSQuery* GetInjectionQuery() const { return m_pInjectionQuery; }

    /** @brief Get the language name. */
    const std::wstring& GetName() const { return m_sName; }

private:
    /** @brief Helper to load and compile a .scm query file. */
    TSQuery* LoadQuery(const std::wstring& sPath);

    HMODULE           m_hDll;
    const TSLanguage* m_pLanguage;
    TSQuery*          m_pHighlightQuery;
    TSQuery*          m_pLocalsQuery;
    TSQuery*          m_pInjectionQuery;
    std::wstring      m_sName;
};


/**
 * @brief Maps tree-sitter highlight capture names to COLORINDEX values.
 *
 * Standard capture names from nvim-treesitter / tree-sitter highlight queries:
 *   @keyword, @function, @function.call, @string, @comment, @number,
 *   @operator, @type, @variable, @property, @constant, @punctuation,
 *   @constructor, @module, @attribute, @label, @preproc, etc.
 *
 * These are collapsed into WinMerge's 9 syntax COLORINDEX values.
 */
class CTreeSitterColorMap
{
public:
    CTreeSitterColorMap();

    /**
     * @brief Map a tree-sitter capture name to a COLORINDEX.
     * @param sCaptureName  The capture name (e.g. "keyword", "string").
     * @return The COLORINDEX, or COLORINDEX_NORMALTEXT if unknown.
     */
    int MapCapture(const std::string& sCaptureName) const;

private:
    std::unordered_map<std::string, int> m_map;
};


/**
 * @brief Per-line cached highlight result.
 */
struct TreeSitterLineBlock
{
    int nCharPos;
    int nColorIndex;
};


/**
 * @brief Tree-sitter based syntax parser for CrystalEdit views.
 *
 * This class replaces the line-by-line keyword parsers with full-document
 * AST-based parsing via tree-sitter. It:
 *   1. Parses the entire document into a tree-sitter AST.
 *   2. Runs highlight queries to extract token ranges.
 *   3. Caches per-line color block arrays.
 *   4. Serves cached results on per-line ParseLine() calls.
 *   5. Supports lazy re-parsing: edits mark the cache dirty,
 *      and the next paint cycle triggers a single reparse.
 *
 * Override ParseLine() in the view to call this parser's GetLineBlocks().
 *
 * Usage:
 * @code
 *   auto pLang = TreeSitterRegistry::Instance().GetLanguageForExt(L"fs");
 *   if (pLang) {
 *       m_treeSitterParser.SetLanguage(pLang);
 *       m_treeSitterParser.ParseFromView(pView);
 *       // In ParseLine override:
 *       m_treeSitterParser.GetLineBlocks(nLineIndex, pBuf, nActualItems);
 *   }
 * @endcode
 */
class CTreeSitterParser
{
public:
    CTreeSitterParser();
    ~CTreeSitterParser();

    CTreeSitterParser(const CTreeSitterParser&) = delete;
    CTreeSitterParser& operator=(const CTreeSitterParser&) = delete;

    /**
     * @brief Set the language to use for parsing.
     * @param pLang  Pointer to a loaded CTreeSitterLanguage.
     */
    void SetLanguage(const CTreeSitterLanguage* pLang);

    /**
     * @brief Parse (or re-parse) the full document.
     * @param ppszLines  Array of line pointers (from CCrystalTextBuffer).
     * @param pnLineLengths  Array of line lengths.
     * @param nLineCount  Number of lines in the document.
     */
    void ParseDocument(const tchar_t* const* ppszLines, const int* pnLineLengths, int nLineCount);

    /**
     * @brief Convenience: parse document from a text buffer view.
     * @param pView  The text view to read line data from.
     */
    void ParseFromView(class CCrystalTextView* pView);

    /**
     * @brief Mark the parse cache as dirty (e.g. after an edit).
     *
     * The next call to EnsureParsed() (from ParseLine/GetLineBlocks)
     * will trigger a single reparse. This avoids reparsing on every
     * keystroke and instead defers to the next paint cycle.
     */
    void MarkDirty() { m_bDirty = true; }

    /**
     * @brief Notify the parser of an edit for incremental reparsing.
     *
     * Extracts the last edit's position from the buffer's UndoRecord
     * and calls ts_tree_edit() on the existing tree so tree-sitter
     * can reuse unchanged subtrees during the next reparse.
     *
     * Also marks the cache dirty.
     *
     * @param pBuf  The text buffer that was edited.
     */
    void NotifyEdit(class CCrystalTextBuffer* pBuf);

    /**
     * @brief Ensure the document is parsed and cache is up-to-date.
     * @param pView  The text view to read line data from (if reparse needed).
     *
     * Called lazily from ParseLine. Only reparses if marked dirty.
     */
    void EnsureParsed(class CCrystalTextView* pView);

    /**
     * @brief Get the cached color blocks for a specific line.
     * @param nLineIndex   Zero-based line index.
     * @param pBuf         Output buffer for TEXTBLOCK entries (may be nullptr for cookie-only).
     * @param nActualItems In/out: on entry, the number of blocks already in pBuf
     *                     (caller pre-inserts a default NORMALTEXT block at position 0);
     *                     on return, the total number of blocks.
     * @param nMaxBlocks   Maximum number of TEXTBLOCK entries that fit in pBuf.
     *                     Pass 0 to skip bounds checking (legacy behavior).
     */
    void GetLineBlocks(int nLineIndex,
                       CrystalLineParser::TEXTBLOCK* pBuf,
                       int& nActualItems,
                       int nMaxBlocks = 0) const;

    /** @brief Check if a valid tree is available. */
    bool HasTree() const { return m_pTree != nullptr; }

    /** @brief Check if a language is set. */
    bool HasLanguage() const { return m_pLang != nullptr; }

    /** @brief Get the number of lines in the cached parse result. */
    int GetCachedLineCount() const { return m_nLineCount; }

    /** @brief Check if cache needs rebuilding. */
    bool IsDirty() const { return m_bDirty; }

    /** @brief Invalidate cached results and free tree. */
    void Invalidate();

private:
    void EnsureParser();
    void RunHighlightQuery();
    void RunLocalsQuery();
    void RunInjectionQuery();
    void BuildLineCache(int nLineCount);
    int Utf8ByteOffsetToCharPos(int nLine, uint32_t byteCol) const;

    /**
     * @brief Extract #set! predicate properties from a query pattern.
     * @param pQuery       The query containing the pattern.
     * @param patternIndex The pattern index.
     * @param key          The property key to look for (e.g. "injection.language").
     * @return The property value, or empty string if not found.
     */
    static std::string GetSetProperty(const TSQuery* pQuery,
                                       uint32_t patternIndex,
                                       const std::string& key);

    TSParser*           m_pParser;      // Created lazily on first use
    TSTree*             m_pTree;
    const CTreeSitterLanguage* m_pLang;
    CTreeSitterColorMap m_colorMap;
    bool                m_bDirty;       // True when cache needs rebuild

    // Cached per-line highlight blocks
    std::vector<std::vector<TreeSitterLineBlock>> m_lineBlocks;

    // Per-line UTF-8 content (for byte offset -> character position mapping)
    std::vector<std::string> m_lineUtf8;

    // Full document text (needed for tree-sitter, which requires contiguous input
    // or a custom read callback)
    std::string m_documentText;
    int         m_nLineCount;

    // --- Locals support ---
    // Maps (startByte, endByte) of definition nodes to their highlight color.
    // Built by RunLocalsQuery + RunHighlightQuery cross-referencing.
    struct LocalDef
    {
        std::string name;       // Variable/symbol name
        uint32_t    startByte;  // Definition node start
        uint32_t    endByte;    // Definition node end
        int         highlight;  // COLORINDEX from highlights.scm (-1 = unknown)
    };

    struct LocalScope
    {
        uint32_t startByte;
        uint32_t endByte;
        bool     inherits;
        std::vector<LocalDef> defs;
    };

    // Scopes from locals.scm, sorted by startByte
    std::vector<LocalScope> m_localScopes;

    // Map from reference node byte range -> resolved highlight color.
    // Key: (startByte << 32 | endByte). Assumes files < 4GB.
    std::unordered_map<uint64_t, int> m_localRefHighlights;

    // Pending references from RunLocalsQuery, resolved during RunHighlightQuery
    struct PendingRef
    {
        std::string name;
        uint32_t    startByte;
        uint32_t    endByte;
        uint32_t    scopeStartByte;
    };
    std::vector<PendingRef> m_pendingRefs;
};


/**
 * @brief Global registry for tree-sitter grammars.
 *
 * Scans the grammar directory on first use to discover available grammar DLLs,
 * but loads them lazily on first request. Maps file extensions to languages.
 */
class TreeSitterRegistry
{
public:
    /**
     * @brief Get the singleton instance.
     */
    static TreeSitterRegistry& Instance();

    /**
     * @brief Initialize the registry by scanning the grammar directory.
     * @param sGrammarDir  Path to directory containing grammar DLLs and .scm files.
     *                     If empty, uses "<exe_dir>/TreeSitterGrammars/".
     *
     * This only discovers available DLL filenames. Grammars are loaded
     * lazily on first request via GetLanguageForExt().
     */
    void Initialize(const std::wstring& sGrammarDir = L"");

    /**
     * @brief Find a loaded language for a file extension.
     * @param sExt  File extension without dot (e.g. "fs", "py", "cpp").
     * @return Pointer to the language, or nullptr if not available.
     *
     * Loads the grammar DLL lazily on first request for each language.
     */
    const CTreeSitterLanguage* GetLanguageForExt(const std::wstring& sExt);

    /**
     * @brief Find a loaded language by language name.
     * @param sLangName  Language name (e.g. "javascript", "css", "python").
     * @return Pointer to the language, or nullptr if not available.
     *
     * Used by injection processing to look up grammars for embedded languages.
     * Loads the grammar DLL lazily on first request.
     */
    const CTreeSitterLanguage* GetLanguageForName(const std::wstring& sLangName);

    /**
     * @brief Register a file extension mapping to a language name.
     * @param sExt       Extension (e.g. "fs").
     * @param sLanguage  Language name (e.g. "fsharp").
     */
    void RegisterExtension(const std::wstring& sExt, const std::wstring& sLanguage);

    /** @brief Check if the registry has been initialized. */
    bool IsInitialized() const { return m_bInitialized; }

private:
    TreeSitterRegistry() : m_bInitialized(false) {}

    bool m_bInitialized;
    std::wstring m_sGrammarDir;

    // language name -> loaded grammar (loaded lazily)
    std::unordered_map<std::wstring, std::unique_ptr<CTreeSitterLanguage>> m_languages;

    // language names that have a DLL available but haven't been loaded yet
    std::unordered_set<std::wstring> m_availableLanguages;

    // language names that failed to load (don't retry)
    std::unordered_set<std::wstring> m_failedLanguages;

    // file extension -> language name
    std::unordered_map<std::wstring, std::wstring> m_extMap;
};
