/**
 * @file  StructuralDiff.cpp
 *
 * @brief Implementation of the pure structural (AST) diff engine. See StructuralDiff.h.
 *
 * The algorithm is Dijkstra's shortest path over a graph whose vertices are pairs
 * of positions in the two syntax trees. A vertex is (lhs node, rhs node, stack of
 * entered delimiters). Edges advance one or both positions:
 *
 *   - UnchangedNode            both nodes have equal content id (identical subtree)
 *   - EnterUnchangedDelimiter  both are lists whose delimiters match; recurse
 *   - NovelAtom{LHS,RHS}       consume a leaf on one side only
 *   - EnterNovelDelimiter{...} recurse into a list on one side only
 *
 * Matching is cheap and novelty is expensive, so the minimum-cost route marks the
 * fewest nodes as Novel. The delimiter stack keeps matched "(...)" pairs aligned
 * (PopBoth) while still allowing one-sided nesting to be unwound independently
 * (PopEither).
 */
#include "pch.h"
#include "StructuralDiff.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <queue>
#include <unordered_map>
#include <utility>

namespace structdiff
{

// ---------------------------------------------------------------------------
// Arena
// ---------------------------------------------------------------------------

Node* Arena::newAtom(std::string content, int startLine, int startCol, int endLine, int endCol)
{
	auto node = std::make_unique<Node>();
	node->kind = NodeKind::Atom;
	node->content = std::move(content);
	node->startLine = startLine;
	node->startCol = startCol;
	node->endLine = endLine;
	node->endCol = endCol;
	Node* raw = node.get();
	m_nodes.push_back(std::move(node));
	return raw;
}

Node* Arena::newList(std::string openContent, std::string closeContent, std::vector<Node*> children,
	int startLine, int startCol, int endLine, int endCol)
{
	auto node = std::make_unique<Node>();
	node->kind = NodeKind::List;
	node->openContent = std::move(openContent);
	node->closeContent = std::move(closeContent);
	node->children = std::move(children);
	node->startLine = startLine;
	node->startCol = startCol;
	node->endLine = endLine;
	node->endCol = endCol;
	Node* raw = node.get();
	m_nodes.push_back(std::move(node));
	return raw;
}

namespace
{

// ---------------------------------------------------------------------------
// Hashing helpers
// ---------------------------------------------------------------------------

inline void hashCombine(std::size_t& seed, std::size_t value)
{
	// 64-bit variant of the well-known boost::hash_combine mixer.
	seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

// ---------------------------------------------------------------------------
// Persistent (immutable, shared) stack of entered delimiters
// ---------------------------------------------------------------------------

// A frame is either:
//   - Both:   we entered a matching "(...)" pair on both sides together. We may
//             only leave it (pop) once both sides have consumed all children.
//   - Either: we entered novel "(...)" on one side only. Consecutive one-sided
//             entries (on either side) are merged into a single Either frame with
//             two independent return stacks, so they can be unwound in any order.
struct Frame
{
	bool both = false;
	// Both:
	Node* lhsNode = nullptr; // the matched list node on the left
	Node* rhsNode = nullptr; // the matched list node on the right
	// Either: list nodes we entered, innermost last; resume at node->nextSibling.
	std::vector<Node*> lhsReturns;
	std::vector<Node*> rhsReturns;
};

struct StackNode
{
	Frame frame;
	std::shared_ptr<const StackNode> below;
	std::size_t hash = 0;
};

using Stack = std::shared_ptr<const StackNode>;

std::size_t frameHash(const Frame& f)
{
	std::size_t h = f.both ? 0x11 : 0x22;
	if (f.both)
	{
		hashCombine(h, f.lhsNode ? static_cast<std::size_t>(f.lhsNode->id) : 0);
		hashCombine(h, f.rhsNode ? static_cast<std::size_t>(f.rhsNode->id) : 0);
	}
	else
	{
		hashCombine(h, 0xAAAA);
		for (Node* n : f.lhsReturns)
			hashCombine(h, static_cast<std::size_t>(n->id));
		hashCombine(h, 0xBBBB);
		for (Node* n : f.rhsReturns)
			hashCombine(h, static_cast<std::size_t>(n->id));
	}
	return h;
}

bool frameEqual(const Frame& a, const Frame& b)
{
	if (a.both != b.both)
		return false;
	if (a.both)
		return a.lhsNode == b.lhsNode && a.rhsNode == b.rhsNode;
	return a.lhsReturns == b.lhsReturns && a.rhsReturns == b.rhsReturns;
}

// Structural equality of two persistent stacks. Pointer-identical tails short-
// circuit (the common case, since a push shares its tail); otherwise compare frame
// by frame. This keeps vertex deduplication SOUND - a stack-hash match alone is
// never trusted as identity - while staying cheap.
bool stackEqual(const Stack& a, const Stack& b)
{
	const StackNode* pa = a.get();
	const StackNode* pb = b.get();
	while (pa && pb)
	{
		if (pa == pb)
			return true; // shared tail: everything below is identical
		if (pa->hash != pb->hash || !frameEqual(pa->frame, pb->frame))
			return false;
		pa = pa->below.get();
		pb = pb->below.get();
	}
	return pa == pb; // equal only if both reached the bottom together
}

Stack makeStack(Frame frame, const Stack& below)
{
	auto node = std::make_shared<StackNode>();
	node->frame = std::move(frame);
	node->below = below;
	node->hash = frameHash(node->frame);
	hashCombine(node->hash, below ? below->hash : 0);
	return node;
}

Stack pushBoth(const Stack& stack, Node* lhsNode, Node* rhsNode)
{
	Frame f;
	f.both = true;
	f.lhsNode = lhsNode;
	f.rhsNode = rhsNode;
	return makeStack(std::move(f), stack);
}

Stack pushEitherLhs(const Stack& stack, Node* node)
{
	if (stack && !stack->frame.both)
	{
		Frame f = stack->frame; // copy the existing Either frame
		f.lhsReturns.push_back(node);
		return makeStack(std::move(f), stack->below);
	}
	Frame f;
	f.both = false;
	f.lhsReturns.push_back(node);
	return makeStack(std::move(f), stack);
}

Stack pushEitherRhs(const Stack& stack, Node* node)
{
	if (stack && !stack->frame.both)
	{
		Frame f = stack->frame;
		f.rhsReturns.push_back(node);
		return makeStack(std::move(f), stack->below);
	}
	Frame f;
	f.both = false;
	f.rhsReturns.push_back(node);
	return makeStack(std::move(f), stack);
}

// After advancing a position, unwind any delimiters whose side(s) are now finished,
// resuming at the entered list node's next sibling. Mutates L, R, the parent ids and
// the stack in place. The parent ids track the immediate enclosing list on each side
// (0 = top level); they are part of the vertex identity, so a position reached at the
// end of two different lists is correctly kept distinct.
void popAllParents(Node*& L, Node*& R, int& lhsParentId, int& rhsParentId, Stack& stack)
{
	while (stack)
	{
		const Frame& f = stack->frame;
		if (f.both)
		{
			if (L == nullptr && R == nullptr)
			{
				Node* ln = f.lhsNode;
				Node* rn = f.rhsNode;
				stack = stack->below;
				L = ln->nextSibling;
				R = rn->nextSibling;
				lhsParentId = ln->parent ? ln->parent->id : 0;
				rhsParentId = rn->parent ? rn->parent->id : 0;
				continue;
			}
			break;
		}
		// Either frame: pop from whichever side is finished and still has returns.
		std::vector<Node*> lr = f.lhsReturns;
		std::vector<Node*> rr = f.rhsReturns;
		bool advanced = false;
		if (L == nullptr && !lr.empty())
		{
			Node* ln = lr.back();
			lr.pop_back();
			L = ln->nextSibling;
			lhsParentId = ln->parent ? ln->parent->id : 0;
			advanced = true;
		}
		if (R == nullptr && !rr.empty())
		{
			Node* rn = rr.back();
			rr.pop_back();
			R = rn->nextSibling;
			rhsParentId = rn->parent ? rn->parent->id : 0;
			advanced = true;
		}
		if (lr.empty() && rr.empty())
		{
			// Frame fully consumed: drop it and keep unwinding the frame below.
			stack = stack->below;
			continue;
		}
		// Frame still holds returns for a side that isn't finished yet.
		Frame nf;
		nf.both = false;
		nf.lhsReturns = std::move(lr);
		nf.rhsReturns = std::move(rr);
		stack = makeStack(std::move(nf), stack->below);
		if (!advanced)
			break;
	}
}

// ---------------------------------------------------------------------------
// Graph vertices
// ---------------------------------------------------------------------------

enum class EdgeKind
{
	UnchangedNode,
	EnterUnchangedDelimiter,
	NovelAtomLHS,
	EnterNovelDelimiterLHS,
	NovelAtomRHS,
	EnterNovelDelimiterRHS,
};

struct Edge
{
	EdgeKind kind = EdgeKind::UnchangedNode;
	Node* lhs = nullptr; // node acted on, left side  (null for RHS-only edges)
	Node* rhs = nullptr; // node acted on, right side (null for LHS-only edges)
};

constexpr int kCostUnchangedNode = 1;
constexpr int kCostEnterUnchangedDelimiter = 100;
constexpr int kCostNovel = 300;

struct Vertex
{
	Node* lhs = nullptr;
	Node* rhs = nullptr;
	int lhsParentId = 0; // id of the immediate enclosing lhs list (0 = top level)
	int rhsParentId = 0; // id of the immediate enclosing rhs list (0 = top level)
	Stack stack;         // full entered-delimiter stack (NOT part of vertex identity)

	std::uint64_t distance = UINT64_MAX;
	bool finalized = false;
	Vertex* predecessor = nullptr;
	Edge predecessorEdge;
};

// True iff the top of the stack is an Either frame, i.e. we can pop one side
// independently. This single bit (rather than the whole stack) is what the vertex
// key carries, mirroring difftastic's can_pop_either_parent.
inline bool canPopEither(const Stack& s)
{
	return s && !s->frame.both;
}

// Vertex identity, deliberately NOT the full delimiter stack.
//
// difftastic's key insight (graph.rs `impl PartialEq for Vertex`): keying a vertex on
// its entire entered-delimiter stack is "strictly correct" but makes the graph size
// EXPONENTIAL in tree nesting depth, because one-sided enters/pops can reach the same
// node pair with a combinatorial number of distinct stacks. Instead we key on a cheap
// path-dependent summary: the two node ids, the two immediate parent ids, and whether
// the top frame is poppable on either side. The first (lowest-cost) vertex to reach a
// key "wins" and its stack is used for subsequent pops. The full stack is still kept
// on the Vertex and used to allow a small, bounded number (kMaxNestingsPerKey) of
// genuinely distinct stacks per key - see getVertex - so we don't lose good diffs.
struct VertexKey
{
	int lhsId;
	int rhsId;
	int lhsParentId;
	int rhsParentId;
	bool canPopEither;
	bool operator==(const VertexKey& o) const
	{
		return lhsId == o.lhsId && rhsId == o.rhsId &&
			lhsParentId == o.lhsParentId && rhsParentId == o.rhsParentId &&
			canPopEither == o.canPopEither;
	}
};

struct VertexKeyHash
{
	std::size_t operator()(const VertexKey& k) const
	{
		std::size_t h = static_cast<std::size_t>(k.lhsId);
		hashCombine(h, static_cast<std::size_t>(k.rhsId));
		hashCombine(h, static_cast<std::size_t>(k.lhsParentId));
		hashCombine(h, static_cast<std::size_t>(k.rhsParentId));
		hashCombine(h, k.canPopEither ? 1u : 0u);
		return h;
	}
};

VertexKey keyOf(Node* L, Node* R, int lhsParentId, int rhsParentId, const Stack& stack)
{
	return VertexKey{ L ? L->id : 0, R ? R->id : 0, lhsParentId, rhsParentId, canPopEither(stack) };
}

// At most this many distinct full stacks are explored per vertex key. difftastic uses
// 2: it bounds the graph (keeping it ~linear in depth) while still letting the search
// consider popping sides together vs. separately.
constexpr std::size_t kMaxNestingsPerKey = 2;

// ---------------------------------------------------------------------------
// Tree preparation: content ids, links, reset
// ---------------------------------------------------------------------------

std::string contentSignature(Node* n)
{
	if (n->kind == NodeKind::Atom)
		return "A\x1f" + n->content;
	std::string sig = "L\x1f" + n->openContent + "\x1f" + n->closeContent + "\x1f";
	for (Node* c : n->children)
	{
		sig += std::to_string(c->contentId);
		sig += ',';
	}
	return sig;
}

// Iterative post-order so a deeply nested tree cannot overflow the call stack.
void assignContentIds(Node* root, std::unordered_map<std::string, int>& ids)
{
	std::vector<std::pair<Node*, bool>> stack;
	stack.emplace_back(root, false);
	while (!stack.empty())
	{
		Node* n = stack.back().first;
		const bool childrenDone = stack.back().second;
		stack.pop_back();
		if (!childrenDone)
		{
			stack.emplace_back(n, true);
			for (Node* c : n->children)
				stack.emplace_back(c, false);
			continue;
		}
		// All children have their content id by now (post-order).
		const std::string sig = contentSignature(n);
		auto it = ids.find(sig);
		if (it == ids.end())
		{
			const int newId = static_cast<int>(ids.size()) + 1;
			ids.emplace(sig, newId);
			n->contentId = newId;
		}
		else
		{
			n->contentId = it->second;
		}
	}
}

// Set parent/sibling/child links, depth, unique ids and reset change marks for a
// whole forest. Iterative (explicit stack) to avoid recursion on deep trees; the
// order ids are handed out in does not matter, only that they are unique.
void linkForest(const std::vector<Node*>& roots, int& idCounter)
{
	struct Item { Node* node; Node* parent; int depth; };
	std::vector<Item> stack;
	for (std::size_t i = 0; i < roots.size(); ++i)
	{
		roots[i]->nextSibling = (i + 1 < roots.size()) ? roots[i + 1] : nullptr;
		stack.push_back({ roots[i], nullptr, 0 });
	}
	while (!stack.empty())
	{
		const Item item = stack.back();
		stack.pop_back();
		Node* n = item.node;
		n->parent = item.parent;
		n->numAncestors = item.depth;
		n->id = ++idCounter;
		n->change = ChangeKind::Novel;
		n->firstChild = n->children.empty() ? nullptr : n->children.front();
		for (std::size_t i = 0; i < n->children.size(); ++i)
		{
			Node* child = n->children[i];
			child->nextSibling = (i + 1 < n->children.size()) ? n->children[i + 1] : nullptr;
			stack.push_back({ child, n, item.depth + 1 });
		}
	}
}

void markDeepUnchanged(Node* root)
{
	std::vector<Node*> stack{ root };
	while (!stack.empty())
	{
		Node* n = stack.back();
		stack.pop_back();
		n->change = ChangeKind::Unchanged;
		for (Node* c : n->children)
			stack.push_back(c);
	}
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Diff
// ---------------------------------------------------------------------------

DiffResult Diff(const std::vector<Node*>& lhsRoots, const std::vector<Node*>& rhsRoots, int graphLimit)
{
	DiffResult result;

	// 1. Content ids (shared across both sides so identical content collides).
	std::unordered_map<std::string, int> contentIds;
	for (Node* r : lhsRoots)
		assignContentIds(r, contentIds);
	for (Node* r : rhsRoots)
		assignContentIds(r, contentIds);

	// 2. Unique ids, parent/sibling/child links, reset change marks.
	int idCounter = 0;
	linkForest(lhsRoots, idCounter);
	linkForest(rhsRoots, idCounter);

	// 3. Dijkstra over the position-pair graph.
	std::deque<Vertex> pool; // stable addresses
	// Each key maps to a small bucket of up to kMaxNestingsPerKey vertices that share
	// the cheap key but have genuinely different full stacks (checked with stackEqual).
	std::unordered_map<VertexKey, std::vector<Vertex*>, VertexKeyHash> seen;

	auto getVertex = [&](Node* L, Node* R, int lpid, int rpid, const Stack& stack) -> Vertex* {
		const VertexKey key = keyOf(L, R, lpid, rpid, stack);
		std::vector<Vertex*>& bucket = seen[key];
		if (!bucket.empty())
		{
			// Cap reached: reuse the first vertex allocated for this key (difftastic's
			// "first vertex wins" - it has the lowest cost / most PopBoth frames).
			if (bucket.size() >= kMaxNestingsPerKey)
				return bucket.back();
			// Otherwise reuse only an EXACT full-stack match, so we never conflate two
			// distinct nestings within the bucket.
			for (Vertex* ex : bucket)
				if (stackEqual(ex->stack, stack))
					return ex;
		}
		pool.push_back(Vertex{});
		Vertex* v = &pool.back();
		v->lhs = L;
		v->rhs = R;
		v->lhsParentId = lpid;
		v->rhsParentId = rpid;
		v->stack = stack;
		bucket.push_back(v);
		return v;
	};

	Node* startL = lhsRoots.empty() ? nullptr : lhsRoots.front();
	Node* startR = rhsRoots.empty() ? nullptr : rhsRoots.front();
	Vertex* start = getVertex(startL, startR, 0, 0, Stack{});
	start->distance = 0;

	// Min-heap on distance; lazy decrease-key (skip stale pops).
	using QItem = std::pair<std::uint64_t, Vertex*>;
	std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> heap;
	heap.emplace(0, start);

	Vertex* end = nullptr;
	while (!heap.empty())
	{
		auto [dist, v] = heap.top();
		heap.pop();
		if (v->finalized)
			continue;
		if (dist != v->distance)
			continue;
		v->finalized = true;

		if (v->lhs == nullptr && v->rhs == nullptr && !v->stack)
		{
			end = v;
			break;
		}

		if (static_cast<int>(pool.size()) > graphLimit)
		{
			result.exceededLimit = true;
			result.verticesExplored = static_cast<int>(pool.size());
			return result;
		}

		// Generate neighbours. Each candidate copies the base position, applies one
		// move, unwinds finished delimiters (updating the parent ids), then relaxes the
		// target vertex.
		struct Candidate
		{
			Node* L;
			Node* R;
			int lpid;
			int rpid;
			Stack stack;
			Edge edge;
			int cost;
		};
		std::vector<Candidate> candidates;
		Node* L0 = v->lhs;
		Node* R0 = v->rhs;
		const int LP0 = v->lhsParentId;
		const int RP0 = v->rhsParentId;
		const Stack& S0 = v->stack;

		// (1) Unchanged node: identical subtree on both sides.
		if (L0 && R0 && L0->contentId == R0->contentId)
		{
			Node* L = L0->nextSibling;
			Node* R = R0->nextSibling;
			int lpid = LP0, rpid = RP0;
			Stack s = S0;
			popAllParents(L, R, lpid, rpid, s);
			candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::UnchangedNode, L0, R0 }, kCostUnchangedNode });
		}

		// (2) Enter matching delimiters; children diffed by later edges.
		if (L0 && R0 && L0->kind == NodeKind::List && R0->kind == NodeKind::List &&
			L0->openContent == R0->openContent && L0->closeContent == R0->closeContent)
		{
			Node* L = L0->firstChild;
			Node* R = R0->firstChild;
			int lpid = L0->id, rpid = R0->id;
			Stack s = pushBoth(S0, L0, R0);
			popAllParents(L, R, lpid, rpid, s);
			candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::EnterUnchangedDelimiter, L0, R0 }, kCostEnterUnchangedDelimiter });
		}

		// (3) Novel on the left.
		if (L0)
		{
			if (L0->kind == NodeKind::Atom)
			{
				Node* L = L0->nextSibling;
				Node* R = R0;
				int lpid = LP0, rpid = RP0;
				Stack s = S0;
				popAllParents(L, R, lpid, rpid, s);
				candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::NovelAtomLHS, L0, nullptr }, kCostNovel });
			}
			else
			{
				Node* L = L0->firstChild;
				Node* R = R0;
				int lpid = L0->id, rpid = RP0;
				Stack s = pushEitherLhs(S0, L0);
				popAllParents(L, R, lpid, rpid, s);
				candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::EnterNovelDelimiterLHS, L0, nullptr }, kCostNovel });
			}
		}

		// (4) Novel on the right.
		if (R0)
		{
			if (R0->kind == NodeKind::Atom)
			{
				Node* L = L0;
				Node* R = R0->nextSibling;
				int lpid = LP0, rpid = RP0;
				Stack s = S0;
				popAllParents(L, R, lpid, rpid, s);
				candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::NovelAtomRHS, nullptr, R0 }, kCostNovel });
			}
			else
			{
				Node* L = L0;
				Node* R = R0->firstChild;
				int lpid = LP0, rpid = R0->id;
				Stack s = pushEitherRhs(S0, R0);
				popAllParents(L, R, lpid, rpid, s);
				candidates.push_back({ L, R, lpid, rpid, s, Edge{ EdgeKind::EnterNovelDelimiterRHS, nullptr, R0 }, kCostNovel });
			}
		}

		for (const Candidate& c : candidates)
		{
			Vertex* next = getVertex(c.L, c.R, c.lpid, c.rpid, c.stack);
			if (next->finalized)
				continue;
			const std::uint64_t nd = v->distance + static_cast<std::uint64_t>(c.cost);
			if (nd < next->distance)
			{
				next->distance = nd;
				next->predecessor = v;
				next->predecessorEdge = c.edge;
				heap.emplace(nd, next);
			}
		}
	}

	if (end == nullptr)
	{
		// No route found (should not happen: novel edges always reach the end).
		result.exceededLimit = true;
		result.verticesExplored = static_cast<int>(pool.size());
		return result;
	}

	// 4. Walk the route backwards, applying change marks. Nodes never touched keep
	//    their default Novel mark.
	for (Vertex* v = end; v && v->predecessor; v = v->predecessor)
	{
		const Edge& e = v->predecessorEdge;
		switch (e.kind)
		{
		case EdgeKind::UnchangedNode:
			markDeepUnchanged(e.lhs);
			markDeepUnchanged(e.rhs);
			break;
		case EdgeKind::EnterUnchangedDelimiter:
			e.lhs->change = ChangeKind::Unchanged;
			e.rhs->change = ChangeKind::Unchanged;
			break;
		case EdgeKind::NovelAtomLHS:
		case EdgeKind::EnterNovelDelimiterLHS:
			e.lhs->change = ChangeKind::Novel;
			break;
		case EdgeKind::NovelAtomRHS:
		case EdgeKind::EnterNovelDelimiterRHS:
			e.rhs->change = ChangeKind::Novel;
			break;
		}
	}

	result.verticesExplored = static_cast<int>(pool.size());
	return result;
}

} // namespace structdiff
