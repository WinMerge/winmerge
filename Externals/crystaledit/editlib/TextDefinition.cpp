#include "pch.h"
#include "TextDefinition.h"
#include "utils/fpattern.h"
#include <string>
#include <algorithm>

namespace LangServices
{

std::array<TextDefinition, static_cast<size_t>(SRC_MAX_ENTRY)> m_SourceDefs =
{
	SRC_PLAIN, _T("Plain"), _T("txt;doc;diz"), false, SRCOPT_AUTOINDENT, /*4,*/ _T(""), _T(""), _T(""), (unsigned)-1,
	SRC_ABAP, _T("ABAP"), _T("abap"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI , /*2,*/ _T(""), _T(""), _T("'"), (unsigned)-1,
	SRC_ADA, _T("Ada"), _T("ads;adb"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*3,*/ _T(""), _T(""), _T("'"), (unsigned)-1,
	SRC_ASP, _T("ASP"), _T("asp;ascx"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T(""), _T(""), _T("'"), (unsigned)-1,
	SRC_AUTOIT, _T("AutoIt"), _T("au3"), false, SRCOPT_AUTOINDENT, /*4,*/ _T(""), _T(""), _T(";"), (unsigned)-1,
	SRC_BASIC, _T("Basic"), _T("bas;vb;vbs;frm;dsm;cls;ctl;pag;dsr"), false, SRCOPT_AUTOINDENT, /*4,*/ _T(""), _T(""), _T("\'"), (unsigned)-1,
	SRC_BATCH, _T("Batch"), _T("bat;btm;cmd"), false, SRCOPT_INSERTTABS | SRCOPT_AUTOINDENT, /*4,*/ _T(""), _T(""), _T("rem "), (unsigned)-1,
	SRC_C, _T("C"), _T("c;cc;cpp;cppm;ixx;cxx;h;hpp;hxx;hm;inl;rh;tlh;tli;xs"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_CSHARP, _T("C#"), _T("cs"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_CSS, _T("CSS"), _T("css"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T(""), (unsigned)-1,
	SRC_DCL, _T("DCL"), _T("dcl;dcc"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_DLANG, _T("D"), _T("d;di"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_FORTRAN, _T("Fortran"), _T("f;f90;f9p;fpp;for;f77"), false, SRCOPT_INSERTTABS | SRCOPT_AUTOINDENT, /*8,*/ _T(""), _T(""), _T("!"), (unsigned)-1,
	SRC_FSHARP, _T("F#"), _T("fs;fsx"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_GO, _T("Go"), _T("go"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_HTML, _T("HTML"), _T("html;htm;shtml;ihtml;ssi;stm;stml;jsp"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("<!--"), _T("-->"), _T(""), (unsigned)-1,
	SRC_INI, _T("INI"), _T("ini;reg;vbp;isl"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU | SRCOPT_EOLNUNIX, /*2,*/ _T(""), _T(""), _T(";"), (unsigned)-1,
	SRC_INNOSETUP, _T("InnoSetup"), _T("iss"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("{"), _T("}"), _T(";"), (unsigned)-1,
	SRC_INSTALLSHIELD, _T("InstallShield"), _T("rul"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_JAVA, _T("Java"), _T("java;jav"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_JAVASCRIPT, _T("JavaScript"), _T("js;ts"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_JSON, _T("JSON"), _T("json"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_LISP, _T("AutoLISP"), _T("lsp;dsl"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T(";|"), _T("|;"), _T(";"), (unsigned)-1,
	SRC_LUA, _T("Lua"), _T("lua"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("--[["), _T("]]"), _T("--"), (unsigned)-1,
	SRC_MATLAB, _T("MATLAB"), _T("m"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("%{"), _T("%}"), _T("%"), (unsigned)-1,
	SRC_NSIS, _T("NSIS"), _T("nsi;nsh"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T(";"), (unsigned)-1,
	SRC_PASCAL, _T("Pascal"), _T("pas"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("{"), _T("}"), _T(""), (unsigned)-1,
	SRC_PERL, _T("Perl"), _T("pl;pm;plx"), false, SRCOPT_AUTOINDENT | SRCOPT_EOLNUNIX, /*4,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_PHP, _T("PHP"), _T("php;php3;php4;php5;phtml"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_PO, _T("PO"), _T("po;pot"), false, SRCOPT_AUTOINDENT | SRCOPT_EOLNUNIX, /*4,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_POWERSHELL, _T("PowerShell"), _T("ps1;psm1;psd1"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_PYTHON, _T("Python"), _T("py"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_REXX, _T("REXX"), _T("rex;rexx"), false, SRCOPT_AUTOINDENT, /*4,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_RSRC, _T("Resources"), _T("rc;dlg;r16;r32;rc2"), false, SRCOPT_AUTOINDENT, /*4,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_RUBY, _T("Ruby"), _T("rb;rbw;rake;gemspec"), false, SRCOPT_AUTOINDENT | SRCOPT_EOLNUNIX, /*4,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_RUST, _T("Rust"), _T("rs"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_SGML, _T("Sgml"), _T("sgml"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("<!--"), _T("-->"), _T(""), (unsigned)-1,
	SRC_SH, _T("Shell"), _T("sh;conf"), false, SRCOPT_INSERTTABS | SRCOPT_AUTOINDENT | SRCOPT_EOLNUNIX, /*4,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_SIOD, _T("SIOD"), _T("scm"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T(";|"), _T("|;"), _T(";"), (unsigned)-1,
	SRC_SMARTY, _T("Smarty"), _T("tpl"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU, /*2,*/ _T("{*"), _T("*}"), _T(""), (unsigned)-1,
	SRC_SQL, _T("SQL"), _T("sql"), false, SRCOPT_AUTOINDENT, /*4,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_TCL, _T("TCL"), _T("tcl"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEGNU | SRCOPT_EOLNUNIX, /*2,*/ _T(""), _T(""), _T("#"), (unsigned)-1,
	SRC_TEX, _T("TEX"), _T("tex;sty;clo;ltx;fd;dtx"), false, SRCOPT_AUTOINDENT, /*4,*/ _T(""), _T(""), _T("%"), (unsigned)-1,
	SRC_VERILOG, _T("Verilog"), _T("v;vh"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("/*"), _T("*/"), _T("//"), (unsigned)-1,
	SRC_VHDL, _T("VHDL"), _T("vhd;vhdl;vho"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T(""), _T(""), _T("--"), (unsigned)-1,
	SRC_XML, _T("XML"), _T("xml"), false, SRCOPT_AUTOINDENT | SRCOPT_BRACEANSI, /*2,*/ _T("<!--"), _T("-->"), _T(""), (unsigned)-1
};

static bool
MatchType(const std::basic_string<tchar_t>& pattern, const tchar_t* lpszExt)
{
  std::basic_string<tchar_t> pattern2(pattern);
  std::basic_string<tchar_t> part;
  size_t pos, len = pattern2.length();

  while ((pos = pattern2.find(_T(';'))) != -1)
	{
	  part = pattern2.substr(0, pos);
	  if (!part.empty() && fpattern_isvalid(part.c_str()))
		{
		  if (fpattern_matchn(part.c_str(), lpszExt))
			{
			  return true;
			}
		}
	  len -= pos + 1;
	  pattern2 = pattern2.substr(pattern2.length() - len, len);
	}
  if (!pattern2.empty() && fpattern_isvalid(pattern2.c_str()))
	{
	  if (fpattern_matchn(pattern2.c_str(), lpszExt))
		{
		  return true;
		}
	}
  return false;
}

TextDefinition*
GetTextType(const tchar_t* pszExt)
{
  TextDefinition* def;
  std::basic_string<tchar_t> sExt = pszExt;

  def = &m_SourceDefs[1];
  std::transform(sExt.begin(), sExt.end(), sExt.begin(), tc::totlower);
  for (size_t i = 1; i < m_SourceDefs.size(); i++, def++)
	if (MatchType(def->exts, sExt.c_str()))
	  return def;

  // Check if the extension matches the text type "Plain" at the end.
  def = &m_SourceDefs[0];
  if (MatchType(def->exts, sExt.c_str()))
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

TextDefinition*
GetTextType(LanguageId type)
{
  int index = static_cast<int>(type);
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
SetExtension(int index, const tchar_t* pszExts)
{
  if (index < 0 || index > SRC_XML)
	return;

  auto& def = m_SourceDefs[index];
  if (tc::tcscmp(def.exts, pszExts) == 0)
	return;
  if (def.extsIsDynamic)
	delete[] def.exts;
  const size_t size = tc::tcslen(pszExts) + 1;
  def.exts = new tchar_t[size];
  tc::tcslcpy(def.exts, size, pszExts);
  def.extsIsDynamic = true;
}

}
