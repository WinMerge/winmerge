/**
 * @file  SemanticMergeAnalyzer_test.cpp
 *
 * @brief Unit tests for the tree-sitter based semantic merge analyzer.
 *
 * The analyzer is exercised with synthetic pane data plus the manual test
 * triples in Testing/Data/SemanticMergeManual. Tags are produced by a small
 * scanner that mimics tree-sitter tags.scm output for the controlled C++
 * test inputs (top-level definitions starting at column 0).
 */
#include "pch.h"
#include <gtest/gtest.h>
#include "SemanticMergeAnalyzer.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include <tchar.h>
#include <fstream>
#include <string>
#include <vector>

using SemanticMerge::TagRange;

namespace
{

/** @brief ITextSource over a plain vector of lines (LF line endings). */
class VectorTextSource : public SemanticMerge::ITextSource
{
public:
	explicit VectorTextSource(std::vector<String> lines) : m_lines(std::move(lines)) {}

	int GetLineCount() const override { return static_cast<int>(m_lines.size()); }
	int GetLineLength(int nLine) const override { return static_cast<int>(m_lines[nLine].size()); }

	void GetTextRange(int nStartLine, int nStartChar, int nEndLine, int nEndChar, String& text) const override
	{
		text.clear();
		for (int line = nStartLine; line <= nEndLine; ++line)
		{
			const String& lineText = m_lines[line];
			const int from = (line == nStartLine) ? nStartChar : 0;
			const int to = (line == nEndLine) ? nEndChar : static_cast<int>(lineText.size());
			text.append(lineText, from, to - from);
			if (line != nEndLine)
				text += _T('\n');
		}
	}

	String GetLineEol(int) const override { return _T("\n"); }

	const std::vector<String>& Lines() const { return m_lines; }

private:
	std::vector<String> m_lines;
};

bool IsIdentChar(tchar_t ch)
{
	return _istalnum(ch) || ch == _T('_');
}

/**
 * @brief Scan top-level function definitions the way tags.scm would report them.
 *
 * A definition starts at a column-0 line containing "name(" and ends at the
 * next line that consists of a single closing brace at column 0.
 */
std::vector<TagRange> ScanTopLevelFunctions(const std::vector<String>& lines)
{
	std::vector<TagRange> tags;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		const String& line = lines[i];
		if (line.empty() || !(_istalpha(line[0]) || line[0] == _T('_')))
			continue;
		const size_t paren = line.find(_T('('));
		if (paren == String::npos || line.find(_T('{')) != String::npos)
			continue;

		size_t nameEnd = paren;
		while (nameEnd > 0 && line[nameEnd - 1] == _T(' '))
			--nameEnd;
		size_t nameBegin = nameEnd;
		while (nameBegin > 0 && IsIdentChar(line[nameBegin - 1]))
			--nameBegin;
		if (nameBegin == nameEnd)
			continue;

		size_t endLine = lines.size();
		for (size_t j = i + 1; j < lines.size(); ++j)
		{
			if (lines[j] == _T("}"))
			{
				endLine = j;
				break;
			}
		}
		if (endLine == lines.size())
			continue;

		TagRange tag{};
		tag.name = ucr::toUTF8(line.substr(nameBegin, nameEnd - nameBegin));
		tag.startLine = static_cast<int>(i);
		tag.startChar = 0;
		tag.endLine = static_cast<int>(endLine);
		tag.endChar = 1;
		tags.push_back(tag);
		i = endLine;
	}
	return tags;
}

bool LoadLines(const std::string& path, std::vector<String>& lines)
{
	std::ifstream file(path);
	if (!file)
		return false;
	lines.clear();
	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		lines.push_back(ucr::toTString(line));
	}
	return true;
}

/** @brief Holds three panes plus their analyzer, keeping sources alive. */
class ThreeWayFixture
{
public:
	ThreeWayFixture(std::vector<String> pane0, std::vector<String> pane1, std::vector<String> pane2)
	{
		std::vector<String> all[3] = { std::move(pane0), std::move(pane1), std::move(pane2) };
		for (int pane = 0; pane < 3; ++pane)
		{
			m_sources[pane] = std::make_unique<VectorTextSource>(std::move(all[pane]));
			m_panes[pane].pText = m_sources[pane].get();
			m_panes[pane].tags = ScanTopLevelFunctions(m_sources[pane]->Lines());
			m_panes[pane].hasLanguage = true;
		}
	}

	SemanticMerge::Analyzer MakeAnalyzer(const tchar_t* language = _T("cpp"),
		const SemanticMerge::MergeRoles& roles = SemanticMerge::MergeRoles()) const
	{
		return SemanticMerge::Analyzer(m_panes, 3, SemanticMerge::TraitsForLanguage(language), roles);
	}

	SemanticMerge::PaneData m_panes[3];

private:
	std::unique_ptr<VectorTextSource> m_sources[3];
};

SemanticMerge::DiffInfo MakeDiff(int dbegin, int dend, OP_TYPE op = OP_DIFF)
{
	SemanticMerge::DiffInfo diff;
	diff.dbegin = dbegin;
	diff.dend = dend;
	diff.op = op;
	return diff;
}

const std::string kDataDir = "../../Data/SemanticMergeManual/";

// ---------------------------------------------------------------------------
// Helper function tests
// ---------------------------------------------------------------------------

TEST(SemanticMergeHelpers, NormalizeSemanticTextStripsCrAndTrailingNewlines)
{
	EXPECT_EQ(_T("a\nb"), SemanticMerge::NormalizeSemanticText(_T("a\r\nb\r\n\n")));
	EXPECT_EQ(_T(""), SemanticMerge::NormalizeSemanticText(_T("\n\n")));
}

TEST(SemanticMergeHelpers, FindWholeWordOccurrencesRespectsBoundaries)
{
	const auto positions = SemanticMerge::FindWholeWordOccurrences(
		_T("score + scores + score_x + score"), _T("score"));
	ASSERT_EQ(2u, positions.size());
	EXPECT_EQ(0u, positions[0]);
	EXPECT_EQ(27u, positions[1]);
}

TEST(SemanticMergeHelpers, ReplaceWholeWordOccurrencesEnforcesExpectedCount)
{
	String result;
	EXPECT_FALSE(SemanticMerge::ReplaceWholeWordOccurrences(_T("a + a + a"), _T("a"), _T("b"), 2, result));
	EXPECT_TRUE(SemanticMerge::ReplaceWholeWordOccurrences(_T("a + a + a"), _T("a"), _T("b"), 3, result));
	EXPECT_EQ(_T("b + b + b"), result);
}

TEST(SemanticMergeHelpers, ExtractFunctionParameterNamesHandlesDefaultsAndNesting)
{
	std::vector<String> names;
	ASSERT_TRUE(SemanticMerge::ExtractFunctionParameterNames(
		_T("int Calc(int base, std::map<int, int> lookup, double scale = 1.0)\n{\n}"), names));
	ASSERT_EQ(3u, names.size());
	EXPECT_EQ(_T("base"), names[0]);
	EXPECT_EQ(_T("lookup"), names[1]);
	EXPECT_EQ(_T("scale"), names[2]);

	ASSERT_TRUE(SemanticMerge::ExtractFunctionParameterNames(_T("int Zero(void)\n{\n}"), names));
	EXPECT_TRUE(names.empty());
}

TEST(SemanticMergeHelpers, ExtractSingleStringLiteralChangeFindsUniqueChange)
{
	String oldLit, newLit;
	ASSERT_TRUE(SemanticMerge::ExtractSingleStringLiteralChange(
		_T("return \"old\"; log(\"same\");"),
		_T("return \"new\"; log(\"same\");"),
		_T("\""), oldLit, newLit));
	EXPECT_EQ(_T("\"old\""), oldLit);
	EXPECT_EQ(_T("\"new\""), newLit);

	// Two changed literals must be rejected.
	EXPECT_FALSE(SemanticMerge::ExtractSingleStringLiteralChange(
		_T("f(\"a\", \"b\");"), _T("f(\"x\", \"y\");"), _T("\""), oldLit, newLit));

	// So must a literal change that comes with other code changes.
	EXPECT_FALSE(SemanticMerge::ExtractSingleStringLiteralChange(
		_T("return \"old\";"), _T("log(x);\nreturn \"new\";"), _T("\""), oldLit, newLit));

	// Single-quoted literals are found when the language allows them.
	ASSERT_TRUE(SemanticMerge::ExtractSingleStringLiteralChange(
		_T("say('hi')"), _T("say('bye')"), _T("\"'"), oldLit, newLit));
	EXPECT_EQ(_T("'hi'"), oldLit);
	EXPECT_EQ(_T("'bye'"), newLit);
	EXPECT_FALSE(SemanticMerge::ExtractSingleStringLiteralChange(
		_T("say('hi')"), _T("say('bye')"), _T("\""), oldLit, newLit));
}

TEST(SemanticMergeHelpers, ReplaceUniqueStringLiteralRejectsAmbiguousTarget)
{
	String result;
	EXPECT_TRUE(SemanticMerge::ReplaceUniqueStringLiteral(
		_T("say(\"hi\");"), _T("\"hi\""), _T("\"bye\""), result));
	EXPECT_EQ(_T("say(\"bye\");"), result);

	EXPECT_FALSE(SemanticMerge::ReplaceUniqueStringLiteral(
		_T("say(\"hi\"); say(\"hi\");"), _T("\"hi\""), _T("\"bye\""), result));
}

TEST(SemanticMergeHelpers, ExtractSingleParameterRenameChangeRequiresExactPattern)
{
	const String before = _T("int Calc(int base)\n{\n\treturn base + 1;\n}");
	const String after = _T("int Calc(int offset)\n{\n\treturn offset + 1;\n}");
	String oldName, newName;
	ASSERT_TRUE(SemanticMerge::ExtractSingleParameterRenameChange(before, after, oldName, newName));
	EXPECT_EQ(_T("base"), oldName);
	EXPECT_EQ(_T("offset"), newName);

	// A rename combined with another edit must be rejected.
	const String afterWithEdit = _T("int Calc(int offset)\n{\n\treturn offset + 2;\n}");
	EXPECT_FALSE(SemanticMerge::ExtractSingleParameterRenameChange(before, afterWithEdit, oldName, newName));
}

TEST(SemanticMergeHelpers, ExtractSingleIdentifierRenameChangeLimitsOccurrences)
{
	const String before = _T("int f()\n{\n\tint score = 1;\n\tscore += 2;\n\treturn score;\n}");
	const String after = _T("int f()\n{\n\tint total = 1;\n\ttotal += 2;\n\treturn total;\n}");
	String oldName, newName;
	size_t count = 0;
	ASSERT_TRUE(SemanticMerge::ExtractSingleIdentifierRenameChange(before, after, oldName, newName, count));
	EXPECT_EQ(_T("score"), oldName);
	EXPECT_EQ(_T("total"), newName);
	EXPECT_EQ(3u, count);

	// More than 4 occurrences is rejected as too broad.
	const String beforeMany = _T("a;a;a;a;a;");
	const String afterMany = _T("b;b;b;b;b;");
	EXPECT_FALSE(SemanticMerge::ExtractSingleIdentifierRenameChange(beforeMany, afterMany, oldName, newName, count));
}

TEST(SemanticMergeHelpers, CommentedBlockRoundTrip)
{
	SemanticMerge::CommentedBlockInfo info;
	const std::vector<String> slashes = { _T("//") };
	ASSERT_TRUE(SemanticMerge::TryParseLineCommentedBlock(
		_T("// int f()\n// {\n// \treturn 1;\n// }\n"), slashes, info));
	EXPECT_EQ(_T("// "), info.linePrefix);
	// Trailing newlines are not significant; NormalizeSemanticText strips them
	// before the analyzer compares decommented text with definition text.
	EXPECT_EQ(_T("int f()\n{\n\treturn 1;\n}"),
		SemanticMerge::NormalizeSemanticText(info.decommentedText));

	const String rendered = SemanticMerge::RenderLineCommentedBlock(_T("int g()\n{\n}"), _T("// "));
	EXPECT_EQ(_T("// int g()\n// {\n// }"), SemanticMerge::NormalizeSemanticText(rendered));

	// Mixed comment styles are rejected.
	EXPECT_FALSE(SemanticMerge::TryParseLineCommentedBlock(_T("// a\nb\n"), slashes, info));
	// A prefix outside the language's comment set is rejected.
	EXPECT_FALSE(SemanticMerge::TryParseLineCommentedBlock(_T("# a\n# b\n"), slashes, info));
}

TEST(SemanticMergeHelpers, TryFindBestTagForDiffPicksLargestOverlap)
{
	std::vector<TagRange> tags(2);
	tags[0].name = "first";
	tags[0].startLine = 0;
	tags[0].endLine = 4;
	tags[1].name = "second";
	tags[1].startLine = 6;
	tags[1].endLine = 14;

	TagRange best{};
	ASSERT_TRUE(SemanticMerge::TryFindBestTagForDiff(tags, 3, 9, best));
	EXPECT_EQ("second", best.name);

	EXPECT_FALSE(SemanticMerge::TryFindBestTagForDiff(tags, 20, 25, best));
}

TEST(SemanticMergeHelpers, TraitsForLanguageMapping)
{
	const auto cpp = SemanticMerge::TraitsForLanguage(_T("cpp"));
	EXPECT_EQ(_T("\""), cpp.stringQuoteChars);
	EXPECT_TRUE(cpp.cStyleParameters);
	EXPECT_TRUE(cpp.identifierRename);
	ASSERT_EQ(1u, cpp.lineCommentPrefixes.size());
	EXPECT_EQ(_T("//"), cpp.lineCommentPrefixes[0]);

	const auto python = SemanticMerge::TraitsForLanguage(_T("python"));
	EXPECT_EQ(_T("\"'"), python.stringQuoteChars);
	EXPECT_FALSE(python.cStyleParameters);
	ASSERT_EQ(1u, python.lineCommentPrefixes.size());
	EXPECT_EQ(_T("#"), python.lineCommentPrefixes[0]);

	// Registry names may use dashes and mixed case.
	const auto csharp = SemanticMerge::TraitsForLanguage(_T("C-Sharp"));
	EXPECT_TRUE(csharp.cStyleParameters);

	// Markup/data and unknown languages get every heuristic disabled.
	for (const tchar_t* name : { _T("json"), _T("html"), _T("css"), _T("xml"), _T("unknown-lang") })
	{
		const auto traits = SemanticMerge::TraitsForLanguage(name);
		EXPECT_TRUE(traits.stringQuoteChars.empty()) << ucr::toUTF8(name);
		EXPECT_TRUE(traits.lineCommentPrefixes.empty()) << ucr::toUTF8(name);
		EXPECT_FALSE(traits.cStyleParameters) << ucr::toUTF8(name);
		EXPECT_FALSE(traits.identifierRename) << ucr::toUTF8(name);
	}
}

// ---------------------------------------------------------------------------
// Analyzer scenario tests (synthetic panes). Roles default to the WinMerge
// layout: mine = left (0), base = middle (1), theirs = right (2).
// ---------------------------------------------------------------------------

std::vector<String> SimpleFunction(const tchar_t* body)
{
	return {
		_T("int Compute(int value)"),
		_T("{"),
		String(_T("\t")) + body,
		_T("}"),
	};
}

SemanticMerge::MergeRoles BaseLeftRoles()
{
	SemanticMerge::MergeRoles roles;
	roles.base = 0;
	roles.mine = 1;
	roles.theirs = 2;
	return roles;
}

TEST(SemanticMergeAnalyzer, OneSidedChangeTakesChangedDefinition)
{
	ThreeWayFixture fx(
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::DefinitionChange, suggestion.kind);
	EXPECT_EQ(1, suggestion.unchangedPane);
	EXPECT_EQ(2, suggestion.changedPane);
	EXPECT_EQ(1, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("value + 5")));
}

TEST(SemanticMergeAnalyzer, BaseComesFromTheRolesNotFromAMajority)
{
	// Mine and theirs made the same change; the base is the odd one out. A
	// majority vote would take the base for "unchanged" and copy it back.
	ThreeWayFixture fx(
		SimpleFunction(_T("return value + 5;")),
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message));
	EXPECT_FALSE(message.empty());

	// With the base on the left, the same panes are a one-sided change by
	// the middle pane
	SemanticMerge::MergeRoles roles = BaseLeftRoles();
	auto analyzerBaseLeft = fx.MakeAnalyzer(_T("cpp"), roles);
	ASSERT_TRUE(analyzerBaseLeft.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(1, suggestion.changedPane);
	EXPECT_EQ(0, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("value + 1")));
}

TEST(SemanticMergeAnalyzer, InvalidRolesAreRejected)
{
	ThreeWayFixture fx(
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	SemanticMerge::MergeRoles roles;
	roles.base = 1;
	roles.mine = 1;
	roles.theirs = 2;
	auto analyzer = fx.MakeAnalyzer(_T("cpp"), roles);

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, ShiftedDefinitionsAreReconciledByName)
{
	// The changed definition sits at different line positions per pane, so
	// mapping the diff's line range lands in differently named definitions.
	// The analyzer must fall back to matching the definition by name.
	std::vector<String> left = {
		_T("int Helper(int x)"),
		_T("{"),
		_T("\treturn x;"),
		_T("}"),
		_T(""),
		_T("int Compute(int base)"),
		_T("{"),
		_T("\treturn base + 9;"),
		_T("}"),
	};
	std::vector<String> other = {
		_T("int Compute(int base)"),
		_T("{"),
		_T("\treturn base + 1;"),
		_T("}"),
		_T(""),
		_T("int Helper(int x)"),
		_T("{"),
		_T("\treturn x;"),
		_T("}"),
	};

	ThreeWayFixture fx(left, other, other);
	auto analyzer = fx.MakeAnalyzer();

	// Diff covers left's Compute (lines 5-8); the same range in the other
	// panes overlaps Helper instead.
	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(5, 8), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(0, suggestion.changedPane);
	EXPECT_EQ(0, suggestion.tags[1].startLine);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("base + 9")));
}

TEST(SemanticMergeAnalyzer, CandidateLoopSkipsIdenticalDefinition)
{
	// The middle pane reordered the functions and changed BuildMessage; the
	// base is on the left. The diff line lands inside BuildMessage in the
	// middle pane but inside Multiply in the outer panes. Multiply is
	// identical everywhere, so the analyzer must move on to BuildMessage.
	std::vector<String> outer = {
		_T("int Multiply(int left, int right)"),
		_T("{"),
		_T("\treturn left * right;"),
		_T("}"),
		_T(""),
		_T("const char* BuildMessage()"),
		_T("{"),
		_T("\treturn \"stable version\";"),
		_T("}"),
	};
	std::vector<String> middle = {
		_T("const char* BuildMessage()"),
		_T("{"),
		_T("\treturn \"changed version\";"),
		_T("}"),
		_T(""),
		_T("int Multiply(int left, int right)"),
		_T("{"),
		_T("\treturn left * right;"),
		_T("}"),
	};

	ThreeWayFixture fx(outer, middle, outer);
	auto analyzer = fx.MakeAnalyzer(_T("cpp"), BaseLeftRoles());

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(1, suggestion.changedPane);
	EXPECT_EQ(_T("BuildMessage"), suggestion.displayName);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("changed version")));
}

TEST(SemanticMergeAnalyzer, CursorAnchorSelectsDefinitionInsideWideDiff)
{
	// The middle pane changed two reordered functions; a single diff block
	// spans both. The cursor anchor decides which definition the user means.
	std::vector<String> outer = {
		_T("int F(int a)"),
		_T("{"),
		_T("\treturn 1;"),
		_T("}"),
		_T(""),
		_T("int G(int a)"),
		_T("{"),
		_T("\treturn 2;"),
		_T("}"),
	};
	std::vector<String> middle = {
		_T("int G(int a)"),
		_T("{"),
		_T("\treturn 20;"),
		_T("}"),
		_T(""),
		_T("int F(int a)"),
		_T("{"),
		_T("\treturn 10;"),
		_T("}"),
	};

	ThreeWayFixture fx(outer, middle, outer);
	auto analyzer = fx.MakeAnalyzer(_T("cpp"), BaseLeftRoles());

	SemanticMerge::DiffInfo diff = MakeDiff(0, 8);
	diff.anchorPane = 1;
	diff.anchorLine = 2; // middle's G body

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(diff, suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(_T("G"), suggestion.displayName);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("return 20")));

	diff.anchorLine = 7; // middle's F body
	ASSERT_TRUE(analyzer.TryBuildSuggestion(diff, suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(_T("F"), suggestion.displayName);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("return 10")));
}

TEST(SemanticMergeAnalyzer, StringLiteralReplayOntoRefactoredSide)
{
	std::vector<String> base = {
		_T("const char* Msg(int value)"),
		_T("{"),
		_T("\tif (value > 0)"),
		_T("\t\treturn \"stable\";"),
		_T("\treturn \"fallback\";"),
		_T("}"),
	};
	std::vector<String> literalChanged = base;
	literalChanged[3] = _T("\t\treturn \"changed\";");
	std::vector<String> refactored = {
		_T("const char* Msg(int value)"),
		_T("{"),
		_T("\tconst bool hasValue = value > 0;"),
		_T("\tif (hasValue)"),
		_T("\t\treturn \"stable\";"),
		_T("\treturn \"fallback\";"),
		_T("}"),
	};

	ThreeWayFixture fx(literalChanged, base, refactored);
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(2, 4), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::StringLiteral, suggestion.kind);
	EXPECT_EQ(1, suggestion.unchangedPane);
	EXPECT_EQ(0, suggestion.changedPane);
	EXPECT_EQ(2, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("\"changed\"")));
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("hasValue")));
	EXPECT_EQ(String::npos, suggestion.replacementText.find(_T("\"stable\"")));

	// A language without known string syntax disables the replay heuristics.
	auto gatedAnalyzer = fx.MakeAnalyzer(_T("unknown-lang"));
	EXPECT_FALSE(gatedAnalyzer.TryBuildSuggestion(MakeDiff(2, 4), suggestion, message));
}

TEST(SemanticMergeAnalyzer, ParameterRenameReplayOntoRefactoredSide)
{
	std::vector<String> base = {
		_T("int Calc(int base)"),
		_T("{"),
		_T("\treturn base + 1;"),
		_T("}"),
	};
	std::vector<String> renamed = {
		_T("int Calc(int offset)"),
		_T("{"),
		_T("\treturn offset + 1;"),
		_T("}"),
	};
	std::vector<String> refactored = {
		_T("int Calc(int base)"),
		_T("{"),
		_T("\tconst int result = base + 1;"),
		_T("\treturn result;"),
		_T("}"),
	};

	ThreeWayFixture fx(renamed, base, refactored);
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(0, 3), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::ParameterRename, suggestion.kind);
	EXPECT_EQ(2, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("int Calc(int offset)")));
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("offset + 1")));
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("result")));
}

TEST(SemanticMergeAnalyzer, IdentifierRenameReplayOntoRefactoredSide)
{
	std::vector<String> base = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tint score = value;"),
		_T("\tscore += 2;"),
		_T("\treturn score;"),
		_T("}"),
	};
	std::vector<String> renamed = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tint total = value;"),
		_T("\ttotal += 2;"),
		_T("\treturn total;"),
		_T("}"),
	};
	std::vector<String> refactored = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tint score = value;"),
		_T("\tscore += 2;"),
		_T("\tif (score > 10)"),
		_T("\t\treturn 10;"),
		_T("\treturn score;"),
		_T("}"),
	};

	ThreeWayFixture fx(renamed, base, refactored);
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	// The refactor added a fourth occurrence of "score", so the rename has 3
	// occurrences on the renamed side but 4 on the refactored side; the
	// replay must be rejected as not uniquely mappable.
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 5), suggestion, message));

	// With a refactor that keeps the occurrence count, the replay succeeds.
	std::vector<String> refactoredSameCount = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tconst int bonus = 2;"),
		_T("\tint score = value;"),
		_T("\tscore += bonus;"),
		_T("\treturn score;"),
		_T("}"),
	};
	ThreeWayFixture fx2(renamed, base, refactoredSameCount);
	auto analyzer2 = fx2.MakeAnalyzer();
	ASSERT_TRUE(analyzer2.TryBuildSuggestion(MakeDiff(2, 5), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::IdentifierRename, suggestion.kind);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("total += bonus")));
	EXPECT_EQ(String::npos, suggestion.replacementText.find(_T("score")));
}

TEST(SemanticMergeAnalyzer, RenameOntoAVersionThatUsesTheNewNameIsRejected)
{
	std::vector<String> base = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tint score = value;"),
		_T("\tscore += 2;"),
		_T("\treturn score;"),
		_T("}"),
	};
	std::vector<String> renamed = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tint total = value;"),
		_T("\ttotal += 2;"),
		_T("\treturn total;"),
		_T("}"),
	};
	// "total" is another variable here; the rename would merge the two
	std::vector<String> refactored = {
		_T("int Calc(int value)"),
		_T("{"),
		_T("\tconst int total = 2;"),
		_T("\tint score = value;"),
		_T("\tscore += total;"),
		_T("\treturn score;"),
		_T("}"),
	};

	ThreeWayFixture fx(renamed, base, refactored);
	auto analyzer = fx.MakeAnalyzer();
	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 5), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, ChangeOutsideTheDefinitionRangeOnItsLinesIsRejected)
{
	std::vector<String> base = {
		_T("int Load(int value)"),
		_T("{"),
		_T("\treturn Read(value, \"utf8\");"),
		_T("}"),
	};
	// the definition's range starts after the keyword, as some grammars report it
	std::vector<String> exported = {
		_T("export int Load(int value)"),
		_T("{"),
		_T("\treturn Read(value, \"utf-8\");"),
		_T("}"),
	};
	std::vector<String> refactored = {
		_T("int Load(int value)"),
		_T("{"),
		_T("\tif (value < 0)"),
		_T("\t\treturn 0;"),
		_T("\treturn Read(value, \"utf8\");"),
		_T("}"),
	};
	SemanticMerge::Suggestion suggestion;
	String message;

	// R5: the literal change would be replayed and the keyword lost
	ThreeWayFixture fx(exported, base, refactored);
	ASSERT_EQ(1u, fx.m_panes[0].tags.size());
	fx.m_panes[0].tags[0].startChar = 7;
	EXPECT_FALSE(fx.MakeAnalyzer().TryBuildSuggestion(MakeDiff(0, 4), suggestion, message));
	EXPECT_FALSE(message.empty());

	// R4: the same with the other side unchanged
	ThreeWayFixture fx2(exported, base, base);
	fx2.m_panes[0].tags[0].startChar = 7;
	EXPECT_FALSE(fx2.MakeAnalyzer().TryBuildSuggestion(MakeDiff(0, 3), suggestion, message));

	// without the keyword the literal change is replayed
	exported[0] = base[0];
	ThreeWayFixture fx3(exported, base, refactored);
	ASSERT_TRUE(fx3.MakeAnalyzer().TryBuildSuggestion(MakeDiff(0, 4), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::StringLiteral, suggestion.kind);
}

TEST(SemanticMergeAnalyzer, BothSidesChangedSameDefinitionIsRejected)
{
	ThreeWayFixture fx(
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 2;")),
		SimpleFunction(_T("return value + 3;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, MissingTreeSitterDataIsRejected)
{
	ThreeWayFixture fx(
		SimpleFunction(_T("return value;")),
		SimpleFunction(_T("return value;")),
		SimpleFunction(_T("return value + 1;")));
	fx.m_panes[1].hasLanguage = false;
	SemanticMerge::Analyzer analyzer(fx.m_panes, 3);

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(2, 2), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, TwoWayComparisonIsRejected)
{
	ThreeWayFixture fx(
		SimpleFunction(_T("return value;")),
		SimpleFunction(_T("return value;")),
		SimpleFunction(_T("return value;")));
	SemanticMerge::Analyzer analyzer(fx.m_panes, 2);

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(0, 1), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, CommentedOutFunctionReceivesUpdatedContent)
{
	std::vector<String> commentedOut = {
		_T("// int Compute(int value)"),
		_T("// {"),
		_T("// \treturn value + 1;"),
		_T("// }"),
	};

	ThreeWayFixture fx(
		commentedOut,
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(0, 3), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::CommentedOut, suggestion.kind);
	EXPECT_EQ(1, suggestion.unchangedPane);
	EXPECT_EQ(2, suggestion.changedPane);
	EXPECT_EQ(0, suggestion.anchorPane);
	EXPECT_EQ(_T("// int Compute(int value)\n// {\n// \treturn value + 5;\n// }"),
		suggestion.replacementText);
	// The anchor range covers the commented block.
	EXPECT_EQ(0, suggestion.defs[0].tag.startLine);
	EXPECT_EQ(3, suggestion.defs[0].tag.endLine);
}

TEST(SemanticMergeAnalyzer, CommentedCopyOfTheEditedVersionIsRejected)
{
	// The commented block matches the edited version, not the base: the
	// commenting side did not comment out the base definition.
	std::vector<String> commentedOut = {
		_T("// int Compute(int value)"),
		_T("// {"),
		_T("// \treturn value + 5;"),
		_T("// }"),
	};

	ThreeWayFixture fx(
		commentedOut,
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(0, 3), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, CommentedBlockWithManualNotesIsRejected)
{
	std::vector<String> commentedOut = {
		_T("// NOTE: disabled until the new scoring ships"),
		_T("// int Compute(int value)"),
		_T("// {"),
		_T("// \treturn value + 1;"),
		_T("// }"),
	};

	ThreeWayFixture fx(
		commentedOut,
		SimpleFunction(_T("return value + 1;")),
		SimpleFunction(_T("return value + 5;")));
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	EXPECT_FALSE(analyzer.TryBuildSuggestion(MakeDiff(0, 4), suggestion, message));
	EXPECT_FALSE(message.empty());
}

TEST(SemanticMergeAnalyzer, CommentedOutFunctionOnTheirsSideAlsoSupported)
{
	std::vector<String> commentedOut = {
		_T("# def compute(value)"),
		_T("# \treturn value + 1"),
	};
	std::vector<String> original = {
		_T("def compute(value)"),
		_T("\treturn value + 1"),
	};
	std::vector<String> updated = {
		_T("def compute(value)"),
		_T("\treturn value + 5"),
	};

	// Tags scanned from python-style snippets will not match the C++ scanner,
	// so hand-build them: panes 0 and 1 hold the definition, pane 2 holds the
	// commented copy.
	ThreeWayFixture fx(updated, original, commentedOut);
	for (int pane = 0; pane < 2; ++pane)
	{
		TagRange tag{};
		tag.name = "compute";
		tag.startLine = 0;
		tag.startChar = 0;
		tag.endLine = 1;
		tag.endChar = static_cast<int>(original[1].size());
		fx.m_panes[pane].tags = { tag };
	}
	fx.m_panes[2].tags.clear();
	auto analyzer = fx.MakeAnalyzer(_T("python"));

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(0, 1), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(1, suggestion.unchangedPane);
	EXPECT_EQ(0, suggestion.changedPane);
	EXPECT_EQ(2, suggestion.anchorPane);
	EXPECT_EQ(_T("# def compute(value)\n# \treturn value + 5"), suggestion.replacementText);
}

// ---------------------------------------------------------------------------
// File-based scenario tests over Testing/Data/SemanticMergeManual (middle = base)
// ---------------------------------------------------------------------------

TEST(SemanticMergeManualData, StringLiteralTripleReplaysOntoRight)
{
	std::vector<String> left, middle, right;
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticStringLiteral3WayLeft.cpp", left));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticStringLiteral3WayMiddle.cpp", middle));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticStringLiteral3WayRight.cpp", right));

	ThreeWayFixture fx(left, middle, right);
	for (int pane = 0; pane < 3; ++pane)
		ASSERT_EQ(1u, fx.m_panes[pane].tags.size()) << "pane " << pane;
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(2, 6), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(2, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("\"changed version\"")));
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("hasValue")));
	EXPECT_EQ(String::npos, suggestion.replacementText.find(_T("\"stable version\"")));
}

TEST(SemanticMergeManualData, ParameterTripleReplaysOntoRight)
{
	std::vector<String> left, middle, right;
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticParameter3WayLeft.cpp", left));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticParameter3WayMiddle.cpp", middle));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticParameter3WayRight.cpp", right));

	ThreeWayFixture fx(left, middle, right);
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(2, 7), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(SemanticMerge::SuggestionKind::ParameterRename, suggestion.kind);
	EXPECT_EQ(2, suggestion.anchorPane);
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("int ComputeScore(int offset)")));
	EXPECT_NE(String::npos, suggestion.replacementText.find(_T("bonus")));
}

TEST(SemanticMergeManualData, CommentedTripleUpdatesCommentedCopy)
{
	std::vector<String> left, middle, right;
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticCommented3WayLeft.cpp", left));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticCommented3WayMiddle.cpp", middle));
	ASSERT_TRUE(LoadLines(kDataDir + "SemanticCommented3WayRight.cpp", right));

	ThreeWayFixture fx(left, middle, right);
	EXPECT_EQ(1u, fx.m_panes[0].tags.size()); // only BaseValue; ComputeScore is commented out
	ASSERT_EQ(2u, fx.m_panes[1].tags.size());
	ASSERT_EQ(2u, fx.m_panes[2].tags.size());
	auto analyzer = fx.MakeAnalyzer();

	SemanticMerge::Suggestion suggestion;
	String message;
	ASSERT_TRUE(analyzer.TryBuildSuggestion(MakeDiff(7, 10), suggestion, message)) << ucr::toUTF8(message);
	EXPECT_EQ(1, suggestion.unchangedPane);
	EXPECT_EQ(2, suggestion.changedPane);
	EXPECT_EQ(0, suggestion.anchorPane);
	EXPECT_EQ(_T("// int ComputeScore(int base)\n// {\n// \treturn base + 2;\n// }"),
		suggestion.replacementText);
}

// ---------------------------------------------------------------------------
// Definition-level decisions of R1 (duplicate addition) and R3 (moved definition)
// ---------------------------------------------------------------------------

TEST(SemanticMergeDecisions, SqueezeNonBlankLinesIgnoresLayout)
{
	const std::vector<String> a = { _T("int  F( int a )"), _T(""), _T("\t{ return a; }") };
	const std::vector<String> b = { _T("int F(int a)"), _T("{ return a; }"), _T("   ") };
	EXPECT_EQ(SemanticMerge::SqueezeNonBlankLines(a), SemanticMerge::SqueezeNonBlankLines(b));
}

TEST(SemanticMergeDecisions, DuplicateAdditionDropsTheSideWithNothingExtra)
{
	const std::vector<String> plain = { _T("int Helper()"), _T("{"), _T("\treturn 1;"), _T("}") };
	std::vector<String> documented = { _T("/// The helper"), _T("int Helper()"), _T("{"), _T("\treturn 1;"), _T("}") };

	// mine has the doc comment as well: theirs adds nothing, drop theirs
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Theirs, SemanticMerge::ChooseDuplicateAdditionToDrop(documented, plain));
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Mine, SemanticMerge::ChooseDuplicateAdditionToDrop(plain, documented));
	// the same lines, indented differently: either may go
	const std::vector<String> indented = { _T("int Helper()"), _T("{"), _T("    return 1;"), _T("}") };
	EXPECT_NE(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseDuplicateAdditionToDrop(plain, indented));

	// each has something the other lacks: keep both
	std::vector<String> otherComment = { _T("/// Other words"), _T("int Helper()"), _T("{"), _T("\treturn 1;"), _T("}") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseDuplicateAdditionToDrop(documented, otherComment));
}

TEST(SemanticMergeDecisions, ContainedAdditionKeepsTheSideThatHasMore)
{
	const std::vector<String> helper = { _T("/// The helper"), _T("int Helper()"), _T("{"), _T("\treturn 1;"), _T("}") };
	std::vector<String> helperAndMore = helper;
	helperAndMore.push_back(_T(""));
	helperAndMore.push_back(_T("int More() { return 2; }"));

	const SemanticMerge::LanguageTraits cpp = SemanticMerge::TraitsForLanguage(_T("cpp"));
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Theirs, SemanticMerge::ChooseContainedAdditionToDrop(helperAndMore, helper, cpp));
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Mine, SemanticMerge::ChooseContainedAdditionToDrop(helper, helperAndMore, cpp));
	// the same addition, or two different ones: not this rule
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(helper, helper, cpp));
	const std::vector<String> other = { _T("int Other()"), _T("{"), _T("\treturn 3;"), _T("}") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(helper, other, cpp));
	// the lines must appear in the same order
	const std::vector<String> reordered = { _T("int Helper()"), _T("/// The helper"), _T("{"), _T("\treturn 1;"), _T("}"), _T("int More() { return 2; }") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(reordered, helper, cpp));
}

TEST(SemanticMergeDecisions, ContainedAdditionMustSitAtTheSameDepth)
{
	const SemanticMerge::LanguageTraits cpp = SemanticMerge::TraitsForLanguage(_T("cpp"));
	const std::vector<String> call = { _T("    log(\"x\");") };
	// the other side made the call conditional: not the same addition
	const std::vector<String> guarded = { _T("    if (debug)"), _T("    {"), _T("        log(\"x\");"), _T("    }") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(call, guarded, cpp));
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(guarded, call, cpp));
	const std::vector<String> conditional = { _T("#ifdef DEBUG"), _T("    log(\"x\");"), _T("#endif") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::None, SemanticMerge::ChooseContainedAdditionToDrop(call, conditional, cpp));
	// a closed conditional in front of the call, or a directive that is not one, does not change what the call means
	const std::vector<String> afterConditional = { _T("#if DEBUG"), _T("    trace();"), _T("#endif"), _T("    log(\"x\");") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Mine, SemanticMerge::ChooseContainedAdditionToDrop(call, afterConditional, cpp));
	const std::vector<String> include = { _T("#include \"b.h\"") };
	const std::vector<String> includes = { _T("#include \"a.h\""), _T("#include \"b.h\"") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Theirs, SemanticMerge::ChooseContainedAdditionToDrop(includes, include, cpp));
	// the same call with another one after it, at the same depth
	const std::vector<String> callAndMore = { _T("    log(\"x\");"), _T("    log(\"y\");") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Mine, SemanticMerge::ChooseContainedAdditionToDrop(call, callAndMore, cpp));
	// "#" starts a comment in Python
	const SemanticMerge::LanguageTraits python = SemanticMerge::TraitsForLanguage(_T("python"));
	const std::vector<String> pyCall = { _T("    log(\"x\")") };
	const std::vector<String> pyCommented = { _T("    # note"), _T("    log(\"x\")") };
	EXPECT_EQ(SemanticMerge::DuplicateDrop::Mine, SemanticMerge::ChooseContainedAdditionToDrop(pyCall, pyCommented, python));
}

TEST(SemanticMergeDecisions, IndentedNonBlankLinesKeepTheDepth)
{
	const std::vector<String> inside = { _T("    for x in xs:"), _T("        t += x"), _T(""), _T("        print( t )") };
	const std::vector<String> after = { _T("    for x in xs:"), _T("        t += x"), _T("    print(t)") };
	EXPECT_EQ(SemanticMerge::SqueezeNonBlankLines(inside), SemanticMerge::SqueezeNonBlankLines(after));
	EXPECT_NE(SemanticMerge::IndentedNonBlankLines(inside, true), SemanticMerge::IndentedNonBlankLines(after, true));
	// relative: the same lines one level deeper compare equal
	const std::vector<String> deeper = { _T("        for x in xs:"), _T("            t += x"), _T("        print(t)") };
	EXPECT_EQ(SemanticMerge::IndentedNonBlankLines(after, true), SemanticMerge::IndentedNonBlankLines(deeper, true));
	EXPECT_NE(SemanticMerge::IndentedNonBlankLines(after, false), SemanticMerge::IndentedNonBlankLines(deeper, false));
	const std::vector<String> expected = { _T("forxinxs:"), _T("    t+=x"), _T("print(t)") };
	EXPECT_EQ(expected, SemanticMerge::IndentedNonBlankLines(after, true));
}

TEST(SemanticMergeDecisions, MovedDefinitionIsTheSideAwayFromTheBasePosition)
{
	// base at lines 10-14
	EXPECT_EQ(SemanticMerge::MovedSide::Mine, SemanticMerge::DetectMovedDefinition(10, 14, 30, 34, 10, 15));
	EXPECT_EQ(SemanticMerge::MovedSide::Theirs, SemanticMerge::DetectMovedDefinition(10, 14, 11, 14, 2, 6));
	// both moved, or neither: not this rule
	EXPECT_EQ(SemanticMerge::MovedSide::None, SemanticMerge::DetectMovedDefinition(10, 14, 30, 34, 2, 6));
	EXPECT_EQ(SemanticMerge::MovedSide::None, SemanticMerge::DetectMovedDefinition(10, 14, 10, 14, 12, 16));
}

TEST(SemanticMergeDecisions, MovedDefinitionCarriesTheEdit)
{
	const auto traits = SemanticMerge::TraitsForLanguage(_T("cpp"));
	const std::vector<String> base = { _T("int F(int a)"), _T("{"), _T("\treturn a + 1;"), _T("}") };
	const std::vector<String> edited = { _T("int F(int a)"), _T("{"), _T("\treturn a + 2;"), _T("}") };
	std::vector<String> merged;

	// the mover only moved it: the editor's version
	ASSERT_TRUE(SemanticMerge::MergeMovedDefinition(base, base, edited, traits, merged));
	EXPECT_EQ(edited, merged);

	// the mover also renamed it: both changes, token by token
	const std::vector<String> renamed = { _T("int G(int a)"), _T("{"), _T("\treturn a + 1;"), _T("}") };
	ASSERT_TRUE(SemanticMerge::MergeMovedDefinition(base, renamed, edited, traits, merged));
	const std::vector<String> expected = { _T("int G(int a)"), _T("{"), _T("\treturn a + 2;"), _T("}") };
	EXPECT_EQ(expected, merged);

	// the editor only re-indented: a pure move, which the line merge handles
	const std::vector<String> reindented = { _T("int F(int a)"), _T("{"), _T("    return a + 1;"), _T("}") };
	EXPECT_FALSE(SemanticMerge::MergeMovedDefinition(base, renamed, reindented, traits, merged));

	// both changed the same token differently: no merge
	const std::vector<String> otherEdit = { _T("int F(int a)"), _T("{"), _T("\treturn a + 3;"), _T("}") };
	const std::vector<String> moverEdit = { _T("int F(int a)"), _T("{"), _T("\treturn a + 4;"), _T("}") };
	EXPECT_FALSE(SemanticMerge::MergeMovedDefinition(base, moverEdit, otherEdit, traits, merged));
}

// ---------------------------------------------------------------------------
// Token-level three-way merge
// ---------------------------------------------------------------------------

String TokenMerge(const String& base, const String& ours, const String& theirs, const tchar_t* language = _T("cpp"))
{
	String merged;
	if (!SemanticMerge::MergeTokensThreeWay(base, ours, theirs, SemanticMerge::TraitsForLanguage(language), merged))
		return _T("<conflict>");
	return merged;
}

TEST(TokenMerge, DifferentTokensOnTheSameLineMerge)
{
	EXPECT_EQ(_T("\treturn bar(a, c);\n"),
		TokenMerge(_T("\treturn foo(a, b);\n"), _T("\treturn foo(a, c);\n"), _T("\treturn bar(a, b);\n")));
}

TEST(TokenMerge, AdjacentLinesMerge)
{
	EXPECT_EQ(_T("int a = 10;\nint b = 20;\n"),
		TokenMerge(_T("int a = 1;\nint b = 2;\n"), _T("int a = 10;\nint b = 2;\n"), _T("int a = 1;\nint b = 20;\n")));
}

TEST(TokenMerge, SameTokenChangedDifferentlyConflicts)
{
	EXPECT_EQ(_T("<conflict>"),
		TokenMerge(_T("return a;\n"), _T("return x;\n"), _T("return y;\n")));
}

TEST(TokenMerge, SameChangeOnBothSidesIsTakenOnce)
{
	EXPECT_EQ(_T("return x + 2;\n"),
		TokenMerge(_T("return a + 1;\n"), _T("return x + 1;\n"), _T("return x + 2;\n")));
}

TEST(TokenMerge, ReformattingDoesNotConflictWithAnEdit)
{
	EXPECT_EQ(_T("foo(a,\n    b,\n    d);\n"),
		TokenMerge(_T("foo(a, b, c);\n"), _T("foo(a,\n    b,\n    c);\n"), _T("foo(a, b, d);\n")));
}

TEST(TokenMerge, InsertionAndEditMerge)
{
	EXPECT_EQ(_T("int a = 2;\nint n = 0;\nint b = a;\n"),
		TokenMerge(_T("int a = 1;\nint b = a;\n"), _T("int a = 1;\nint n = 0;\nint b = a;\n"), _T("int a = 2;\nint b = a;\n")));
}

TEST(TokenMerge, DeletionAndEditMerge)
{
	EXPECT_EQ(_T("call(x);\n"),
		TokenMerge(_T("call(a);\nlog(a);\n"), _T("call(a);\n"), _T("call(x);\nlog(a);\n")));
}

TEST(TokenMerge, SameWordAddedInDifferentPlacesConflicts)
{
	// both sides made the method readonly, each in its own position
	EXPECT_EQ(_T("<conflict>"),
		TokenMerge(_T("public override bool Equals(object obj)\n"),
			_T("public readonly override bool Equals(object obj)\n"),
			_T("public override readonly bool Equals(object obj)\n")));
}

TEST(TokenMerge, StringLiteralIsOneToken)
{
	EXPECT_EQ(_T("<conflict>"),
		TokenMerge(_T("s = \"hello world\";\n"), _T("s = \"hello there\";\n"), _T("s = \"hi world\";\n")));
}

TEST(TokenMerge, RewrittenRegionIsNotMerged)
{
	EXPECT_EQ(_T("<conflict>"),
		TokenMerge(_T("x = a + b;\n"), _T("total = sum(values) * scale(factor);\n"), _T("x = a + c;\n")));
}

TEST(TokenMerge, TwoAdditionsInTheSamePlaceConflict)
{
	EXPECT_EQ(_T("<conflict>"), TokenMerge(_T(""), _T("int a;\n"), _T("int b;\n")));
}

TEST(TokenMerge, PreprocessorLinesAreLeftAlone)
{
	EXPECT_EQ(_T("<conflict>"),
		TokenMerge(_T("#define A 1\nint b = 2;\n"), _T("#define A 10\nint b = 2;\n"), _T("#define A 1\nint b = 20;\n")));
}

TEST(TokenMerge, CodeIsNotPulledIntoALineComment)
{
	// ours appends a comment, theirs changes the token after it: still two lines
	EXPECT_EQ(_T("a = 1; // note\nb = 3;\n"),
		TokenMerge(_T("a = 1;\nb = 2;\n"), _T("a = 1; // note\nb = 2;\n"), _T("a = 1;\nb = 3;\n")));
}

TEST(TokenMerge, IndentationChangeAndNewLineConflictWhereLayoutMatters)
{
	const String base = _T("if x:\n    a = 1\n    b = 2\n");
	const String ours = _T("if x:\n    if y:\n        a = 1\n        b = 2\n");
	const String theirs = _T("if x:\n    a = 1\n    c = 3\n    b = 2\n");
	EXPECT_EQ(_T("<conflict>"), TokenMerge(base, ours, theirs, _T("python")));
}

TEST(TokenMerge, ReindentedLinesStayInTheirBlockWhereLayoutMatters)
{
	// theirs put the lines under an if; ours changed a value in the first one
	EXPECT_EQ(_T("if c:\n    x = 10\n    y = 2\n"),
		TokenMerge(_T("x = 1\ny = 2\n"), _T("x = 10\ny = 2\n"), _T("if c:\n    x = 1\n    y = 2\n"), _T("python")));
	// ours deleted the middle line
	EXPECT_EQ(_T("if k:\n    a = 1\n    c = 3\n"),
		TokenMerge(_T("a = 1\nb = 2\nc = 3\n"), _T("a = 1\nc = 3\n"),
			_T("if k:\n    a = 1\n    b = 2\n    c = 3\n"), _T("python")));
}

TEST(TokenMerge, EditsMergeWhereLayoutMatters)
{
	EXPECT_EQ(_T("let a = 10\nlet b = 20\n"),
		TokenMerge(_T("let a = 1\nlet b = 2\n"), _T("let a = 10\nlet b = 2\n"), _T("let a = 1\nlet b = 20\n"), _T("fsharp")));
}

// ---------------------------------------------------------------------------
// Replay of real conflicts (run by Testing/SemanticMergeReplay/Replay.ps1 tokens)
// ---------------------------------------------------------------------------

String SqueezeLines(const std::vector<String>& lines)
{
	String squeezed;
	for (const String& line : lines)
	{
		String text;
		for (tchar_t ch : line)
		{
			if (ch != _T(' ') && ch != _T('\t') && ch != _T('\r'))
				text += ch;
		}
		if (!text.empty())
			squeezed += text + _T("\n");
	}
	return squeezed;
}

std::vector<String> SplitLines(const String& text)
{
	std::vector<String> lines;
	size_t pos = 0;
	while (pos < text.length())
	{
		size_t end = text.find(_T('\n'), pos);
		if (end == String::npos)
			end = text.length();
		lines.push_back(text.substr(pos, end - pos));
		pos = end + 1;
	}
	return lines;
}

/**
 * Each line of the list file names a language, a file with the conflicts of
 * one merge in diff3 format and the file as it was committed. A merge counts
 * as confirmed when the committed file contains the merged text.
 */
TEST(TokenMergeCorpus, DISABLED_Replay)
{
#pragma warning(suppress: 4996)
	const char* listPath = getenv("TOKEN_MERGE_CORPUS");
	ASSERT_NE(nullptr, listPath);
	std::ifstream list(listPath);
	ASSERT_TRUE(list.good());

	int conflicts = 0, merged = 0, confirmed = 0;
	std::string entry;
	while (std::getline(list, entry))
	{
		const size_t sep1 = entry.find('|');
		const size_t sep2 = entry.find('|', sep1 + 1);
		if (sep1 == std::string::npos || sep2 == std::string::npos)
			continue;
		const auto traits = SemanticMerge::TraitsForLanguage(ucr::toTString(entry.substr(0, sep1)));
		const std::string conflictsPath = entry.substr(sep1 + 1, sep2 - sep1 - 1);
		std::vector<String> lines, resultLines;
		if (!LoadLines(conflictsPath, lines) || !LoadLines(entry.substr(sep2 + 1), resultLines))
			continue;
		const String result = SqueezeLines(resultLines);

		int state = 0;
		String sides[3]; // ours, base, theirs
		for (const String& line : lines)
		{
			if (line.compare(0, 8, _T("<<<<<<< ")) == 0)
			{
				state = 1;
				sides[0].clear(); sides[1].clear(); sides[2].clear();
			}
			else if (state == 1 && line.compare(0, 8, _T("||||||| ")) == 0)
				state = 2;
			else if (state == 2 && line == _T("======="))
				state = 3;
			else if (state == 3 && line.compare(0, 8, _T(">>>>>>> ")) == 0)
			{
				state = 0;
				++conflicts;
				String mergedText;
				if (!SemanticMerge::MergeTokensThreeWay(sides[1], sides[0], sides[2], traits, mergedText))
					continue;
				++merged;
				if (result.find(SqueezeLines(SplitLines(mergedText))) != String::npos)
					++confirmed;
				else
					printf("UNCONFIRMED %s\n--- base\n%s--- ours\n%s--- theirs\n%s--- merged\n%s\n", conflictsPath.c_str(),
						ucr::toUTF8(sides[1]).c_str(), ucr::toUTF8(sides[0]).c_str(),
						ucr::toUTF8(sides[2]).c_str(), ucr::toUTF8(mergedText).c_str());
			}
			else if (state > 0)
				sides[state - 1] += line + _T("\n");
		}
	}
	printf("conflicts: %d, merged: %d, confirmed by the committed file: %d\n", conflicts, merged, confirmed);
}

} // namespace
