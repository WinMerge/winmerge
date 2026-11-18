/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  SemanticMergeAnalyzer.h
 *
 * @brief Tree-sitter based semantic merge suggestion analyzer.
 *
 * Pure analysis layer for the experimental semantic merge feature.
 * It has no dependency on CMergeDoc, MFC or the UI: panes are described
 * by an ITextSource plus a list of tree-sitter tag ranges, differences by
 * plain line ranges. The analyzer produces suggestion records; applying
 * them to buffers is the caller's responsibility.
 */
#pragma once

#include "UnicodeString.h"
#include "DiffList.h"
#include "TreeSitterParser.h"
#include <vector>

namespace SemanticMerge
{

using TagRange = CTreeSitterParser::TagRange;

/**
 * @brief Read-only text access for one pane.
 *
 * CMergeDoc adapts CDiffTextBuffer to this; tests can use a plain
 * line-vector implementation.
 */
class ITextSource
{
public:
	virtual int GetLineCount() const = 0;
	virtual int GetLineLength(int nLine) const = 0;
	/**
	 * @brief Get text in the given range, with EOLs normalized the same way
	 * the pane text is rendered (ghost lines excluded for diff buffers).
	 */
	virtual void GetTextRange(int nStartLine, int nStartChar, int nEndLine, int nEndChar, String& text) const = 0;
	virtual String GetLineEol(int nLine) const = 0;

protected:
	~ITextSource() = default;
};

/**
 * @brief Language-dependent knobs for the localized-change heuristics.
 *
 * The text-based heuristics (string literal replay, parameter rename replay,
 * identifier rename replay, commented-block update) are only valid for
 * languages whose syntax they understand. Defaults disable all of them;
 * whole-definition replace and insertion suggestions are language-agnostic
 * and always available.
 */
struct LanguageTraits
{
	std::vector<String> lineCommentPrefixes; /**< empty: commented-block update disabled */
	String stringQuoteChars;                 /**< empty: string literal replay disabled */
	bool cStyleParameters = false;           /**< enables parameter rename replay */
	bool identifierRename = false;           /**< enables identifier rename replay */
	bool preprocessorLines = false;          /**< lines starting with '#' are directives: no token merge across them */
	bool layoutSensitive = false;            /**< indentation carries meaning */
};

/**
 * @brief Traits for a tree-sitter language name (e.g. "cpp", "python").
 * Unknown languages get all heuristics disabled.
 */
LanguageTraits TraitsForLanguage(const String& languageName);

/** @brief Analyzer input for one pane. */
struct PaneData
{
	const ITextSource* pText = nullptr;
	std::vector<TagRange> tags;
	bool hasLanguage = false; /**< true when tree-sitter parsed this pane */
};

/** @brief The subset of DIFFRANGE the analyzer needs. */
struct DiffInfo
{
	int dbegin = 0;
	int dend = 0;
	OP_TYPE op = OP_NONE;
	/**
	 * Optional anchor: the pane and line the user invoked the command from.
	 * When the diff spans several definitions, the definition under the
	 * anchor is preferred. -1 means no anchor.
	 */
	int anchorPane = -1;
	int anchorLine = -1;
};

struct DefinitionInfo
{
	TagRange tag;
	int pane = -1;
	String text;
};

/** @brief Which pane is the common ancestor and which are the two sides. */
struct MergeRoles
{
	int base = 1;
	int mine = 0;
	int theirs = 2;
};

/** @brief What kind of change a suggestion carries over. */
enum class SuggestionKind
{
	DefinitionChange, /**< R4: whole definition from the only side that changed it */
	StringLiteral,    /**< R5: one string literal change replayed onto the other side's refactoring */
	ParameterRename,  /**< R5: one parameter rename replayed onto the other side's refactoring */
	IdentifierRename, /**< R5: one identifier rename replayed onto the other side's refactoring */
	CommentedOut,     /**< R6: edited definition written into the other side's commented-out copy */
};

/**
 * @brief A definition-level resolution of a conflict.
 *
 * The result reads like anchorPane with defs[anchorPane] replaced by
 * replacementText.
 */
struct Suggestion
{
	SuggestionKind kind = SuggestionKind::DefinitionChange;
	TagRange tags[3];
	DefinitionInfo defs[3];
	int unchangedPane = -1; /**< the base */
	int changedPane = -1;   /**< the side whose change is carried over */
	int anchorPane = -1;    /**< the pane whose definition lines the replacement rewrites */
	String replacementText;
	String displayName;
};

/**
 * @brief Builds definition-level resolutions (rules R4-R6 of
 * Docs/Developers/SemanticMerge.md) for conflicts of a 3-way comparison.
 *
 * The analyzer validates pane count, roles and tree-sitter availability;
 * everything else is the caller's.
 */
class Analyzer
{
public:
	Analyzer(const PaneData* panes, int nPanes, const LanguageTraits& traits = LanguageTraits(),
		const MergeRoles& roles = MergeRoles());

	/**
	 * @brief Build a resolution for a single difference.
	 * @return true when a safe resolution exists; otherwise false with an
	 *         explanatory message.
	 */
	bool TryBuildSuggestion(const DiffInfo& diff, Suggestion& suggestion, String& message) const;

private:
	std::vector<std::string> CollectCandidateDefinitionNames(const DiffInfo& diff) const;
	bool TryReconcileTagsByName(const DiffInfo& diff, Suggestion& suggestion, String& message) const;
	bool FinalizeStandardSuggestion(Suggestion& suggestion, String& message) const;
	bool TryBuildCommentedBlockSuggestion(int commentPane, const DiffInfo& diff, Suggestion& suggestion, String& message) const;
	bool AllPanesHaveLanguage() const;
	bool RolesAreValid() const;

	const PaneData* m_panes;
	int m_nPanes;
	LanguageTraits m_traits;
	MergeRoles m_roles;
};


// ---------------------------------------------------------------------------
// Internal building blocks, exposed for unit testing.
// ---------------------------------------------------------------------------

struct CommentedBlockInfo
{
	String linePrefix;
	String decommentedText;
};

/** @brief Strip CRs and trailing newlines for content comparison. */
String NormalizeSemanticText(String text);

/** @brief Find start offsets of whole-word occurrences of token in text. */
std::vector<size_t> FindWholeWordOccurrences(const String& text, const String& token);

/** @brief Replace whole-word occurrences; fails unless exactly expectedCount matches. */
bool ReplaceWholeWordOccurrences(const String& sourceText, const String& oldToken, const String& newToken,
	size_t expectedCount, String& resultText);

/** @brief Extract parameter names from a C-style function definition text. */
bool ExtractFunctionParameterNames(const String& text, std::vector<String>& parameterNames);

/** @brief Detect that exactly one quoted string literal differs between the two texts. */
bool ExtractSingleStringLiteralChange(const String& unchangedText, const String& changedText,
	const String& quoteChars, String& oldLiteralWithQuotes, String& newLiteralWithQuotes);

/** @brief Replace a string literal that occurs exactly once in sourceText. */
bool ReplaceUniqueStringLiteral(const String& sourceText, const String& oldLiteralWithQuotes,
	const String& newLiteralWithQuotes, String& resultText);

/** @brief Detect a single parameter rename with exactly one matching use-site update. */
bool ExtractSingleParameterRenameChange(const String& unchangedText, const String& changedText,
	String& oldParameterName, String& newParameterName);

/** @brief Detect a single identifier rename with 2-4 matching references. */
bool ExtractSingleIdentifierRenameChange(const String& unchangedText, const String& changedText,
	String& oldIdentifierName, String& newIdentifierName, size_t& occurrenceCount);

/** @brief Parse a block where every non-blank line starts with the same line-comment prefix. */
bool TryParseLineCommentedBlock(const String& text, const std::vector<String>& lineCommentPrefixes, CommentedBlockInfo& info);

/** @brief Render text as a line-commented block using the given prefix. */
String RenderLineCommentedBlock(const String& text, const String& prefix);

/** @brief Find the definition that best overlaps the given line range. */
bool TryFindBestTagForDiff(const std::vector<TagRange>& tags, int startLine, int endLine, TagRange& bestTag);

/**
 * @brief Three-way merge of a conflicting region by what each side changed.
 *
 * The region is compared token by token: what ours did to the base and what
 * theirs did to the base are both applied when they touch different tokens.
 * White space is layout, not content: a side that only re-wrapped or
 * re-indented does not conflict with the other side's edit.
 *
 * @return false when the changes overlap, or when a side changed so much
 *         that the changes cannot be lined up reliably.
 */
bool MergeTokensThreeWay(const String& baseText, const String& oursText, const String& theirsText,
	const LanguageTraits& traits, String& merged);

/** @brief Find the definition with the given name; fails if absent or ambiguous. */
bool TryFindUniqueTagByName(const std::vector<TagRange>& tags, const std::string& name, TagRange& found);

/** @brief The lines with spaces and tabs removed, blank lines left out: text compared as content, not layout. */
std::vector<String> SqueezeNonBlankLines(const std::vector<String>& lines);

/**
 * @brief Like SqueezeNonBlankLines, with each line's indentation kept: text
 * compared as content at a nesting depth.
 * @param bRelative Leave out the indentation all the lines share.
 */
std::vector<String> IndentedNonBlankLines(const std::vector<String>& lines, bool bRelative);

/** @brief Which side of a duplicate addition (R1) is left out. */
enum class DuplicateDrop
{
	None,   /**< neither addition holds all the other has: keep both */
	Theirs, /**< mine has everything theirs has */
	Mine,   /**< theirs has everything mine has */
};

/**
 * @brief R1: both sides added the same definition, each in its own place.
 * The addition that has nothing the other one lacks is left out (a doc
 * comment in front of the definition may be in one addition only).
 * @param mineLines, theirsLines The lines of each side's whole addition.
 */
DuplicateDrop ChooseDuplicateAdditionToDrop(const std::vector<String>& mineLines, const std::vector<String>& theirsLines);

/**
 * @brief R1 within one conflict: both sides added lines at the same place,
 * and one side's addition appears whole, in order and at the same
 * indentation, inside the other's.
 * @return The side whose addition holds the other's, None when neither does,
 *         when they are the same, or when the holding side opens a
 *         preprocessor conditional around the other's lines.
 */
DuplicateDrop ChooseContainedAdditionToDrop(const std::vector<String>& mineLines, const std::vector<String>& theirsLines,
	const LanguageTraits& traits);

/** @brief Which side moved a definition (R3). */
enum class MovedSide
{
	None,
	Mine,
	Theirs,
};

/**
 * @brief R3: the side whose definition no longer overlaps its base position,
 * while the other side's still does. Ranges are in the aligned line numbers
 * all three panes share.
 */
MovedSide DetectMovedDefinition(int baseStart, int baseEnd, int mineStart, int mineEnd, int theirsStart, int theirsEnd);

/**
 * @brief R3: the moved definition with the editor's change applied.
 * @return false when the editor only re-laid the definition out (a pure move,
 *         which the line merge handles) or the changes cannot be merged.
 */
bool MergeMovedDefinition(const std::vector<String>& baseLines, const std::vector<String>& moverLines,
	const std::vector<String>& editorLines, const LanguageTraits& traits, std::vector<String>& mergedLines);

} // namespace SemanticMerge
