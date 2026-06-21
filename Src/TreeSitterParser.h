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

#include "SyntaxColors.h"
#include "TextDefinition.h"
#include "ITextBuffer.h"
#include "ISyntaxParser.h"
#include "UnicodeString.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

// Forward declarations for tree-sitter C API types.
// The actual tree_sitter/api.h header is included only in the .cpp file.
struct TSParser;
struct TSTree;
struct TSQuery;
struct TSLanguage;
struct TSPoint;
struct CEPoint;

/**
 * @brief Manages a tree-sitter grammar loaded from a DLL.
 *
 * Each instance holds a loaded grammar DLL, the TSLanguage pointer,
 * and the compiled highlight, locals, tags, and injection queries for that language.
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
	 * Also attempts to load locals.scm, tags.scm, and injections.scm if present.
	 * Failure to load optional queries is not fatal.
	 */
	bool Load(const std::wstring& sGrammarDir, const std::wstring& sLanguage);

	/** @brief Get the loaded TSLanguage pointer (or nullptr). */
	const TSLanguage* GetLanguage() const { return m_pLanguage; }

	/** @brief Get the compiled highlight TSQuery (or nullptr). */
	const TSQuery* GetHighlightQuery() const { return m_pHighlightQuery; }

	/** @brief Get the compiled locals TSQuery (or nullptr). */
	const TSQuery* GetLocalsQuery() const { return m_pLocalsQuery; }

	/** @brief Get the compiled tags TSQuery (or nullptr). */
	const TSQuery* GetTagsQuery() const { return m_pTagsQuery; }

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
	TSQuery*          m_pTagsQuery;
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
	int nPriority;
	uint32_t nOrder;
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
 */
/**
 * @brief Maximum number of lines for which tree-sitter highlighting is enabled.
 *
 * Files with more lines than this threshold will fall back to plain-text
 * display to avoid the cost of parsing a huge document on every paint.
 */
static constexpr int kMaxLinesForHighlight = 65536;

class CTreeSitterParser : public LangServices::ISyntaxParser
{
public:
	CTreeSitterParser() = delete;
	virtual ~CTreeSitterParser();

	explicit CTreeSitterParser(LangServices::LanguageId textType);
	CTreeSitterParser(const CTreeSitterParser&) = delete;
	CTreeSitterParser& operator=(const CTreeSitterParser&) = delete;

	// ISyntaxParser interface implementation
	void Invalidate() override;
	void SetTextBuffer(LangServices::ITextBuffer* pTextBuffer) override;
	LangServices::ITextBuffer* GetTextBuffer() const override { return m_pTextBuffer; }
	std::vector<LangServices::TEXTBLOCK> ParseLine(int nLineIndex) override;
	void NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType) override;
	LangServices::LanguageId GetParserType() const override { return m_textType; }
	bool FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) override;

	/**
	 * @brief Set the language to use for parsing.
	 * @param pLang  Pointer to a loaded CTreeSitterLanguage.
	 */
	void SetLanguage(const CTreeSitterLanguage* pLang);

	/**
	 * @brief Parse (or re-parse) the full document.
	 */
	void ParseDocument();

	/**
	 * @brief Ensure the document is parsed and cache is up-to-date.
	 *
	 * Called lazily from ParseLine. Only reparses if marked dirty.
	 */
	void EnsureParsed(int nLineIndex = 0);

	void EnsureTagsQueried();

	/** @brief Check if a language is set. */
	bool HasLanguage() const { return m_pLang != nullptr; }

	/**
	 * @brief Get the node type name at a specific position.
	 * @param nLineIndex  Zero-based line index.
	 * @param nCharPos    Zero-based character position in line.
	 * @return Node type name (e.g. "comment", "string_literal"), or empty string.
	 * 
	 * This is used by TreeSitterHighlightService to determine if a position
	 * is within a comment or string for filtering purposes.
	 */
	String GetNodeTypeAt(int nLineIndex, int nCharPos);

	bool FindDefinition(int nLineIndex, int nCharPos, int& nDefLine, int& nDefChar);

private:
	void EnsureParser();
	void RunHighlightQuery(int nStartLine, int nEndLine);
	void RunLocalsQuery();
	void RunTagsQuery();
	void RunInjectionQuery(int nStartLine, int nEndLine);
	void BuildLineCache(int nStartLine, int nEndLine);
	uint32_t CharPosToByteOffset(int nLine, int nCharPos, bool useCache = true) const;
	void CharPosToTSPoint(int line, int charPos, TSPoint& pt) const;
	std::wstring GetUtf16Text(uint32_t startByte, uint32_t endByte) const;
	uint32_t GetTotalBytes() const;
	bool TryGetDefinitionByteRangeAt(uint32_t byteOffset, uint32_t& defStartByte, uint32_t& defEndByte) const;
	bool ByteOffsetToLineChar(uint32_t byteOffset, int& nLineIndex, int& nCharPos) const;
	bool TryGetTagDefinitionByNameAt(int nLineIndex, int nCharPos, uint32_t& defStartByte, uint32_t& defEndByte) const;
	uint32_t NextBlockOrder() { return m_nextBlockOrder++; }

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

	LangServices::ITextBuffer* m_pTextBuffer;
	TSParser*           m_pParser;      // Created lazily on first use
	TSTree*             m_pTree;
	const CTreeSitterLanguage* m_pLang;
	LangServices::LanguageId m_textType;  ///< Original LanguageId for this parser (e.g. SRC_CPP)
	CTreeSitterColorMap m_colorMap;
	bool                m_bNeedsParse;       // True when cache needs rebuild
	bool                m_bTagsQueried;      // True if we've already run the tags query at least once
	bool                m_bTreeSitterDisabled; // True if we disabled treesitter due to excessive line count
	int                 m_nLineCount;
	uint32_t            m_nextBlockOrder;

	// Cached per-line highlight blocks
	std::vector<std::vector<TreeSitterLineBlock>> m_lineBlocks;
	static constexpr int kCacheChunkSize = 300;
	std::vector<bool> m_cachedChunks;
	std::vector<uint32_t> m_lineStartBytes;

	// --- Locals support ---
	// Maps (startByte, endByte) of definition nodes to their highlight color.
	// Built by RunLocalsQuery + RunHighlightQuery cross-referencing.
	struct LocalDef
	{
		std::wstring name;      // Variable/symbol name
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
		std::wstring name;
		uint32_t    startByte;
		uint32_t    endByte;
		uint32_t    scopeStartByte;
	};
	std::vector<PendingRef> m_pendingRefs;

	struct TagDef
	{
		std::wstring name;
		uint32_t    startByte;
		uint32_t    endByte;
	};

	struct TagRef
	{
		std::wstring name;
		uint32_t    startByte;
		uint32_t    endByte;
	};

	std::vector<TagDef> m_tagDefs;
	std::vector<TagRef> m_tagRefs;
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
	 * @brief Find a loaded language by language name.
	 * @param sLangName  Language name (e.g. "javascript", "css", "python").
	 * @return Pointer to the language, or nullptr if not available.
	 *
	 * Used by injection processing to look up grammars for embedded languages.
	 * Loads the grammar DLL lazily on first request.
	 */
	const CTreeSitterLanguage* GetLanguageForName(const std::wstring& sLangName);

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
};

class TreeSitterSyntaxParserFactory : public LangServices::ISyntaxParserFactory
{
public:
	static TreeSitterSyntaxParserFactory& GetInstance()
	{
		static TreeSitterSyntaxParserFactory instance;
		return instance;
	}

	TreeSitterSyntaxParserFactory() {}
	virtual ~TreeSitterSyntaxParserFactory() = default;

	bool IsSupported(LangServices::LanguageId type) const;
	virtual std::shared_ptr<LangServices::ISyntaxParser> Create(LangServices::LanguageId type) const override
	{
		if (!IsSupported(type))
			return nullptr;
		return std::make_shared<CTreeSitterParser>(type);
	}
};
