# AMP-5 Spike: RichTextFX Feasibility for CrystalEdit Replacement

## Scope and Decision

- Issue: [AMP-5](/AMP/issues/AMP-5)
- Goal: determine whether RichTextFX can replace CrystalEdit for Phase 2c.
- Required decision: go/no-go on editor foundation plus parser strategy.

## CrystalEdit Parser Inventory

- Source scanned: `Externals/crystaledit/editlib/parsers/`
- `*.cpp` files found: 47
- Language parsers (excluding `crystallineparser.cpp` core): 46
- Detailed inventory: `migration-reports/pre-migration/spikes/richtextfx/parser-inventory.json`

## Capability Evaluation: RichTextFX `CodeArea`

| Capability | Evidence | Assessment |
| --- | --- | --- |
| Syntax highlighting extensibility | RichTextFX positions `CodeArea` as a base for code editors and supports arbitrary style ranges. | **Pass** |
| Line numbering | RichTextFX documents paragraph graphics/line numbers support. | **Pass** |
| Code folding | No first-class folding API is documented in RichTextFX core; folding must be custom-built on top of paragraph graphics/visibility controls. | **Partial** |
| Undo/redo | `CodeArea` exposes editor undo/redo behavior and the PoC wires undo/redo actions directly. | **Pass** |
| Large file performance | RichTextFX describes itself as memory-efficient and uses virtualized text rendering (Flowless-based). | **Pass with validation needed** |

## Parser Backend Options

### Option A: RichTextFX + TextMate (TM4E Core)

Strengths:
- TM4E provides a Java TextMate engine and is embeddable in Java applications.
- TM4E language pack includes 50+ grammars/configs, exceeding current CrystalEdit parser count.
- Good bootstrap path for broad language coverage with consistent token scopes/themes.

Risks:
- TM4E is optimized around Eclipse ecosystem defaults; integration work is still required for JavaFX + RichTextFX token stream bridging.
- Grammar quality varies by upstream TextMate grammar.

Assessment:
- **Best near-term coverage strategy** for replacing 40+ language parsers quickly.

### Option B: RichTextFX + Tree-sitter

Strengths:
- Tree-sitter provides incremental parsing with robust syntax trees and query-based highlighting.
- Better long-term path for semantic features (folding, structural selection, symbol navigation).

Risks:
- Official `java-tree-sitter` currently targets newer JDK tooling (build notes call for JDK 23+), creating compatibility friction for a JDK 21 migration baseline.
- Per-language grammar integration and query tuning has higher implementation overhead than TextMate tokenization.

Assessment:
- **Strong long-term architecture**, but higher immediate delivery risk for Phase 2c schedule.

## Minimal PoC (C++, Java, Python)

PoC location:
- `migration-reports/pre-migration/spikes/richtextfx/poc/`

Implemented:
- JavaFX app with RichTextFX `CodeArea`.
- Syntax highlighting for:
  - C++
  - Java
  - Python
- Line numbers enabled.
- Undo/redo controls wired.
- Language switcher with representative sample source per language.

Build/run helpers:
- `scripts/download-deps.sh`
- `scripts/build.sh`
- `scripts/run.sh`

## Recommendation (Go/No-Go)

Decision:
- **GO with RichTextFX foundation**, paired with **TM4E/TextMate as primary parser backend** for Phase 2c.

Execution strategy:
- Phase 2c baseline: RichTextFX + TM4E grammar-driven highlighting for broad language support.
- Parallel hardening track: prototype tree-sitter for high-value languages where structural tooling or precise folding is required.
- Accept feature parity trajectory (not 1:1 implementation parity at first cut), then close gaps iteratively.

## Open Risks and Mitigations

1. Folding parity risk:
- Risk: no built-in RichTextFX folding abstraction.
- Mitigation: implement fold markers + hidden paragraph ranges incrementally; prioritize large-file folding scenarios.

2. Tokenization throughput risk on huge files:
- Risk: multi-language regex/token bridge can stall UI if done on UI thread.
- Mitigation: debounce + background tokenization + incremental visible-range updates.

3. Theme parity risk vs WinMerge color schemes:
- Risk: TextMate scopes must map cleanly to WinMerge syntax color expectations.
- Mitigation: scope-to-token theme adapter layer and snapshot tests for key languages.

## References

- RichTextFX README: <https://github.com/FXMisc/RichTextFX>
- RichTextFX Javadoc index: <https://fxmisc.github.io/richtext/javadoc/0.11.4/>
- TM4E README: <https://github.com/eclipse-tm4e/tm4e>
- Tree-sitter highlighting docs: <https://tree-sitter.github.io/tree-sitter/3-syntax-highlighting.html>
- Java Tree-sitter bindings: <https://github.com/tree-sitter/java-tree-sitter>
