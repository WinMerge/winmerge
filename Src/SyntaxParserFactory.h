#pragma once

#include "ISyntaxParser.h"
#include "CrystalLineParserAdapter.h"
#include "TreeSitterParserAdapter.h"
#include <memory>

/**
 * @brief Factory for creating syntax parser instances.
 *
 * This factory provides a unified interface for creating both line-based
 * CrystalEdit parsers and Tree-sitter parsers, abstracting the choice
 * between parser types from calling code.
 */
class SyntaxParserFactory
{
public:
	/**
	 * @brief Create a parser for a specific file extension.
	 * @param sFileExt File extension (e.g., ".cpp", ".py", ".fs").
	 * @param bPreferTreeSitter If true, attempt to use Tree-sitter if available.
	 * @return Unique pointer to ISyntaxParser, or nullptr if no parser is available.
	 *
	 * The factory first attempts to create a Tree-sitter parser if bPreferTreeSitter
	 * is true and a grammar is available. If that fails, it falls back to a
	 * line-based CrystalEdit parser based on the file extension.
	 */
	static std::unique_ptr<ISyntaxParser> CreateParser(
		const std::wstring& sFileExt,
		bool bPreferTreeSitter = true);

	/**
	 * @brief Create a line-based CrystalEdit parser for a specific text type.
	 * @param textType The language/format type to parse.
	 * @return Unique pointer to ISyntaxParser.
	 */
	static std::unique_ptr<ISyntaxParser> CreateLineBasedParser(
		CrystalLineParser::TextType textType);

	/**
	 * @brief Create a Tree-sitter parser for a specific language.
	 * @param pLanguage Pointer to a loaded CTreeSitterLanguage.
	 * @return Unique pointer to ISyntaxParser, or nullptr if language is null.
	 */
	static std::unique_ptr<ISyntaxParser> CreateTreeSitterParser(
		const CTreeSitterLanguage* pLanguage);

private:
	SyntaxParserFactory() = delete; // Static factory class
};
