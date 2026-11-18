/**
 * @file  StructuralDiff.h
 *
 * @brief Declaration of a pure, structural (AST) diff engine.
 *
 * This is a from-scratch C++ port of the core idea behind difftastic: instead of
 * diffing lines, diff two syntax trees by finding the lowest-cost route through a
 * graph whose vertices are *pairs of positions* in the two trees. Matching nodes
 * are cheap, novel (added/removed) nodes are expensive, so the shortest path marks
 * the smallest set of nodes as changed. This naturally handles moved, wrapped and
 * reformatted code that a line diff reports as wholesale rewrites.
 *
 * The engine is intentionally free of any MFC, tree-sitter or WinMerge dependency:
 * it operates on a generic @ref structdiff::Node tree built by the caller. That
 * keeps it unit-testable in isolation (it compiles and runs with a plain C++
 * compiler) and decoupled from the integration layers that build the tree from a
 * tree-sitter parse and map the result back onto WinMerge's diff display.
 *
 * Scope of this first version (MVP): it produces, for every node, a @ref
 * structdiff::ChangeKind of Unchanged or Novel. The readability optimizations from
 * difftastic (unchanged-subtree pre-detection, sliders, depth/punctuation cost
 * tuning, comment/string similarity edges) are deliberately deferred - they affect
 * performance and aesthetics, not the correctness of the Unchanged/Novel marking.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace structdiff
{

/** @brief A node is either a leaf token (Atom) or a delimited sequence (List). */
enum class NodeKind
{
	Atom, /**< A leaf token: identifier, literal, keyword, punctuation, comment. */
	List, /**< A delimited sequence: open delimiter, children, close delimiter. */
};

/** @brief Whether a node corresponds to a node on the other side, or is added/removed. */
enum class ChangeKind
{
	Unchanged, /**< This node (or, for a List, its delimiters) matches the other side. */
	Novel,     /**< This node is present on only one side. */
};

/**
 * @brief One node of a simplified syntax tree.
 *
 * The caller fills in @ref kind, the content fields, @ref children and (optionally)
 * the source position. Everything below the "filled by the engine" line is computed
 * by @ref Diff and must not be set by the caller.
 *
 * For a List, @ref change describes the *delimiters* (e.g. the `(` and `)`): each
 * child carries its own @ref change. A whole identical subtree is marked Unchanged
 * on the node and all descendants.
 */
struct Node
{
	NodeKind kind = NodeKind::Atom;

	// ---- caller-provided content ----
	std::string content;      /**< Atom: the token text. */
	std::string openContent;  /**< List: the open delimiter text, e.g. "(" or "{". */
	std::string closeContent; /**< List: the close delimiter text, e.g. ")" or "}". */
	std::vector<Node*> children; /**< List: ordered child nodes. */

	// ---- caller-provided source position (0-based; optional) ----
	int startLine = 0;
	int startCol = 0;
	int endLine = 0;
	int endCol = 0;

	// ---- filled by the engine; do not set manually ----
	int id = 0;            /**< Unique id (1..N) across both trees. 0 = not yet assigned. */
	int contentId = 0;     /**< Equal content id  <=>  identical text & structure. */
	Node* parent = nullptr;
	Node* firstChild = nullptr;
	Node* nextSibling = nullptr;
	int numAncestors = 0;
	ChangeKind change = ChangeKind::Novel;
};

/**
 * @brief Owns the lifetime of all @ref Node objects (simple arena).
 *
 * Build nodes with @ref newAtom / @ref newList; the arena frees them on destruction.
 * Nodes must outlive any @ref Diff call that uses them.
 */
class Arena
{
public:
	Node* newAtom(std::string content, int startLine = 0, int startCol = 0, int endLine = 0, int endCol = 0);
	Node* newList(std::string openContent, std::string closeContent, std::vector<Node*> children,
		int startLine = 0, int startCol = 0, int endLine = 0, int endCol = 0);

private:
	std::vector<std::unique_ptr<Node>> m_nodes;
};

/** @brief Outcome of a @ref Diff run. */
struct DiffResult
{
	/**
	 * @brief True if the search hit @p graphLimit before completing.
	 *
	 * When this happens the node marks are left at their default (Novel) and the
	 * caller is expected to fall back to a line-based diff.
	 */
	bool exceededLimit = false;

	int verticesExplored = 0; /**< Diagnostic: number of distinct graph vertices created. */
};

/**
 * @brief Compute the structural diff of two top-level node sequences.
 *
 * On success every node reachable from @p lhsRoots / @p rhsRoots has its @ref
 * Node::change set. @p lhsRoots and @p rhsRoots are the ordered top-level nodes of
 * each side (as if children of a virtual root). The trees are mutated in place
 * (ids, links and change marks); they are not otherwise modified.
 *
 * @param lhsRoots    Top-level nodes of the left side, in order.
 * @param rhsRoots    Top-level nodes of the right side, in order.
 * @param graphLimit  Maximum number of distinct vertices to explore before giving up.
 *                    Bounds worst-case time and memory; on overflow @ref
 *                    DiffResult::exceededLimit is set and the caller should fall
 *                    back to a line diff. The default is a conservative guard -
 *                    tune it (up or down) when the integration knows the file size.
 * @return            Result flags (see @ref DiffResult).
 */
DiffResult Diff(const std::vector<Node*>& lhsRoots, const std::vector<Node*>& rhsRoots,
	int graphLimit = 250000);

} // namespace structdiff
