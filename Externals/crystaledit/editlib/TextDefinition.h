#pragma once

#include "utils/ctchar.h"
#include <array>

#define SRCOPT_INSERTTABS 1
#define SRCOPT_SHOWTABS 2
#define SRCOPT_BSATBOL 4
#define SRCOPT_SELMARGIN 8
#define SRCOPT_AUTOINDENT 16
#define SRCOPT_BRACEANSI 32
#define SRCOPT_BRACEGNU 64
#define SRCOPT_EOLNDOS 128
#define SRCOPT_EOLNUNIX 256
#define SRCOPT_EOLNMAC 512
#define SRCOPT_FNBRACE 1024
#define SRCOPT_WORDWRAP 2048
#define SRCOPT_TOPMARGIN 4096

namespace LangServices
{

/**
 * @brief Language / file-format type enum.
 */
enum LanguageId
{
	SRC_PLAIN = 0,
	SRC_ABAP,
	SRC_ADA,
	SRC_ASP,
	SRC_AUTOIT,
	SRC_BASIC,
	SRC_BATCH,
	SRC_C,
	SRC_CSHARP,
	SRC_CSS,
	SRC_DCL,
	SRC_DLANG,
	SRC_FORTRAN,
	SRC_FSHARP,
	SRC_FSHARP_SIGNATURE,
	SRC_GO,
	SRC_HTML,
	SRC_INI,
	SRC_INNOSETUP,
	SRC_INSTALLSHIELD,
	SRC_JAVA,
	SRC_JAVASCRIPT,
	SRC_JSON,
	SRC_LISP,
	SRC_LUA,
	SRC_MARKDOWN,
	SRC_MATLAB,
	SRC_NSIS,
	SRC_PASCAL,
	SRC_PERL,
	SRC_PHP,
	SRC_PO,
	SRC_POWERSHELL,
	SRC_PYTHON,
	SRC_REXX,
	SRC_RSRC,
	SRC_RUBY,
	SRC_RUST,
	SRC_SGML,
	SRC_SH,
	SRC_SIOD,
	SRC_SMARTY,
	SRC_SQL,
	SRC_TCL,
	SRC_TEX,
	SRC_TSX,
	SRC_TYPESCRIPT,
	SRC_VERILOG,
	SRC_VHDL,
	SRC_XML,
	SRC_YAML,
	SRC_MAX_ENTRY, /* always last entry, used for bound checking */
};

// Tabsize is commented out since we have only GUI setting for it now.
// Not removed because we may later want to have per-filetype settings again.
// See ccrystaltextview.cpp for per filetype table initialization.
struct TextDefinition
{
	LanguageId type;
	const tchar_t* name;
	tchar_t* exts;
	bool extsIsDynamic;
	unsigned flags;
	//        unsigned tabsize;
	tchar_t opencomment[8];
	tchar_t closecomment[8];
	tchar_t commentline[8];
	unsigned encoding;
};

TextDefinition* GetTextType(const tchar_t* pszExt);
TextDefinition* GetTextType(int index);
TextDefinition* GetTextType(LanguageId type);
void SetExtension(int index, const tchar_t* pszExts);

}
