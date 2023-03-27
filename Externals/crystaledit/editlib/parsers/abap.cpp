///////////////////////////////////////////////////////////////////////////
//  File:    abap.cpp
//  Version: 1.0.0.0
//  Updated: 15-May-2022
//
//  Copyright:  YuanShoyan
//  E-mail:     yuanshouyan1823@hotmail.com
//
//  ABAP(SAP) syntax highlighing definition
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

//  ABAP Keyword(The Language version:754)
static const tchar_t* s_apszAbapKeywordList[] =
{
	_T("*-INPUT"),
	_T("?TO"),
	_T("ABAP-SOURCE"),
	_T("ABAP_SYSTEM_TIMEZONE"),
	_T("ABAP_USER_TIMEZONE"),
	_T("ABBREVIATED"),
	_T("ABBREVIATION"),
	_T("ABS"),
	_T("ABSTRACT"),
	_T("ACCEPT"),
	_T("ACCEPTING"),
	_T("ACCESS"),
	_T("ACCORDING"),
	_T("ACTION"),
	_T("ACTIVATION"),
	_T("ACTUAL"),
	_T("ADABAS"),
	_T("ADD"),
	_T("ADD-CORRESPONDING"),
	_T("ADJACENT"),
	_T("ADJUST_NUMBERS"),
	_T("ALIAS"),
	_T("ALIASES"),
	_T("ALIGN"),
	_T("ALL"),
	_T("ALLOCATE"),
	_T("ALLOWED"),
	_T("ALPHA"),
	_T("AMDP"),
	_T("ANALYSIS"),
	_T("ANALYZER"),
	_T("AND"),
	_T("ANY"),
	_T("APPEND"),
	_T("APPENDAGE"),
	_T("APPENDING"),
	_T("APPLICATION"),
	_T("ARCHIVE"),
	_T("AREA"),
	_T("ARITHMETIC"),
	_T("AS"),
	_T("AS400"),
	_T("ASCENDING"),
	_T("ASSERT"),
	_T("ASSIGN"),
	_T("ASSIGNED"),
	_T("ASSIGNING"),
	_T("ASSOCIATION"),
	_T("ASSOCIATIONS"),
	_T("ASYNCHRONOUS"),
	_T("AT"),
	_T("ATTRIBUTES"),
	_T("AUTHORITY"),
	_T("AUTHORITY-CHECK"),
	_T("AVG"),
	_T("AVG,"),
	_T("BACK"),
	_T("BACKGROUND"),
	_T("BACKUP"),
	_T("BACKWARD"),
	_T("BADI"),
	_T("BALANCE"),
	_T("BASE"),
	_T("BEFORE"),
	_T("BEGIN"),
	_T("BEHAVIOR"),
	_T("BETWEEN"),
	_T("BIG"),
	_T("BINARY"),
	_T("BINTOHEX"),
	_T("BIT"),
	_T("BIT-AND"),
	_T("BIT-NOT"),
	_T("BIT-OR"),
	_T("BIT-XOR"),
	_T("BLACK"),
	_T("BLANK"),
	_T("BLANKS"),
	_T("BLOB"),
	_T("BLOCK"),
	_T("BLOCKS"),
	_T("BLUE"),
	_T("BOUND"),
	_T("BOUNDARIES"),
	_T("BOUNDS"),
	_T("BOXED"),
	_T("BREAK-POINT"),
	_T("BREAKUP"),
	_T("BT"),
	_T("BUFFER"),
	_T("BY"),
	_T("BYPASSING"),
	_T("BYTE"),
	_T("BYTE-CA"),
	_T("BYTE-CN"),
	_T("BYTE-CO"),
	_T("BYTE-CS"),
	_T("BYTE-NA"),
	_T("BYTE-NS"),
	_T("BYTE-ORDER"),
	_T("CA"),
	_T("CALL"),
	_T("CALLING"),
	_T("CASE"),
	_T("CAST"),
	_T("CASTING"),
	_T("CATCH"),
	_T("CDS"),
	_T("CEIL"),
	_T("CENTER"),
	_T("CENTERED"),
	_T("CHANGE"),
	_T("CHANGING"),
	_T("CHANNELS"),
	_T("CHAR"),
	_T("CHAR-TO-HEX"),
	_T("CHARACTER"),
	_T("CHECK"),
	_T("CHECK_BEFORE_SAVE"),
	_T("CHECKBOX"),
	_T("CHILD"),
	_T("CI_"),
	_T("CIRCULAR"),
	_T("CL_DBI_UTILITIES"),
	_T("CLASS"),
	_T("CLASS-CODING"),
	_T("CLASS-DATA"),
	_T("CLASS-EVENTS"),
	_T("CLASS-METHODS"),
	_T("CLASS-POOL"),
	_T("CLEANUP"),
	_T("CLEAR"),
	_T("CLIENT"),
	_T("CLIENTS"),
	_T("CLNT"),
	_T("CLOB"),
	_T("CLOCK"),
	_T("CLOSE"),
	_T("CN"),
	_T("CO"),
	_T("COALESCE"),
	_T("CODE"),
	_T("CODING"),
	_T("COL_BACKGROUND"),
	_T("COL_GROUP"),
	_T("COL_HEADING"),
	_T("COL_KEY"),
	_T("COL_NEGATIVE"),
	_T("COL_NORMAL"),
	_T("COL_POSITIVE"),
	_T("COL_TOTAL"),
	_T("COLLECT"),
	_T("COLOR"),
	_T("COLUMN"),
	_T("COLUMNS"),
	_T("COMMENT"),
	_T("COMMENTS"),
	_T("COMMIT"),
	_T("COMMON"),
	_T("COMMUNICATION"),
	_T("COMPARING"),
	_T("COMPONENT"),
	_T("COMPONENTS"),
	_T("COMPRESSION"),
	_T("COMPUTE"),
	_T("CONCAT"),
	_T("CONCAT_WITH_SPACE"),
	_T("CONCATENATE"),
	_T("COND"),
	_T("CONDENSE"),
	_T("CONDITION"),
	_T("CONNECT"),
	_T("CONNECTION"),
	_T("CONSTANTS"),
	_T("CONTEXT"),
	_T("CONTEXTS"),
	_T("CONTINUE"),
	_T("CONTROL"),
	_T("CONTROLS"),
	_T("CONV"),
	_T("CONVERSION"),
	_T("CONVERT"),
	_T("COPIES"),
	_T("COPY"),
	_T("CORRESPONDING"),
	_T("COUNT"),
	_T("COUNTRY"),
	_T("COVER"),
	_T("CP"),
	_T("CPI"),
	_T("CREATE"),
	_T("CREATING"),
	_T("CRITICAL"),
	_T("CROSS"),
	_T("CS"),
	_T("CUKY"),
	_T("CURR"),
	_T("CURRENCY"),
	_T("CURRENCY_CONVERSION"),
	_T("CURRENT"),
	_T("CURSOR"),
	_T("CUSTOMER-FUNCTION"),
	_T("CYCLES"),
	_T("D16N"),
	_T("D34N"),
	_T("DANGEROUS"),
	_T("DATA"),
	_T("DATABASE"),
	_T("DATAINFO"),
	_T("DATASET"),
	_T("DATE"),
	_T("DATN"),
	_T("DATS"),
	_T("DATS_ADD_DAYS"),
	_T("DATS_ADD_MONTHS"),
	_T("DATS_DAYS_BETWEEN"),
	_T("DATS_IS_VALID"),
	_T("DATS_TIMS_TO_TSTMP"),
	_T("DAYLIGHT"),
	_T("DB2"),
	_T("DB6"),
	_T("DD/MM/YY"),
	_T("DD/MM/YYYY"),
	_T("DDL"),
	_T("DDMMYY"),
	_T("DEALLOCATE"),
	_T("DEC"),
	_T("DECIMAL_SHIFT"),
	_T("DECIMALS"),
	_T("DECLARATIONS"),
	_T("DEEP"),
	_T("DEFAULT"),
	_T("DEFERRED"),
	_T("DEFINE"),
	_T("DEFINING"),
	_T("DEFINITION"),
	_T("DELETE"),
	_T("DELETING"),
	_T("DEMAND"),
	_T("DENSE_RANK"),
	_T("DEPARTMENT"),
	_T("DEPENDENT"),
	_T("DEPTH"),
	_T("DESCENDING"),
	_T("DESCRIBE"),
	_T("DESTINATION"),
	_T("DETAIL"),
	_T("DETERMINISTIC"),
	_T("DIALOG"),
	_T("DIRECTORY"),
	_T("DISCARDING"),
	_T("DISCONNECT"),
	_T("DISPLAY"),
	_T("DISPLAY-MODE"),
	_T("DISTANCE"),
	_T("DISTINCT"),
	_T("DIV"),
	_T("DIVIDE"),
	_T("DIVIDE-CORRESPONDING"),
	_T("DIVISION"),
	_T("DO"),
	_T("DUMMY"),
	_T("DUPLICATE"),
	_T("DUPLICATES"),
	_T("DURATION"),
	_T("DURING"),
	_T("DYNAMIC"),
	_T("DYNPRO"),
	_T("EDIT"),
	_T("EDITOR-CALL"),
	_T("ELSE"),
	_T("ELSEIF"),
	_T("EMPTY"),
	_T("ENABLED"),
	_T("ENABLING"),
	_T("ENCODING"),
	_T("END"),
	_T("END-ENHANCEMENT-SECTION"),
	_T("END-LINES"),
	_T("END-OF-DEFINITION"),
	_T("END-OF-EDITING"),
	_T("END-OF-FILE"),
	_T("END-OF-PAGE"),
	_T("END-OF-SELECTION"),
	_T("END-TEST-INJECTION"),
	_T("END-TEST-SEAM"),
	_T("ENDAT"),
	_T("ENDCASE"),
	_T("ENDCATCH"),
	_T("ENDCLASS"),
	_T("ENDDO"),
	_T("ENDENHANCEMENT"),
	_T("ENDEXEC"),
	_T("ENDFORM"),
	_T("ENDFUNCTION"),
	_T("ENDIAN"),
	_T("ENDIF"),
	_T("ENDING"),
	_T("ENDINTERFACE"),
	_T("ENDLOOP"),
	_T("ENDMETHOD"),
	_T("ENDMODULE"),
	_T("ENDON"),
	_T("ENDPROVIDE"),
	_T("ENDSELECT"),
	_T("ENDTRY"),
	_T("ENDWHILE"),
	_T("ENDWITH"),
	_T("ENGINEERING"),
	_T("ENHANCEMENT"),
	_T("ENHANCEMENT-POINT"),
	_T("ENHANCEMENT-SECTION"),
	_T("ENHANCEMENTS"),
	_T("ENTITIES"),
	_T("ENTITY"),
	_T("ENTRIES"),
	_T("ENTRY"),
	_T("ENUM"),
	_T("ENVIRONMENT"),
	_T("EQ"),
	_T("EQUIV"),
	_T("ERROR"),
	_T("ERRORS"),
	_T("ESCAPE"),
	_T("ESCAPING"),
	_T("ETAG"),
	_T("EVENT"),
	_T("EVENTS"),
	_T("EXACT"),
	_T("EXCEPT"),
	_T("EXCEPTION"),
	_T("EXCEPTION-TABLE"),
	_T("EXCEPTIONS"),
	_T("EXCLUDE"),
	_T("EXCLUDING"),
	_T("EXEC"),
	_T("EXECUTE"),
	_T("EXISTS"),
	_T("EXIT"),
	_T("EXIT-COMMAND"),
	_T("EXPAND"),
	_T("EXPANDING"),
	_T("EXPIRATION"),
	_T("EXPLICIT"),
	_T("EXPONENT"),
	_T("EXPORT"),
	_T("EXPORTING"),
	_T("EXPOSE"),
	_T("EXTENDED"),
	_T("EXTENSION"),
	_T("EXTERNAL"),
	_T("EXTRACT"),
	_T("FAIL"),
	_T("FAILED"),
	_T("FETCH"),
	_T("FIELD"),
	_T("FIELD-GROUPS"),
	_T("FIELD-SYMBOL"),
	_T("FIELD-SYMBOLS"),
	_T("FIELDS"),
	_T("FILE"),
	_T("FILTER"),
	_T("FILTER-TABLE"),
	_T("FILTERS"),
	_T("FINAL"),
	_T("FINALIZE"),
	_T("FIND"),
	_T("FIRST"),
	_T("FIRST-LINE"),
	_T("FIXED-POINT"),
	_T("FKEQ"),
	_T("FKGE"),
	_T("FLOOR"),
	_T("FLTP"),
	_T("FLTP_TO_DEC"),
	_T("FLUSH"),
	_T("FONT"),
	_T("FOR"),
	_T("FORM"),
	_T("FORMAT"),
	_T("FORWARD"),
	_T("FOUND"),
	_T("FRACTIONAL"),
	_T("FRAME"),
	_T("FRAMES"),
	_T("FREE"),
	_T("FRIENDS"),
	_T("FROM"),
	_T("FULL"),
	_T("FUNCTION"),
	_T("FUNCTION-POOL"),
	_T("FUNCTIONALITY"),
	_T("FURTHER"),
	_T("GAPS"),
	_T("GE"),
	_T("GENERATE"),
	_T("GET"),
	_T("GIVING"),
	_T("GKEQ"),
	_T("GKGE"),
	_T("GLOBAL"),
	_T("GREEN"),
	_T("GROUP"),
	_T("GROUPING"),
	_T("GROUPS"),
	_T("GT"),
	_T("HANDLE"),
	_T("HANDLER"),
	_T("HARMLESS"),
	_T("HASHED"),
	_T("HAVING"),
	_T("HDB"),
	_T("HEAD-LINES"),
	_T("HEADER"),
	_T("HEADERS"),
	_T("HEADING"),
	_T("HELP-ID"),
	_T("HELP-REQUEST"),
	_T("HEXTOBIN"),
	_T("HIDE"),
	_T("HIERARCHY"),
	_T("HIERARCHY_ANCESTORS"),
	_T("HIERARCHY_ANCESTORS_AGGREGATE"),
	_T("HIERARCHY_DESCENDANTS"),
	_T("HIERARCHY_DESCENDANTS_AGGREGATE"),
	_T("HIERARCHY_SIBLINGS"),
	_T("HIGH"),
	_T("HINT"),
	_T("HOLD"),
	_T("HOTSPOT"),
	_T("ICON"),
	_T("ID"),
	_T("IDENTIFICATION"),
	_T("IDENTIFIER"),
	_T("IDS"),
	_T("IF"),
	_T("IGNORE"),
	_T("IGNORING"),
	_T("IMMEDIATELY"),
	_T("IMPLEMENTATION"),
	_T("IMPLEMENTATIONS"),
	_T("IMPLEMENTED"),
	_T("IMPLICIT"),
	_T("IMPORT"),
	_T("IMPORTING"),
	_T("IN"),
	_T("INACTIVE"),
	_T("INCL"),
	_T("INCLUDE"),
	_T("INCLUDES"),
	_T("INCLUDING"),
	_T("INCREMENT"),
	_T("INDEX"),
	_T("INDEX-LINE"),
	_T("INDICATORS"),
	_T("INFOTYPES"),
	_T("INHERITING"),
	_T("INIT"),
	_T("INITIAL"),
	_T("INITIALIZATION"),
	_T("INNER"),
	_T("INOUT"),
	_T("INPUT"),
	_T("INSERT"),
	_T("INSTANCE"),
	_T("INSTANCES"),
	_T("INSTR"),
	_T("INT1"),
	_T("INT2"),
	_T("INT4"),
	_T("INT8"),
	_T("INTENSIFIED"),
	_T("INTERFACE"),
	_T("INTERFACE-POOL"),
	_T("INTERFACES"),
	_T("INTERNAL"),
	_T("INTERVALS"),
	_T("INTO"),
	_T("INVERSE"),
	_T("INVERTED-DATE"),
	_T("IS"),
	_T("ISO"),
	_T("ITNO"),
	_T("JOB"),
	_T("JOIN"),
	_T("KEEP"),
	_T("KEEPING"),
	_T("KERNEL"),
	_T("KEY"),
	_T("KEYS"),
	_T("KEYWORDS"),
	_T("KIND"),
	_T("LAG"),
	_T("LANG"),
	_T("LANGUAGE"),
	_T("LAST"),
	_T("LATE"),
	_T("LAYOUT"),
	_T("LE"),
	_T("LEAD"),
	_T("LEADING"),
	_T("LEAVE"),
	_T("LEAVES"),
	_T("LEFT"),
	_T("LEFT-JUSTIFIED"),
	_T("LEFTPLUS"),
	_T("LEFTSPACE"),
	_T("LEGACY"),
	_T("LENGTH"),
	_T("LET"),
	_T("LEVEL"),
	_T("LEVELS"),
	_T("LIKE"),
	_T("LINE"),
	_T("LINE-COUNT"),
	_T("LINE-SELECTION"),
	_T("LINE-SIZE"),
	_T("LINEFEED"),
	_T("LINES"),
	_T("LINK"),
	_T("LIST"),
	_T("LIST-PROCESSING"),
	_T("LISTBOX"),
	_T("LITTLE"),
	_T("LLANG"),
	_T("LOAD"),
	_T("LOAD-OF-PROGRAM"),
	_T("LOB"),
	_T("LOCAL"),
	_T("LOCALE"),
	_T("LOCATOR"),
	_T("LOCK"),
	_T("LOCKS"),
	_T("LOG-POINT"),
	_T("LOGFILE"),
	_T("LOGICAL"),
	_T("LONG"),
	_T("LOOP"),
	_T("LOW"),
	_T("LOWER"),
	_T("LPAD"),
	_T("LPI"),
	_T("LT"),
	_T("LTRIM"),
	_T("MAIL"),
	_T("MAIN"),
	_T("MAJOR-ID"),
	_T("MANAGED"),
	_T("MANDATORY"),
	_T("MANY"),
	_T("MAPPED"),
	_T("MAPPING"),
	_T("MARGIN"),
	_T("MARK"),
	_T("MASK"),
	_T("MASTER"),
	_T("MATCH"),
	_T("MATCHCODE"),
	_T("MATCHED"),
	_T("MAX"),
	_T("MAXIMUM"),
	_T("MEASURES"),
	_T("MEDIUM"),
	_T("MEMBERS"),
	_T("MEMORY"),
	_T("MESH"),
	_T("MESSAGE"),
	_T("MESSAGE-ID"),
	_T("MESSAGES"),
	_T("MESSAGING"),
	_T("METHOD"),
	_T("METHODS"),
	_T("MIN"),
	_T("MINIMUM"),
	_T("MINOR-ID"),
	_T("MM/DD/YY"),
	_T("MM/DD/YYYY"),
	_T("MMDDYY"),
	_T("MOD"),
	_T("MODE"),
	_T("MODIF"),
	_T("MODIFIER"),
	_T("MODIFY"),
	_T("MODULE"),
	_T("MOVE"),
	_T("MOVE-CORRESPONDING"),
	_T("MSSQLNT"),
	_T("MULTIPLE"),
	_T("MULTIPLY"),
	_T("MULTIPLY-CORRESPONDING"),
	_T("NA"),
	_T("NAME"),
	_T("NAMETAB"),
	_T("NATIVE"),
	_T("NB"),
	_T("NE"),
	_T("NESTED"),
	_T("NESTING"),
	_T("NEW"),
	_T("NEW-LINE"),
	_T("NEW-PAGE"),
	_T("NEW-SECTION"),
	_T("NEXT"),
	_T("NO"),
	_T("NO-DISPLAY"),
	_T("NO-EXTENSION"),
	_T("NO-GAP"),
	_T("NO-GAPS"),
	_T("NO-GROUPING"),
	_T("NO-HEADING"),
	_T("NO-SCROLLING"),
	_T("NO-SIGN"),
	_T("NO-TITLE"),
	_T("NO-TOPOFPAGE"),
	_T("NO-ZERO"),
	_T("NODE"),
	_T("NODES"),
	_T("NON-UNICODE"),
	_T("NON-UNIQUE"),
	_T("NOT"),
	_T("NP"),
	_T("NS"),
	_T("NULL"),
	_T("NUMBER"),
	_T("NUMBERING"),
	_T("NUMC"),
	_T("OBJECT"),
	_T("OBJECTS"),
	_T("OBLIGATORY"),
	_T("OCCURRENCE"),
	_T("OCCURRENCES"),
	_T("OCCURS"),
	_T("OF"),
	_T("OFF"),
	_T("OFFSET"),
	_T("OLE"),
	_T("ON"),
	_T("ONE"),
	_T("ONLY"),
	_T("OPEN"),
	_T("OPERATIONS"),
	_T("OPTION"),
	_T("OPTIONAL"),
	_T("OPTIONS"),
	_T("OR"),
	_T("ORACLE"),
	_T("ORDER"),
	_T("ORPHANS"),
	_T("OTHER"),
	_T("OTHERS"),
	_T("OUT"),
	_T("OUTER"),
	_T("OUTPUT"),
	_T("OUTPUT-LENGTH"),
	_T("OVER"),
	_T("OVERFLOW"),
	_T("OVERLAY"),
	_T("PACK"),
	_T("PACKAGE"),
	_T("PAD"),
	_T("PADDING"),
	_T("PAGE"),
	_T("PAGES"),
	_T("PARAMETER"),
	_T("PARAMETER-TABLE"),
	_T("PARAMETERS"),
	_T("PARENT"),
	_T("PARENTS"),
	_T("PART"),
	_T("PARTIALLY"),
	_T("PARTITION"),
	_T("PATTERN"),
	_T("PERCENTAGE"),
	_T("PERFORM"),
	_T("PERFORMING"),
	_T("PERIOD"),
	_T("PERSON"),
	_T("PF"),
	_T("PF-STATUS"),
	_T("PINK"),
	_T("PLACE"),
	_T("PLACES"),
	_T("POOL"),
	_T("POS_HIGH"),
	_T("POS_LOW"),
	_T("POSITION"),
	_T("PRAGMAS"),
	_T("PRECOMPILED"),
	_T("PREFERRED"),
	_T("PRESERVING"),
	_T("PRIMARY"),
	_T("PRINT"),
	_T("PRINT-CONTROL"),
	_T("PRIORITY"),
	_T("PRIVATE"),
	_T("PRIVILEGED"),
	_T("PROCEDURE"),
	_T("PRODUCT"),
	_T("PROGRAM"),
	_T("PROPERTY"),
	_T("PROTECTED"),
	_T("PROVIDE"),
	_T("PUBLIC"),
	_T("PUSH"),
	_T("PUSHBUTTON"),
	_T("PUT"),
	_T("QUAN"),
	_T("QUEUE-ONLY"),
	_T("QUICKINFO"),
	_T("RADIOBUTTON"),
	_T("RAISE"),
	_T("RAISING"),
	_T("RANGE"),
	_T("RANGES"),
	_T("RANK"),
	_T("RAW"),
	_T("READ"),
	_T("READ-ONLY"),
	_T("READER"),
	_T("RECEIVE"),
	_T("RECEIVED"),
	_T("RECEIVER"),
	_T("RECEIVING"),
	_T("RED"),
	_T("REDEFINITION"),
	_T("REDIRECTED"),
	_T("REDUCE"),
	_T("REDUCED"),
	_T("REF"),
	_T("REFERENCE"),
	_T("REFRESH"),
	_T("REGEX"),
	_T("REJECT"),
	_T("REMOTE"),
	_T("RENAMING"),
	_T("REPLACE"),
	_T("REPLACEMENT"),
	_T("REPLACING"),
	_T("REPORT"),
	_T("REPORTED"),
	_T("REQUESTED"),
	_T("RESERVE"),
	_T("RESET"),
	_T("RESOLUTION"),
	_T("RESPECTING"),
	_T("RESPONSE"),
	_T("RESPONSES"),
	_T("RESPONSIBLE"),
	_T("RESULT"),
	_T("RESULTS"),
	_T("RESUMABLE"),
	_T("RESUME"),
	_T("RETRY"),
	_T("RETURN"),
	_T("RETURNCODE"),
	_T("RETURNING"),
	_T("RIGHT"),
	_T("RIGHT-JUSTIFIED"),
	_T("RIGHTPLUS"),
	_T("RIGHTSPACE"),
	_T("RISK"),
	_T("RMC_COMMUNICATION_FAILURE"),
	_T("RMC_INVALID_STATUS"),
	_T("RMC_SYSTEM_FAILURE"),
	_T("ROLLBACK"),
	_T("ROOT"),
	_T("ROUND"),
	_T("ROW_NUMBER"),
	_T("ROWS"),
	_T("RPAD"),
	_T("RTRIM"),
	_T("RUN"),
	_T("SAP"),
	_T("SAP-SPOOL"),
	_T("SAVE"),
	_T("SAVING"),
	_T("SCALE_PRESERVING"),
	_T("SCALE_PRESERVING_SCIENTIFIC"),
	_T("SCAN"),
	_T("SCHEMA"),
	_T("SCIENTIFIC"),
	_T("SCIENTIFIC_WITH_LEADING_ZERO"),
	_T("SCREEN"),
	_T("SCROLL"),
	_T("SCROLL-BOUNDARY"),
	_T("SCROLLING"),
	_T("SEARCH"),
	_T("SECONDARY"),
	_T("SECONDS"),
	_T("SECTION"),
	_T("SELECT"),
	_T("SELECT-OPTIONS"),
	_T("SELECTION"),
	_T("SELECTION-SCREEN"),
	_T("SELECTION-SET"),
	_T("SELECTION-SETS"),
	_T("SELECTION-TABLE"),
	_T("SELECTIONS"),
	_T("SEND"),
	_T("SEPARATE"),
	_T("SEPARATED"),
	_T("SERVICE"),
	_T("SESSION"),
	_T("SET"),
	_T("SETS"),
	_T("SHARED"),
	_T("SHIFT"),
	_T("SHORT"),
	_T("SHORTDUMP"),
	_T("SHORTDUMP-ID"),
	_T("SIBLINGS"),
	_T("SIGN"),
	_T("SIGN_AS_POSTFIX"),
	_T("SIMPLE"),
	_T("SINGLE"),
	_T("SIZE"),
	_T("SKIP"),
	_T("SKIPPING"),
	_T("SMART"),
	_T("SOME"),
	_T("SORT"),
	_T("SORTABLE"),
	_T("SORTED"),
	_T("SOURCE"),
	_T("SPACE"),
	_T("SPANTREE"),
	_T("SPECIFIED"),
	_T("SPLIT"),
	_T("SPOOL"),
	_T("SPOTS"),
	_T("SQL"),
	_T("SQLSCRIPT"),
	_T("SSTRING"),
	_T("STABLE"),
	_T("STAMP"),
	_T("STANDARD"),
	_T("START"),
	_T("START-OF-EDITING"),
	_T("START-OF-SELECTION"),
	_T("STARTING"),
	_T("STATE"),
	_T("STATEMENT"),
	_T("STATEMENTS"),
	_T("STATIC"),
	_T("STATICS"),
	_T("STATUSINFO"),
	_T("STEP-LOOP"),
	_T("STOP"),
	_T("STRING_AGG"),
	_T("STRUCTURE"),
	_T("STRUCTURES"),
	_T("STYLE"),
	_T("SUBKEY"),
	_T("SUBMATCHES"),
	_T("SUBMIT"),
	_T("SUBROUTINE"),
	_T("SUBSCREEN"),
	_T("SUBSTRING"),
	_T("SUBTOTAL"),
	_T("SUBTRACT"),
	_T("SUBTRACT-CORRESPONDING"),
	_T("SUFFIX"),
	_T("SUM"),
	_T("SUMMARY"),
	_T("SUMMING"),
	_T("SUPPLIED"),
	_T("SUPPLY"),
	_T("SUPPRESS"),
	_T("SWITCH"),
	_T("SWITCHSTATES"),
	_T("SYBASE"),
	_T("SYMBOL"),
	_T("SYNCPOINTS"),
	_T("SYNTAX"),
	_T("SYNTAX-CHECK"),
	_T("SYNTAX-TRACE"),
	_T("SYSTEM-CALL"),
	_T("SYSTEM-EXCEPTIONS"),
	_T("SYSTEM-EXIT"),
	_T("T000"),
	_T("TAB"),
	_T("TABBED"),
	_T("TABLE"),
	_T("TABLES"),
	_T("TABLEVIEW"),
	_T("TABSTRIP"),
	_T("TARGET"),
	_T("TASK"),
	_T("TASKS"),
	_T("TEST"),
	_T("TEST-INJECTION"),
	_T("TEST-SEAM"),
	_T("TESTING"),
	_T("TEXT"),
	_T("TEXTPOOL"),
	_T("THEN"),
	_T("THROW"),
	_T("TIME"),
	_T("TIMES"),
	_T("TIMESTAMP"),
	_T("TIMEZONE"),
	_T("TIMN"),
	_T("TIMS"),
	_T("TIMS_IS_VALID"),
	_T("TITLE"),
	_T("TITLE-LINES"),
	_T("TITLEBAR"),
	_T("TO"),
	_T("TOKENIZATION"),
	_T("TOKENS"),
	_T("TOP-LINES"),
	_T("TOP-OF-PAGE"),
	_T("TOTAL"),
	_T("TRACE-ENTRY"),
	_T("TRACE-FILE"),
	_T("TRACE-TABLE"),
	_T("TRAILING"),
	_T("TRANSACTION"),
	_T("TRANSFER"),
	_T("TRANSFORMATION"),
	_T("TRANSLATE"),
	_T("TRANSPORTING"),
	_T("TRMAC"),
	_T("TRUNCATE"),
	_T("TRUNCATION"),
	_T("TRY"),
	_T("TSTMP_ADD_SECONDS"),
	_T("TSTMP_CURRENT_UTCTIMESTAMP"),
	_T("TSTMP_IS_VALID"),
	_T("TSTMP_SECONDS_BETWEEN"),
	_T("TSTMP_TO_DATS"),
	_T("TSTMP_TO_DST"),
	_T("TSTMP_TO_TIMS"),
	_T("TYPE"),
	_T("TYPE-POOL"),
	_T("TYPE-POOLS"),
	_T("TYPES"),
	_T("ULINE"),
	_T("UNASSIGN"),
	_T("UNDER"),
	_T("UNICODE"),
	_T("UNION"),
	_T("UNIQUE"),
	_T("UNIT"),
	_T("UNIT_CONVERSION"),
	_T("UNIX"),
	_T("UNMANAGED"),
	_T("UNPACK"),
	_T("UNTIL"),
	_T("UNWIND"),
	_T("UP"),
	_T("UPDATE"),
	_T("UPPER"),
	_T("USER"),
	_T("USER-COMMAND"),
	_T("USING"),
	_T("UTCL"),
	_T("UTCLONG"),
	_T("UTF-8"),
	_T("UUID"),
	_T("VALID"),
	_T("VALUE"),
	_T("VALUE-REQUEST"),
	_T("VALUES"),
	_T("VARY"),
	_T("VARYING"),
	_T("VERIFICATION-MESSAGE"),
	_T("VERSION"),
	_T("VIA"),
	_T("VISIBLE"),
	_T("WAIT"),
	_T("WHEN"),
	_T("WHERE"),
	_T("WHILE"),
	_T("WIDTH"),
	_T("WINDOW"),
	_T("WINDOWS"),
	_T("WITH"),
	_T("WITH-HEADING"),
	_T("WITH-TITLE"),
	_T("WITHOUT"),
	_T("WORD"),
	_T("WORK"),
	_T("WRITE"),
	_T("WRITER"),
	_T("XML"),
	_T("XSD"),
	_T("YELLOW"),
	_T("YES"),
	_T("YYMMDD"),
	_T("ZERO"),
	_T("ZONE")
};

static bool
IsAbapKeyword(const tchar_t* pszChars, int nLength)
{
	return ISXKEYWORDI(s_apszAbapKeywordList, pszChars, nLength);
}

static const tchar_t* specialAbapSymbol = _T("+*/=()|@:<>&#{}~");

static bool
IsAbapSingleOperator(tchar_t symbol)
{
	return tc::tcschr(specialAbapSymbol, symbol) != nullptr;
}

static const tchar_t* whiteSpaces = _T(" \f\n\r\t\v");
static bool
IsAsciiWhiteSpace(tchar_t ch)
{
   return tc::tcschr(whiteSpaces, ch) != nullptr;
}

static bool IsNumber(const tchar_t* pszChars, int nLength)
{
	bool result = true;

	for (int i = 0; i < nLength; i++)
		result = result && tc::istdigit(pszChars[i]);

	return result;
}

unsigned
CrystalLineParser::ParseLineAbap(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems)
{
	if (nLength == 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	bool bRedefineBlock = true;
	bool bDecIndex = false;
	int nIdentBegin = -1;
	int nPrevI = -1;
	int I = 0;
	for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars + I) - pszChars))
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
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else
			{
				if (xisalnum(pszChars[nPos]) && nPos > 0 && (!xisalpha(*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha(*tc::tcharnext(pszChars + nPos))))
				{
					DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
				}
				else if (IsAbapSingleOperator(pszChars[nPos]))
				{
					DEFINE_BLOCK(nPos, COLORINDEX_USER2);
					bRedefineBlock = true;
					bDecIndex = true;
					goto out;
				}
				else
				{
					DEFINE_BLOCK(nPos, COLORINDEX_OPERATOR);
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
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//  String constant "...."
		if (dwCookie & COOKIE_STRING)
		{
			if (pszChars[I] == '\'' && !(dwCookie & COOKIE_SECTION) && (I == 0 || I == 1 && pszChars[nPrevI] != '*' || I >= 2 && (pszChars[nPrevI] != '*' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '*')))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = true;
			}
			else if (pszChars[I] == '\'' && !(dwCookie & COOKIE_SECTION) && (I == 0 || I == 1 && pszChars[nPrevI] != '"' || I >= 2 && (pszChars[nPrevI] != '"' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '"')))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = true;
			}

			if (pszChars[I] == '{')
			{
				DEFINE_BLOCK(nPrevI, COLORINDEX_STRING);
				DEFINE_BLOCK(I, COLORINDEX_OPERATOR);
				dwCookie &= ~COOKIE_STRING;
				dwCookie |= COOKIE_VARIABLE;
				bRedefineBlock = true;
			}

			continue;
		}

		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (pszChars[0] == '*' || pszChars[nPrevI] == '"')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
			}
			continue;
		}


		if (pszChars[0] == '*' || pszChars[nPrevI] == '"' || (pszChars[I] == '#' && pszChars[nPrevI] == '#'))
		{
			if (pszChars[0] == '*')
			{
				DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			}
			else
			{
				DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
			}

			dwCookie |= COOKIE_COMMENT;
			break;
		}

		if (pszChars[I] == '\'' || pszChars[I] == '|')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;

			if (pszChars[I] == '|' && dwCookie & COOKIE_SECTION)
				dwCookie &= ~COOKIE_SECTION;
			else if(pszChars[I] == '|' && ! (dwCookie & COOKIE_SECTION))
				dwCookie |= COOKIE_SECTION;

			continue;
		}

		if (dwCookie & COOKIE_VARIABLE && pszChars[nPrevI] == '}')
		{
			DEFINE_BLOCK(nPrevI, COLORINDEX_OPERATOR);
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			dwCookie &= ~COOKIE_VARIABLE;
			bRedefineBlock = true;
		  	continue; 
		}

		if (pBuf == nullptr)
			continue;               //  We don't need to extract keywords,
		  //  for faster parsing skip the rest of loop

		if (xisalnum(pszChars[I]) || pszChars[I] == '-' && I > 0 && ( !IsAsciiWhiteSpace(pszChars[nPrevI]) && !IsAsciiWhiteSpace(pszChars[I + 1])))
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsAbapKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else if (IsNumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}

				bRedefineBlock = true;
				bDecIndex = true;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsAbapKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else if (IsNumber(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
		dwCookie &= COOKIE_EXT_COMMENT;
	return dwCookie;
}





