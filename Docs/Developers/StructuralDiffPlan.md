# Structural (AST) Diff — Design & Plan

## Motivation

WinMerge's comparison is line-oriented: tree-sitter is layered on top for
highlighting, navigation, the semantic-merge analyzer and the comment filter, but
the diff *alignment* itself is still computed line-by-line (diffutils / xdiff).
This is the one area where difftastic is genuinely ahead: it diffs the *syntax
trees*, so moved, wrapped and reformatted code is shown as a small structural
change instead of a wholesale add/delete.

This document describes a structural-diff mode for WinMerge that reuses the
tree-sitter parses we already produce, and tracks its implementation.

## Approach (ported from difftastic)

Diffing is modeled as a **shortest-path search over a graph of position pairs**.

* A **vertex** is a triple `(lhs node, rhs node, stack of entered delimiters)` —
  "we are about to process *this* node on the left and *that* node on the right,
  having entered these delimiters".
* **Edges** advance one or both positions:
  * `UnchangedNode` — both nodes have equal content (identical subtree). Cheap.
  * `EnterUnchangedDelimiter` — both are lists whose delimiters match; recurse into
    the children (which are diffed by later edges). Moderate.
  * `NovelAtomLHS/RHS`, `EnterNovelDelimiterLHS/RHS` — consume / enter a node on one
    side only. Expensive.
* Because matching is cheap and novelty is expensive, the **minimum-cost route**
  marks the fewest nodes as changed. The delimiter stack keeps matched `(...)`
  pairs aligned (`PopBoth`) while still letting one-sided nesting unwind
  independently (`PopEither`).

**Vertex identity (critical for performance).** A vertex is *not* keyed on its full
delimiter stack. Keying on the whole stack is "strictly correct" but makes the graph
**exponential in nesting depth** — one-sided enters/pops reach the same node pair with
a combinatorial number of distinct stacks (measured: ~1.65^depth, so a 50-deep change
never terminates). Following difftastic (`graph.rs` `impl PartialEq for Vertex`), we
key on a cheap path-dependent summary — `(lhs id, rhs id, lhs parent id, rhs parent
id, can-pop-either)` — and keep a *bounded* number of genuinely distinct stacks per
key (`kMaxNestingsPerKey = 2`). The first (lowest-cost) vertex to reach a key wins.
This makes the graph polynomial; the residual worst case is `O(depth²)` on
pathological all-novel nesting, bounded by `graphLimit` (and shrinkable later by
unchanged-subtree pre-detection).

Costs (initial): `UnchangedNode = 1`, `EnterUnchangedDelimiter = 100`, any novel
edge `= 300`. These reproduce difftastic's preference ordering for the cases that
matter; the depth/punctuation refinements are deferred (see below).

## Architecture: pure core + integration layers

Following the pattern set by `SemanticMergeAnalyzer` (a pure, MFC-free, unit-tested
core with thin integration around it):

```
  ┌──────────────────────────────────────────────────────────────┐
  │ Phase 2: TSTree → structdiff::Node builder   (Windows only)   │
  │   walk the tree-sitter parse, emit Atom/List nodes + spans    │
  └───────────────┬──────────────────────────────────────────────┘
                  ▼
  ┌──────────────────────────────────────────────────────────────┐
  │ Phase 1: structdiff core   StructuralDiff.{h,cpp}   ✅ DONE    │
  │   pure C++, no tree-sitter / MFC; Dijkstra position-pair diff │
  │   → marks every node Unchanged | Novel                        │
  └───────────────┬──────────────────────────────────────────────┘
                  ▼
  ┌──────────────────────────────────────────────────────────────┐
  │ Phase 3: ChangeKind → WinMerge display   (Windows only)       │
  │   line blocks (DIFFRANGE / DiffList) + intra-line WordDiff     │
  └──────────────────────────────────────────────────────────────┘
```

The core is deliberately decoupled so it can be (and is) compiled and unit-tested
with a plain C++ compiler, away from the Windows-only build.

## Status

### Phase 1 — pure core — **DONE & validated**

* `Src/StructuralDiff.h` / `Src/StructuralDiff.cpp` — `namespace structdiff`:
  `Node`, `Arena`, `ChangeKind`, `Diff()`. No tree-sitter / MFC dependency.
* Validated with a standalone harness (and mirrored as GoogleTest in
  `Testing/GoogleTest/StructuralDiff/StructuralDiff_test.cpp`) covering:
  identical, single-token change, **wrapping `x` vs `(x)`**, reorder
  (one match per side), insertion, nested inner change, **one-sided nesting on
  both sides** (`(a)` vs `{a}` — stresses the `PopEither` unwinding), the
  **graph-limit fallback** (on overflow, marks stay Novel and the caller falls
  back to a line diff), and two **deep-nesting** guards: depth-200000 identical
  (no call-stack overflow — every traversal is iterative; tiny graph) and
  depth-100 inner-change (stays polynomial, would be exponential under naive
  full-stack vertex keying).
* Wired into `Src/Merge.vcxproj`(+`.filters`) and the unit-test project
  `Testing/GoogleTest/UnitTests/UnitTests.vcxproj`.

### Phase 2 — tree-sitter → node tree — TODO

Build a `structdiff::Node` forest from a parsed file. Reuse the existing diff-time
parse contexts:

* `CreateTreeSitterParseContextForDiff(path, lines)` already parses a file's exact
  diffed text and returns an opaque `CTreeSitterParser*`
  (`Src/TreeSitterWrapper.{h,cpp}`); it is created per hunk inside
  `CDiffWrapper::PostFilter` (`Src/DiffWrapper.cpp`). For a whole-file structural
  diff we'd create it once for each side.
* Add a tree walk on the `TSTree` (in `CTreeSitterParser`) that emits:
  * **Atom** for leaf / anonymous tokens (identifiers, literals, keywords,
    punctuation, comments) — `content` = token text, with line/col span.
  * **List** for named nodes that have children — `openContent`/`closeContent` =
    the bracket/delimiter tokens (difftastic's `delimiter_tokens`), children =
    the rest.
  * Insignificant whitespace is dropped (positions are retained on the nodes).
* Mirror difftastic's per-language knobs later (`atom_nodes`,
  `ignore_trailing_tokens`); not required for a first cut.

### Phase 3 — map result onto the display — TODO

`Diff()` leaves each node tagged Unchanged/Novel with a source span. Map that onto
WinMerge's existing structures (no renderer changes needed):

* Group novel nodes into line ranges → `DIFFRANGE` blocks in
  `CMergeDoc::m_diffList` (`Src/DiffList.h`).
* Emit per-line novel character ranges as `WordDiff` (`Src/MergeDoc.h`), which the
  view already renders via `CMergeEditView::GetAdditionalTextBlocks`.

Integration options:
1. **New algorithm value** `DIFF_ALGORITHM_STRUCTURAL` alongside the existing
   `m_diffAlgorithm` choices; `CDiffWrapper` runs the structural engine when a
   grammar is available and falls back to line diff otherwise (and on
   `exceededLimit`).
2. **Refinement pass** that keeps diffutils line blocks but replaces the
   intra-line `WordDiff` with structural novel ranges.

Option 1 is the closer match to difftastic; option 2 is a smaller, lower-risk
first step.

### Phase 4 — quality & perf — TODO (deferred, not correctness-critical)

* Unchanged-subtree pre-detection (difftastic `unchanged.rs` `mark_unchanged`) to
  shrink the graph before the search. This is the main remaining perf lever: it
  collapses identical subtrees so the `O(depth²)` graph never sees deep nesting in
  real files. Not required for *correctness* (the bounded vertex key already prevents
  the exponential blowup), but it lifts the depth at which large files hit
  `graphLimit`.
* Sliders (`sliders.rs`) to make equally-valid diffs more readable.
* Depth / punctuation cost tuning; comment/string similarity edges
  (`ReplacedComment`/`ReplacedString`).
* A user-facing option + menu entry, and an `exceededLimit`/no-grammar fallback.

Done in Phase 1 (was previously listed here): persistent shared-tail stack instead of
per-edge copies, and difftastic's bounded vertex dedup (`kMaxNestingsPerKey`) so the
graph stays polynomial in nesting depth.

## Testing

`Testing/GoogleTest/StructuralDiff/StructuralDiff_test.cpp` holds the Phase-1
cases. The pure core also builds with a stock compiler, e.g.:

```
g++ -std=c++17 Src/StructuralDiff.cpp your_harness.cpp -o t   # (with an empty pch.h)
```

## References

* difftastic algorithm: `difftastic/src/diff/{graph,dijkstra,unchanged,sliders}.rs`,
  `difftastic/src/parse/syntax.rs`.
* WinMerge diff representation: `Src/DiffList.h` (`DIFFRANGE`/`DiffList`),
  `Src/MergeDoc.h` (`WordDiff`), `Src/DiffWrapper.cpp`
  (`RunFileDiff`/`Diff2Files`/`PostFilter`), `Src/TreeSitterWrapper.h`.
