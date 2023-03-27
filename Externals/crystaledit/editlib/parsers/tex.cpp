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

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  TEX keywords
static const tchar_t * s_apszTexKeywordList[] =
  {
    _T ("above"),
    _T ("acute"),
    _T ("addtocounter"),
    _T ("addtolength"),
    _T ("advance"),
    _T ("aleph"),
    _T ("alpha"),
    _T ("amalg"),
    _T ("angle"),
    _T ("appendix"),
    _T ("approx"),
    _T ("arccos"),
    _T ("arcsin"),
    _T ("arctan"),
    _T ("arg"),
    _T ("arrow"),
    _T ("ast"),
    _T ("asymp"),
    _T ("atop"),
    _T ("author"),
    _T ("axis"),
    _T ("backslash"),
    _T ("bar"),
    _T ("baselineskip"),
    _T ("begin"),
    _T ("beginpicture"),
    _T ("beta"),
    _T ("bf"),
    _T ("bibitem"),
    _T ("bigcap"),
    _T ("bigcirc"),
    _T ("bigcup"),
    _T ("biggl"),
    _T ("Biggl"),
    _T ("Biggr"),
    _T ("biggr"),
    _T ("Bigl"),
    _T ("bigl"),
    _T ("bigodot"),
    _T ("bigoplus"),
    _T ("bigotimes"),
    _T ("Bigr"),
    _T ("bigr"),
    _T ("bigsqcup"),
    _T ("bigtriangledown"),
    _T ("bigtriangleleft"),
    _T ("biguplus"),
    _T ("bigvee"),
    _T ("bigwedge"),
    _T ("bordermatrix"),
    _T ("bot"),
    _T ("bowtie"),
    _T ("brace"),
    _T ("brack"),
    _T ("break"),
    _T ("break"),
    _T ("breve"),
    _T ("bullet"),
    _T ("cap"),
    _T ("caption"),
    _T ("cdot"),
    _T ("cdots"),
    _T ("centerline"),
    _T ("chapter"),
    _T ("check"),
    _T ("chi"),
    _T ("choose"),
    _T ("circ"),
    _T ("circle"),
    _T ("circulararc"),
    _T ("cite"),
    _T ("clearpage"),
    _T ("cline"),
    _T ("closein"),
    _T ("closeout"),
    _T ("clubsuit"),
    _T ("cong"),
    _T ("coprod"),
    _T ("cos"),
    _T ("cosh"),
    _T ("cot"),
    _T ("coth"),
    _T ("csc"),
    _T ("cup"),
    _T ("dagger"),
    _T ("dashbox"),
    _T ("dashv"),
    _T ("date"),
    _T ("ddagger"),
    _T ("ddot"),
    _T ("ddots"),
    _T ("def"),
    _T ("def"),
    _T ("deg"),
    _T ("Delta"),
    _T ("delta"),
    _T ("det"),
    _T ("diamond"),
    _T ("diamondsuit"),
    _T ("dim"),
    _T ("displaystyle"),
    _T ("div"),
    _T ("divide"),
    _T ("document"),
    _T ("documentclass"),
    _T ("documentstyle"),
    _T ("dot"),
    _T ("doteq"),
    _T ("downarrow"),
    _T ("Downarrow"),
    _T ("edef"),
    _T ("eject"),
    _T ("eject"),
    _T ("ell"),
    _T ("ellipticarc"),
    _T ("else"),
    _T ("em"),
    _T ("emptyset"),
    _T ("end"),
    _T ("endinput"),
    _T ("endpicture"),
    _T ("endpicturesave"),
    _T ("enskip"),
    _T ("epsilon"),
    _T ("eqalign"),
    _T ("eqalignno"),
    _T ("eqno"),
    _T ("equiv"),
    _T ("eta"),
    _T ("evensidemargin"),
    _T ("exists"),
    _T ("exp"),
    _T ("fbos"),
    _T ("fi"),
    _T ("flat"),
    _T ("folio"),
    _T ("font"),
    _T ("footnote"),
    _T ("footnote"),
    _T ("footnote"),
    _T ("footnotesize"),
    _T ("forall"),
    _T ("frame"),
    _T ("framebox"),
    _T ("framebox"),
    _T ("frown"),
    _T ("gamma"),
    _T ("Gamma"),
    _T ("gcd"),
    _T ("gdef"),
    _T ("ge"),
    _T ("geq"),
    _T ("gets"),
    _T ("gg"),
    _T ("goodbreak"),
    _T ("grave"),
    _T ("haling"),
    _T ("handindent"),
    _T ("hat"),
    _T ("hbar"),
    _T ("hbox"),
    _T ("headheight"),
    _T ("headline"),
    _T ("headsep"),
    _T ("heartsuit"),
    _T ("hfil"),
    _T ("hfill"),
    _T ("hfill"),
    _T ("hline"),
    _T ("hoffset"),
    _T ("hoffset"),
    _T ("hom"),
    _T ("hookleftarrow"),
    _T ("hookrightarrow"),
    _T ("hrule"),
    _T ("hrulefill"),
    _T ("hshade"),
    _T ("hsize"),
    _T ("hskip"),
    _T ("hspace"),
    _T ("hss"),
    _T ("huge"),
    _T ("HUGE"),
    _T ("if"),
    _T ("ifcase"),
    _T ("ifcat"),
    _T ("ifdim"),
    _T ("ifeof"),
    _T ("ifhmode"),
    _T ("ifmmode"),
    _T ("ifnum"),
    _T ("ifodd"),
    _T ("ifvmode"),
    _T ("ifx"),
    _T ("Im"),
    _T ("imath"),
    _T ("in"),
    _T ("inboundscheckoff"),
    _T ("inboundscheckon"),
    _T ("includegraphics"),
    _T ("inf"),
    _T ("infty"),
    _T ("input"),
    _T ("int"),
    _T ("iota"),
    _T ("it"),
    _T ("item"),
    _T ("jmath"),
    _T ("jobname"),
    _T ("kappa"),
    _T ("ker"),
    _T ("kill"),
    _T ("label"),
    _T ("lambda"),
    _T ("Lambda"),
    _T ("land"),
    _T ("langle"),
    _T ("langle"),
    _T ("Large"),
    _T ("large"),
    _T ("LARGE"),
    _T ("lbrace"),
    _T ("lbrack"),
    _T ("lceil"),
    _T ("ldots"),
    _T ("le"),
    _T ("leaders"),
    _T ("left"),
    _T ("leftarrow"),
    _T ("Leftarrow"),
    _T ("leftharpoondown"),
    _T ("leftharpoonup"),
    _T ("Leftrightarrow"),
    _T ("leftrightarrow"),
    _T ("leftskip"),
    _T ("leq"),
    _T ("lfloor"),
    _T ("lg"),
    _T ("lgroup"),
    _T ("lim"),
    _T ("liminfo"),
    _T ("limsup"),
    _T ("line"),
    _T ("line"),
    _T ("linebreak"),
    _T ("lines"),
    _T ("linethickness"),
    _T ("linethickness"),
    _T ("listoffigures"),
    _T ("listoftables"),
    _T ("ll"),
    _T ("ln"),
    _T ("lnot"),
    _T ("log"),
    _T ("longleftarrow"),
    _T ("Longleftarrow"),
    _T ("Longleftrightarrow"),
    _T ("longleftrightarrow"),
    _T ("longmapsto"),
    _T ("Longrightarrow"),
    _T ("longrightarrow"),
    _T ("loop"),
    _T ("lor"),
    _T ("magnification"),
    _T ("magstep"),
    _T ("magstephalf"),
    _T ("makebox"),
    _T ("maketitle"),
    _T ("mapsto"),
    _T ("matrix"),
    _T ("max"),
    _T ("mbox"),
    _T ("mid"),
    _T ("midinsert"),
    _T ("min"),
    _T ("models"),
    _T ("mp"),
    _T ("mu"),
    _T ("multicolumn"),
    _T ("multiply"),
    _T ("multiput"),
    _T ("multiput"),
    _T ("nabla"),
    _T ("natural"),
    _T ("nearrow"),
    _T ("neg"),
    _T ("newbox"),
    _T ("newcommand"),
    _T ("newcount"),
    _T ("newcounter"),
    _T ("newdimen"),
    _T ("newdimen"),
    _T ("newenvironment"),
    _T ("newlength"),
    _T ("newpage"),
    _T ("newread"),
    _T ("newsavebox"),
    _T ("newtheorem"),
    _T ("newwrite"),
    _T ("ni"),
    _T ("nobreak"),
    _T ("noindent"),
    _T ("nolinebreak"),
    _T ("nopagebreak"),
    _T ("nopagenumbers"),
    _T ("normalsize"),
    _T ("not"),
    _T ("nu"),
    _T ("number"),
    _T ("nwarrow"),
    _T ("o"),
    _T ("oddsidemargin"),
    _T ("odot"),
    _T ("oint"),
    _T ("Omega"),
    _T ("omega"),
    _T ("ominus"),
    _T ("openin"),
    _T ("openouput"),
    _T ("oplus"),
    _T ("or"),
    _T ("oslash"),
    _T ("otimes"),
    _T ("oval"),
    _T ("overbrace"),
    _T ("overleftarrow"),
    _T ("overline"),
    _T ("overrightarrow"),
    _T ("owns"),
    _T ("pagebreak"),
    _T ("pageno"),
    _T ("pagenumbering"),
    _T ("pageref"),
    _T ("pagestyle"),
    _T ("par"),
    _T ("paragraph"),
    _T ("parallel"),
    _T ("parindent"),
    _T ("parskip"),
    _T ("part"),
    _T ("partial"),
    _T ("perp"),
    _T ("phi"),
    _T ("Phi"),
    _T ("pi"),
    _T ("Pi"),
    _T ("plot"),
    _T ("plotheading"),
    _T ("pm"),
    _T ("pmatrix"),
    _T ("Pr"),
    _T ("prec"),
    _T ("preceq"),
    _T ("prime"),
    _T ("prod"),
    _T ("propto"),
    _T ("Psi"),
    _T ("psi"),
    _T ("put"),
    _T ("put"),
    _T ("putrule"),
    _T ("qquad"),
    _T ("quad"),
    _T ("quad"),
    _T ("raisebox"),
    _T ("rangle"),
    _T ("rangle"),
    _T ("rbrace"),
    _T ("rbrack"),
    _T ("rceil"),
    _T ("Re"),
    _T ("ref"),
    _T ("renewcommand"),
    _T ("repeat"),
    _T ("rfloor"),
    _T ("rgroup"),
    _T ("rho"),
    _T ("right"),
    _T ("rightarrow"),
    _T ("Rightarrow"),
    _T ("rightharpoondown"),
    _T ("rightharpoonup"),
    _T ("rightline"),
    _T ("rightskip"),
    _T ("rm"),
    _T ("romannumeral"),
    _T ("samepage"),
    _T ("savebox"),
    _T ("sbox"),
    _T ("sc"),
    _T ("scriptsize"),
    _T ("searrow"),
    _T ("sec"),
    _T ("section"),
    _T ("setbox"),
    _T ("setbox"),
    _T ("setcoordinatesystem"),
    _T ("setcounter"),
    _T ("setdashes"),
    _T ("setdashpattern"),
    _T ("setdots"),
    _T ("sethistograms"),
    _T ("setlength"),
    _T ("setlinear"),
    _T ("setminus"),
    _T ("setplotarea"),
    _T ("setplotsymbol"),
    _T ("setquadratic"),
    _T ("setquadratic"),
    _T ("setshadegrid"),
    _T ("setshadesymbol"),
    _T ("setsolid"),
    _T ("settabs"),
    _T ("settowidth"),
    _T ("sf"),
    _T ("shaderectangleson"),
    _T ("sharerectanglesoff"),
    _T ("sharp"),
    _T ("shortstack"),
    _T ("sigma"),
    _T ("Sigma"),
    _T ("sim"),
    _T ("simeq"),
    _T ("sin"),
    _T ("sinh"),
    _T ("sl"),
    _T ("small"),
    _T ("smallskip"),
    _T ("smile"),
    _T ("spadesuit"),
    _T ("sqcap"),
    _T ("sqcup"),
    _T ("sqsubseteq"),
    _T ("sqsupsetqe"),
    _T ("star"),
    _T ("startrotation"),
    _T ("stepcounter"),
    _T ("stoprotation"),
    _T ("subparagraph"),
    _T ("subsection"),
    _T ("subset"),
    _T ("subsetqe"),
    _T ("subsubsection"),
    _T ("succ"),
    _T ("succeq"),
    _T ("sum"),
    _T ("sup"),
    _T ("supset"),
    _T ("supsetqe"),
    _T ("surd"),
    _T ("swarrow"),
    _T ("tableofcontents"),
    _T ("tan"),
    _T ("tanh"),
    _T ("tau"),
    _T ("textheight"),
    _T ("textstyle"),
    _T ("textwidth"),
    _T ("Theta"),
    _T ("theta"),
    _T ("thicklines"),
    _T ("thinlines"),
    _T ("thispagestyle"),
    _T ("tilde"),
    _T ("times"),
    _T ("tiny"),
    _T ("title"),
    _T ("to"),
    _T ("top"),
    _T ("topinsert"),
    _T ("topmargin"),
    _T ("topskip"),
    _T ("triangle"),
    _T ("triangleleft"),
    _T ("tt"),
    _T ("underbrace"),
    _T ("uparrow"),
    _T ("uparrow"),
    _T ("Uparrow"),
    _T ("Updownarrow"),
    _T ("updownarrow"),
    _T ("uplus"),
    _T ("upsilon"),
    _T ("Upsilon"),
    _T ("usebox"),
    _T ("value"),
    _T ("varepsilon"),
    _T ("varphi"),
    _T ("varpi"),
    _T ("varrho"),
    _T ("varsigma"),
    _T ("vartheta"),
    _T ("vbox"),
    _T ("vdash"),
    _T ("vdots"),
    _T ("vec"),
    _T ("vector"),
    _T ("vee"),
    _T ("Vert"),
    _T ("vert"),
    _T ("vfil"),
    _T ("vfill"),
    _T ("vfill"),
    _T ("vglue"),
    _T ("voffset"),
    _T ("voffset"),
    _T ("vrule"),
    _T ("vshade"),
    _T ("vsize"),
    _T ("vskip"),
    _T ("vspace"),
    _T ("vss"),
    _T ("wedge"),
    _T ("widehat"),
    _T ("widetilde"),
    _T ("wp"),
    _T ("wr"),
    _T ("write"),
    _T ("Xi"),
    _T ("xi"),
    _T ("zeta"),
  };

static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T ("addcontentsline"),
    _T ("addpenalty"),
    _T ("addtocontents"),
    _T ("addtoversion"),
    _T ("addvspace"),
    _T ("afterassignment"),
    _T ("allocationnumber"),
    _T ("allowbreak"),
    _T ("alph"),
    _T ("Alph"),
    _T ("arrayrulewidth"),
    _T ("arraystretch"),
    _T ("AtBeginDocument"),
    _T ("AtBeginDvi"),
    _T ("AtEndDocument"),
    _T ("AtEndOfPackage"),
    _T ("baselinestretch"),
    _T ("bezier"),
    _T ("bfdefault"),
    _T ("bfseries"),
    _T ("bibcite"),
    _T ("bibliography"),
    _T ("bibliographystyle"),
    _T ("Big"),
    _T ("big"),
    _T ("bigbreak"),
    _T ("bigg"),
    _T ("Bigg"),
    _T ("bigskip"),
    _T ("bigskipamount"),
    _T ("bigskipamount"),
    _T ("bmod"),
    _T ("boldmath"),
    _T ("Box"),
    _T ("buildrel"),
    _T ("cases"),
    _T ("catcode"),
    _T ("center"),
    _T ("centering"),
    _T ("chardef"),
    _T ("CheckCommand"),
    _T ("ClassError"),
    _T ("ClassInfo"),
    _T ("ClassWarning"),
    _T ("ClassWarningNoLine"),
    _T ("cleardoublepage"),
    _T ("contentsline"),
    _T ("countdef"),
    _T ("cr"),
    _T ("csname"),
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
    _T ("depth"),
    _T ("discretionary"),
    _T ("displaymath"),
    _T ("dospecials"),
    _T ("dotfill"),
    _T ("encodingdefault"),
    _T ("endcsname"),
    _T ("everycr"),
    _T ("ExecuteOptions"),
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
    _T ("IfFileExists"),
    _T ("ifinner"),
    _T ("immediate"),
    _T ("include"),
    _T ("includeonly"),
    _T ("InputIfFileExists"),
    _T ("itdefault"),
    _T ("iterate"),
    _T ("Join"),
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
    _T ("LoadClass"),
    _T ("LoadClassWithOptions"),
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
    _T ("MessageBreak"),
    _T ("mho"),
    _T ("minipage"),
    _T ("mskip"),
    _T ("multispan"),
    _T ("narrower"),
    _T ("NeedsTeXFormat"),
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
    _T ("OptionNotUsed"),
    _T ("PackageError"),
    _T ("PackageInfo"),
    _T ("PackageWarning"),
    _T ("PackageWarningNoLine"),
    _T ("pagenumberin"),
    _T ("parbox"),
    _T ("PassOptionsToClass"),
    _T ("PassOptionsToPackage"),
    _T ("penalty"),
    _T ("phantom"),
    _T ("pmod"),
    _T ("ProcessOptions"),
    _T ("protect"),
    _T ("providecommand"),
    _T ("ProvidesFile"),
    _T ("ProvidesPackage"),
    _T ("ProvideTextCommand"),
    _T ("ProvideTextCommandDefault"),
    _T ("raggedbottom"),
    _T ("raggedleft"),
    _T ("raggedright"),
    _T ("refstepcounter"),
    _T ("relax"),
    _T ("removelastskip"),
    _T ("renewenvironment"),
    _T ("RequirePackage"),
    _T ("RequirePackageWithOptions"),
    _T ("reversemarginpar"),
    _T ("rhd"),
    _T ("rightmark"),
    _T ("rlap"),
    _T ("rmdefault"),
    _T ("Roman"),
    _T ("roman"),
    _T ("root"),
    _T ("rq"),
    _T ("rule"),
    _T ("scdefault"),
    _T ("secdef"),
    _T ("selectfont"),
    _T ("seriesdefault"),
    _T ("SetMathAlphabet"),
    _T ("SetSymbolFont"),
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
    _T ("TeX"),
    _T ("TextSymbolUnavailable"),
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
    _T ("UseTextAccent"),
    _T ("UseTextSymbol"),
    _T ("vcenter"),
    _T ("verb"),
    _T ("verbatim"),
    _T ("vline"),
    _T ("vphantom"),
    _T ("width"),
    _T ("wlog"),
    _T ("xdef"),
  };

static bool
IsTexKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszTexKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineTex (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars+I) - pszChars))
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
                  bRedefineBlock = true;
                  bDecIndex = true;
                  goto out;
                }
            }
          bRedefineBlock = false;
          bDecIndex = false;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength || pszChars[I] == 0)
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
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
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

      if (pBuf == nullptr)
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
              else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = false;
                  for (int j = I; j >= 0; j--)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '$')
                            {
                              bFunction = true;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = true;
              bDecIndex = true;
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
      else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = false;
          for (int j = I; j >= 0; j--)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '$')
                    {
                      bFunction = true;
                    }
                  break;
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
