#pragma once

#include "ISyntaxParserFactory.h"
#include <memory>
#include <vector>

/**
 * @brief Central registry for ISyntaxParserFactory instances.
 *
 * Factories are registered at application startup and queried whenever a
 * parser needs to be created.  The registry selects the best factory by
 * comparing priority values returned by ISyntaxParserFactory::GetPriority()
 * and ::GetPriorityFromContent().
 *
 * Typical usage:
 * @code
 *   auto& reg = SyntaxParserRegistry::GetInstance();
 *   reg.RegisterFactory(new CrystalLineParserFactory()); // priority=100
 *   reg.RegisterFactory(new TreeSitterParserFactory());  // priority=200 for supported langs
 *
 *   // Later:
 *   auto parser = reg.CreateParser(L"cpp");
 * @endcode
 *
 * Dependency constraints: this file MUST NOT include CCrystalTextView.
 */
class SyntaxParserRegistry
{
public:
	/**
	 * @brief Get the singleton instance.
	 */
	static SyntaxParserRegistry& GetInstance();

	/**
	 * @brief Register a factory.
	 * @param factory Raw pointer. The registry does NOT take ownership;
	 *                the caller is responsible for the lifetime of the factory.
	 *                Typically factories are static or application-lifetime objects.
	 */
	void RegisterFactory(ISyntaxParserFactory* factory);

	/**
	 * @brief Unregister a previously registered factory.
	 * @param factory Pointer to remove. No-op if not found.
	 */
	void UnregisterFactory(ISyntaxParserFactory* factory);

	/**
	 * @brief Create a parser selected by file extension.
	 * @param pszExt File extension without leading dot (e.g., L"cpp").
	 *              May be nullptr for extension-less files.
	 * @return Best-matching ISyntaxParser, or nullptr if no factory is registered.
	 *
	 * The factory with the highest GetPriority(pszExt) > 0 is chosen.
	 */
	std::unique_ptr<ISyntaxParser> CreateParser(
		const tchar_t* pszExt) const;

	/**
	 * @brief Create a parser selected by file content alone.
	 * @param pszContent Pointer to the first bytes/chars of the file.
	 * @param len Number of characters available in pszContent.
	 * @return Best-matching ISyntaxParser, or nullptr if no factory matches.
	 */
	std::unique_ptr<ISyntaxParser> CreateParserFromContent(
		const tchar_t* pszContent, size_t len) const;

	/**
	 * @brief Create a parser selected by extension AND content.
	 * @param pszExt File extension (may be nullptr).
	 * @param pszContent First chars of the file (may be nullptr).
	 * @param len Length of pszContent.
	 * @return Best-matching ISyntaxParser, or nullptr if no factory matches.
	 *
	 * For each factory the effective priority is:
	 *   max(GetPriority(pszExt), GetPriorityFromContent(pszContent, len))
	 * The factory with the highest effective priority > 0 wins.
	 */
	std::unique_ptr<ISyntaxParser> CreateParser(
		const tchar_t* pszExt,
		const tchar_t* pszContent, size_t len) const;

	/**
	 * @brief Create a parser for an explicitly specified TextType.
	 * @param type The language / file-format type.
	 * @return Parser from the highest-priority factory that returns non-null
	 *         for this type, or nullptr if none can handle it.
	 *
	 * Factories are tried in descending priority order (using GetPriority with
	 * an empty extension as the ordering key, then falling back to registration
	 * order for ties).
	 */
	std::unique_ptr<ISyntaxParser> CreateParser(
		ISyntaxParser::TextType type) const;

private:
	SyntaxParserRegistry() = default;
	~SyntaxParserRegistry() = default;
	SyntaxParserRegistry(const SyntaxParserRegistry&) = delete;
	SyntaxParserRegistry& operator=(const SyntaxParserRegistry&) = delete;

	std::vector<ISyntaxParserFactory*> m_factories;
};
