///////////////////////////////////////////////////////////////////////////
//  File:    tex.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  TEX syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"
#include "SyntaxColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszTexKeywordList[] =
  {
    _T ("documentstyle"),
    _T ("document"),
    _T ("documentclass"),
    _T ("pagestyle"),
    _T ("pagenumbering"),
    _T ("thispagestyle"),
    _T ("title"),
    _T ("author"),
    _T ("date"),
    _T ("maketitle"),
    _T ("begin"),
    _T ("end"),
    _T ("part"),
    _T ("chapter"),
    _T ("section"),
    _T ("subsection"),
    _T ("subsubsection"),
    _T ("paragraph"),
    _T ("subparagraph"),
    _T ("appendix"),
    _T ("tableofcontents"),
    _T ("listoffigures"),
    _T ("listoftables"),
    _T ("rm"),
    _T ("em"),
    _T ("bf"),
    _T ("sc"),
    _T ("it"),
    _T ("sl"),
    _T ("sf"),
    _T ("tt"),
    _T ("tiny"),
    _T ("scriptsize"),
    _T ("footnotesize"),
    _T ("small"),
    _T ("normalsize"),
    _T ("large"),
    _T ("Large"),
    _T ("LARGE"),
    _T ("huge"),
    _T ("HUGE"),
    _T ("bibitem"),
    _T ("cite"),
    _T ("label"),
    _T ("ref"),
    _T ("pageref"),
    _T ("footnote"),
    _T ("item"),
    _T ("caption"),
    _T ("kill"),
    _T ("hline"),
    _T ("cline"),
    _T ("multicolumn"),
    _T ("def"),
    _T ("hspace"),
    _T ("vspace"),
    _T ("linebreak"),
    _T ("nolinebreak"),
    _T ("newpage"),
    _T ("clearpage"),
    _T ("pagebreak"),
    _T ("nopagebreak"),
    _T ("samepage"),
    _T ("newcommand"),
    _T ("renewcommand"),
    _T ("newenvironment"),
    _T ("newtheorem"),
    _T ("newcounter"),
    _T ("setcounter"),
    _T ("addtocounter"),
    _T ("value"),
    _T ("stepcounter"),

    _T ("newlength"),
    _T ("setlength"),
    _T ("addtolength"),
    _T ("settowidth"),
    _T ("textheight"),
    _T ("textwidth"),
    _T ("topmargin"),
    _T ("hoffset"),
    _T ("voffset"),
    _T ("oddsidemargin"),
    _T ("evensidemargin"),
    _T ("mbox"),
    _T ("makebox"),
    _T ("fbos"),
    _T ("framebox"),
    _T ("newsavebox"),
    _T ("sbox"),
    _T ("savebox"),
    _T ("usebox"),
    _T ("raisebox"),
    _T ("put"),
    _T ("framebox"),
    _T ("dashbox"),
    _T ("line"),
    _T ("vector"),
    _T ("circle"),
    _T ("oval"),
    _T ("frame"),
    _T ("shortstack"),
    _T ("multiput"),
    _T ("thinlines"),
    _T ("thicklines"),
    _T ("linethickness"),
    _T ("font"),
    _T ("magnification"),
    _T ("magstephalf"),
    _T ("magstep"),
    _T ("hsize"),
    _T ("vsize"),
    _T ("voffset"),
    _T ("hoffset"),
    _T ("topskip"),
    _T ("leftskip"),
    _T ("rightskip"),
    _T ("eject"),
    _T ("vfill"),
    _T ("eject"),
    _T ("goodbreak"),
    _T ("nobreak"),
    _T ("nopagenumbers"),
    _T ("headline"),
    _T ("headheight"),
    _T ("headsep"),
    _T ("footnote"),
    _T ("pageno"),
    _T ("folio"),
    _T ("par"),
    _T ("parindent"),
    _T ("noindent"),
    _T ("break"),
    _T ("hfill"),
    _T ("break"),
    _T ("line"),
    _T ("rightline"),
    _T ("centerline"),

    _T ("includegraphics"),

    _T ("enskip"),
    _T ("hskip"),
    _T ("hfil"),
    _T ("hfill"),
    _T ("hss"),
    _T ("smallskip"),
    _T ("vskip"),
    _T ("vglue"),
    _T ("vfil"),
    _T ("vfill"),
    _T ("vss"),
    _T ("baselineskip"),
    _T ("parskip"),
    _T ("topinsert"),
    _T ("midinsert"),
    _T ("handindent"),
    _T ("footnote"),
    _T ("hrule"),
    _T ("vrule"),
    _T ("leaders"),
    _T ("hrulefill"),
    _T ("settabs"),
    _T ("haling"),
    _T ("hbox"),
    _T ("vbox"),
    _T ("newbox"),
    _T ("setbox"),

    _T ("arccos"),
    _T ("cos"),
    _T ("csc"),
    _T ("exp"),
    _T ("ker"),
    _T ("limsup"),
    _T ("min"),
    _T ("sinh"),
    _T ("arcsin"),
    _T ("cosh"),
    _T ("deg"),
    _T ("gcd"),
    _T ("lg"),
    _T ("ln"),
    _T ("Pr"),
    _T ("sup"),
    _T ("arctan"),
    _T ("cot"),
    _T ("det"),
    _T ("hom"),
    _T ("lim"),
    _T ("log"),
    _T ("sec"),
    _T ("tan"),
    _T ("arg"),
    _T ("coth"),
    _T ("dim"),
    _T ("inf"),
    _T ("liminfo"),
    _T ("max"),
    _T ("sin"),
    _T ("tanh"),

    _T ("displaystyle"),
    _T ("textstyle"),
    _T ("alpha"),
    _T ("beta"),
    _T ("gamma"),
    _T ("delta"),
    _T ("epsilon"),
    _T ("varepsilon"),
    _T ("zeta"),
    _T ("eta"),
    _T ("theta"),
    _T ("vartheta"),
    _T ("iota"),
    _T ("kappa"),
    _T ("lambda"),
    _T ("mu"),
    _T ("nu"),
    _T ("xi"),
    _T ("o"),
    _T ("pi"),
    _T ("varpi"),
    _T ("rho"),
    _T ("varrho"),
    _T ("sigma"),
    _T ("varsigma"),
    _T ("tau"),
    _T ("upsilon"),
    _T ("phi"),
    _T ("varphi"),
    _T ("chi"),
    _T ("psi"),
    _T ("omega"),
    _T ("Gamma"),
    _T ("Delta"),
    _T ("Theta"),
    _T ("Lambda"),
    _T ("Xi"),
    _T ("Pi"),
    _T ("Sigma"),
    _T ("Upsilon"),
    _T ("Phi"),
    _T ("Psi"),
    _T ("Omega"),

    _T ("aleph"),
    _T ("hbar"),
    _T ("imath"),
    _T ("jmath"),
    _T ("ell"),
    _T ("wp"),
    _T ("Re"),
    _T ("Im"),
    _T ("partial"),
    _T ("infty"),
    _T ("backslash"),
    _T ("prime"),
    _T ("emptyset"),
    _T ("nabla"),
    _T ("surd"),
    _T ("triangle"),
    _T ("angle"),
    _T ("bot"),
    _T ("top"),
    _T ("forall"),
    _T ("exists"),
    _T ("neg"),
    _T ("lnot"),
    _T ("flat"),
    _T ("natural"),
    _T ("sharp"),
    _T ("clubsuit"),
    _T ("diamondsuit"),
    _T ("heartsuit"),
    _T ("spadesuit"),

    _T ("pm"),
    _T ("mp"),
    _T ("setminus"),
    _T ("cdot"),
    _T ("times"),
    _T ("ast"),
    _T ("star"),
    _T ("diamond"),
    _T ("circ"),
    _T ("bullet"),
    _T ("triangleleft"),
    _T ("cap"),
    _T ("cup"),
    _T ("uplus"),
    _T ("sqcap"),
    _T ("sqcup"),
    _T ("amalg"),
    _T ("div"),
    _T ("wr"),
    _T ("bigcirc"),
    _T ("vee"),
    _T ("lor"),
    _T ("wedge"),
    _T ("land"),
    _T ("oplus"),
    _T ("ominus"),
    _T ("otimes"),
    _T ("oslash"),
    _T ("odot"),
    _T ("dagger"),
    _T ("ddagger"),
    _T ("bigtriangleleft"),
    _T ("bigtriangledown"),
    _T ("sum"),
    _T ("prod"),
    _T ("coprod"),
    _T ("int"),
    _T ("oint"),
    _T ("bigcap"),
    _T ("bigcup"),
    _T ("bigsqcup"),
    _T ("bigvee"),
    _T ("bigwedge"),
    _T ("bigodot"),
    _T ("bigotimes"),
    _T ("bigoplus"),
    _T ("biguplus"),
    _T ("hat"),
    _T ("acute"),
    _T ("ddot"),
    _T ("vec"),
    _T ("check"),
    _T ("grave"),
    _T ("breve"),
    _T ("tilde"),
    _T ("dot"),
    _T ("bar"),
    _T ("widehat"),
    _T ("widetilde"),

    _T ("ldots"),
    _T ("vdots"),
    _T ("cdots"),
    _T ("ddots"),
    _T ("leq"),
    _T ("le"),
    _T ("prec"),
    _T ("preceq"),
    _T ("ll"),
    _T ("subset"),
    _T ("subsetqe"),
    _T ("supsetqe"),
    _T ("in"),
    _T ("vdash"),
    _T ("smile"),
    _T ("frown"),
    _T ("geq"),
    _T ("ge"),
    _T ("succ"),
    _T ("succeq"),
    _T ("gg"),
    _T ("supset"),
    _T ("sqsubseteq"),
    _T ("sqsupsetqe"),
    _T ("ni"),
    _T ("owns"),
    _T ("dashv"),
    _T ("mid"),
    _T ("parallel"),
    _T ("equiv"),
    _T ("sim"),
    _T ("simeq"),
    _T ("asymp"),
    _T ("approx"),
    _T ("cong"),
    _T ("bowtie"),
    _T ("propto"),
    _T ("models"),
    _T ("doteq"),
    _T ("perp"),
    _T ("not"),

    _T ("leftarrow"),
    _T ("Leftarrow"),
    _T ("gets"),
    _T ("longleftarrow"),
    _T ("Longleftarrow"),
    _T ("rightarrow"),
    _T ("Rightarrow"),
    _T ("to"),
    _T ("longrightarrow"),
    _T ("Longrightarrow"),
    _T ("leftrightarrow"),
    _T ("Leftrightarrow"),
    _T ("longleftrightarrow"),
    _T ("Longleftrightarrow"),
    _T ("mapsto"),
    _T ("longmapsto"),
    _T ("hookleftarrow"),
    _T ("hookrightarrow"),
    _T ("leftharpoonup"),
    _T ("rightharpoonup"),
    _T ("leftharpoondown"),
    _T ("rightharpoondown"),
    _T ("uparrow"),
    _T ("Uparrow"),
    _T ("downarrow"),
    _T ("Downarrow"),
    _T ("updownarrow"),
    _T ("Updownarrow"),
    _T ("nearrow"),
    _T ("searrow"),
    _T ("swarrow"),
    _T ("nwarrow"),
    _T ("lbrack"),
    _T ("lfloor"),
    _T ("rbrack"),
    _T ("rfloor"),
    _T ("lceil"),
    _T ("lbrace"),
    _T ("langle"),
    _T ("rceil"),
    _T ("rbrace"),
    _T ("rangle"),
    _T ("quad"),
    _T ("qquad"),

    _T ("Biggl"),
    _T ("biggl"),
    _T ("Bigl"),
    _T ("bigl"),
    _T ("bigr"),
    _T ("Bigr"),
    _T ("biggr"),
    _T ("Biggr"),
    _T ("lgroup"),
    _T ("rgroup"),
    _T ("langle"),
    _T ("rangle"),
    _T ("vert"),
    _T ("Vert"),
    _T ("uparrow"),
    _T ("left"),
    _T ("right"),
    _T ("overline"),
    _T ("overrightarrow"),
    _T ("overleftarrow"),
    _T ("overbrace"),
    _T ("underbrace"),
    _T ("atop"),
    _T ("choose"),
    _T ("brack"),
    _T ("above"),
    _T ("brace"),
    _T ("pmatrix"),
    _T ("matrix"),
    _T ("bordermatrix"),
    _T ("eqalign"),
    _T ("eqno"),
    _T ("eqalignno"),
    _T ("quad"),

    _T ("def"),
    _T ("gdef"),
    _T ("edef"),
    _T ("newcount"),
    _T ("advance"),
    _T ("multiply"),
    _T ("divide"),
    _T ("number"),
    _T ("romannumeral"),
    _T ("newdimen"),
    _T ("newread"),
    _T ("openin"),
    _T ("closein"),
    _T ("input"),
    _T ("endinput"),
    _T ("newwrite"),
    _T ("openouput"),
    _T ("closeout"),
    _T ("write"),
    _T ("jobname"),
    _T ("if"),
    _T ("else"),
    _T ("fi"),
    _T ("ifx"),
    _T ("ifeof"),
    _T ("ifhmode"),
    _T ("ifvmode"),
    _T ("ifmmode"),
    _T ("ifcat"),
    _T ("ifnum"),
    _T ("ifdim"),
    _T ("ifodd"),
    _T ("ifcase"),
    _T ("or"),
    _T ("loop"),
    _T ("repeat"),

    _T ("beginpicture"),
    _T ("setcoordinatesystem"),
    _T ("endpicture"),
    _T ("plotheading"),
    _T ("lines"),
    _T ("putrule"),
    _T ("linethickness"),
    _T ("setlinear"),
    _T ("setquadratic"),
    _T ("sethistograms"),
    _T ("setsolid"),
    _T ("setdashes"),
    _T ("setdots"),
    _T ("setdashpattern"),
    _T ("setplotsymbol"),
    _T ("plot"),
    _T ("arrow"),
    _T ("put"),
    _T ("multiput"),
    _T ("circulararc"),
    _T ("ellipticarc"),
    _T ("startrotation"),
    _T ("stoprotation"),
    _T ("setshadegrid"),
    _T ("setshadesymbol"),
    _T ("shaderectangleson"),
    _T ("sharerectanglesoff"),
    _T ("vshade"),
    _T ("setquadratic"),
    _T ("hshade"),
    _T ("setplotarea"),
    _T ("inboundscheckon"),
    _T ("inboundscheckoff"),
    _T ("axis"),
    _T ("setbox"),
    _T ("endpicturesave"),
    _T ("newdimen"),
    NULL
  };

static LPTSTR s_apszUser1KeywordList[] =
  {
    _T ("Alph"),
    _T ("AtBeginDocument"),
    _T ("AtBeginDvi"),
    _T ("AtEndDocument"),
    _T ("AtEndOfPackage"),
    _T ("Big"),
    _T ("Bigg"),
    _T ("Box"),
    _T ("CheckCommand"),
    _T ("ClassError"),
    _T ("ClassInfo"),
    _T ("ClassWarning"),
    _T ("ClassWarningNoLine"),
    _T ("DeclareErrorFont"),
    _T ("DeclareFixedFont"),
    _T ("DeclareFontEncoding"),
    _T ("DeclareFontEncodingDefaults"),
    _T ("DeclareFontFamily"),
    _T ("DeclareFontShape"),
    _T ("DeclareFontSubstitution"),
    _T ("DeclareMathAccent"),
    _T ("DeclareMathAlphabet"),
    _T ("DeclareMathDelimiter"),
    _T ("DeclareMathRadical"),
    _T ("DeclareMathSizes"),
    _T ("DeclareMathSymbol"),
    _T ("DeclareMathVersion"),
    _T ("DeclareOldFontCommand"),
    _T ("DeclareOption"),
    _T ("DeclarePreloadSizes"),
    _T ("DeclareRobustCommand"),
    _T ("DeclareSizeFunction"),
    _T ("DeclareSymbolFont"),
    _T ("DeclareSymbolFontAlphabet"),
    _T ("DeclareTextAccent"),
    _T ("DeclareTextAccentDefault"),
    _T ("DeclareTextCommand"),
    _T ("DeclareTextCommandDefault"),
    _T ("DeclareTextComposite"),
    _T ("DeclareTextCompositeCommand"),
    _T ("DeclareTextFontCommand"),
    _T ("DeclareTextSymbol"),
    _T ("DeclareTextSymbolDefault"),
    _T ("ExecuteOptions"),
    _T ("IfFileExists"),
    _T ("InputIfFileExists"),
    _T ("Join"),
    _T ("LoadClass"),
    _T ("LoadClassWithOptions"),
    _T ("MessageBreak"),
    _T ("NeedsTeXFormat"),
    _T ("OptionNotUsed"),
    _T ("PackageError"),
    _T ("PackageInfo"),
    _T ("PackageWarning"),
    _T ("PackageWarningNoLine"),
    _T ("PassOptionsToClass"),
    _T ("PassOptionsToPackage"),
    _T ("ProcessOptions"),
    _T ("ProvideTextCommand"),
    _T ("ProvideTextCommandDefault"),
    _T ("ProvidesFile"),
    _T ("ProvidesPackage"),
    _T ("RequirePackage"),
    _T ("RequirePackageWithOptions"),
    _T ("Roman"),
    _T ("SetMathAlphabet"),
    _T ("SetSymbolFont"),
    _T ("TeX"),
    _T ("TextSymbolUnavailable"),
    _T ("UseTextAccent"),
    _T ("UseTextSymbol"),
    _T ("addcontentsline"),
    _T ("addpenalty"),
    _T ("addtocontents"),
    _T ("addtoversion"),
    _T ("addvspace"),
    _T ("afterassignment"),
    _T ("allocationnumber"),
    _T ("allowbreak"),
    _T ("alph"),
    _T ("arrayrulewidth"),
    _T ("arraystretch"),
    _T ("baselinestretch"),
    _T ("bezier"),
    _T ("bfdefault"),
    _T ("bfseries"),
    _T ("bibcite"),
    _T ("bibliography"),
    _T ("bibliographystyle"),
    _T ("big"),
    _T ("bigbreak"),
    _T ("bigg"),
    _T ("bigskip"),
    _T ("bigskipamount"),
    _T ("bigskipamount"),
    _T ("bmod"),
    _T ("boldmath"),
    _T ("buildrel"),
    _T ("cases"),
    _T ("catcode"),
    _T ("center"),
    _T ("centering"),
    _T ("chardef"),
    _T ("cleardoublepage"),
    _T ("contentsline"),
    _T ("countdef"),
    _T ("cr"),
    _T ("csname"),
    _T ("depth"),
    _T ("discretionary"),
    _T ("displaymath"),
    _T ("dospecials"),
    _T ("dotfill"),
    _T ("encodingdefault"),
    _T ("endcsname"),
    _T ("everycr"),
    _T ("expandafter"),
    _T ("expandafter"),
    _T ("extracolsep"),
    _T ("familydefault"),
    _T ("familydefault"),
    _T ("fbox"),
    _T ("filbreak"),
    _T ("flushleft"),
    _T ("flushright"),
    _T ("fmtname"),
    _T ("fmtversion"),
    _T ("fontencoding"),
    _T ("fontfamily"),
    _T ("fontseries"),
    _T ("fontshape"),
    _T ("footnotemark"),
    _T ("footnoterule"),
    _T ("footnotetext"),
    _T ("frac"),
    _T ("frenchspacing"),
    _T ("fussy"),
    _T ("futurelet"),
    _T ("global"),
    _T ("glossary"),
    _T ("halign"),
    _T ("height"),
    _T ("hphantom"),
    _T ("ifinner"),
    _T ("immediate"),
    _T ("include"),
    _T ("includeonly"),
    _T ("itdefault"),
    _T ("iterate"),
    _T ("kern"),
    _T ("lastskip"),
    _T ("leadsto"),
    _T ("leavevmode"),
    _T ("lefteqn"),
    _T ("leftline"),
    _T ("leftmark"),
    _T ("let"),
    _T ("lhd"),
    _T ("liminf"),
    _T ("lineskip"),
    _T ("lower"),
    _T ("lowercase"),
    _T ("makeatletter"),
    _T ("makeatother"),
    _T ("makeglossary"),
    _T ("makeindex"),
    _T ("marginpar"),
    _T ("markboth"),
    _T ("markright"),
    _T ("mathclose"),
    _T ("mathop"),
    _T ("mathopen"),
    _T ("mathrel"),
    _T ("mathstrut"),
    _T ("mddefault"),
    _T ("medbreak"),
    _T ("medmuskip"),
    _T ("medskip"),
    _T ("mho"),
    _T ("minipage"),
    _T ("mskip"),
    _T ("multispan"),
    _T ("narrower"),
    _T ("negthinspace"),
    _T ("newfont"),
    _T ("newhelp"),
    _T ("newif"),
    _T ("newinsert"),
    _T ("newlabel"),
    _T ("newlanguage"),
    _T ("newline"),
    _T ("newmathalphabet"),
    _T ("newmuskip"),
    _T ("newskip"),
    _T ("newtoks"),
    _T ("nocite"),
    _T ("nocorrlist"),
    _T ("noexpand"),
    _T ("nointerlineskip"),
    _T ("nolimits"),
    _T ("nonfrenchspacing"),
    _T ("nonumber"),
    _T ("normalbaselines"),
    _T ("normallineskip"),
    _T ("normalmarginpar"),
    _T ("null"),
    _T ("numberline"),
    _T ("obeylines"),
    _T ("obeyspaces"),
    _T ("offinterlineskip"),
    _T ("onecolumn"),
    _T ("openup"),
    _T ("pagenumberin"),
    _T ("parbox"),
    _T ("penalty"),
    _T ("phantom"),
    _T ("pmod"),
    _T ("protect"),
    _T ("providecommand"),
    _T ("raggedbottom"),
    _T ("raggedleft"),
    _T ("raggedright"),
    _T ("refstepcounter"),
    _T ("relax"),
    _T ("removelastskip"),
    _T ("renewenvironment"),
    _T ("reversemarginpar"),
    _T ("rhd"),
    _T ("rightmark"),
    _T ("rlap"),
    _T ("rmdefault"),
    _T ("roman"),
    _T ("root"),
    _T ("rq"),
    _T ("rule"),
    _T ("scdefault"),
    _T ("secdef"),
    _T ("selectfont"),
    _T ("seriesdefault"),
    _T ("settodepth"),
    _T ("settoheight"),
    _T ("sfdefault"),
    _T ("shapedefault"),
    _T ("showhyphens"),
    _T ("showoutput"),
    _T ("showoverfull"),
    _T ("sldefault"),
    _T ("sloppy"),
    _T ("sloppypar"),
    _T ("smallbreak"),
    _T ("space"),
    _T ("spacefactor"),
    _T ("sqsubset"),
    _T ("stackrel"),
    _T ("stretch"),
    _T ("strut"),
    _T ("suppressfloats"),
    _T ("symbol"),
    _T ("tabskip"),
    _T ("thanks"),
    _T ("thefootnote"),
    _T ("thickmuskip"),
    _T ("thinmuskip"),
    _T ("thinspace"),
    _T ("today"),
    _T ("tracingfonts"),
    _T ("tracingoutput"),
    _T ("trivlist"),
    _T ("ttdefault"),
    _T ("twocolumn"),
    _T ("typein"),
    _T ("typeout"),
    _T ("unboldmath"),
    _T ("undefinedpagestyle"),
    _T ("underbar"),
    _T ("underline"),
    _T ("unlhd"),
    _T ("unrhd"),
    _T ("updefault"),
    _T ("usecounter"),
    _T ("usefont"),
    _T ("usepackage"),
    _T ("vcenter"),
    _T ("verb"),
    _T ("verbatim"),
    _T ("vline"),
    _T ("vphantom"),
    _T ("width"),
    _T ("wlog"),
    _T ("xdef"),
    NULL
  };

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != NULL; L++)
    {
      if (_tcsnicmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsTexKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszTexKeywordList, pszChars, nLength);
}

static BOOL
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszUser1KeywordList, pszChars, nLength);
}

static BOOL
IsTexNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return FALSE;
        }
      return TRUE;
    }
  if (!_istdigit (pszChars[0]))
    return FALSE;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return FALSE;
    }
  return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010

DWORD CCrystalTextView::
ParseLineTex (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = ::CharNext(pszChars+I) - pszChars)
    {
      if (I == nPrevI)
        {
          // CharNext did not advance, so we're at the end of the string
          // and we already handled this character, so stop
          break;
        }

      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  goto out;
                }
            }
          bRedefineBlock = FALSE;
          bDecIndex = FALSE;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      if (pszChars[I] == '%')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }
      if (pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsTexKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              else if (IsTexNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = FALSE;

                  if (!bFunction)
                    {
                      for (int j = I; j >= 0; j--)
                        {
                          if (!xisspace (pszChars[j]))
                            {
                              if (pszChars[j] == '$')
                                {
                                  bFunction = TRUE;
                                }
                              break;
                            }
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsTexKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsTexNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          if (!bFunction)
            {
              for (int j = I; j >= 0; j--)
                {
                  if (!xisspace (pszChars[j]))
                    {
                      if (pszChars[j] == '$')
                        {
                          bFunction = TRUE;
                        }
                      break;
                    }
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
