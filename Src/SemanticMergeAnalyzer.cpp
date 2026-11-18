/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  SemanticMergeAnalyzer.cpp
 *
 * @brief Implementation of the tree-sitter based semantic merge analyzer.
 */

#include "pch.h"
#include "SemanticMergeAnalyzer.h"
#include "MergeApp.h"
#include "unicoder.h"
#include <tchar.h>
#include <algorithm>
#include <unordered_map>

namespace SemanticMerge
{

LanguageTraits TraitsForLanguage(const String& languageName)
{
	String name = languageName;
	std::transform(name.begin(), name.end(), name.begin(),
		[](tchar_t ch) { return static_cast<tchar_t>(_totlower(ch)); });
	strutils::replace(name, _T("-"), _T("_"));

	LanguageTraits traits;
	const bool cFamily =
		name == _T("c") || name == _T("cpp") || name == _T("c_sharp") ||
		name == _T("java") || name == _T("javascript") || name == _T("typescript") ||
		name == _T("tsx") || name == _T("flow") || name == _T("php") || name == _T("php_only");
	const bool hashCommentFamily =
		name == _T("python") || name == _T("bash") || name == _T("ruby");
	const bool slashCommentOnly =
		name == _T("go") || name == _T("rust") || name == _T("fsharp") ||
		name == _T("fsharp_signature");

	if (cFamily)
	{
		traits.lineCommentPrefixes = { _T("//") };
		traits.cStyleParameters = true;
		traits.identifierRename = true;
		// Single quotes delimit char literals in C/C++/C#/Java, so only the
		// script-flavored members of the family get them as string quotes.
		if (name == _T("javascript") || name == _T("typescript") ||
			name == _T("tsx") || name == _T("flow") ||
			name == _T("php") || name == _T("php_only"))
			traits.stringQuoteChars = _T("\"'");
		else
			traits.stringQuoteChars = _T("\"");
		if (name == _T("php") || name == _T("php_only"))
			traits.lineCommentPrefixes.push_back(_T("#"));
	}
	else if (hashCommentFamily)
	{
		traits.lineCommentPrefixes = { _T("#") };
		traits.stringQuoteChars = _T("\"'");
		traits.identifierRename = true;
	}
	else if (slashCommentOnly)
	{
		traits.lineCommentPrefixes = { _T("//") };
		traits.stringQuoteChars = _T("\"");
		traits.identifierRename = true;
	}
	traits.preprocessorLines = name == _T("c") || name == _T("cpp") || name == _T("c_sharp");
	traits.layoutSensitive = name == _T("python") || name == _T("fsharp") || name == _T("fsharp_signature");
	// Markup and data languages (json, html, css, xml, dtd) and unknown
	// languages keep the defaults: every text heuristic disabled.

	return traits;
}

// Helpers below have external linkage; the subset declared in the header is
// exposed for unit testing, the rest is internal to the analyzer.

bool GetDefinitionText(const ITextSource& src, const TagRange& tag, String& text)
{
	if (tag.startLine < 0 || tag.endLine < tag.startLine || tag.endLine >= src.GetLineCount())
		return false;

	if (tag.startChar < 0 || tag.startChar > src.GetLineLength(tag.startLine) ||
		tag.endChar < 0 || tag.endChar > src.GetLineLength(tag.endLine))
		return false;

	src.GetTextRange(tag.startLine, tag.startChar, tag.endLine, tag.endChar, text);
	return true;
}

/**
 * @brief What the first and the last line of a definition hold outside its
 * range, white space removed. The tag must be valid for the text.
 */
String GetDefinitionMargins(const ITextSource& src, const TagRange& tag)
{
	String before, after;
	if (tag.startChar > 0)
		src.GetTextRange(tag.startLine, 0, tag.startLine, tag.startChar, before);
	const int endLength = src.GetLineLength(tag.endLine);
	if (tag.endChar < endLength)
		src.GetTextRange(tag.endLine, tag.endChar, tag.endLine, endLength, after);
	String margins = before + _T('\n') + after;
	margins.erase(std::remove_if(margins.begin(), margins.end(),
		[](tchar_t ch) { return ch == _T(' ') || ch == _T('\t') || ch == _T('\r'); }), margins.end());
	return margins;
}

bool GetLineRangeText(const ITextSource& src, int startLine, int endLine, String& text)
{
	if (startLine < 0 || endLine < startLine || endLine >= src.GetLineCount())
		return false;

	const int endChar = src.GetLineLength(endLine);
	src.GetTextRange(startLine, 0, endLine, endChar, text);
	text += src.GetLineEol(endLine);
	return true;
}

String NormalizeSemanticText(String text)
{
	strutils::replace(text, _T("\r"), _T(""));
	while (!text.empty() && text.back() == _T('\n'))
		text.pop_back();
	return text;
}

String TrimWhitespace(const String& text)
{
	size_t start = 0;
	while (start < text.size() && _istspace(text[start]))
		++start;

	size_t end = text.size();
	while (end > start && _istspace(text[end - 1]))
		--end;

	return text.substr(start, end - start);
}

bool IsIdentifierChar(tchar_t ch)
{
	return _istalnum(ch) || ch == _T('_');
}

bool ExtractTrailingIdentifier(const String& text, String& identifier)
{
	identifier.clear();
	size_t end = text.size();
	while (end > 0 && (_istspace(text[end - 1]) || text[end - 1] == _T('&') || text[end - 1] == _T('*')))
		--end;
	if (end == 0 || !IsIdentifierChar(text[end - 1]) || _istdigit(text[end - 1]))
		return false;

	size_t start = end - 1;
	while (start > 0 && IsIdentifierChar(text[start - 1]))
		--start;
	identifier = text.substr(start, end - start);
	return true;
}

bool ExtractFunctionParameterNames(const String& text, std::vector<String>& parameterNames)
{
	parameterNames.clear();
	const size_t bodyStart = text.find(_T('{'));
	const size_t openParen = text.find(_T('('));
	if (bodyStart == String::npos || openParen == String::npos || openParen > bodyStart)
		return false;

	int parenDepth = 0;
	size_t closeParen = String::npos;
	for (size_t i = openParen; i < bodyStart; ++i)
	{
		if (text[i] == _T('('))
			++parenDepth;
		else if (text[i] == _T(')'))
		{
			--parenDepth;
			if (parenDepth == 0)
			{
				closeParen = i;
				break;
			}
		}
	}
	if (closeParen == String::npos)
		return false;

	const String parametersText = text.substr(openParen + 1, closeParen - openParen - 1);
	std::vector<String> parameterDecls;
	String current;
	int nestedParens = 0;
	int nestedAngles = 0;
	int nestedBraces = 0;
	int nestedBrackets = 0;
	for (size_t i = 0; i < parametersText.size(); ++i)
	{
		const tchar_t ch = parametersText[i];
		switch (ch)
		{
		case _T('('): ++nestedParens; break;
		case _T(')'): if (nestedParens > 0) --nestedParens; break;
		case _T('<'): ++nestedAngles; break;
		case _T('>'): if (nestedAngles > 0) --nestedAngles; break;
		case _T('{'): ++nestedBraces; break;
		case _T('}'): if (nestedBraces > 0) --nestedBraces; break;
		case _T('['): ++nestedBrackets; break;
		case _T(']'): if (nestedBrackets > 0) --nestedBrackets; break;
		case _T(','):
			if (nestedParens == 0 && nestedAngles == 0 && nestedBraces == 0 && nestedBrackets == 0)
			{
				parameterDecls.push_back(current);
				current.clear();
				continue;
			}
			break;
		}
		current += ch;
	}
	parameterDecls.push_back(current);

	for (String parameterDecl : parameterDecls)
	{
		parameterDecl = TrimWhitespace(parameterDecl);
		if (parameterDecl.empty() || parameterDecl == _T("void"))
			continue;

		int defaultParens = 0;
		int defaultAngles = 0;
		int defaultBraces = 0;
		int defaultBrackets = 0;
		size_t defaultValuePos = String::npos;
		for (size_t i = 0; i < parameterDecl.size(); ++i)
		{
			const tchar_t ch = parameterDecl[i];
			switch (ch)
			{
			case _T('('): ++defaultParens; break;
			case _T(')'): if (defaultParens > 0) --defaultParens; break;
			case _T('<'): ++defaultAngles; break;
			case _T('>'): if (defaultAngles > 0) --defaultAngles; break;
			case _T('{'): ++defaultBraces; break;
			case _T('}'): if (defaultBraces > 0) --defaultBraces; break;
			case _T('['): ++defaultBrackets; break;
			case _T(']'): if (defaultBrackets > 0) --defaultBrackets; break;
			case _T('='):
				if (defaultParens == 0 && defaultAngles == 0 && defaultBraces == 0 && defaultBrackets == 0)
				{
					defaultValuePos = i;
					i = parameterDecl.size();
				}
				break;
			}
		}

		const String declarationWithoutDefault = TrimWhitespace(parameterDecl.substr(0, defaultValuePos));
		String parameterName;
		if (!ExtractTrailingIdentifier(declarationWithoutDefault, parameterName))
			return false;
		parameterNames.push_back(parameterName);
	}

	return true;
}

std::vector<size_t> FindWholeWordOccurrences(const String& text, const String& token)
{
	std::vector<size_t> positions;
	if (token.empty())
		return positions;

	for (size_t pos = text.find(token); pos != String::npos; pos = text.find(token, pos + 1))
	{
		const bool leftBoundary = (pos == 0) || !IsIdentifierChar(text[pos - 1]);
		const size_t end = pos + token.size();
		const bool rightBoundary = (end >= text.size()) || !IsIdentifierChar(text[end]);
		if (leftBoundary && rightBoundary)
			positions.push_back(pos);
	}

	return positions;
}

bool ReplaceWholeWordOccurrences(const String& sourceText, const String& oldToken, const String& newToken,
	size_t expectedCount, String& resultText)
{
	const std::vector<size_t> positions = FindWholeWordOccurrences(sourceText, oldToken);
	if (positions.size() != expectedCount)
		return false;

	resultText = sourceText;
	for (auto it = positions.rbegin(); it != positions.rend(); ++it)
		resultText.replace(*it, oldToken.size(), newToken);
	return true;
}

bool ExtractSingleStringLiteralChange(const String& unchangedText, const String& changedText,
	const String& quoteChars, String& oldLiteralWithQuotes, String& newLiteralWithQuotes)
{
	if (quoteChars.empty())
		return false;

	auto collectStringLiterals = [&quoteChars](const String& text)
	{
		std::vector<String> literals;
		for (size_t i = 0; i < text.size(); ++i)
		{
			if (quoteChars.find(text[i]) == String::npos)
				continue;

			const tchar_t quote = text[i];
			size_t j = i + 1;
			bool escaped = false;
			for (; j < text.size(); ++j)
			{
				if (escaped)
				{
					escaped = false;
					continue;
				}
				if (text[j] == _T('\\'))
				{
					escaped = true;
					continue;
				}
				if (text[j] == quote)
					break;
			}
			if (j >= text.size() || text[j] != quote)
				continue;

			literals.push_back(text.substr(i, j - i + 1));
			i = j;
		}
		return literals;
	};

	const std::vector<String> unchangedLiterals = collectStringLiterals(unchangedText);
	const std::vector<String> changedLiterals = collectStringLiterals(changedText);
	if (unchangedLiterals.empty() || changedLiterals.empty())
		return false;

	std::unordered_map<String, int> unchangedCounts;
	std::unordered_map<String, int> changedCounts;
	for (const auto& literal : unchangedLiterals)
		++unchangedCounts[literal];
	for (const auto& literal : changedLiterals)
		++changedCounts[literal];

	std::vector<String> removed;
	std::vector<String> added;
	for (const auto& entry : unchangedCounts)
	{
		const int delta = entry.second - changedCounts[entry.first];
		for (int i = 0; i < delta; ++i)
			removed.push_back(entry.first);
	}
	for (const auto& entry : changedCounts)
	{
		const int delta = entry.second - unchangedCounts[entry.first];
		for (int i = 0; i < delta; ++i)
			added.push_back(entry.first);
	}

	if (removed.size() != 1 || added.size() != 1)
		return false;

	// the literal must be the only change: putting the old one back gives the unchanged text
	String revertedText;
	if (!ReplaceUniqueStringLiteral(changedText, added[0], removed[0], revertedText) || revertedText != unchangedText)
		return false;

	oldLiteralWithQuotes = removed[0];
	newLiteralWithQuotes = added[0];
	return true;
}

bool ReplaceUniqueStringLiteral(const String& sourceText, const String& oldLiteralWithQuotes,
	const String& newLiteralWithQuotes, String& resultText)
{
	resultText = sourceText;
	const size_t first = resultText.find(oldLiteralWithQuotes);
	if (first == String::npos)
		return false;
	if (resultText.find(oldLiteralWithQuotes, first + oldLiteralWithQuotes.size()) != String::npos)
		return false;

	resultText.replace(first, oldLiteralWithQuotes.size(), newLiteralWithQuotes);
	return true;
}

bool ExtractSingleParameterRenameChange(const String& unchangedText, const String& changedText,
	String& oldParameterName, String& newParameterName)
{
	std::vector<String> unchangedParameters;
	std::vector<String> changedParameters;
	if (!ExtractFunctionParameterNames(unchangedText, unchangedParameters) ||
		!ExtractFunctionParameterNames(changedText, changedParameters) ||
		unchangedParameters.size() != changedParameters.size())
	{
		return false;
	}

	int changedParameterIndex = -1;
	for (size_t i = 0; i < unchangedParameters.size(); ++i)
	{
		if (unchangedParameters[i] == changedParameters[i])
			continue;
		if (changedParameterIndex != -1)
			return false;
		changedParameterIndex = static_cast<int>(i);
	}
	if (changedParameterIndex == -1)
		return false;

	oldParameterName = unchangedParameters[changedParameterIndex];
	newParameterName = changedParameters[changedParameterIndex];
	if (oldParameterName.empty() || newParameterName.empty())
		return false;

	if (FindWholeWordOccurrences(unchangedText, oldParameterName).size() != 2 ||
		FindWholeWordOccurrences(changedText, newParameterName).size() != 2)
	{
		return false;
	}

	String revertedText;
	if (!ReplaceWholeWordOccurrences(changedText, newParameterName, oldParameterName, 2, revertedText))
		return false;

	return revertedText == unchangedText;
}

bool ExtractSingleIdentifierRenameChange(const String& unchangedText, const String& changedText,
	String& oldIdentifierName, String& newIdentifierName, size_t& occurrenceCount)
{
	oldIdentifierName.clear();
	newIdentifierName.clear();
	occurrenceCount = 0;

	auto collectIdentifiers = [](const String& text)
	{
		std::vector<String> identifiers;
		for (size_t i = 0; i < text.size(); )
		{
			if (!IsIdentifierChar(text[i]) || _istdigit(text[i]))
			{
				++i;
				continue;
			}

			size_t end = i + 1;
			while (end < text.size() && IsIdentifierChar(text[end]))
				++end;
			identifiers.push_back(text.substr(i, end - i));
			i = end;
		}
		return identifiers;
	};

	const std::vector<String> unchangedIdentifiers = collectIdentifiers(unchangedText);
	const std::vector<String> changedIdentifiers = collectIdentifiers(changedText);
	if (unchangedIdentifiers.empty() || changedIdentifiers.empty())
		return false;

	std::unordered_map<String, int> unchangedCounts;
	std::unordered_map<String, int> changedCounts;
	for (const auto& identifier : unchangedIdentifiers)
		++unchangedCounts[identifier];
	for (const auto& identifier : changedIdentifiers)
		++changedCounts[identifier];

	// A rename replaces every occurrence, so exactly one identifier name must
	// disappear completely and exactly one new name must appear, with equal
	// occurrence counts.
	std::vector<String> removed;
	std::vector<String> added;
	for (const auto& entry : unchangedCounts)
	{
		if (changedCounts.find(entry.first) == changedCounts.end())
			removed.push_back(entry.first);
	}
	for (const auto& entry : changedCounts)
	{
		if (unchangedCounts.find(entry.first) == unchangedCounts.end())
			added.push_back(entry.first);
	}

	if (removed.size() != 1 || added.size() != 1)
		return false;

	oldIdentifierName = removed[0];
	newIdentifierName = added[0];
	if (oldIdentifierName.empty() || newIdentifierName.empty() || oldIdentifierName == newIdentifierName)
		return false;
	if (unchangedCounts[oldIdentifierName] != changedCounts[newIdentifierName])
		return false;

	const std::vector<size_t> oldPositions = FindWholeWordOccurrences(unchangedText, oldIdentifierName);
	const std::vector<size_t> newPositions = FindWholeWordOccurrences(changedText, newIdentifierName);
	if (oldPositions.size() != newPositions.size() || oldPositions.size() < 2 || oldPositions.size() > 4)
		return false;

	String revertedText;
	if (!ReplaceWholeWordOccurrences(changedText, newIdentifierName, oldIdentifierName, newPositions.size(), revertedText))
		return false;
	if (revertedText != unchangedText)
		return false;

	occurrenceCount = newPositions.size();
	return true;
}

bool TryDetectLineCommentPrefix(const String& line, const std::vector<String>& tokens, String& prefix, String& uncommented)
{
	size_t indent = 0;
	while (indent < line.size() && _istspace(line[indent]) && line[indent] != _T('\n') && line[indent] != _T('\r'))
		++indent;

	for (const String& token : tokens)
	{
		const size_t tokenLen = token.size();
		if (tokenLen == 0 || indent + tokenLen > line.size())
			continue;
		if (line.compare(indent, tokenLen, token) != 0)
			continue;

		size_t afterToken = indent + tokenLen;
		if (afterToken < line.size() && line[afterToken] == _T(' '))
			++afterToken;

		prefix = line.substr(0, afterToken);
		uncommented = line.substr(afterToken);
		return true;
	}

	return false;
}

bool TryParseLineCommentedBlock(const String& text, const std::vector<String>& lineCommentPrefixes, CommentedBlockInfo& info)
{
	info = {};
	if (lineCommentPrefixes.empty())
		return false;
	String detectedPrefix;
	String decommented;
	bool foundNonEmptyLine = false;

	size_t pos = 0;
	while (pos <= text.size())
	{
		size_t end = text.find(_T('\n'), pos);
		const bool hasNewline = (end != String::npos);
		String line = hasNewline ? text.substr(pos, end - pos) : text.substr(pos);
		if (!line.empty() && line.back() == _T('\r'))
			line.pop_back();

		bool blank = true;
		for (tchar_t ch : line)
		{
			if (!_istspace(ch))
			{
				blank = false;
				break;
			}
		}

		if (blank)
		{
			decommented += _T("\n");
		}
		else
		{
			String prefix;
			String uncommented;
			if (!TryDetectLineCommentPrefix(line, lineCommentPrefixes, prefix, uncommented))
				return false;
			if (!foundNonEmptyLine)
			{
				detectedPrefix = prefix;
				foundNonEmptyLine = true;
			}
			decommented += uncommented;
			decommented += _T("\n");
		}

		if (!hasNewline)
			break;
		pos = end + 1;
	}

	if (!foundNonEmptyLine)
		return false;

	info.linePrefix = detectedPrefix;
	info.decommentedText = decommented;
	return true;
}

String RenderLineCommentedBlock(const String& text, const String& prefix)
{
	String result;
	size_t pos = 0;
	while (pos <= text.size())
	{
		size_t end = text.find(_T('\n'), pos);
		const bool hasNewline = (end != String::npos);
		String line = hasNewline ? text.substr(pos, end - pos) : text.substr(pos);
		if (!line.empty() && line.back() == _T('\r'))
			line.pop_back();

		result += prefix;
		result += line;
		if (hasNewline || pos < text.size())
			result += _T("\n");

		if (!hasNewline)
			break;
		pos = end + 1;
	}
	return result;
}

int GetOverlapLength(const TagRange& tag, int startLine, int endLine)
{
	const int overlapStart = (std::max)(tag.startLine, startLine);
	const int overlapEnd = (std::min)(tag.endLine, endLine);
	return overlapEnd >= overlapStart ? (overlapEnd - overlapStart + 1) : 0;
}

bool TryFindBestTagForDiff(const std::vector<TagRange>& tags, int startLine, int endLine, TagRange& bestTag)
{
	if (tags.empty())
		return false;

	std::unordered_map<std::string, int> overlapByName;
	for (const auto& tag : tags)
	{
		const int overlap = GetOverlapLength(tag, startLine, endLine);
		if (overlap > 0)
			overlapByName[tag.name] += overlap;
	}

	if (overlapByName.empty())
		return false;

	std::string bestName;
	int bestNameScore = -1;
	for (const auto& entry : overlapByName)
	{
		if (entry.second > bestNameScore)
		{
			bestName = entry.first;
			bestNameScore = entry.second;
		}
	}

	bool found = false;
	int bestOverlap = -1;
	for (const auto& tag : tags)
	{
		if (tag.name != bestName)
			continue;
		const int overlap = GetOverlapLength(tag, startLine, endLine);
		if (overlap > bestOverlap)
		{
			bestTag = tag;
			bestOverlap = overlap;
			found = true;
		}
	}

	return found;
}

bool TryFindUniqueTagByName(const std::vector<TagRange>& tags, const std::string& name, TagRange& found)
{
	bool any = false;
	for (const auto& tag : tags)
	{
		if (tag.name != name)
			continue;
		if (any)
			return false;
		found = tag;
		any = true;
	}
	return any;
}

Analyzer::Analyzer(const PaneData* panes, int nPanes, const LanguageTraits& traits, const MergeRoles& roles)
	: m_panes(panes)
	, m_nPanes(nPanes)
	, m_traits(traits)
	, m_roles(roles)
{
}

bool Analyzer::AllPanesHaveLanguage() const
{
	for (int pane = 0; pane < m_nPanes; ++pane)
	{
		if (m_panes[pane].pText == nullptr || !m_panes[pane].hasLanguage)
			return false;
	}
	return m_nPanes > 0;
}

bool Analyzer::RolesAreValid() const
{
	const int b = m_roles.base, m = m_roles.mine, t = m_roles.theirs;
	return b >= 0 && b < m_nPanes && m >= 0 && m < m_nPanes && t >= 0 && t < m_nPanes &&
		b != m && b != t && m != t;
}

bool Analyzer::FinalizeStandardSuggestion(Suggestion& suggestion, String& message) const
{
	for (int pane = 0; pane < m_nPanes; ++pane)
	{
		suggestion.defs[pane].tag = suggestion.tags[pane];
		suggestion.defs[pane].pane = pane;
		if (!GetDefinitionText(*m_panes[pane].pText, suggestion.defs[pane].tag, suggestion.defs[pane].text))
		{
			message = _("Failed to extract the definition text for semantic merge.");
			return false;
		}
	}

	const int b = m_roles.base, m = m_roles.mine, t = m_roles.theirs;
	const String& baseText = suggestion.defs[b].text;
	const bool bMineChanged = suggestion.defs[m].text != baseText;
	const bool bTheirsChanged = suggestion.defs[t].text != baseText;
	suggestion.displayName = ucr::toTString(suggestion.tags[b].name);
	suggestion.unchangedPane = b;

	if (!bMineChanged && !bTheirsChanged)
	{
		message = _("The definition is the same in all panes.");
		return false;
	}

	// The resolution accounts for the changed side's whole definition lines,
	// so that side must have left the rest of those lines as the base has them
	// (a grammar can start the definition after a keyword such as "export")
	const String baseMargins = GetDefinitionMargins(*m_panes[b].pText, suggestion.defs[b].tag);
	auto changedOnlyTheDefinition = [&](int pane)
	{
		return GetDefinitionMargins(*m_panes[pane].pText, suggestion.defs[pane].tag) == baseMargins;
	};
	const String marginsMessage = _("The changing side also changed the lines of this definition outside the definition itself.");

	// R4: only one side changed the definition
	if (bMineChanged != bTheirsChanged)
	{
		if (!changedOnlyTheDefinition(bMineChanged ? m : t))
		{
			message = marginsMessage;
			return false;
		}
		suggestion.kind = SuggestionKind::DefinitionChange;
		suggestion.changedPane = bMineChanged ? m : t;
		suggestion.anchorPane = b;
		suggestion.replacementText = suggestion.defs[suggestion.changedPane].text;
		return true;
	}

	if (suggestion.defs[m].text == suggestion.defs[t].text)
	{
		message = _("Both sides changed the definition the same way.");
		return false;
	}

	// R5: both changed it; one side made one small change, which is
	// replayed onto the other side's version
	String replayMessage;
	for (const int localized : { m, t })
	{
		const int refactored = localized == m ? t : m;
		const String& localizedText = suggestion.defs[localized].text;
		String oldToken, newToken;
		size_t occurrences = 0;
		SuggestionKind kind;
		if (ExtractSingleStringLiteralChange(baseText, localizedText, m_traits.stringQuoteChars, oldToken, newToken))
			kind = SuggestionKind::StringLiteral;
		else if (m_traits.cStyleParameters && ExtractSingleParameterRenameChange(baseText, localizedText, oldToken, newToken))
		{
			kind = SuggestionKind::ParameterRename;
			occurrences = 2;
		}
		else if (m_traits.identifierRename &&
			ExtractSingleIdentifierRenameChange(baseText, localizedText, oldToken, newToken, occurrences))
			kind = SuggestionKind::IdentifierRename;
		else
			continue;

		if (!changedOnlyTheDefinition(localized))
		{
			replayMessage = marginsMessage;
			continue;
		}
		String replayed;
		const String& refactoredText = suggestion.defs[refactored].text;
		// a rename onto a version that already uses the new name would merge two names into one
		if (kind != SuggestionKind::StringLiteral && !FindWholeWordOccurrences(refactoredText, newToken).empty())
		{
			replayMessage = _("The other side's version of the definition already uses the new name.");
			continue;
		}
		const bool bReplayed = kind == SuggestionKind::StringLiteral ?
			ReplaceUniqueStringLiteral(refactoredText, oldToken, newToken, replayed) :
			ReplaceWholeWordOccurrences(refactoredText, oldToken, newToken, occurrences, replayed);
		if (!bReplayed)
		{
			replayMessage = _("The small change could not be mapped uniquely onto the other side's version of the definition.");
			continue;
		}
		suggestion.kind = kind;
		suggestion.changedPane = localized;
		suggestion.anchorPane = refactored;
		suggestion.replacementText = replayed;
		return true;
	}

	message = !replayMessage.empty() ? replayMessage :
		_("Both sides changed this definition, and neither change is one small change (a string literal, a parameter rename or an identifier rename).");
	return false;
}

bool Analyzer::TryBuildCommentedBlockSuggestion(int commentPane, const DiffInfo& diff, Suggestion& suggestion, String& message) const
{
	// The commenting side has no tag for the definition at the diff; base
	// and the editing side both have it, under the same name.
	const int b = m_roles.base;
	const int editor = commentPane == m_roles.mine ? m_roles.theirs : m_roles.mine;
	if (!TryFindBestTagForDiff(m_panes[b].tags, diff.dbegin, diff.dend, suggestion.tags[b]) ||
		!TryFindBestTagForDiff(m_panes[editor].tags, diff.dbegin, diff.dend, suggestion.tags[editor]))
	{
		message = _("No matching definition was found for the difference in base and the editing side.");
		return false;
	}
	if (suggestion.tags[b].name != suggestion.tags[editor].name)
	{
		message = _("The difference maps to different definitions across panes.");
		return false;
	}

	String baseText, editorText, commentedBlockText;
	if (!GetDefinitionText(*m_panes[b].pText, suggestion.tags[b], baseText) ||
		!GetDefinitionText(*m_panes[editor].pText, suggestion.tags[editor], editorText) ||
		!GetLineRangeText(*m_panes[commentPane].pText, diff.dbegin, diff.dend, commentedBlockText))
	{
		message = _("Failed to extract the definition text for semantic merge.");
		return false;
	}

	CommentedBlockInfo commentInfo;
	if (!TryParseLineCommentedBlock(commentedBlockText, m_traits.lineCommentPrefixes, commentInfo))
	{
		message = _("The block is not a line-commented definition.");
		return false;
	}
	if (NormalizeSemanticText(baseText) == NormalizeSemanticText(editorText))
	{
		message = _("The other side did not change the commented-out definition.");
		return false;
	}
	if (NormalizeSemanticText(commentInfo.decommentedText) != NormalizeSemanticText(baseText))
	{
		message = _("The commented-out block does not match the base version of the definition.");
		return false;
	}

	// The commenting side has no tag: its target is the commented block
	TagRange commentRange{};
	commentRange.name = suggestion.tags[b].name;
	commentRange.startLine = diff.dbegin;
	commentRange.startChar = 0;
	commentRange.endLine = diff.dend;
	commentRange.endChar = m_panes[commentPane].pText->GetLineLength(diff.dend);
	suggestion.tags[commentPane] = commentRange;

	String replacement = RenderLineCommentedBlock(editorText, commentInfo.linePrefix);
	while (!replacement.empty() && replacement.back() == _T('\n'))
		replacement.pop_back();

	for (int pane = 0; pane < m_nPanes; ++pane)
	{
		suggestion.defs[pane].tag = suggestion.tags[pane];
		suggestion.defs[pane].pane = pane;
	}
	suggestion.defs[b].text = baseText;
	suggestion.defs[editor].text = editorText;
	suggestion.defs[commentPane].text = commentedBlockText;

	suggestion.kind = SuggestionKind::CommentedOut;
	suggestion.unchangedPane = b;
	suggestion.changedPane = editor;
	suggestion.anchorPane = commentPane;
	suggestion.displayName = ucr::toTString(suggestion.tags[b].name);
	suggestion.replacementText = replacement;
	return true;
}

std::vector<std::string> Analyzer::CollectCandidateDefinitionNames(const DiffInfo& diff) const
{
	// Candidate definitions the diff may refer to, most relevant first: the
	// definition under the user's cursor (anchor), then each pane's own
	// mapping of the diff's line range.
	std::vector<std::string> names;
	auto addName = [&names](const std::string& name)
	{
		if (std::find(names.begin(), names.end(), name) == names.end())
			names.push_back(name);
	};

	if (diff.anchorPane >= 0 && diff.anchorPane < m_nPanes && diff.anchorLine >= 0)
	{
		TagRange anchorTag;
		if (TryFindBestTagForDiff(m_panes[diff.anchorPane].tags, diff.anchorLine, diff.anchorLine, anchorTag))
			addName(anchorTag.name);
	}

	for (int pane = 0; pane < m_nPanes; ++pane)
	{
		TagRange tag;
		if (TryFindBestTagForDiff(m_panes[pane].tags, diff.dbegin, diff.dend, tag))
			addName(tag.name);
	}
	return names;
}

bool Analyzer::TryReconcileTagsByName(const DiffInfo& diff, Suggestion& suggestion, String& message) const
{
	// The diff's line range may land in differently named definitions across
	// panes when surrounding code has shifted or moved. Try each candidate
	// definition that exists exactly once in every pane, wherever it is
	// located, and accept the first that yields a valid suggestion.
	String firstMessage;
	for (const std::string& name : CollectCandidateDefinitionNames(diff))
	{
		TagRange reconciled[3];
		bool foundInAllPanes = true;
		for (int pane = 0; pane < m_nPanes && foundInAllPanes; ++pane)
			foundInAllPanes = TryFindUniqueTagByName(m_panes[pane].tags, name, reconciled[pane]);
		if (!foundInAllPanes)
			continue;

		Suggestion trial;
		for (int pane = 0; pane < m_nPanes; ++pane)
			trial.tags[pane] = reconciled[pane];

		String trialMessage;
		if (FinalizeStandardSuggestion(trial, trialMessage))
		{
			suggestion = trial;
			return true;
		}
		if (firstMessage.empty())
			firstMessage = trialMessage;
	}

	if (!firstMessage.empty())
		message = firstMessage;
	return false;
}

bool Analyzer::TryBuildSuggestion(const DiffInfo& diff, Suggestion& suggestion, String& message) const
{
	message.clear();
	if (m_nPanes != 3)
	{
		message = _("Semantic merge supports only 3-way comparisons.");
		return false;
	}
	if (!RolesAreValid())
	{
		message = _("The base and the two sides of the merge are not set.");
		return false;
	}
	if (!AllPanesHaveLanguage())
	{
		message = _("Tree-sitter data is not available for all panes.");
		return false;
	}

	bool foundAllTags = true;
	bool foundTag[3] = { false, false, false };
	for (int pane = 0; pane < m_nPanes; ++pane)
	{
		foundTag[pane] = TryFindBestTagForDiff(m_panes[pane].tags, diff.dbegin, diff.dend, suggestion.tags[pane]);
		foundAllTags = foundAllTags && foundTag[pane];
	}
	const bool namesAgree = foundAllTags &&
		suggestion.tags[0].name == suggestion.tags[1].name &&
		suggestion.tags[0].name == suggestion.tags[2].name;

	if (namesAgree)
	{
		// The anchor may point at a different definition inside the same
		// diff block; prefer it when it yields a valid suggestion.
		TagRange anchorTag;
		if (diff.anchorPane >= 0 && diff.anchorPane < m_nPanes && diff.anchorLine >= 0 &&
			TryFindBestTagForDiff(m_panes[diff.anchorPane].tags, diff.anchorLine, diff.anchorLine, anchorTag) &&
			anchorTag.name != suggestion.tags[0].name)
		{
			String anchoredMessage;
			if (TryReconcileTagsByName(diff, suggestion, anchoredMessage))
				return true;
		}
		return FinalizeStandardSuggestion(suggestion, message);
	}

	// R6: a side without a definition at the diff lines may hold a
	// commented-out copy of the definition the other side edited
	String commentedMessage;
	for (const int commentPane : { m_roles.mine, m_roles.theirs })
	{
		if (!foundTag[commentPane] && TryBuildCommentedBlockSuggestion(commentPane, diff, suggestion, commentedMessage))
			return true;
	}

	// The diff's line range maps to differently named (or missing)
	// definitions across panes; retry the candidates by name, wherever each
	// definition is located in each pane.
	if (TryReconcileTagsByName(diff, suggestion, message))
		return true;

	if (message.empty())
		message = commentedMessage;
	if (message.empty())
		message = _("The difference maps to different definitions across panes.");
	return false;
}


// ---------------------------------------------------------------------------
// Token-level three-way merge
// ---------------------------------------------------------------------------

namespace
{

/** @brief A token and the white space in front of it. */
struct MergeToken
{
	String text;
	String space;
	bool word = false;        /**< identifier, number or string literal */
	bool lineComment = false; /**< runs to the end of its line */
};

bool IsMergeSpace(tchar_t ch)
{
	return ch == _T(' ') || ch == _T('\t') || ch == _T('\r') || ch == _T('\n');
}

bool IsMergeWordChar(tchar_t ch)
{
	return ch == _T('_') || ch > 127 || _istalnum(ch);
}

void TokenizeForMerge(const String& text, const LanguageTraits& traits,
	std::vector<MergeToken>& tokens, String& trailingSpace)
{
	const size_t n = text.length();
	size_t i = 0;
	while (i < n)
	{
		size_t start = i;
		while (i < n && IsMergeSpace(text[i]))
			++i;
		String space = text.substr(start, i - start);
		if (i == n)
		{
			trailingSpace = space;
			return;
		}

		MergeToken token;
		token.space = std::move(space);
		start = i;
		bool found = false;
		for (const String& prefix : traits.lineCommentPrefixes)
		{
			if (!prefix.empty() && text.compare(i, prefix.length(), prefix) == 0)
			{
				while (i < n && text[i] != _T('\r') && text[i] != _T('\n'))
					++i;
				// white space at the end of the comment is not part of it
				while (i > start && (text[i - 1] == _T(' ') || text[i - 1] == _T('\t')))
					--i;
				token.lineComment = true;
				found = true;
				break;
			}
		}
		if (!found && traits.stringQuoteChars.find(text[i]) != String::npos)
		{
			// a string literal is one token when it closes on its line
			const tchar_t quote = text[i];
			size_t j = i + 1;
			while (j < n && text[j] != quote && text[j] != _T('\r') && text[j] != _T('\n'))
				j += (text[j] == _T('\\') && j + 1 < n) ? 2 : 1;
			if (j < n && text[j] == quote)
			{
				i = j + 1;
				token.word = true;
				found = true;
			}
		}
		if (!found && IsMergeWordChar(text[i]))
		{
			while (i < n && IsMergeWordChar(text[i]))
				++i;
			token.word = true;
			found = true;
		}
		if (!found)
			++i;
		token.text = text.substr(start, i - start);
		tokens.push_back(std::move(token));
	}
	trailingSpace.clear();
}

/**
 * @brief Longest common subsequence of two id sequences.
 * @param [out] matchA For each element of a, the index of its partner in b or -1.
 * @return false when the sequences are too long to compare.
 */
bool MatchTokenSequences(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& matchA)
{
	const size_t n = a.size(), m = b.size();
	matchA.assign(n, -1);
	if (n == 0 || m == 0)
		return true;
	if (n * m > 6000000 || n > 60000 || m > 60000)
		return false;
	const size_t width = m + 1;
	std::vector<uint16_t> table((n + 1) * width, 0);
	for (size_t i = n; i-- > 0; )
	{
		for (size_t j = m; j-- > 0; )
		{
			table[i * width + j] = (a[i] == b[j]) ?
				static_cast<uint16_t>(table[(i + 1) * width + j + 1] + 1) :
				(std::max)(table[(i + 1) * width + j], table[i * width + j + 1]);
		}
	}
	size_t i = 0, j = 0;
	while (i < n && j < m)
	{
		if (a[i] == b[j])
		{
			matchA[i] = static_cast<int>(j);
			++i;
			++j;
		}
		else if (table[(i + 1) * width + j] >= table[i * width + j + 1])
			++i;
		else
			++j;
	}
	return true;
}

bool HasLineBreak(const String& space)
{
	return space.find_first_of(_T("\r\n")) != String::npos;
}

bool HasPreprocessorLine(const String& text)
{
	size_t pos = 0;
	while (pos < text.length())
	{
		size_t i = pos;
		while (i < text.length() && (text[i] == _T(' ') || text[i] == _T('\t')))
			++i;
		if (i < text.length() && text[i] == _T('#'))
			return true;
		pos = text.find(_T('\n'), pos);
		if (pos == String::npos)
			break;
		++pos;
	}
	return false;
}

}

bool MergeTokensThreeWay(const String& baseText, const String& oursText, const String& theirsText,
	const LanguageTraits& traits, String& merged)
{
	merged.clear();
	if (traits.preprocessorLines &&
		(HasPreprocessorLine(baseText) || HasPreprocessorLine(oursText) || HasPreprocessorLine(theirsText)))
		return false;

	// 0 = base, 1 = ours, 2 = theirs
	std::vector<MergeToken> tokens[3];
	String trailing[3];
	TokenizeForMerge(baseText, traits, tokens[0], trailing[0]);
	TokenizeForMerge(oursText, traits, tokens[1], trailing[1]);
	TokenizeForMerge(theirsText, traits, tokens[2], trailing[2]);
	if (tokens[0].empty())
		return false; // two additions in the same place: nothing to tell them apart

	std::unordered_map<String, int> idByText;
	std::vector<int> ids[3];
	for (int side = 0; side < 3; ++side)
	{
		for (const MergeToken& token : tokens[side])
			ids[side].push_back(idByText.emplace(token.text, static_cast<int>(idByText.size())).first->second);
	}

	// What each side did to the base
	std::vector<int> match[3];
	for (int side = 1; side < 3; ++side)
	{
		if (!MatchTokenSequences(ids[0], ids[side], match[side]))
			return false;
		// A side that rewrote most of the region leaves too little to line
		// the other side's change up against
		const size_t kept = static_cast<size_t>(std::count_if(match[side].begin(), match[side].end(),
			[](int partner) { return partner >= 0; }));
		const size_t changed = (ids[0].size() - kept) + (ids[side].size() - kept);
		if (changed > (std::max<size_t>)(4, ids[0].size()))
			return false;
	}

	struct OutToken
	{
		const MergeToken* token;
		String space;
	};
	std::vector<OutToken> out;
	std::vector<int> insertedWords[3]; // ids of the words each side's changes bring in
	bool layoutChanged[3] = { false, false, false };
	bool multiLineChange[3] = { false, false, false };
	int lastTakenSide = 0; // side whose change was emitted just before, 0 for none

	// White space is merged three ways as well: the side that changed it
	// wins; where both did, the side whose change was emitted just before
	auto mergeSpace = [&](const String& baseSpace, const String& oursSpace, const String& theirsSpace) -> const String&
	{
		if (oursSpace == baseSpace)
			return theirsSpace;
		if (theirsSpace == baseSpace)
			return oursSpace;
		return lastTakenSide == 2 ? theirsSpace : oursSpace;
	};
	auto emitStable = [&](size_t o, size_t a, size_t b)
	{
		const String& baseSpace = tokens[0][o].space;
		const String& oursSpace = tokens[1][a].space;
		const String& theirsSpace = tokens[2][b].space;
		for (int side = 1; side < 3; ++side)
		{
			const String& sideSpace = side == 1 ? oursSpace : theirsSpace;
			if (sideSpace != baseSpace && (HasLineBreak(sideSpace) || HasLineBreak(baseSpace)))
				layoutChanged[side] = true;
		}
		out.push_back({ &tokens[1][a], mergeSpace(baseSpace, oursSpace, theirsSpace) });
		lastTakenSide = 0;
	};
	// A changed stretch comes with its own layout, except in front: where
	// the changing side left the white space alone, the other side's counts
	auto emitTaken = [&](int side, size_t begin, size_t end, size_t baseBegin, size_t baseCount, size_t otherBegin)
	{
		for (size_t i = begin; i < end; ++i)
		{
			const String* space = &tokens[side][i].space;
			if (i == begin && baseCount > 0 && *space == tokens[0][baseBegin].space)
				space = &tokens[3 - side][otherBegin].space;
			if (HasLineBreak(tokens[side][i].space))
				multiLineChange[side] = true;
			if (tokens[side][i].word)
				insertedWords[side].push_back(ids[side][i]);
			out.push_back({ &tokens[side][i], *space });
		}
		if (baseCount > 0 || end > begin)
			lastTakenSide = side;
	};

	// Walk the base from one token that both sides kept to the next; what
	// lies in between was changed by one side, by both alike, or by both
	// differently, which is a conflict
	size_t o = 0, a = 0, b = 0;
	const size_t nBase = ids[0].size();
	for (;;)
	{
		size_t sync = o;
		while (sync < nBase && (match[1][sync] < 0 || match[2][sync] < 0))
			++sync;
		const size_t aEnd = sync < nBase ? static_cast<size_t>(match[1][sync]) : ids[1].size();
		const size_t bEnd = sync < nBase ? static_cast<size_t>(match[2][sync]) : ids[2].size();

		const bool oursSame = (aEnd - a == sync - o) && std::equal(ids[1].begin() + a, ids[1].begin() + aEnd, ids[0].begin() + o);
		const bool theirsSame = (bEnd - b == sync - o) && std::equal(ids[2].begin() + b, ids[2].begin() + bEnd, ids[0].begin() + o);
		if (oursSame && theirsSame)
		{
			for (size_t i = 0; i < sync - o; ++i)
				emitStable(o + i, a + i, b + i);
		}
		else if (oursSame)
			emitTaken(2, b, bEnd, o, sync - o, a);
		else if (theirsSame)
			emitTaken(1, a, aEnd, o, sync - o, b);
		else if ((aEnd - a == bEnd - b) && std::equal(ids[1].begin() + a, ids[1].begin() + aEnd, ids[2].begin() + b))
		{
			// the same change on both sides; it is not one side's own
			for (size_t i = a; i < aEnd; ++i)
				out.push_back({ &tokens[1][i], tokens[1][i].space });
			lastTakenSide = 1;
		}
		else
			return false;

		if (sync == nBase)
			break;
		emitStable(sync, aEnd, bEnd);
		o = sync + 1;
		a = aEnd + 1;
		b = bEnd + 1;
	}
	const String& trailingSpace = mergeSpace(trailing[0], trailing[1], trailing[2]);

	// Both sides bringing in the same word in different places is most
	// likely the same change made twice; applying both would double it
	std::sort(insertedWords[1].begin(), insertedWords[1].end());
	for (int id : insertedWords[2])
	{
		if (std::binary_search(insertedWords[1].begin(), insertedWords[1].end(), id))
			return false;
	}

	// Where indentation carries meaning, one side's new lines cannot be
	// fitted into the other side's new layout
	if (traits.layoutSensitive &&
		((layoutChanged[1] && (layoutChanged[2] || multiLineChange[2])) ||
		 (layoutChanged[2] && multiLineChange[1])))
		return false;

	for (size_t i = 0; i < out.size(); ++i)
	{
		if (i > 0)
		{
			// the joined text must still read as the same tokens
			if (out[i - 1].token->lineComment && !HasLineBreak(out[i].space))
				return false;
			if (out[i].space.empty() && out[i - 1].token->word && out[i].token->word)
				return false;
		}
		merged += out[i].space;
		merged += out[i].token->text;
	}
	merged += trailingSpace;
	return true;
}

// ---------------------------------------------------------------------------
// Definition-level decisions of rules R1 and R3
// ---------------------------------------------------------------------------

std::vector<String> SqueezeNonBlankLines(const std::vector<String>& lines)
{
	std::vector<String> squeezed;
	for (const String& line : lines)
	{
		String text;
		for (tchar_t ch : line)
		{
			if (ch != _T(' ') && ch != _T('\t') && ch != _T('\r') && ch != _T('\n'))
				text += ch;
		}
		if (!text.empty())
			squeezed.push_back(std::move(text));
	}
	return squeezed;
}

DuplicateDrop ChooseDuplicateAdditionToDrop(const std::vector<String>& mineLines, const std::vector<String>& theirsLines)
{
	std::vector<String> mine = SqueezeNonBlankLines(mineLines);
	std::vector<String> theirs = SqueezeNonBlankLines(theirsLines);
	std::sort(mine.begin(), mine.end());
	std::sort(theirs.begin(), theirs.end());
	if (std::includes(mine.begin(), mine.end(), theirs.begin(), theirs.end()))
		return DuplicateDrop::Theirs;
	if (std::includes(theirs.begin(), theirs.end(), mine.begin(), mine.end()))
		return DuplicateDrop::Mine;
	return DuplicateDrop::None;
}

std::vector<String> IndentedNonBlankLines(const std::vector<String>& lines, bool bRelative)
{
	std::vector<String> indents, indented;
	for (const String& line : lines)
	{
		size_t nIndent = 0;
		while (nIndent < line.length() && (line[nIndent] == _T(' ') || line[nIndent] == _T('\t')))
			++nIndent;
		const std::vector<String> squeezed = SqueezeNonBlankLines({ line.substr(nIndent) });
		if (squeezed.empty())
			continue;
		indents.push_back(line.substr(0, nIndent));
		indented.push_back(squeezed[0]);
	}
	size_t nCommon = 0;
	if (bRelative && !indents.empty())
	{
		nCommon = indents[0].length();
		for (const String& indent : indents)
		{
			size_t n = 0;
			while (n < nCommon && n < indent.length() && indent[n] == indents[0][n])
				++n;
			nCommon = n;
		}
	}
	for (size_t i = 0; i < indented.size(); ++i)
		indented[i] = indents[i].substr(nCommon) + indented[i];
	return indented;
}

DuplicateDrop ChooseContainedAdditionToDrop(const std::vector<String>& mineLines, const std::vector<String>& theirsLines,
	const LanguageTraits& traits)
{
	// A line that the other side put inside a block or a conditional section
	// of its own is not the same addition: the lines must sit at the same
	// depth, and what the outer addition has in front of them must not
	// leave a preprocessor conditional open or switch its branch
	const std::vector<String> mine = IndentedNonBlankLines(mineLines, false);
	const std::vector<String> theirs = IndentedNonBlankLines(theirsLines, false);
	auto startsConditional = [&](const std::vector<String>& outer, size_t nCount)
	{
		int nDepth = 0;
		bool bSwitched = false;
		for (size_t i = 0; i < nCount; ++i)
		{
			const size_t nFirst = outer[i].find_first_not_of(_T(" \t"));
			if (nFirst == String::npos || outer[i][nFirst] != _T('#'))
				continue;
			if (outer[i].compare(nFirst, 3, _T("#if")) == 0)
				++nDepth;
			else if (outer[i].compare(nFirst, 6, _T("#endif")) == 0)
				--nDepth;
			else if (outer[i].compare(nFirst, 3, _T("#el")) == 0)
				bSwitched = bSwitched || nDepth == 0;
		}
		return nDepth != 0 || bSwitched;
	};
	auto contains = [&](const std::vector<String>& outer, const std::vector<String>& inner)
	{
		if (inner.empty() || inner.size() >= outer.size())
			return false;
		const auto found = std::search(outer.begin(), outer.end(), inner.begin(), inner.end());
		return found != outer.end() &&
			!(traits.preprocessorLines && startsConditional(outer, static_cast<size_t>(found - outer.begin())));
	};
	if (contains(mine, theirs))
		return DuplicateDrop::Theirs;
	if (contains(theirs, mine))
		return DuplicateDrop::Mine;
	return DuplicateDrop::None;
}

MovedSide DetectMovedDefinition(int baseStart, int baseEnd, int mineStart, int mineEnd, int theirsStart, int theirsEnd)
{
	auto overlapsBase = [&](int start, int end) { return start <= baseEnd && end >= baseStart; };
	const bool bMineInPlace = overlapsBase(mineStart, mineEnd);
	const bool bTheirsInPlace = overlapsBase(theirsStart, theirsEnd);
	if (!bMineInPlace && bTheirsInPlace)
		return MovedSide::Mine;
	if (!bTheirsInPlace && bMineInPlace)
		return MovedSide::Theirs;
	return MovedSide::None;
}

bool MergeMovedDefinition(const std::vector<String>& baseLines, const std::vector<String>& moverLines,
	const std::vector<String>& editorLines, const LanguageTraits& traits, std::vector<String>& mergedLines)
{
	mergedLines.clear();
	// the mover only moved it: the editor's version as it is
	if (moverLines == baseLines)
	{
		mergedLines = editorLines;
		return true;
	}
	// the editor only re-laid it out: nothing but the move, which the line merge does
	if (SqueezeNonBlankLines(editorLines) == SqueezeNonBlankLines(baseLines))
		return false;
	// both changed it: merge what each did, token by token
	auto join = [](const std::vector<String>& lines)
	{
		String text;
		for (const String& line : lines)
			text += line + _T("\n");
		return text;
	};
	String merged;
	if (!MergeTokensThreeWay(join(baseLines), join(moverLines), join(editorLines), traits, merged))
		return false;
	size_t pos = 0;
	while (pos < merged.length())
	{
		const size_t end = merged.find(_T('\n'), pos);
		if (end == String::npos)
		{
			mergedLines.push_back(merged.substr(pos));
			break;
		}
		mergedLines.push_back(merged.substr(pos, end - pos));
		pos = end + 1;
	}
	return true;
}

} // namespace SemanticMerge
