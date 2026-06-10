#pragma once

#include "ISyntaxParserFactory.h"
#include <memory>
#include <vector>

namespace LangServices
{

/**
 * @brief Central registry for ISyntaxParserFactory instances.
 *
 * Factories are registered at application startup and queried whenever a
 * parser needs to be created.  The registry selects the best factory by
 * comparing priority values returned by ISyntaxParserFactory::GetPriority()
 * and ::GetPriorityFromContent().
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
	 * @brief Create a parser for an explicitly specified LanguageId.
	 * @param type The language / file-format type.
	 * @return Parser from the highest-priority factory that returns non-null
	 *         for this type, or nullptr if none can handle it.
	 *
	 * Factories are tried in descending priority order (using GetPriority with
	 * an empty extension as the ordering key, then falling back to registration
	 * order for ties).
	 */
	std::shared_ptr<ISyntaxParser> CreateParser(LanguageId type) const;

private:
	SyntaxParserRegistry() = default;
	~SyntaxParserRegistry() = default;
	SyntaxParserRegistry(const SyntaxParserRegistry&) = delete;
	SyntaxParserRegistry& operator=(const SyntaxParserRegistry&) = delete;

	std::vector<ISyntaxParserFactory*> m_factories;
};

}
