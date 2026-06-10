#pragma once

#include "ISyntaxParser.h"
#include <memory>

namespace LangServices
{

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

	virtual bool IsSupported(LanguageId type) const = 0;

	/**
	 * @brief Create a parser instance for the given LanguageId.
	 * @param type The language / file-format type.
	 * @return A newly created ISyntaxParser, or nullptr if this factory
	 *         does not support the requested type.
	 */
	virtual std::shared_ptr<ISyntaxParser> Create(LanguageId type) const = 0;
};

}