#include "pch.h"
#include <Windows.h>
#include "SyntaxParserFactory.h"

/**
 * @brief Create a parser for a specific file extension.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserFactory::CreateParser(
	const std::wstring& sFileExt,
	bool bPreferTreeSitter)
{
	// Try Tree-sitter first if preferred
	if (bPreferTreeSitter)
	{
		auto pLanguage = TreeSitterRegistry::Instance().GetLanguageForExt(sFileExt);
		if (pLanguage != nullptr)
		{
			return std::make_unique<TreeSitterParserAdapter>(pLanguage);
		}
	}

	// Fall back to line-based parser
	// Map file extension to LanguageId
	LanguageId textType = LanguageId::SRC_PLAIN;

	// Find matching text definition by extension
	for (size_t i = 0; i < CrystalLineParser::m_SourceDefs.size(); i++)
	{
		auto& def = CrystalLineParser::m_SourceDefs[i];
		if (def.exts != nullptr)
		{
			// Simple extension matching (could be improved with proper tokenization)
			std::wstring exts(def.exts);
			if (exts.find(sFileExt) != std::wstring::npos)
			{
				textType = def.type;
				break;
			}
		}
	}

	return std::make_unique<CrystalLineSyntaxParser>(textType);
}

/**
 * @brief Create a line-based CrystalEdit parser for a specific text type.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserFactory::CreateLineBasedParser(
	LanguageId textType)
{
	return std::make_unique<CrystalLineSyntaxParser>(textType);
}

/**
 * @brief Create a Tree-sitter parser for a specific language.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserFactory::CreateTreeSitterParser(
	const CTreeSitterLanguage* pLanguage)
{
	if (pLanguage == nullptr)
		return nullptr;

	return std::make_unique<TreeSitterParserAdapter>(pLanguage);
}
