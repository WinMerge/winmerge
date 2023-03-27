#include "pch.h"
#include "crystallineparser.h"
#include "../utils/fpattern.h"
#include <string>
#include <algorithm>

namespace CrystalLineParser
{

// Tabsize is commented out since we have only GUI setting for it now.
// Not removed because we may later want to have per-filetype settings again.
// See ccrystaltextview.h for table declaration.
TextDefinition m_SourceDefs[] =
{
	SRC_PLAIN, _T ("Plain"), _T ("txt;doc;diz"), &ParseLinePlain, SRCOPT_AUTOINDENT, /*4,*/ _T (""), _T (""), _T (""), (unsigned)-1,
	SRC_ABAP, _T("ABAP"), _T("abap"),& ParseLineAbap, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI , /*2,*/ _T(""), _T(""), _T("'"), (unsigned)-1,
	SRC_ASP, _T ("ASP"), _T ("asp;ascx"), &ParseLineAsp, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T (""), _T (""), _T ("'"), (unsigned)-1,
	SRC_AUTOIT, _T ("AutoIt"), _T ("au3"), &ParseLineAutoIt, SRCOPT_AUTOINDENT, /*4,*/ _T (""), _T (""), _T (";"), (unsigned)-1,
	SRC_BASIC, _T ("Basic"), _T ("bas;vb;vbs;frm;dsm;cls;ctl;pag;dsr"), &ParseLineBasic, SRCOPT_AUTOINDENT, /*4,*/ _T (""), _T (""), _T ("\'"), (unsigned)-1,
	SRC_BATCH, _T ("Batch"), _T ("bat;btm;cmd"), &ParseLineBatch, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT, /*4,*/ _T (""), _T (""), _T ("rem "), (unsigned)-1,
	SRC_C, _T ("C"), _T ("c;cc;cpp;cxx;h;hpp;hxx;hm;inl;rh;tlh;tli;xs"), &ParseLineC, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_CSHARP, _T ("C#"), _T ("cs"), &ParseLineCSharp, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_CSS, _T ("CSS"), _T ("css"), &ParseLineCss, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T (""), (unsigned)-1,
	SRC_DCL, _T ("DCL"), _T ("dcl;dcc"), &ParseLineDcl, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_DLANG, _T ("D"), _T ("d;di"), &ParseLineDlang, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_FORTRAN, _T ("Fortran"), _T ("f;f90;f9p;fpp;for;f77"), &ParseLineFortran, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT, /*8,*/ _T (""), _T (""), _T ("!"), (unsigned)-1,
	SRC_GO, _T ("Go"), _T ("go"), &ParseLineGo, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_HTML, _T ("HTML"), _T ("html;htm;shtml;ihtml;ssi;stm;stml;jsp"), &ParseLineHtml, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("<!--"), _T ("-->"), _T (""), (unsigned)-1,
	SRC_INI, _T ("INI"), _T ("ini;reg;vbp;isl"), &ParseLineIni, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU|SRCOPT_EOLNUNIX, /*2,*/ _T (""), _T (""), _T (";"), (unsigned)-1,
	SRC_INNOSETUP, _T ("InnoSetup"), _T ("iss"), &ParseLineInnoSetup, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("{"), _T ("}"), _T (";"), (unsigned)-1,
	SRC_INSTALLSHIELD, _T ("InstallShield"), _T ("rul"), &ParseLineIS, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_JAVA, _T ("Java"), _T ("java;jav"), &ParseLineJava, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_JAVASCRIPT, _T ("JavaScript"), _T ("js"), &ParseLineJavaScript, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_JSON, _T ("JSON"), _T ("json"), &ParseLineJavaScript, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_LISP, _T ("AutoLISP"), _T ("lsp;dsl"), &ParseLineLisp, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T (";|"), _T ("|;"), _T (";"), (unsigned)-1,
	SRC_LUA, _T ("Lua"), _T ("lua"), &ParseLineLua, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("--[["), _T ("]]"), _T ("--"), (unsigned)-1,
	SRC_MATLAB, _T ("MATLAB"), _T ("m"), &ParseLineMatlab, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T ("%{"), _T ("%}"), _T ("%"), (unsigned)-1,
	SRC_NSIS, _T ("NSIS"), _T ("nsi;nsh"), &ParseLineNsis, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T (";"), (unsigned)-1,
	SRC_PASCAL, _T ("Pascal"), _T ("pas"), &ParseLinePascal, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("{"), _T ("}"), _T (""), (unsigned)-1,
	SRC_PERL, _T ("Perl"), _T ("pl;pm;plx"), &ParseLinePerl, SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, /*4,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_PHP, _T ("PHP"), _T ("php;php3;php4;php5;phtml"), &ParseLinePhp, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_PO, _T ("PO"), _T ("po;pot"), &ParseLinePo, SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, /*4,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_POWERSHELL, _T ("PowerShell"), _T ("ps1"), &ParseLinePowerShell, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_PYTHON, _T ("Python"), _T ("py"), &ParseLinePython, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_REXX, _T ("REXX"), _T ("rex;rexx"), &ParseLineRexx, SRCOPT_AUTOINDENT, /*4,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_RSRC, _T ("Resources"), _T ("rc;dlg;r16;r32;rc2"), &ParseLineRsrc, SRCOPT_AUTOINDENT, /*4,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_RUBY, _T ("Ruby"), _T ("rb;rbw;rake;gemspec"), &ParseLineRuby, SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, /*4,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_RUST, _T ("Rust"), _T ("rs"), &ParseLineRust, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_SGML, _T ("Sgml"), _T ("sgml"), &ParseLineSgml, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("<!--"), _T ("-->"), _T (""), (unsigned)-1,
	SRC_SH, _T ("Shell"), _T ("sh;conf"), &ParseLineSh, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, /*4,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_SIOD, _T ("SIOD"), _T ("scm"), &ParseLineSiod, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, /*2,*/ _T (";|"), _T ("|;"), _T (";"), (unsigned)-1,
	SRC_SMARTY, _T("Smarty"), _T("tpl"), &ParseLineSmarty, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T("{*"), _T("*}"), _T(""), (unsigned)-1,
	SRC_SQL, _T ("SQL"), _T ("sql"), &ParseLineSql, SRCOPT_AUTOINDENT, /*4,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_TCL, _T ("TCL"), _T ("tcl"), &ParseLineTcl, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU|SRCOPT_EOLNUNIX, /*2,*/ _T (""), _T (""), _T ("#"), (unsigned)-1,
	SRC_TEX, _T ("TEX"), _T ("tex;sty;clo;ltx;fd;dtx"), &ParseLineTex, SRCOPT_AUTOINDENT, /*4,*/ _T (""), _T (""), _T ("%"), (unsigned)-1,
	SRC_VERILOG, _T ("Verilog"), _T ("v;vh"), &ParseLineVerilog, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("/*"), _T ("*/"), _T ("//"), (unsigned)-1,
	SRC_VHDL, _T ("VHDL"), _T ("vhd;vhdl;vho"), &ParseLineVhdl, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T (""), _T (""), _T ("--"), (unsigned)-1,
	SRC_XML, _T ("XML"), _T ("xml"), &ParseLineXml, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, /*2,*/ _T ("<!--"), _T ("-->"), _T (""), (unsigned)-1
};

static bool
MatchType (const std::basic_string<tchar_t>& pattern, const tchar_t* lpszExt)
{
  std::basic_string<tchar_t> pattern2(pattern);
  std::basic_string<tchar_t> part;
  size_t pos, len = pattern2.length ();

  while ((pos = pattern2.find (_T (';'))) != -1)
    {
      part = pattern2.substr (0, pos);
      if (!part.empty() && fpattern_isvalid (part.c_str()))
        {
          if (fpattern_matchn (part.c_str(), lpszExt))
            {
              return true;
            }
        }
      len -= pos + 1;
      pattern2 = pattern2.substr (pattern2.length() - len, len);
    }
  if (!pattern2.empty () && fpattern_isvalid (pattern2.c_str()))
    {
      if (fpattern_matchn (pattern2.c_str(), lpszExt))
        {
          return true;
        }
    }
  return false;
}

TextDefinition*
GetTextType (const tchar_t *pszExt)
{
  TextDefinition *def;
  std::basic_string<tchar_t> sExt = pszExt;

  def = &m_SourceDefs[1];
  std::transform(sExt.begin(), sExt.end(), sExt.begin(), tc::totlower);
  for (int i = 1; i < _countof (m_SourceDefs); i++, def++)
    if (MatchType (def->exts, sExt.c_str()))
      return def;

  // Check if the extension matches the text type "Plain" at the end.
  def = m_SourceDefs;
  if (MatchType (def->exts, sExt.c_str()))
    return def;

  return nullptr;
}

/**
 * @brief Get the text type of the specified index.
 * @param [in] index Index of m_SourceDefs
 * @return the text type of the specified index.
 */
TextDefinition*
GetTextType(int index)
{
  if (index < 0 || index > SRC_XML)
    return nullptr;
  return &m_SourceDefs[index];
}

/**
 * @brief Set the extension settings for the specified index.
 * @param [in] index Index of m_SourceDefs
 * @param [in] exts Extension setting
 */
void
SetExtension(int index, const tchar_t *pszExts)
{
  if (index < 0 || index > SRC_XML)
    return;

  tc::tcslcpy(m_SourceDefs[index].exts, pszExts);
}

}
