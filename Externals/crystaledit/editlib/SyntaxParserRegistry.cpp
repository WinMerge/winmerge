#include "pch.h"
#include "SyntaxParserRegistry.h"
#include "parsers/crystallineparser.h"
#include <algorithm>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

SyntaxParserRegistry& SyntaxParserRegistry::GetInstance()
{
	static SyntaxParserRegistry instance;
	return instance;
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void SyntaxParserRegistry::RegisterFactory(ISyntaxParserFactory* factory)
{
	if (factory == nullptr)
		return;
	// Avoid duplicate registrations
	auto it = std::find(m_factories.begin(), m_factories.end(), factory);
	if (it == m_factories.end())
		m_factories.push_back(factory);
}

void SyntaxParserRegistry::UnregisterFactory(ISyntaxParserFactory* factory)
{
	auto it = std::find(m_factories.begin(), m_factories.end(), factory);
	if (it != m_factories.end())
		m_factories.erase(it);
}

// ---------------------------------------------------------------------------
// Internal helper: pick the best factory given a priority function
// ---------------------------------------------------------------------------

namespace {

/**
 * @brief Select the factory with the highest priority > 0, then call Create(type).
 *
 * @tparam PriorityFn  Callable: (ISyntaxParserFactory*) -> int
 */
template <typename PriorityFn>
std::unique_ptr<ISyntaxParser> SelectAndCreate(
	const std::vector<ISyntaxParserFactory*>& factories,
	ISyntaxParser::TextType type,
	PriorityFn priorityFn)
{
	ISyntaxParserFactory* bestFactory = nullptr;
	int bestPriority = 0; // Only consider factories with priority > 0

	for (ISyntaxParserFactory* f : factories)
	{
		if (f == nullptr)
			continue;
		int priority = priorityFn(f);
		if (priority > bestPriority)
		{
			bestPriority = priority;
			bestFactory  = f;
		}
	}

	if (bestFactory != nullptr)
		return bestFactory->Create(type);

	return nullptr;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// CreateParser overloads
// ---------------------------------------------------------------------------

/**
 * @brief Create a parser selected by file extension.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserRegistry::CreateParser(
	const tchar_t* pszExt) const
{
	// Resolve the TextType from the file extension using the CrystalLine table.
	// If no match is found, TextType::Plain is used as fallback.
	ISyntaxParser::TextType type = ISyntaxParser::TextType::Plain;
	if (pszExt != nullptr && *pszExt != _T('\0'))
	{
		CrystalLineParser::TextDefinition* pDef =
			CrystalLineParser::GetTextType(pszExt);
		if (pDef != nullptr)
			type = pDef->type;  // using TextType = ISyntaxParser::TextType, so compatible
	}

	// Select the best factory by extension priority, then create for the resolved type.
	ISyntaxParserFactory* bestFactory = nullptr;
	int bestPriority = 0;

	for (ISyntaxParserFactory* f : m_factories)
	{
		if (f == nullptr)
			continue;
		int priority = f->GetPriority(pszExt);
		if (priority > bestPriority)
		{
			bestPriority = priority;
			bestFactory  = f;
		}
	}

	if (bestFactory != nullptr)
		return bestFactory->Create(type);

	return nullptr;
}

/**
 * @brief Create a parser selected by file content alone.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserRegistry::CreateParserFromContent(
	const tchar_t* pszContent, size_t len) const
{
	return SelectAndCreate(
		m_factories,
		ISyntaxParser::TextType::Plain,
		[&](ISyntaxParserFactory* f) -> int
		{
			return f->GetPriorityFromContent(pszContent, len);
		});
}

/**
 * @brief Create a parser selected by extension AND content.
 *
 * Effective priority = max(GetPriority(ext), GetPriorityFromContent(content))
 */
std::unique_ptr<ISyntaxParser> SyntaxParserRegistry::CreateParser(
	const tchar_t* pszExt,
	const tchar_t* pszContent, size_t len) const
{
	ISyntaxParserFactory* bestFactory = nullptr;
	int bestPriority = 0;

	for (ISyntaxParserFactory* f : m_factories)
	{
		if (f == nullptr)
			continue;

		int extPriority     = f->GetPriority(pszExt);
		int contentPriority = (pszContent != nullptr && len > 0)
		                      ? f->GetPriorityFromContent(pszContent, len)
		                      : 0;
		int priority = (extPriority > contentPriority) ? extPriority : contentPriority;

		if (priority > bestPriority)
		{
			bestPriority = priority;
			bestFactory  = f;
		}
	}

	if (bestFactory != nullptr)
		return bestFactory->Create(ISyntaxParser::TextType::Plain);

	return nullptr;
}

/**
 * @brief Create a parser for an explicitly specified TextType.
 *
 * Factories are sorted by their generic priority (GetPriority with empty
 * extension).  The highest-priority factory whose Create() returns non-null
 * is used.  This allows e.g. TreeSitterParserFactory (priority=200) to win
 * over CrystalLineParserFactory (priority=100) for languages it supports,
 * while falling back gracefully for unsupported ones.
 */
std::unique_ptr<ISyntaxParser> SyntaxParserRegistry::CreateParser(
	ISyntaxParser::TextType type) const
{
	// Build a sorted list of factories by their generic priority (descending).
	// Use an empty-string extension as the sorting key so factories that
	// prefer explicit type dispatch (returning e.g. 50 for unknown ext) still
	// participate in ordering.
	std::vector<ISyntaxParserFactory*> sorted = m_factories;
	std::stable_sort(sorted.begin(), sorted.end(),
		[](ISyntaxParserFactory* a, ISyntaxParserFactory* b) -> bool
		{
			// Higher priority first
			return a->GetPriority(_T("")) > b->GetPriority(_T(""));
		});

	for (ISyntaxParserFactory* f : sorted)
	{
		if (f == nullptr)
			continue;
		auto parser = f->Create(type);
		if (parser != nullptr)
			return parser;
	}

	return nullptr;
}
