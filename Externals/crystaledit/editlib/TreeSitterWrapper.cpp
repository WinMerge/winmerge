/**
 * @file  TreeSitterWrapper.cpp
 *
 * @brief TreeSitter wrapper implementation for integration with TextDefinition::ParseLineX
 */

#include "stdafx.h"
#include "TreeSitterWrapper.h"
#include "TreeSitterParser.h"
#include "utils/ctchar.h"
#include <cassert>

 /**
  * @brief TreeSitter-based ParseLineX implementation
  *
  * Can be registered as TextDefinition::ParseLineX.
  * Cookie upper 16 bits: BufferID
  * Cookie lower 16 bits: Line number
  *
  * @param dwCookie State cookie (BufferID | LineIndex)
  * @param pszChars Line text (unused - TreeSitter parses the entire content)
  * @param nLength Line length (unused)
  * @param pBuf Output buffer
  * @param nActualItems Output item count
  * @return Cookie for the next line
  */
unsigned ParseLineTreeSitter(unsigned dwCookie, int nLineIndex, const tchar_t* pszChars,
    int nLength, CrystalLineParser::TEXTBLOCK* pBuf,
    int& nActualItems, void* pContext)
{
    // Get parser from global context
    CTreeSitterParser* pParser = reinterpret_cast<CTreeSitterParser*>(pContext);
    if (pParser != nullptr)
    {
        // Lazy reparse: if cache is dirty, reparse now (once per paint cycle)
        if (pParser->IsDirty())
            pParser->EnsureParsed(pParser->GetBuffer());

        if (pParser->HasTree())
        {
            // Bounds safety: use the parser's cached line count to avoid
            // accessing stale data. We intentionally avoid calling
            // LocateTextBuffer() here since ParseLine may be called
            // before the window handle is fully set up.
            if (nLineIndex >= 0 && nLineIndex < pParser->GetCachedLineCount())
            {
                // Buffer size: GetTextBlocks allocates (nLength+1)*3 TEXTBLOCK entries
                int nMaxBlocks = (nLength + 1) * 3;

                pParser->GetLineBlocks(nLineIndex, pBuf, nActualItems, 0);

                // Return cookie for next line
                return 0;
            }
        }
    }

	// If parser is not available, set default NORMALTEXT block
	if (pBuf != nullptr)
	{
		nActualItems = 1;
		pBuf[0].m_nCharPos = 0;
		pBuf[0].m_nColorIndex = COLORINDEX_NORMALTEXT;
		pBuf[0].m_nBgColorIndex = COLORINDEX_BKGND;
	}

	// Return cookie for next line
    return 0;
}

/**
 * @brief Dynamically create TextDefinition for TreeSitter
 * 
 * @param pszExt File extension (without dot, e.g., "cpp", "fs")
 * @param pszName Language name (for display, e.g., "C++", "F#")
 * @param nBufferId Buffer ID (for context identification)
 * @return New TextDefinition if TreeSitter is available, nullptr otherwise
 * 
 * @note The returned TextDefinition must be freed by the caller using FreeTreeSitterTextDefinition()
 */
CrystalLineParser::TextDefinition* CreateTreeSitterTextDefinition(
    const tchar_t* pszExt, const tchar_t* pszName, int nBufferId)
{
    if (pszExt == nullptr || pszName == nullptr)
        return nullptr;

    // Check if TreeSitterRegistry has a language for this extension
    const CTreeSitterLanguage* pLang = TreeSitterRegistry::Instance().GetLanguageForExt(pszExt);
    if (pLang == nullptr || pLang->GetLanguage() == nullptr)
    {
        // Return nullptr if TreeSitter language is not available
        return nullptr;
    }

    // Dynamically allocate new TextDefinition
    auto pDef = new CrystalLineParser::TextDefinition();
    if (pDef == nullptr)
        return nullptr;

    // Initialize TextDefinition fields
    pDef->type = CrystalLineParser::SRC_PLAIN; // TreeSitter does not have a specific type
    pDef->ParseLineX = ParseLineTreeSitter;
    pDef->flags = 0;
    pDef->encoding = 0;

    // Copy language name (dynamic allocation)
    const size_t nNameLen = tc::tcslen(pszName);
    pDef->name = new tchar_t[nNameLen + 1];
#ifdef _UNICODE
    _wcslcpy(const_cast<tchar_t*>(pDef->name), nNameLen + 1, pszName);
#else
    _strlcpy(const_cast<tchar_t*>(pDef->name), nNameLen + 1, pszName);
#endif

    // Copy extension (dynamic allocation)
    const size_t nExtLen = tc::tcslen(pszExt);
    pDef->exts = new tchar_t[nExtLen + 1];
#ifdef _UNICODE
    _wcslcpy(pDef->exts, nExtLen + 1, pszExt);
#else
    _strlcpy(pDef->exts, nExtLen + 1, pszExt);
#endif
    pDef->extsIsDynamic = true;

    // Comment syntax is empty (TreeSitter is AST-based, so not needed)
    pDef->opencomment[0] = _T('\0');
    pDef->closecomment[0] = _T('\0');
    pDef->commentline[0] = _T('\0');

    return pDef;
}

/**
 * @brief Free TreeSitter TextDefinition
 * @param pDef TextDefinition to free
 */
void FreeTreeSitterTextDefinition(CrystalLineParser::TextDefinition* pDef)
{
    if (pDef == nullptr)
        return;

    // Free dynamically allocated fields
    if (pDef->name != nullptr)
    {
        delete[] pDef->name;
    }

    if (pDef->extsIsDynamic && pDef->exts != nullptr)
    {
        delete[] pDef->exts;
    }

    // Free TextDefinition itself
    delete pDef;
}

void* CreateTreeSitterParseContextForDiff(const String& filePath, const std::vector<String>& lines)
{
    String ext = filePath;
    size_t posOfDot = ext.rfind('.');
    if (posOfDot != String::npos)
        ext.erase(0, posOfDot + 1);

    TreeSitterRegistry& registry = TreeSitterRegistry::Instance();
    if (!registry.IsInitialized())
        registry.Initialize();

    const CTreeSitterLanguage* pLang = registry.GetLanguageForExt(ext.c_str());
    if (pLang == nullptr || pLang->GetLanguage() == nullptr)
        return nullptr;

    auto* pParser = new CTreeSitterParser();
    pParser->SetLanguage(pLang);

    std::vector<const tchar_t*> linePtrs;
    std::vector<int> lineLens;
    linePtrs.reserve(lines.size());
    lineLens.reserve(lines.size());
    for (const auto& line : lines)
    {
        linePtrs.push_back(line.c_str());
        lineLens.push_back(static_cast<int>(line.size()));
    }
    pParser->ParseDocument(linePtrs.data(), lineLens.data(), static_cast<int>(linePtrs.size()));
    return pParser;
}

void DestroyTreeSitterParseContextForDiff(void* parseContext)
{
    delete reinterpret_cast<CTreeSitterParser*>(parseContext);
}

bool IsTreeSitterCommentPositionForDiff(void* parseContext, int nLineIndex, int nCharPos)
{
    auto* pParser = reinterpret_cast<CTreeSitterParser*>(parseContext);
    if (pParser == nullptr)
        return false;

    return pParser->IsCommentPosition(nLineIndex, nCharPos);
}
