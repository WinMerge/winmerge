# Semantic Merge

Tree-sitter based conflict resolution in the 3-way merge, as implemented on
`feature/tree-sitter-semantic-merge-v3`.

## 1. What it does

A line merge reports a conflict whenever both sides changed overlapping lines,
even when one side moved a function and the other edited it, or both edited
different words of a line. Semantic merge resolves such conflicts with the
syntax tree and leaves all others to the user.

It runs after the line merge, only on what that left unresolved (plus rule
R1), writes only to the Merge Result pane, and every resolution is shown,
explained and undoable.

**Terms.** *Base, mine, theirs*: the panes from the merge pane mapping
(`GetMergePaneMapping`); the base is not necessarily the middle pane.
*Conflict*: a pending result segment with `OP_DIFF`. *Definition*: a named
range from the language's `<language>-tags.scm` query.

**Out of scope:** 2-way and folder compare, merging without the result pane,
edits to the compared panes, both sides changing the same tokens, two different
additions at one place (committers keep both in only ~60% of real merges),
C/C++/C# preprocessor lines, statement reordering.

## 2. Enabling

One option, `OPT_SEMANTIC_MERGE` (`Settings/SemanticMerge`), off by default:
**Options > Editor > Compare/Merge** or **Merge > Semantic Merge > Resolve
Conflicts Semantically (Experimental)**.

It needs three files, the Merge Result Pane option on, and a grammar that
parses all three panes (`CMergeDoc::GetSemanticMergeUnavailableReason`). It
does not depend on syntax highlighting: `CMergeEditView::GetTreeSitterParser`
creates a parser when highlighting has none.

## 3. Pipeline

1. R1 runs once over the whole file.
2. For each conflict in order, R1 within the conflict, then R2–R6; the first
   rule that succeeds resolves it.

Code: `CMergeDoc::DoSemanticMergeConflictsToResult` and
`TryResolveConflictSemantically` (`MergeDoc.cpp`). Decisions are MFC-free in
`SemanticMergeAnalyzer.{h,cpp}`.

| Guard | Rule |
|---|---|
| G1 Explicit roles | Base, mine and theirs come from the pane mapping, never from a majority vote. |
| G2 Unique names | A definition is matched by name only when the name occurs once in each pane. |
| G3 Hands off | Chosen, Edited and Semantic segments are never rewritten, nor one whose surrounding common text was edited (`IsResultCommonTextIntact`). |
| G4 Syntax check | The result may not have more syntax errors than the worse side (`IsSemanticResultSyntaxOk`). |
| G5 Text, not layout | White space and blank lines are ignored, except in Python and F#. |

## 4. Rules

| Rule | Situation | Result |
|---|---|---|
| R1 Duplicate addition | Both sides added the same definition | One copy |
| R2 Token merge | Both changed the same lines, different words | Both changes |
| R3 Moved definition | One moved a definition, the other edited it | Moved, with the edit |
| R4 One-sided change | Only one side changed the definition | That side's definition |
| R5 Small change on refactoring | One refactored, the other made one small change | Refactoring plus the change |
| R6 Commented-out update | One commented it out, the other edited it | Edited, commented out |

### R1 Duplicate addition

The line merge takes both additions, so the definition ends up twice.

- **When:** absent from base, once in each side, same text and same enclosing
  definition (in Python/F# indentation counts), each copy a pending one-sided
  addition holding the whole definition.
- **Result:** the addition with nothing the other lacks is left out.
- **Refuses:** different texts or enclosing definitions; neither addition a
  superset; leaving out a copy that replaced base lines (the other copy is
  dropped instead when possible).
- **Within one conflict** (base empty there): when one side's lines appear
  whole, in order and at the same indentation inside the other's, the larger
  side is taken. Refused when the larger side put them inside its own block
  or an `#if` it opened.
- **Code:** `ResolveDuplicateAdditionsInResult` / `ChooseDuplicateAdditionToDrop`;
  `TryContainedAdditionToResult` / `ChooseContainedAdditionToDrop`.

### R2 Token merge

- **When:** each side's change is computed by LCS over tokens (a string literal
  or line comment is one token) and the changes touch different tokens.
- **Result:** base with both changes. White space is merged three ways too, so
  a re-indented line keeps its indentation.
- **Refuses:** empty base; overlapping changes; a side changing more tokens
  than the region has (min 4; replay showed this cap keeps out a renamed call
  plus a re-typed argument); the same word inserted by both sides in different
  places; both sides adding lines in Python/F#; preprocessor lines; over
  60,000 tokens or a length product over 6,000,000.
- **Code:** `TryTokenMergeConflictToResult` / `MergeTokensThreeWay`.

### R3 Moved definition

- **When:** the conflict lies in a base definition that no longer overlaps its
  base position in one side (the mover) but does in the other (the editor),
  and the editor changed nothing else there.
- **Result:** the mover's layout with the editor's change; a mover's own
  change is combined by R2.
- **Refuses:** definitions sharing lines; an editor that only reformatted
  (a plain move, which the line merge handles); token merge failing.
- **Code:** `TryMovedDefinitionMergeToResult` / `DetectMovedDefinition`,
  `MergeMovedDefinition`.

### R4 One-sided definition change

- **When:** the definition is identical in base and one side and differs in the
  other; the conflict comes from neighbouring changes.
- **Result:** the changed side's definition.
- **Refuses:** changed on both sides or missing in a pane; a change on the
  definition's first or last line outside its range (e.g. `export` before
  it), which would be lost. R5 refuses the same.

### R5 Small change onto a refactored definition

- **When:** the small side made exactly one of these, and nothing else:
  - one string literal changed, old literal once in the refactored version;
  - one parameter renamed with one use site, old name twice in the refactored
    version (C-like languages);
  - one identifier renamed in 2–4 places, same count in the refactored version.
- **Result:** the refactored definition with the change replayed.
- **Refuses:** a second difference; a non-unique target; a new name the
  refactored version already uses.

### R6 Commented-out definition update

- **When:** the commenting side's block has the same line-comment prefix on
  every non-blank line and, uncommented, equals the base definition; the other
  side edited it.
- **Result:** the edited definition, commented out with that prefix.
- **Refuses:** no match with base, mixed prefixes, block comments.

R4–R6 are decided by `SemanticMerge::Analyzer::TryBuildSuggestion` and applied
by `CMergeDoc::ApplySemanticSuggestionToResult`.

**Removed** (not merges): inserting a one-sided addition next to its neighbour,
copying independent additions into the base, adopting the version two panes
agree on.

## 5. Entry points

| Entry point | Does | Code |
|---|---|---|
| Auto Merge, option on | Pipeline, then the usual merge of non-conflicts | `ApplyAutoMergeToResult` |
| Resolve Conflict Semantically (`Ctrl+Alt+Shift+M`) | R2–R6 on the conflict at the cursor; otherwise says why each rule declined | `ResolveConflictSemantically` |
| Resolve All Conflicts Semantically (`Ctrl+Alt+Shift+A`) | Pipeline only | `DoSemanticMergeConflictsToResult` |
| `-semantic` with `/al`, `/am` or `/ar` | Like Auto Merge, for this run only | `CMergeApp::GetSemanticMergeForRun` |

The commands are in **Merge > Semantic Merge** and the result pane's context
menu. Classic Auto Merge (no result pane) does not run semantic merge.

## 6. Languages

R2 works with any grammar; the other rules need a tags query: c, cpp, c_sharp,
go, java, javascript (flow, tsx), typescript, php, python, ruby, rust, fsharp.
Traits (`SemanticMerge::TraitsForLanguage`):

| Languages | Line comment | Quotes | C params (R5) | Rename (R5) | Preprocessor | Layout |
|---|---|---|---|---|---|---|
| c, cpp, c_sharp | `//` | `"` | yes | yes | yes | no |
| java | `//` | `"` | yes | yes | no | no |
| javascript, typescript, tsx, flow | `//` | `"` `'` | yes | yes | no | no |
| php | `//` `#` | `"` `'` | yes | yes | no | no |
| python | `#` | `"` `'` | no | yes | no | yes |
| ruby, bash | `#` | `"` `'` | no | yes | no | no |
| go, rust | `//` | `"` | no | yes | no | no |
| fsharp | `//` | `"` | no | yes | no | yes |
| others | – | – | no | no | no | no |

## 7. Visibility

- Segment state `ResultSegmentState::Semantic` with rule and reason (not
  `Edited`, which means a hand edit).
- `s` in the result margin, moved-block colours, tooltip with rule and reason.
- Next/Previous Semantic Resolution in the result pane's context menu.
- A "Resolved semantically: N" report by rule; each resolution in the Output
  pane.
- "Semantic: N" in the status bar; the rule at the caret.
- One Undo step per run or command; Choose Mine/Base/Theirs overrides it.
- Saved as plain text, no conflict markers.

## 8. Verification

Each rule has unit tests (`Testing/GoogleTest/SemanticMerge`) and a manual
triple in `Testing/Data/SemanticMergeManual/`, named
`Semantic<Case>3Way{Left,Middle,Right}.<ext>` with middle = base:

```
WinMergeU.exe -noprefs -noninteractive -semantic -am -o out.cpp L.cpp M.cpp R.cpp
```

| Triple | Expect |
|---|---|
| `SemanticDuplicate3Way*` | R1 |
| `SemanticDuplicateReplaced3Way*` | R1; the replaced base definition does not come back |
| `SemanticMoved3Way*` | R3 |
| `SemanticStringLiteral3Way*`, `SemanticParameter3Way*`, `SemanticIdentifier3Way*` | R5 |
| `SemanticCommented3Way*` | R6 |
| `SemanticAddition3Way*` | Conflict (two different additions) |
| `SemanticSameName3Way*` | Both kept (same method in two classes) |
| `SemanticWrapped3Way*` | Conflict (same call, one inside an `if`) |

A rule change must pass the replay (`Testing/SemanticMergeReplay/Replay.ps1`):
`tokens` merged and confirmed counts do not drop; `run` moves no case from
match to differs, and each new "differs" is read by hand.

Baseline 2026-11-18, 395 files from 9 repositories:

| | Match | Differ | Keep conflicts | Conflicts left |
|---|---|---|---|---|
| `run`, line merge | 16 | 5 | 374 | 781 |
| `run -semantic` | 46 | 15 | 334 | 640 |

`tokens`: 832 conflicts, 165 merged, 128 confirmed by the commit.

## 9. Open

- The margin tooltip and Next/Previous Semantic Resolution are unchecked in a
  running app (MFC shows tooltips only in the active window).
- After a rewrite resolution, Undo of Auto Merge can take two steps; a rolled
  back rewrite stays available to Redo.
- The `Docs/Manual` changes need a pull request to `winmerge-manual`.
- Separate pull requests, not this feature: tree-sitter breadcrumb
  (`OPT_TREE_SITTER_BREADCRUMB`), occurrence highlighting
  (`OPT_TREE_SITTER_OCCURRENCE_HIGHLIGHT`), `/checkonly`, Ignore Trailing
  Punctuation (`OPT_CMP_IGNORE_TRAILING_PUNCTUATION`), `StructuralDiff` (see
  `StructuralDiffPlan.md`), the filter date-literal DST fix.
