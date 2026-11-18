/**
 * @file  StructuralDiff_test.cpp
 *
 * @brief Unit tests for the pure structural (AST) diff engine (StructuralDiff.h).
 *
 * These exercise the Dijkstra position-pair diff on small hand-built node trees,
 * independent of tree-sitter and the WinMerge display. The same cases were first
 * validated with a standalone compiler; this file wires them into the GoogleTest
 * suite. They cover identical input, single-token change, wrapping (x vs (x)),
 * reorder, insertion, nested change, one-sided nesting on both sides (which
 * stresses the delimiter-stack unwinding), and the graph-limit fallback.
 */
#include "pch.h"
#include <gtest/gtest.h>
#include "StructuralDiff.h"
#include <functional>
#include <string>
#include <vector>

using structdiff::Arena;
using structdiff::ChangeKind;
using structdiff::Diff;
using structdiff::Node;

namespace
{

int CountUnchanged(const std::vector<Node*>& roots)
{
	int n = 0;
	std::function<void(Node*)> rec = [&](Node* node) {
		if (node->change == ChangeKind::Unchanged)
			++n;
		for (Node* c : node->children)
			rec(c);
	};
	for (Node* r : roots)
		rec(r);
	return n;
}

TEST(StructuralDiff, IdenticalIsAllUnchanged)
{
	Arena a;
	std::vector<Node*> L{ a.newAtom("a"), a.newAtom("b"), a.newAtom("c") };
	std::vector<Node*> R{ a.newAtom("a"), a.newAtom("b"), a.newAtom("c") };
	Diff(L, R);
	EXPECT_EQ(3, CountUnchanged(L));
	EXPECT_EQ(3, CountUnchanged(R));
}

TEST(StructuralDiff, SingleAtomChange)
{
	Arena a;
	Node *la = a.newAtom("a"), *lb = a.newAtom("b"), *lc = a.newAtom("c");
	Node *ra = a.newAtom("a"), *rx = a.newAtom("X"), *rc = a.newAtom("c");
	Diff({ la, lb, lc }, { ra, rx, rc });
	EXPECT_EQ(ChangeKind::Unchanged, la->change);
	EXPECT_EQ(ChangeKind::Novel, lb->change);
	EXPECT_EQ(ChangeKind::Unchanged, lc->change);
	EXPECT_EQ(ChangeKind::Unchanged, ra->change);
	EXPECT_EQ(ChangeKind::Novel, rx->change);
	EXPECT_EQ(ChangeKind::Unchanged, rc->change);
}

TEST(StructuralDiff, WrappingInParens)
{
	// x  vs  (x) : the atom matches; only the new parentheses are novel.
	Arena a;
	Node* lx = a.newAtom("x");
	Node* inner = a.newAtom("x");
	Node* list = a.newList("(", ")", { inner });
	Diff({ lx }, { list });
	EXPECT_EQ(ChangeKind::Unchanged, lx->change);
	EXPECT_EQ(ChangeKind::Unchanged, inner->change);
	EXPECT_EQ(ChangeKind::Novel, list->change);
}

TEST(StructuralDiff, ReorderMatchesExactlyOnePerSide)
{
	// a b vs b a : a sequential diff can keep only one pair aligned.
	Arena a;
	Node *la = a.newAtom("a"), *lb = a.newAtom("b");
	Node *rb = a.newAtom("b"), *ra = a.newAtom("a");
	Diff({ la, lb }, { rb, ra });
	EXPECT_EQ(1, CountUnchanged({ la, lb }));
	EXPECT_EQ(1, CountUnchanged({ rb, ra }));
}

TEST(StructuralDiff, Insertion)
{
	// a c vs a b c : only the inserted b is novel.
	Arena a;
	Node *la = a.newAtom("a"), *lc = a.newAtom("c");
	Node *ra = a.newAtom("a"), *rb = a.newAtom("b"), *rc = a.newAtom("c");
	Diff({ la, lc }, { ra, rb, rc });
	EXPECT_EQ(ChangeKind::Unchanged, la->change);
	EXPECT_EQ(ChangeKind::Unchanged, lc->change);
	EXPECT_EQ(ChangeKind::Unchanged, ra->change);
	EXPECT_EQ(ChangeKind::Novel, rb->change);
	EXPECT_EQ(ChangeKind::Unchanged, rc->change);
}

TEST(StructuralDiff, NestedInnerChange)
{
	// ([a b]) vs ([a X]) : delimiters and a match; only b/X are novel.
	Arena a;
	Node *la = a.newAtom("a"), *lb = a.newAtom("b");
	Node* lInner = a.newList("[", "]", { la, lb });
	Node* lOuter = a.newList("(", ")", { lInner });
	Node *ra = a.newAtom("a"), *rX = a.newAtom("X");
	Node* rInner = a.newList("[", "]", { ra, rX });
	Node* rOuter = a.newList("(", ")", { rInner });
	Diff({ lOuter }, { rOuter });
	EXPECT_EQ(ChangeKind::Unchanged, lOuter->change);
	EXPECT_EQ(ChangeKind::Unchanged, rOuter->change);
	EXPECT_EQ(ChangeKind::Unchanged, lInner->change);
	EXPECT_EQ(ChangeKind::Unchanged, rInner->change);
	EXPECT_EQ(ChangeKind::Unchanged, la->change);
	EXPECT_EQ(ChangeKind::Unchanged, ra->change);
	EXPECT_EQ(ChangeKind::Novel, lb->change);
	EXPECT_EQ(ChangeKind::Novel, rX->change);
}

TEST(StructuralDiff, OneSidedNestingBothSides)
{
	// (a) vs {a} : both delimiter pairs are novel, the inner a matches.
	// Exercises the independent (PopEither) unwinding of one-sided delimiters.
	Arena a;
	Node* la = a.newAtom("a");
	Node* lList = a.newList("(", ")", { la });
	Node* ra = a.newAtom("a");
	Node* rList = a.newList("{", "}", { ra });
	Diff({ lList }, { rList });
	EXPECT_EQ(ChangeKind::Novel, lList->change);
	EXPECT_EQ(ChangeKind::Novel, rList->change);
	EXPECT_EQ(ChangeKind::Unchanged, la->change);
	EXPECT_EQ(ChangeKind::Unchanged, ra->change);
}

TEST(StructuralDiff, GraphLimitFallback)
{
	Arena a;
	std::vector<Node*> L, R;
	for (int i = 0; i < 50; ++i)
	{
		L.push_back(a.newAtom("x" + std::to_string(i)));
		R.push_back(a.newAtom("y" + std::to_string(i)));
	}
	auto res = Diff(L, R, /*graphLimit*/ 5);
	EXPECT_TRUE(res.exceededLimit);
	// On limit the marks are left at their default (Novel) for the caller to fall back.
	EXPECT_EQ(0, CountUnchanged(L));
}

TEST(StructuralDiff, DeepIdenticalNestingDoesNotOverflowOrExplode)
{
	// 200000 nested identical lists. This must (a) not overflow the call stack -
	// every tree walk in the engine is iterative - and (b) explore a tiny graph,
	// because the whole identical structure collapses to UnchangedNode edges.
	Arena a;
	Node* l = a.newAtom("z");
	Node* r = a.newAtom("z");
	for (int i = 0; i < 200000; ++i)
	{
		l = a.newList("(", ")", { l });
		r = a.newList("(", ")", { r });
	}
	auto res = Diff({ l }, { r });
	EXPECT_FALSE(res.exceededLimit);
	EXPECT_EQ(ChangeKind::Unchanged, l->change);
}

TEST(StructuralDiff, DeepNestedInnerChangeStaysPolynomial)
{
	// 100 nested lists differing only in the innermost atom (x vs y). Keying a
	// vertex on its full delimiter stack would make this EXPONENTIAL in depth; the
	// engine instead keys on (node ids, parent ids, can-pop-either) with a bounded
	// number of stacks per key, so the graph stays small and the outer delimiters
	// are all matched - only the innermost atoms are novel.
	Arena a;
	Node* l = a.newAtom("x");
	Node* r = a.newAtom("y");
	for (int i = 0; i < 100; ++i)
	{
		l = a.newList("(", ")", { l });
		r = a.newList("(", ")", { r });
	}
	auto res = Diff({ l }, { r });
	EXPECT_FALSE(res.exceededLimit);
	EXPECT_EQ(ChangeKind::Unchanged, l->change); // outer delimiters matched
	Node* innermost = l;
	while (!innermost->children.empty())
		innermost = innermost->children.front();
	EXPECT_EQ(ChangeKind::Novel, innermost->change);
}

} // namespace
