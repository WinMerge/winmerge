#include "pch.h"
#include "SyntaxParserRegistry.h"
#include <algorithm>

namespace LangServices
{

SyntaxParserRegistry& SyntaxParserRegistry::GetInstance()
{
	static SyntaxParserRegistry instance;
	return instance;
}

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

std::shared_ptr<ISyntaxParser> SyntaxParserRegistry::CreateParser(LanguageId type) const
{
	for (ISyntaxParserFactory* f : m_factories)
	{
		if (f == nullptr)
			continue;
		if (!f->IsSupported(type))
			continue;
		auto parser = f->Create(type);
		if (parser != nullptr)
			return parser;
	}

	return nullptr;
}

}
