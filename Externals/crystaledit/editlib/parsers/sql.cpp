///////////////////////////////////////////////////////////////////////////
//  File:    sql.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  SQL syntax highlighing definition
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

//  SQL keywords
static const tchar_t * s_apszSqlKeywordList[] =
  {
    _T("ABS"),
    _T("ABSENT"),
    _T("ACOS"),
    _T("ALL"),
    _T("ALLOCATE"),
    _T("ALTER"),
    _T("AND"),
    _T("ANY"),
    _T("ARE"),
    _T("ARRAY"),
    _T("ARRAY_AGG"),
    _T("ARRAY_MAX_CARDINALITY"),
    _T("AS"),
    _T("ASC"),
    _T("ASENSITIVE"),
    _T("ASIN"),
    _T("ASSERTION"),
    _T("ASYMMETRIC"),
    _T("AT"),
    _T("ATAN"),
    _T("ATOMIC"),
    _T("AUTHORIZATION"),
    _T("AVG"),

    _T("BEGIN"),
    _T("BEGIN_FRAME"),
    _T("BEGIN_PARTITION"),
    _T("BETWEEN"),
    _T("BOTH"),
    _T("BY"),

    _T("CALL"),
    _T("CALLED"),
    _T("CARDINALITY"),
    _T("CASCADED"),
    _T("CASE"),
    _T("CAST"),
    _T("CATALOG"),
    _T("CEIL"),
    _T("CEILING"),
    _T("CHAR_LENGTH"),
    _T("CHARACTER"),
    _T("CHARACTER_LENGTH"),
    _T("CHECK"),
    _T("CLASSIFIER"),
    _T("CLOSE"),
    _T("COALESCE"),
    _T("COLLATE"),
    _T("COLLECT"),
    _T("COLUMN"),
    _T("COMMIT"),
    _T("CONDITION"),
    _T("CONNECT"),
    _T("CONNECTION"),
    _T("CONSTRAINT"),
    _T("CONTAINS"),
    _T("CONVERT"),
    _T("COPY"),
    _T("CORR"),
    _T("CORRESPONDING"),
    _T("COS"),
    _T("COSH"),
    _T("COUNT"),
    _T("COVAR_POP"),
    _T("COVAR_SAMP"),
    _T("CREATE"),
    _T("CROSS"),
    _T("CUBE"),
    _T("CUME_DIST"),
    _T("CURRENT"),
    _T("CURRENT_CATALOG"),
    _T("CURRENT_DATE"),
    _T("CURRENT_DEFAULT_TRANSFORM_GROUP"),
    _T("CURRENT_PATH"),
    _T("CURRENT_ROLE"),
    _T("CURRENT_ROW"),
    _T("CURRENT_SCHEMA"),
    _T("CURRENT_TIME"),
    _T("CURRENT_TIMESTAMP"),
    _T("CURRENT_TRANSFORM_GROUP_FOR_TYPE"),
    _T("CURRENT_USER"),
    _T("CURSOR"),
    _T("CYCLE"),

    _T("DAY"),
    _T("DEALLOCATE"),
    _T("DECFLOAT"),
    _T("DECLARE"),
    _T("DEFAULT"),
    _T("DEFINE"),
    _T("DELETE"),
    _T("DENSE_RANK"),
    _T("DEPTH"),
    _T("DEREF"),
    _T("DESC"),
    _T("DESCRIBE"),
    _T("DETERMINISTIC"),
    _T("DISCONNECT"),
    _T("DISTINCT"),
    _T("DROP"),
    _T("DYNAMIC"),

    _T("EACH"),
    _T("ELEMENT"),
    _T("ELSE"),
    _T("EMPTY"),
    _T("END-EXEC"),
    _T("END"),
    _T("END_FRAME"),
    _T("END_PARTITION"),
    _T("EQUALS"),
    _T("ESCAPE"),
    _T("EVERY"),
    _T("EXCEPT"),
    _T("EXEC"),
    _T("EXECUTE"),
    _T("EXISTS"),
    _T("EXIT"),
    _T("EXP"),
    _T("EXTERNAL"),
    _T("EXTRACT"),

    _T("FALSE"),
    _T("FETCH"),
    _T("FILTER"),
    _T("FIRST_VALUE"),
    _T("FLOOR"),
    _T("FOR"),
    _T("FOREIGN"),
    _T("FOUND"),
    _T("FRAME_ROW"),
    _T("FREE"),
    _T("FROM"),
    _T("FULL"),
    _T("FUNCTION"),
    _T("FUSION"),

    _T("GET"),
    _T("GLOBAL"),
    _T("GRANT"),
    _T("GROUP"),
    _T("GROUPING"),
    _T("GROUPS"),

    _T("HAVING"),
    _T("HOLD"),
    _T("HOUR"),

    _T("IDENTITY"),
    _T("IN"),
    _T("INDEX"),
    _T("INDICATOR"),
    _T("INITIAL"),
    _T("INNER"),
    _T("INOUT"),
    _T("INSENSITIVE"),
    _T("INSERT"),
    _T("INTERSECT"),
    _T("INTERSECTION"),
    _T("INTERVAL"),
    _T("INTO"),
    _T("IS"),

    _T("JOIN"),
    _T("JSON"),
    _T("JSON_ARRAY"),
    _T("JSON_ARRAYAGG"),
    _T("JSON_EXISTS"),
    _T("JSON_OBJECT"),
    _T("JSON_OBJECTAGG"),
    _T("JSON_QUERY"),
    _T("JSON_TABLE"),
    _T("JSON_TABLE_PRIMITIVE"),
    _T("JSON_VALUE"),

    _T("LAG"),
    _T("LANGUAGE"),
    _T("LARGE"),
    _T("LAST_VALUE"),
    _T("LATERAL"),
    _T("LEAD"),
    _T("LEADING"),
    _T("LEFT"),
    _T("LIKE"),
    _T("LIKE_REGEX"),
    _T("LISTAGG"),
    _T("LN"),
    _T("LOCAL"),
    _T("LOCALTIME"),
    _T("LOCALTIMESTAMP"),
    _T("LOG"),
    _T("LOG10"),
    _T("LOWER"),

    _T("MATCH"),
    _T("MATCH_NUMBER"),
    _T("MATCH_RECOGNIZE"),
    _T("MATCHES"),
    _T("MAX"),
    _T("MEMBER"),
    _T("MERGE"),
    _T("METHOD"),
    _T("MIN"),
    _T("MINUTE"),
    _T("MOD"),
    _T("MODIFIES"),
    _T("MODULE"),
    _T("MONTH"),
    _T("MULTISET"),

    _T("NATIONAL"),
    _T("NATURAL"),
    _T("NEW"),
    _T("NO"),
    _T("NONE"),
    _T("NORMALIZE"),
    _T("NOT"),
    _T("NTH_VALUE"),
    _T("NTILE"),
    _T("NULL"),
    _T("NULLIF"),

    _T("OCCURRENCES_REGEX"),
    _T("OCTET_LENGTH"),
    _T("OF"),
    _T("OFFSET"),
    _T("OLD"),
    _T("OMIT"),
    _T("ON"),
    _T("ONE"),
    _T("ONLY"),
    _T("OPEN"),
    _T("OR"),
    _T("ORDER"),
    _T("OUT"),
    _T("OUTER"),
    _T("OVER"),
    _T("OVERLAPS"),
    _T("OVERLAY"),

    _T("PARAMETER"),
    _T("PARTITION"),
    _T("PATTERN"),
    _T("PER"),
    _T("PERCENT"),
    _T("PERCENT_RANK"),
    _T("PERCENTILE_CONT"),
    _T("PERCENTILE_DISC"),
    _T("PERIOD"),
    _T("PORTION"),
    _T("POSITION"),
    _T("POSITION_REGEX"),
    _T("POWER"),
    _T("PRECEDES"),
    _T("PRECISION"),
    _T("PREPARE"),
    _T("PRIMARY"),
    _T("PROCEDURE"),
    _T("PTF"),

    _T("RANGE"),
    _T("RANK"),
    _T("READS"),
    _T("RECURSIVE"),
    _T("REF"),
    _T("REFERENCES"),
    _T("REFERENCING"),
    _T("REGR_AVGX"),
    _T("REGR_AVGY"),
    _T("REGR_COUNT"),
    _T("REGR_INTERCEPT"),
    _T("REGR_R2"),
    _T("REGR_SLOPE"),
    _T("REGR_SXX"),
    _T("REGR_SXY"),
    _T("REGR_SYY"),
    _T("RELEASE"),
    _T("RESULT"),
    _T("RETURN"),
    _T("RETURNS"),
    _T("REVOKE"),
    _T("RIGHT"),
    _T("ROLLBACK"),
    _T("ROLLUP"),
    _T("ROW"),
    _T("ROW_NUMBER"),
    _T("ROWS"),
    _T("RUNNING"),

    _T("SAVEPOINT"),
    _T("SCOPE"),
    _T("SCROLL"),
    _T("SEARCH"),
    _T("SECOND"),
    _T("SEEK"),
    _T("SELECT"),
    _T("SENSITIVE"),
    _T("SESSION_USER"),
    _T("SET"),
    _T("SHOW"),
    _T("SIMILAR"),
    _T("SIN"),
    _T("SINH"),
    _T("SIZE"),
    _T("SKIP"),
    _T("SOME"),
    _T("SPECIFIC"),
    _T("SPECIFICTYPE"),
    _T("SQL"),
    _T("SQLEXCEPTION"),
    _T("SQLSTATE"),
    _T("SQLWARNING"),
    _T("SQRT"),
    _T("START"),
    _T("STATIC"),
    _T("STDDEV_POP"),
    _T("STDDEV_SAMP"),
    _T("SUBMULTISET"),
    _T("SUBSET"),
    _T("SUBSTRING"),
    _T("SUBSTRING_REGEX"),
    _T("SUCCEEDS"),
    _T("SUM"),
    _T("SYMMETRIC"),
    _T("SYSTEM"),
    _T("SYSTEM_TIME"),
    _T("SYSTEM_USER"),

    _T("TABLE"),
    _T("TABLESAMPLE"),
    _T("TAN"),
    _T("TANH"),
    _T("THEN"),
    _T("TIMEZONE_HOUR"),
    _T("TIMEZONE_MINUTE"),
    _T("TO"),
    _T("TRAILING"),
    _T("TRANSLATE"),
    _T("TRANSLATE_REGEX"),
    _T("TRANSLATION"),
    _T("TREAT"),
    _T("TRIGGER"),
    _T("TRIM"),
    _T("TRIM_ARRAY"),
    _T("TRUE"),
    _T("TRUNCATE"),

    _T("UESCAPE"),
    _T("UNION"),
    _T("UNIQUE"),
    _T("UNKNOWN"),
    _T("UNNEST"),
    _T("UPDATE"),
    _T("UPPER"),
    _T("USER"),
    _T("USING"),

    _T("VALUE"),
    _T("VALUE_OF"),
    _T("VALUES"),
    _T("VAR_POP"),
    _T("VAR_SAMP"),
    _T("VARYING"),
    _T("VERSIONING"),
    _T("VIEW"),

    _T("WHEN"),
    _T("WHENEVER"),
    _T("WHERE"),
    _T("WIDTH_BUCKET"),
    _T("WINDOW"),
    _T("WITH"),
    _T("WITHIN"),
    _T("WITHOUT"),

    _T("YEAR"),
  };

//  SQL data type keywords
static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T("BIGINT"),
    _T("BINARY"),
    _T("BIT"),
    _T("BLOB"),
    _T("BOOLEAN"),
    _T("BYTE"),
    _T("CHAR"),
    _T("CLOB"),
    _T("DATE"),
    _T("DATETIME"),
    _T("DECIMAL"),
    _T("DOUBLE"),
    _T("FLOAT"),
    _T("IMAGE"),
    _T("INT"),
    _T("INTEGER"),
    _T("MEMO"),
    _T("MONEY"),
    _T("NCHAR"),
    _T("NUMERIC"),
    _T("NVARCHAR"),
    _T("REAL"),
    _T("SMALLDATETIME"),
    _T("SMALLINT"),
    _T("SMALLMONEY"),
    _T("TEXT"),
    _T("TIME"),
    _T("TIMESTAMP"),
    _T("VARBINARY"),
    _T("VARCHAR"),
    _T("WORD"),
    _T("XML"),
  };

static bool
IsSqlKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszSqlKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineSql (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  const tchar_t *pszCommentBegin = nullptr;
  const tchar_t *pszCommentEnd = nullptr;
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
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha (*tc::tcharnext(pszChars + nPos))))
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

      //  Extended comment /*....*/
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
        }

      if ((pszCommentEnd < pszChars + I) && I > 0 && (
          (pszChars[I] == '/' && pszChars[nPrevI] == '/') ||
          (pszChars[I] == '-' && pszChars[nPrevI] == '-')))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
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
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[nPrevI]) && !xisalpha (pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsSqlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
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

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '(')
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
      if (IsSqlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
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

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
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

  if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
    dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
