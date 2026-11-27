#include "pch.h"
#include <gtest/gtest.h>
#include "parsers/crystallineparser.h"
#include "SyntaxColors.h"

namespace
{
	using Lines = std::vector<std::basic_string<tchar_t>>;

	// Colors of every character of the given F# lines, parsed in order
	class FSharpColors
	{
	public:
		explicit FSharpColors(const Lines& lines) : m_lines(lines)
		{
			unsigned cookie = 0;
			for (const auto& line : lines)
			{
				std::vector<CrystalLineParser::TEXTBLOCK> blocks;
				cookie = CrystalLineParser::ParseLineFSharp(cookie, line.c_str(), static_cast<int>(line.length()), &blocks);
				std::vector<int> colors(line.length(), COLORINDEX_NORMALTEXT);
				for (size_t b = 0; b < blocks.size(); ++b)
				{
					const size_t end = b + 1 < blocks.size() ? blocks[b + 1].m_nCharPos : line.length();
					for (size_t i = blocks[b].m_nCharPos; i < end && i < line.length(); ++i)
						colors[i] = blocks[b].m_nColorIndex;
				}
				m_colors.push_back(colors);
				m_cookies.push_back(cookie);
			}
		}

		// Color of `token` (the n-th occurrence) on line `line`; COLORINDEX_NONE if
		// the token is not found or its characters do not all have the same color
		COLORINDEX Of(size_t line, const tchar_t* token, int n = 0) const
		{
			size_t pos = m_lines[line].find(token);
			while (pos != std::basic_string<tchar_t>::npos && n-- > 0)
				pos = m_lines[line].find(token, pos + 1);
			if (pos == std::basic_string<tchar_t>::npos)
				return COLORINDEX_NONE;
			const int color = m_colors[line][pos];
			for (size_t i = pos; i < pos + tc::tcslen(token); ++i)
				if (m_colors[line][i] != color)
					return COLORINDEX_NONE;
			return static_cast<COLORINDEX>(color);
		}

		unsigned Cookie(size_t line) const { return m_cookies[line]; }

	private:
		Lines m_lines;
		std::vector<std::vector<int>> m_colors;
		std::vector<unsigned> m_cookies;
	};

	COLORINDEX ColorOf(const tchar_t* line, const tchar_t* token, int n = 0)
	{
		return FSharpColors({ line }).Of(0, token, n);
	}
}

TEST(FSharpParser, Keywords)
{
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("abstract member X: int"), _T("abstract")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("let inline f x = x"), _T("inline")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("a lxor b"), _T("lxor")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("yield 1"), _T("yield")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("let! x = y"), _T("let!")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("match! x with"), _T("match!")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("and! y = z"), _T("and!")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let do_it = 1"), _T("do_it")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let x' = x"), _T("x'")));
}

TEST(FSharpParser, BuiltInNames)
{
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let x = Array.empty"), _T("Array")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let x = ValueSome 1"), _T("ValueSome")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let d = __SOURCE_DIRECTORY__"), _T("__SOURCE_DIRECTORY__")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let x = int y"), _T("int")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let x = voption"), _T("voption")));
}

TEST(FSharpParser, TypeVariablesAreNotCharLiterals)
{
	const tchar_t* line = _T("let f (x: List<'T>) : 'a -> 'b = x");
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("'T")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("'a")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("'b")));
	EXPECT_EQ(COLORINDEX_OPERATOR, ColorOf(line, _T("->")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(line, _T("x"), 1));
}

TEST(FSharpParser, CharLiterals)
{
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let c = 'a'"), _T("'a'")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let c = '\\n'"), _T("'\\n'")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let c = '\\''"), _T("'\\''")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let c = '\\u0041'"), _T("'\\u0041'")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let c = '\"' + x"), _T("'\"'")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let c = '\"' + x"), _T("x")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("let b = 'a'B"), _T("'a'B")));
}

TEST(FSharpParser, MultiplicationOperatorIsNotAComment)
{
	FSharpColors c({ _T("let p = List.fold (*) 1 xs"), _T("let q = 2") });
	EXPECT_EQ(COLORINDEX_OPERATOR, c.Of(0, _T("(*)")));
	EXPECT_EQ(COLORINDEX_NUMBER, c.Of(0, _T("1")));
	EXPECT_EQ(0u, c.Cookie(0) & COOKIE_EXT_COMMENT);
	EXPECT_EQ(COLORINDEX_KEYWORD, c.Of(1, _T("let")));
}

TEST(FSharpParser, BlockComments)
{
	FSharpColors c({
		_T("let a = 1 (* one"),
		_T("  (* nested *) still comment"),
		_T("  end *) let b = 2"),
		_T("let c = 3 // line"),
		_T("(* \"*)\" *) let d = 4"),
	});
	EXPECT_EQ(COLORINDEX_COMMENT, c.Of(0, _T("(* one")));
	EXPECT_EQ(COLORINDEX_COMMENT, c.Of(1, _T("still comment")));
	EXPECT_EQ(COLORINDEX_COMMENT, c.Of(2, _T("end *)")));
	EXPECT_EQ(COLORINDEX_KEYWORD, c.Of(2, _T("let")));
	EXPECT_EQ(COLORINDEX_COMMENT, c.Of(3, _T("// line")));
	EXPECT_EQ(COLORINDEX_KEYWORD, c.Of(4, _T("let")));
	EXPECT_EQ(0u, c.Cookie(4) & COOKIE_EXT_COMMENT);
}

TEST(FSharpParser, Strings)
{
	FSharpColors c({
		_T("let a = \"x \\\" y\" + b"),
		_T("let p = @\"C:\\\" + q"),
		_T("let s = $\"{x}\" + tail"),
		_T("let m = \"\"\"line"),
		_T("  \"quoted\" \"\"\" + n"),
		_T("let u = \"multi"),
		_T("line\" + v"),
		_T("let w = \"abc\"B"),
	});
	EXPECT_EQ(COLORINDEX_STRING, c.Of(0, _T("\"x \\\" y\"")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, c.Of(0, _T("b")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(1, _T("@\"C:\\\"")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, c.Of(1, _T("q")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(2, _T("$\"{x}\"")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, c.Of(2, _T("tail")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(3, _T("\"\"\"line")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(4, _T("\"quoted\" \"\"\"")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, c.Of(4, _T("n")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(6, _T("line\"")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, c.Of(6, _T("v")));
	EXPECT_EQ(COLORINDEX_STRING, c.Of(7, _T("\"abc\"B")));
	EXPECT_EQ(0u, c.Cookie(7) & (COOKIE_STRING | COOKIE_RAWSTRING));
}

TEST(FSharpParser, Numbers)
{
	EXPECT_EQ(COLORINDEX_NUMBER, ColorOf(_T("let n = 1_000_000L"), _T("1_000_000L")));
	EXPECT_EQ(COLORINDEX_NUMBER, ColorOf(_T("let n = 0x1Fuy"), _T("0x1Fuy")));
	EXPECT_EQ(COLORINDEX_NUMBER, ColorOf(_T("let n = 1.5e-3f"), _T("1.5e-3f")));
	EXPECT_EQ(COLORINDEX_NUMBER, ColorOf(_T("for i in 1..10 do ()"), _T("1")));
	EXPECT_EQ(COLORINDEX_OPERATOR, ColorOf(_T("for i in 1..10 do ()"), _T("..")));
}

TEST(FSharpParser, Directives)
{
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("#if DEBUG && !TRACE"), _T("#if DEBUG && !TRACE")));
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("#r \"nuget: Foo\""), _T("#r")));
	EXPECT_EQ(COLORINDEX_STRING, ColorOf(_T("#r \"nuget: Foo\""), _T("\"nuget: Foo\"")));
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("#!/usr/bin/env dotnet fsi"), _T("#!/usr/bin/env dotnet fsi")));
}

TEST(FSharpParser, FunctionNames)
{
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let add x y = x + y"), _T("add")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let rec inline private go () = ()"), _T("go")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let map<'T> (f: 'T -> 'T) = f"), _T("map")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let ``my test`` () = ()"), _T("``my test``")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let value = 1"), _T("value")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let mutable count: int = 0"), _T("count")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let x (* c *) = 1"), _T("x")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("member this.Area(scale) = scale"), _T("Area")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("member this.Area(scale) = scale"), _T("this")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("override _.ToString() = \"\""), _T("ToString")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("static member Type = 1"), _T("Type")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("abstract member Run: unit -> unit"), _T("Run")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let inline (>>=) m f = bind f m"), _T(">>=")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let ( |>> ) x f = f x"), _T("|>>")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let (|Even|Odd|) n = Even"), _T("Even")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("let (|Even|Odd|) n = Even"), _T("Odd")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("printfn(\"x\")"), _T("printfn")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("Console.WriteLine (x)"), _T("WriteLine")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let y = f x"), _T("f")));
}

TEST(FSharpParser, TypeNames)
{
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("type Shape ="), _T("Shape")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("type internal Shape<'T>() ="), _T("Shape")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("type [<Struct>] Point = { X: float }"), _T("Point")));
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("type [<Struct>] Point = { X: float }"), _T("Struct")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("and Tree = Leaf"), _T("Tree")));
	EXPECT_EQ(COLORINDEX_FUNCNAME, ColorOf(_T("and walk t = t"), _T("walk")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("exception ParseError of string"), _T("ParseError")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("| Circle of Radius * float"), _T("Radius")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("open System.Collections.Generic"), _T("Collections")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("open System.Collections.Generic"), _T("Generic")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("module internal Shapes ="), _T("Shapes")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("    inherit Base(x)"), _T("Base")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let d = new Dictionary<string, Foo>()"), _T("Dictionary")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let d = new Dictionary<string, Foo>()"), _T("Foo")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("interface IDisposable with"), _T("IDisposable")));
}

TEST(FSharpParser, TypeAnnotations)
{
	const tchar_t* line = _T("let f (x: Shape) (m: Map<Key, Value list>) : Async<Result> = g x");
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("Shape")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("Key")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("Value")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("list")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(line, _T("Result")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(line, _T("g")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(line, _T("m")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("{ Name: string; Owner: Person }"), _T("Person")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("abstract Run: Job * Options -> Outcome"), _T("Options")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("abstract Run: Job * Options -> Outcome"), _T("Outcome")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("| :? ArgumentException as ex -> ex"), _T("ArgumentException")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("| :? ArgumentException as ex -> ex"), _T("ex")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let o = x :> IShape"), _T("IShape")));
	EXPECT_EQ(COLORINDEX_USER1, ColorOf(_T("let s: seq<Item> = xs"), _T("Item")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let s: seq<Item> = xs"), _T("xs")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("let l = x :: rest"), _T("rest")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("r := next"), _T("next")));
}

TEST(FSharpParser, RandomInputGivesOrderedBlocksInsideTheLine)
{
	const tchar_t alphabet[] = _T("aZ_1'\"\\@$#(*)[<>]|:;.,-=` \t{}!B");
	unsigned seed = 12345;
	unsigned cookie = 0;
	for (int n = 0; n < 20000; ++n)
	{
		seed = seed * 1103515245 + 12345;
		std::basic_string<tchar_t> line(seed % 24, _T(' '));
		for (auto& ch : line)
		{
			seed = seed * 1103515245 + 12345;
			ch = alphabet[(seed >> 16) % (std::size(alphabet) - 1)];
		}
		std::vector<CrystalLineParser::TEXTBLOCK> blocks;
		cookie = CrystalLineParser::ParseLineFSharp(cookie, line.c_str(), static_cast<int>(line.length()), &blocks);
		if (!line.empty())
		{
			ASSERT_FALSE(blocks.empty()) << n;
			ASSERT_EQ(0, blocks[0].m_nCharPos) << n;   // the view draws from the first block
		}
		for (size_t b = 0; b < blocks.size(); ++b)
		{
			ASSERT_GE(blocks[b].m_nCharPos, 0) << n;
			ASSERT_LT(blocks[b].m_nCharPos, static_cast<int>(line.length())) << n;
			if (b > 0)
				ASSERT_LT(blocks[b - 1].m_nCharPos, blocks[b].m_nCharPos) << n;
		}
	}
}

TEST(FSharpParser, Attributes)
{
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("[<Literal>]"), _T("Literal")));
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("[<assembly: InternalsVisibleTo(\"Tests\")>]"), _T("InternalsVisibleTo")));
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, ColorOf(_T("[<Obsolete(\"x\"); CompiledName(\"Y\")>]"), _T("CompiledName")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("[<DllImport(\"k\", CallingConvention = CallingConvention.Cdecl)>] let x = 1"), _T("Cdecl")));
	EXPECT_EQ(COLORINDEX_NORMALTEXT, ColorOf(_T("[<DllImport(\"k\", CallingConvention = CallingConvention.Cdecl)>] let x = 1"), _T("x")));
	EXPECT_EQ(COLORINDEX_KEYWORD, ColorOf(_T("let xs = [| 1 |]"), _T("let")));
}
