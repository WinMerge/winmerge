#pragma once

#include "ISyntaxParser.h"
#include <memory>

/**
 * @brief Abstract factory interface for creating ISyntaxParser instances.
 *
 * Implement this interface to provide a custom syntax parser backend
 * (e.g., a Tree-sitter grammar pack, a language server, etc.).
 * Factories are registered with SyntaxParserRegistry at application startup
 * and are selected based on priority when creating parsers.
 *
 * Dependency constraints: this file MUST NOT include CCrystalTextView or
 * crystallineparser.h. Only ISyntaxParser.h is allowed.
 */
class ISyntaxParserFactory
{
public:
	virtual ~ISyntaxParserFactory() = default;

	/**
	 * @brief Get the unique display name for this factory.
	 * @return Null-terminated string identifying this factory (e.g., L"CrystalLine", L"TreeSitter").
	 */
	virtual const tchar_t* GetName() const = 0;

	/**
	 * @brief Return the priority for a given file extension.
	 * @param pszExt File extension without leading dot (e.g., L"cpp", L"py").
	 *              May be nullptr or empty for extension-less files.
	 * @return Priority value. Higher values win. Return 0 or less if this
	 *         factory cannot handle the extension.
	 *
	 * Typical values:
	 *   100 — CrystalLineParserFactory (fallback, covers all languages)
	 *   200 — TreeSitterParserFactory  (high-quality, covers a subset)
	 */
	virtual int GetPriority(const tchar_t* pszExt) const = 0;

	/**
	 * @brief Return the priority based on file content (shebang, XML header, etc.).
	 * @param pszContent Pointer to the first bytes/chars of the file.
	 * @param len Number of characters available in pszContent.
	 * @return Priority value (same semantics as GetPriority). Return 0 if
	 *         content-based detection is not supported or inconclusive.
	 *
	 * This is called in addition to GetPriority() when content is available.
	 * The registry will use the maximum of the two returned values.
	 */
	virtual int GetPriorityFromContent(
		const tchar_t* pszContent, size_t len) const { return 0; }

	/**
	 * @brief Create a parser instance for the given TextType.
	 * @param type The language / file-format type.
	 * @return A newly created ISyntaxParser, or nullptr if this factory
	 *         does not support the requested type.
	 */
	virtual std::unique_ptr<ISyntaxParser> Create(
		ISyntaxParser::TextType type) const = 0;
};
