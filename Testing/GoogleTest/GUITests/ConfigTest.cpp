/**
 * @file  ConfigTest.cpp
 *
 * @brief Implementation for Config testcase.
 */

#include "pch.h"
#include "Resource.h"
#define OP(x) (_T(x), sizeof(_T(x))/sizeof(TCHAR) - 1)
#include "OptionsDef.h"
#include "varprop.h"
#include <variant>

namespace
{

	using namespace GUITestUtils;

	class ConfigTest : public CommonTest
	{
	public:
		ConfigTest()
		{
		}

		virtual ~ConfigTest()
		{
		}

		static void SetUpTestCase()
		{
			// You can do set-up work for each test	here.
		}

		static void TearDownTestCase()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:
		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	struct Option
	{
		const std::wstring name;
		varprop::VT_TYPE type;
		std::vector<int> iValues;
		const std::vector<std::wstring> sValues;
	} options[] =
	{
		// User's language
		{ OPT_SELECTED_LANGUAGE, varprop::VT_INT, {
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA),
			MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
			MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL),
			MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT),
			MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT),
			MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH),
			MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT),
			MAKELANGID(LANG_FINNISH, SUBLANG_FINNISH_FINLAND),
			MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),
			MAKELANGID(LANG_GALICIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),
			MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT),
			MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),
			MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT),
			MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT),
			MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),
			MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN),
			MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT),
			MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC),
			MAKELANGID(LANG_SINHALESE, SUBLANG_DEFAULT),
			MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT),
			MAKELANGID(LANG_SLOVENIAN, SUBLANG_SLOVENIAN_SLOVENIA),
			MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN),
			MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT),
			MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT),
			MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT),
			}, {} },
		{ OPT_SHOW_UNIQUE_LEFT, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_UNIQUE_MIDDLE, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_UNIQUE_RIGHT, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_DIFFERENT, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_IDENTICAL, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_BINARIES, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_SKIPPED, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_DIFFERENT_LEFT_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_DIFFERENT_MIDDLE_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_DIFFERENT_RIGHT_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_MISSING_LEFT_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_MISSING_MIDDLE_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_MISSING_RIGHT_ONLY, varprop::VT_BOOL, {}, {} },
		{ OPT_TREE_MODE, varprop::VT_BOOL, {}, {} },
		// Show/hide toolbar/statusbar/tabbar
		{ OPT_SHOW_TOOLBAR, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_STATUSBAR, varprop::VT_BOOL, {}, {} },
		{ OPT_SHOW_TABBAR, varprop::VT_BOOL, {}, {} },
		{ OPT_TOOLBAR_SIZE, varprop::VT_INT, {0, 1, 2}, {} },
		{ OPT_RESIZE_PANES, varprop::VT_BOOL, {}, {} },
		//
		{ OPT_SYNTAX_HIGHLIGHT, varprop::VT_BOOL, {}, {} },
		{ OPT_VIEW_WHITESPACE, varprop::VT_BOOL, {}, {} },
		{ OPT_VIEW_EOL, varprop::VT_BOOL, {}, {} },
		{ OPT_SCROLL_TO_FIRST, varprop::VT_BOOL, {}, {} },
		{ OPT_SCROLL_TO_FIRST_INLINE_DIFF, varprop::VT_BOOL, {}, {} },
		{ OPT_RENDERING_MODE, varprop::VT_INT, {0, 1, 2, 3, 4, 6}, {} },
		//
		{ OPT_WORDDIFF_HIGHLIGHT, varprop::VT_BOOL, {}, {} },
		{ OPT_BREAK_ON_WORDS, varprop::VT_BOOL, {}, {} },
		{ OPT_BREAK_TYPE, varprop::VT_INT, {0, 1}, {} },
		{ OPT_BREAK_SEPARATORS, varprop::VT_STRING, {}, {L"", L".,:; ? []() {} <=> `'!\"#$%&^~\\|@+-*/"}},
		// Backup options
		{ OPT_BACKUP_FOLDERCMP, varprop::VT_BOOL, {}, {} },
		{ OPT_BACKUP_FILECMP, varprop::VT_BOOL, {}, {} },
		{ OPT_BACKUP_LOCATION, varprop::VT_INT, {0, 1}, {} },
		{ OPT_BACKUP_GLOBALFOLDER, varprop::VT_STRING, {}, {L"", L"c:\\tmp"}},
		{ OPT_BACKUP_ADD_BAK, varprop::VT_BOOL, {}, {} },
		{ OPT_BACKUP_ADD_TIME, varprop::VT_BOOL, {}, {} },
		//
		{ OPT_DIRVIEW_SORT_COLUMN, varprop::VT_INT, {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {} },
		{ OPT_DIRVIEW_SORT_COLUMN3, varprop::VT_INT, {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {} },
		{ OPT_DIRVIEW_SORT_ASCENDING, varprop::VT_BOOL, {}, {} },
		{ OPT_DIRVIEW_EXPAND_SUBDIRS, varprop::VT_BOOL, {}, {} },
		{ OPT_DIRVIEW_COLUMN_ORDERS, varprop::VT_STRING, {}, { L"", L"0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26"}},
		{ OPT_DIRVIEW_COLUMN_WIDTHS, varprop::VT_STRING, {}, {L"", L"222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222"}},
		{ OPT_DIRVIEW3_COLUMN_ORDERS, varprop::VT_STRING, {}, { L"", L"0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26"}},
		{ OPT_DIRVIEW3_COLUMN_WIDTHS, varprop::VT_STRING, {}, {L"", L"222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222 222"}},
		// Window / Pane
		{ OPT_ACTIVE_FRAME_MAX, varprop::VT_BOOL, {}, {} },
		{ OPT_ACTIVE_PANE, varprop::VT_INT, {-1, 0, 1, 2}, {} },
		// Folder Compare Report
		{ OPT_REPORTFILES_REPORTTYPE, varprop::VT_INT, {0, 1, 2, 3}, {} },
		{ OPT_REPORTFILES_COPYTOCLIPBOARD, varprop::VT_BOOL, {}, {} },
		{ OPT_REPORTFILES_INCLUDEFILECMPREPORT, varprop::VT_BOOL, {}, {} },
		//  File compare
		{ OPT_AUTOMATIC_RESCAN, varprop::VT_BOOL, {}, {} },
		{ OPT_ALLOW_MIXED_EOL, varprop::VT_BOOL, {}, {} },
		{ OPT_COPY_FULL_LINE, varprop::VT_BOOL, {}, {} },
		{ OPT_TAB_SIZE, varprop::VT_INT, {1, 2, 4, 8, 128}, {} },
		{ OPT_TAB_TYPE, varprop::VT_INT, {0, 1}, {} },
		{ OPT_WORDWRAP, varprop::VT_BOOL, {}, {} },
		{ OPT_WORDWRAP_TABLE, varprop::VT_BOOL, {}, {} },
		{ OPT_VIEW_LINENUMBERS, varprop::VT_BOOL, {}, {} },
		{ OPT_VIEW_FILEMARGIN, varprop::VT_BOOL, {}, {} },
		{ OPT_DIFF_CONTEXT, varprop::VT_INT, {0, 1, 3, 5, 7, 9}, {} },
		{ OPT_INVERT_DIFF_CONTEXT, varprop::VT_BOOL, {}, {} },
		{ OPT_EXT_EDITOR_CMD, varprop::VT_STRING, {}, {L"", L"notepad.exe"}},
		{ OPT_USE_RECYCLE_BIN, varprop::VT_BOOL, {}, {} },
		{ OPT_SINGLE_INSTANCE, varprop::VT_INT, {0, 1, 2}, {} },
		{ OPT_MERGE_MODE, varprop::VT_BOOL, {}, {} },
		{ OPT_CLOSE_WITH_ESC, varprop::VT_INT, {0, 1, 2, 3}, {} },
		{ OPT_CLOSE_WITH_OK, varprop::VT_BOOL, {}, {} },
		{ OPT_VERIFY_OPEN_PATHS, varprop::VT_BOOL, {}, {} },
		{ OPT_AUTO_COMPLETE_SOURCE, varprop::VT_INT, {0, 1, 2}, {} },
		{ OPT_IGNORE_SMALL_FILETIME, varprop::VT_BOOL, {}, {} },
		{ OPT_ASK_MULTIWINDOW_CLOSE, varprop::VT_BOOL, {}, {} },
		{ OPT_PRESERVE_FILETIMES, varprop::VT_BOOL, {}, {} },
		{ OPT_CP_DEFAULT_CUSTOM, varprop::VT_INT, {1252, 1200, 65001}, {} },
		{ OPT_CP_DETECT, varprop::VT_INT, 
			{0,
			 1,
			 static_cast<int>(2 | (50001u << 16)),
			 static_cast<int>(2 | (50936u << 16)),
			 static_cast<int>(2 | (50950u << 16)),
			 static_cast<int>(2 | (50932u << 16)),
			 static_cast<int>(2 | (50949u << 16)),
			 static_cast<int>(3 | (50001u << 16)),
			 static_cast<int>(3 | (50936u << 16)),
			 static_cast<int>(3 | (50950u << 16)),
			 static_cast<int>(3 | (50932u << 16)),
			 static_cast<int>(3 | (50949u << 16)),
			}, {} },
		{ OPT_PRESERVE_FILETIMES, varprop::VT_BOOL, {}, {} },
		{ OPT_PROJECTS_PATH, varprop::VT_STRING, {}, {L"", L"c:\\tmp"}},
		{ OPT_USE_SYSTEM_TEMP_PATH, varprop::VT_BOOL, {}, {}},
		{ OPT_CUSTOM_TEMP_PATH, varprop::VT_STRING, {}, {L"", L"c:\\tmp"}},
		{ OPT_SPLIT_HORIZONTALLY, varprop::VT_BOOL, {}, {}},
		{ OPT_FILE_SIZE_THRESHOLD, varprop::VT_INT, {0xffff, 0x7fffffff}, {}},
		{ OPT_AUTO_RELOAD_MODIFIED_FILES, varprop::VT_INT, {0, 2}, {}},
		//
		{ OPT_DIRCLR_USE_COLORS, varprop::VT_BOOL, {}, {}},
		{ OPT_COLOR_SCHEME, varprop::VT_STRING, {}, {L"", L"Default", L"Modern", L"Solarized Dark", L"Solarized Light"}},
		// Compare options
		{ OPT_CMP_IGNORE_WHITESPACE, varprop::VT_INT, {0, 1, 2}, {}},
		{ OPT_CMP_IGNORE_BLANKLINES, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_FILTER_COMMENTLINES, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IGNORE_CASE, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IGNORE_EOL, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IGNORE_CODEPAGE, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_METHOD, varprop::VT_INT, {0, 1, 2, 3, 4, 5}, {}},
		{ OPT_CMP_MOVED_BLOCKS, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_MATCH_SIMILAR_LINES, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_STOP_AFTER_FIRST, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_QUICK_LIMIT, varprop::VT_INT, {0, 1, 4, 1024}, {}},
		{ OPT_CMP_BINARY_LIMIT, varprop::VT_INT, {0, 1, 4, 1024}, {}},
		{ OPT_CMP_COMPARE_THREADS, varprop::VT_INT, {-1, 1, 2}, {}},
		{ OPT_CMP_IGNORE_REPARSE_POINTS, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_INCLUDE_SUBDIRS, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_DIFF_ALGORITHM, varprop::VT_INT, {0, 1, 2, 3, 4}, {}},
		{ OPT_CMP_INDENT_HEURISTIC, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES, varprop::VT_BOOL, {}, {}},
		// Image Compare options
		{ OPT_CMP_IMG_FILEPATTERNS, varprop::VT_STRING, {}, {L"", L"*.bmp;*.cut;*.dds;*.exr;*.g3;*.gif;*.hdr;*.ico;*.iff;*.lbm;*.j2k;*.j2c;*.jng;*.jp2;*.jpg;*.jif;*.jpeg;*.jpe;*.jxr;*.wdp;*.hdp;*.koa;*.mng;*.pcd;*.pcx;*.pfm;*.pct;*.pict;*.pic;*.png;*.pbm;*.pgm;*.ppm;*.psd;*.ras;*.sgi;*.rgb;*.rgba;*.bw;*.tga;*.targa;*.tif;*.tiff;*.wap;*.wbmp;*.wbm;*.webp;*.xbm;*.xpm"}},
		{ OPT_CMP_IMG_SHOWDIFFERENCES, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IMG_OVERLAYMODE, varprop::VT_INT, {0, 1, 2, 3}, {}},
		{ OPT_CMP_IMG_OVERLAYALPHA, varprop::VT_INT, {0, 3000, 7000, 10000}, {}},
		{ OPT_CMP_IMG_DRAGGING_MODE, varprop::VT_INT, {0, 1, 2, 3, 4, 5}, {}},
		{ OPT_CMP_IMG_ZOOM, varprop::VT_INT, {1, 50000, 100000, 200000, 800000}, {}},
		{ OPT_CMP_IMG_USEBACKCOLOR, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IMG_BACKCOLOR, varprop::VT_INT, {0, 0xffffff}, {}},
		{ OPT_CMP_IMG_DIFFBLOCKSIZE, varprop::VT_INT, {1, 2, 8, 32}, {}},
		{ OPT_CMP_IMG_DIFFCOLORALPHA, varprop::VT_INT, {0, 3000, 7000, 10000}, {}},
		{ OPT_CMP_IMG_THRESHOLD, varprop::VT_INT, {0, 50000, 100000, 442000}, {}},
		{ OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE, varprop::VT_INT, {0, 1, 2}, {}},
		{ OPT_CMP_IMG_VECTOR_IMAGE_ZOOM_RATIO, varprop::VT_INT, {1, 50000, 100000, 200000, 800000}, {}},
		{ OPT_CMP_ENABLE_IMGCMP_IN_DIRCMP, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_IMG_OCR_RESULT_TYPE, varprop::VT_INT, {0, 1, 2}, {}},
		// Binary options
		{ OPT_CMP_BIN_FILEPATTERNS, varprop::VT_STRING, {}, {L"", L"*.bin;*.frx"}},
		// Table options
		{ OPT_CMP_CSV_FILEPATTERNS, varprop::VT_STRING, {}, {L"", L"*.csv"}},
		{ OPT_CMP_CSV_DELIM_CHAR, varprop::VT_STRING, {}, {L"", L","}},
		{ OPT_CMP_TSV_FILEPATTERNS, varprop::VT_STRING, {}, {L"", L"*.tsv"}},
		{ OPT_CMP_DSV_FILEPATTERNS, varprop::VT_STRING, {}, {L"", L"*.ssv"}},
		{ OPT_CMP_DSV_DELIM_CHAR, varprop::VT_STRING, {}, {L"", L";"}},
		{ OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES, varprop::VT_BOOL, {}, {}},
		{ OPT_CMP_TBL_QUOTE_CHAR, varprop::VT_STRING, {}, {L"", L"\""}},
		// Are regular expression linefilters enabled?
		{ OPT_LINEFILTER_ENABLED, varprop::VT_BOOL, {}, {}},
		// Currently selected filefilter
		{ OPT_FILEFILTER_CURRENT, varprop::VT_STRING, {}, {L"", L"*.*", L"Visual C++ loose"}},
		{ OPT_FILTER_USERPATH, varprop::VT_STRING, {}, {L"", L"c:\\tmp"}},
		{ OPT_FILEFILTER_SHARED, varprop::VT_BOOL, {}, {}},
		// Substitution Filters
		{ OPT_SUBSTITUTION_FILTERS_ENABLED, varprop::VT_BOOL, {}, {}},
		// Archive support
		{ OPT_ARCHIVE_ENABLE, varprop::VT_BOOL, {}, {}},
		{ OPT_ARCHIVE_PROBETYPE, varprop::VT_BOOL, {}, {}},
		{ OPT_ARCHIVE_FILTER_INDEX, varprop::VT_INT, {0, 1, 2, 12}, {}},
		// Patch Creator
		{ OPT_PATCHCREATOR_PATCH_STYLE, varprop::VT_BOOL, {}, {}},
		{ OPT_PATCHCREATOR_CONTEXT_LINES, varprop::VT_INT, {0, 1, 2, 3, 5, 7}, {}},
		{ OPT_PATCHCREATOR_OPEN_TO_EDITOR, varprop::VT_BOOL, {}, {}},
		{ OPT_PATCHCREATOR_INCLUDE_CMD_LINE, varprop::VT_BOOL, {}, {}},
		{ OPT_PATCHCREATOR_COPY_TO_CLIPBOARD, varprop::VT_BOOL, {}, {}},
		// Plugins
		{ OPT_PLUGINS_ENABLED, varprop::VT_BOOL, {}, {}},
		{ OPT_PLUGINS_CUSTOM_SETTINGS_LIST, varprop::VT_STRING, {}, {L"EDITOR_SCRIPT.PrettifyHTML=arguments:-indent --tab-size 1 --indent-spaces 1 -wrap 0 --sort-attributes alpha \tFILE_PACK_UNPACK.PrettifyHTML=arguments:-indent --indent-attributes yes --tab-size 2 --indent-spaces 2 -wrap 0 --sort-attributes alpha 	FILE_PACK_UNPACK.PrettifyXML=arguments:-xml -indent --indent-attributes yes --tab-size 2 --indent-spaces 2 -wrap 0 --sort-attributes alpha|automatic:true	FILE_PACK_UNPACK.PrettifyJSON=automatic:true"}},
		{ OPT_PLUGINS_UNPACKER_MODE, varprop::VT_BOOL, {}, {}},
		{ OPT_PLUGINS_PREDIFFER_MODE, varprop::VT_BOOL, {}, {}},
		{ OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION, varprop::VT_BOOL, {}, {}},
		{ OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE, varprop::VT_BOOL, {}, {}},
		// Startup options
		{ OPT_SHOW_SELECT_FILES_AT_STARTUP, varprop::VT_BOOL, {}, {}},
		// MDI Tab Bar
		{ OPT_TABBAR_AUTO_MAXWIDTH, varprop::VT_BOOL, {}, {}},
		// MRU
		{ OPT_MRU_MAX, varprop::VT_INT, {0, 1, 9}, {}},
		// Font options
		{ OPT_FONT_FILECMP + OPT_FONT_USECUSTOM, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_POINTSIZE, varprop::VT_INT, {0, 12, 72}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_HEIGHT, varprop::VT_INT, {0, -23, -95}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_ESCAPEMENT, varprop::VT_INT, {0, 900}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_ORIENTATION, varprop::VT_INT, {0, 900}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_WEIGHT, varprop::VT_INT, {0, 400, 700, 1000}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_ITALIC, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_UNDERLINE, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_STRIKEOUT, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_CHARSET, varprop::VT_INT, {ANSI_CHARSET, DEFAULT_CHARSET, SYMBOL_CHARSET, SHIFTJIS_CHARSET, HANGUL_CHARSET, GB2312_CHARSET, CHINESEBIG5_CHARSET, JOHAB_CHARSET, HEBREW_CHARSET, ARABIC_CHARSET, GREEK_CHARSET, TURKISH_CHARSET, VIETNAMESE_CHARSET, THAI_CHARSET, EASTEUROPE_CHARSET, RUSSIAN_CHARSET, MAC_CHARSET, BALTIC_CHARSET, OEM_CHARSET}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_OUTPRECISION, varprop::VT_INT, {OUT_DEFAULT_PRECIS, OUT_STRING_PRECIS, OUT_CHARACTER_PRECIS, OUT_STROKE_PRECIS, OUT_TT_PRECIS, OUT_DEVICE_PRECIS, OUT_RASTER_PRECIS, OUT_TT_ONLY_PRECIS, OUT_OUTLINE_PRECIS, OUT_SCREEN_OUTLINE_PRECIS, OUT_PS_ONLY_PRECIS}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_CLIPPRECISION, varprop::VT_INT, {CLIP_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, CLIP_STROKE_PRECIS, CLIP_LH_ANGLES, CLIP_TT_ALWAYS, CLIP_DFA_DISABLE, CLIP_EMBEDDED}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_QUALITY, varprop::VT_INT, {DEFAULT_QUALITY, DRAFT_QUALITY, PROOF_QUALITY, NONANTIALIASED_QUALITY, ANTIALIASED_QUALITY, CLEARTYPE_QUALITY, CLEARTYPE_NATURAL_QUALITY}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_PITCHANDFAMILY, varprop::VT_INT, {DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH, }, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_PITCHANDFAMILY, varprop::VT_INT, 
			{
				DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH,
				DEFAULT_PITCH | FF_ROMAN, FIXED_PITCH | FF_ROMAN, VARIABLE_PITCH | FF_ROMAN,
				DEFAULT_PITCH | FF_SWISS, FIXED_PITCH | FF_SWISS, VARIABLE_PITCH | FF_SWISS,
				DEFAULT_PITCH | FF_MODERN, FIXED_PITCH | FF_MODERN, VARIABLE_PITCH | FF_MODERN,
				DEFAULT_PITCH | FF_SCRIPT, FIXED_PITCH | FF_SCRIPT, VARIABLE_PITCH | FF_SCRIPT,
				DEFAULT_PITCH | FF_DECORATIVE, FIXED_PITCH | FF_DECORATIVE, VARIABLE_PITCH | FF_DECORATIVE,
		}, {}},
		{ OPT_FONT_FILECMP + OPT_FONT_FACENAME, varprop::VT_STRING, {}, {L"Consolas", L"Courier", L"Courier New", L"FixedSys", L"Terminal"}},
		{ OPT_FONT_DIRCMP + OPT_FONT_USECUSTOM, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_POINTSIZE, varprop::VT_INT, {0, 12, 72}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_HEIGHT, varprop::VT_INT, {0, -23, -95}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_ESCAPEMENT, varprop::VT_INT, {0, 900}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_ORIENTATION, varprop::VT_INT, {0, 900}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_WEIGHT, varprop::VT_INT, {0, 400, 700, 1000}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_ITALIC, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_UNDERLINE, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_STRIKEOUT, varprop::VT_BOOL, {}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_CHARSET, varprop::VT_INT, {ANSI_CHARSET, DEFAULT_CHARSET, SYMBOL_CHARSET, SHIFTJIS_CHARSET, HANGUL_CHARSET, GB2312_CHARSET, CHINESEBIG5_CHARSET, JOHAB_CHARSET, HEBREW_CHARSET, ARABIC_CHARSET, GREEK_CHARSET, TURKISH_CHARSET, VIETNAMESE_CHARSET, THAI_CHARSET, EASTEUROPE_CHARSET, RUSSIAN_CHARSET, MAC_CHARSET, BALTIC_CHARSET, OEM_CHARSET}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_OUTPRECISION, varprop::VT_INT, {OUT_DEFAULT_PRECIS, OUT_STRING_PRECIS, OUT_CHARACTER_PRECIS, OUT_STROKE_PRECIS, OUT_TT_PRECIS, OUT_DEVICE_PRECIS, OUT_RASTER_PRECIS, OUT_TT_ONLY_PRECIS, OUT_OUTLINE_PRECIS, OUT_SCREEN_OUTLINE_PRECIS, OUT_PS_ONLY_PRECIS}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_CLIPPRECISION, varprop::VT_INT, {CLIP_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, CLIP_STROKE_PRECIS, CLIP_LH_ANGLES, CLIP_TT_ALWAYS, CLIP_DFA_DISABLE, CLIP_EMBEDDED}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_QUALITY, varprop::VT_INT, {DEFAULT_QUALITY, DRAFT_QUALITY, PROOF_QUALITY, NONANTIALIASED_QUALITY, ANTIALIASED_QUALITY, CLEARTYPE_QUALITY, CLEARTYPE_NATURAL_QUALITY}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_PITCHANDFAMILY, varprop::VT_INT, {DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH, }, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_PITCHANDFAMILY, varprop::VT_INT, 
			{
				DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH,
				DEFAULT_PITCH | FF_ROMAN, FIXED_PITCH | FF_ROMAN, VARIABLE_PITCH | FF_ROMAN,
				DEFAULT_PITCH | FF_SWISS, FIXED_PITCH | FF_SWISS, VARIABLE_PITCH | FF_SWISS,
				DEFAULT_PITCH | FF_MODERN, FIXED_PITCH | FF_MODERN, VARIABLE_PITCH | FF_MODERN,
				DEFAULT_PITCH | FF_SCRIPT, FIXED_PITCH | FF_SCRIPT, VARIABLE_PITCH | FF_SCRIPT,
				DEFAULT_PITCH | FF_DECORATIVE, FIXED_PITCH | FF_DECORATIVE, VARIABLE_PITCH | FF_DECORATIVE,
		}, {}},
		{ OPT_FONT_DIRCMP + OPT_FONT_FACENAME, varprop::VT_STRING, {}, {L"Consolas", L"Courier", L"Courier New", L"FixedSys", L"Terminal"}},
	};

	static std::map<std::wstring, std::variant<bool, int, std::wstring>> SaveOptions()
	{
		std::map<std::wstring, std::variant<bool, int, std::wstring>> savedOptions;
		for (auto& option : options)
		{
			switch (option.type)
			{
			case varprop::VT_BOOL:
			{
				std::optional<bool> bval = regReadBool(option.name);
				if (bval.has_value())
					savedOptions.insert_or_assign(option.name, *bval);
				break;
			}
			case varprop::VT_INT:
			{
				std::optional<int> ival = regReadInt(option.name);
				if (ival.has_value())
					savedOptions.insert_or_assign(option.name, *ival);
				break;
			}
			case varprop::VT_STRING:
			{
				std::optional<std::wstring> sval = regReadString(option.name);
				if (sval.has_value())
					savedOptions.insert_or_assign(option.name, *sval);
				break;
			}
			}
		}
		return savedOptions;
	}

	static void RestoreOptions(const std::map<std::wstring, std::variant<bool, int, std::wstring>>& savedOptions)
	{
		for (auto& option : options)
		{
			auto it = savedOptions.find(option.name);
			if (it != savedOptions.end())
			{
				const auto& [name, value] = *it;
				switch (value.index())
				{
				case 0: regWrite(name, std::get<0>(value)); break;
				case 1: regWrite(name, std::get<1>(value)); break;
				case 2: regWrite(name, std::get<2>(value)); break;
				}
			}
			else
				regDelete(option.name);
		}
	}

	TEST_P(ConfigTest, RandomTest)
	{
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir1\\").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir2\\").u8string();
		std::string file1 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string file2 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8-NOBOM\\DiffItem.h").u8string();
		std::string csvfile1 = (getModuleFolder() / "..\\..\\..\\Externals\\crystaledit\\test\\test.csv").u8string();
		std::string csvfile2 = (getModuleFolder() / "..\\..\\..\\Externals\\crystaledit\\test\\test2.csv").u8string();
		std::string binfile1 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string binfile2 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string imgfile1 = (getModuleFolder() / "..\\..\\..\\Docs\\Logos\\WinMerge_logo_24bit.png").u8string();
		std::string imgfile2 = (getModuleFolder() / "..\\..\\..\\Docs\\Logos\\WinMerge_logo_shadow_24bit.png").u8string();

		auto savedOptions = SaveOptions();

		for (int i = 0; i < 20; ++i)
		{
			const BOOL bValues[] = { TRUE, FALSE };
			for (auto& option : options)
			{
				size_t size;
				int idx;
				switch (option.type)
				{
				case varprop::VT_BOOL:
					idx = rand() % 2;
					regWrite(option.name, bValues[idx]);
					break;
				case varprop::VT_INT:
					size = option.iValues.size();
					idx = rand() % size;
					regWrite(option.name, option.iValues[idx]);
					break;
				case varprop::VT_STRING:
					size = option.sValues.size();
					idx = rand() % size;
					regWrite(option.name, option.sValues[idx]);
					break;
				}
			}

			// Text file compare
			m_hwndWinMerge = execWinMerge(("/t text " + file1 + " " + file2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Table file compare
			m_hwndWinMerge = execWinMerge(("/t table /table-delimiter , " + csvfile1 + " " + csvfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Binary file compare
			m_hwndWinMerge = execWinMerge(("/t binary " + binfile1 + " " + binfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Image file compare
			m_hwndWinMerge = execWinMerge(("/t image " + imgfile1 + " " + imgfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Folder compare
			m_hwndWinMerge = execWinMerge(("-r " + dir1 + " " + dir2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			Sleep(1000);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
		}

		RestoreOptions(savedOptions);
	}

	TEST_P(ConfigTest, FuzzingTest)
	{
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir1\\").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir2\\").u8string();
		std::string file1 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string file2 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8-NOBOM\\DiffItem.h").u8string();
		std::string csvfile1 = (getModuleFolder() / "..\\..\\..\\Externals\\crystaledit\\test\\test.csv").u8string();
		std::string csvfile2 = (getModuleFolder() / "..\\..\\..\\Externals\\crystaledit\\test\\test2.csv").u8string();
		std::string binfile1 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string binfile2 = (getModuleFolder() / "..\\..\\Data\\Unicode\\UTF-8\\DiffItem.h").u8string();
		std::string imgfile1 = (getModuleFolder() / "..\\..\\..\\Docs\\Logos\\WinMerge_logo_24bit.png").u8string();
		std::string imgfile2 = (getModuleFolder() / "..\\..\\..\\Docs\\Logos\\WinMerge_logo_shadow_24bit.png").u8string();

		auto savedOptions = SaveOptions();

		for (int i = 0; i < 20; ++i)
		{
			const BOOL bValues[] = { TRUE, FALSE };
			for (auto& option : options)
			{
				size_t size;
				int idx;
				int ival;
				std::wstring sval;
				switch (option.type)
				{
				case varprop::VT_BOOL:
					idx = rand() % 2;
					regWrite(option.name, bValues[idx]);
					break;
				case varprop::VT_INT:
					ival = rand() * ((rand() < RAND_MAX / 2) ? 1 : -1);
					regWrite(option.name, ival);
					break;
				case varprop::VT_STRING:
					size = option.sValues.size();
					idx = rand() % size;
					sval = option.sValues[idx];
					if (!sval.empty())
						sval[0] = static_cast<wchar_t>(rand());
					regWrite(option.name, sval);
					break;
				}
			}

			// Text file compare
			m_hwndWinMerge = execWinMerge(("/t text " + file1 + " " + file2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Table file compare
			m_hwndWinMerge = execWinMerge(("/t table /table-delimiter , " + csvfile1 + " " + csvfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Binary file compare
			m_hwndWinMerge = execWinMerge(("/t binary " + binfile1 + " " + binfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Image file compare
			m_hwndWinMerge = execWinMerge(("/t image " + imgfile1 + " " + imgfile2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
			// Folder compare
			m_hwndWinMerge = execWinMerge(("-r " + dir1 + " " + dir2).c_str());
			ASSERT_TRUE(IsWindow(m_hwndWinMerge));
			waitForInputIdleByHWND(m_hwndWinMerge);
			Sleep(1000);
			PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
			waitUntilProcessExit(m_hwndWinMerge);
		}

		RestoreOptions(savedOptions);
	}
}

INSTANTIATE_TEST_SUITE_P(ConfigTestInstance,
	ConfigTest,
	testing::ValuesIn(GUITestUtils::languages()));
