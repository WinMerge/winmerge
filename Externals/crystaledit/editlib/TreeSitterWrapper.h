/**
 * @file  TreeSitterWrapper.h
 *
 * @brief TreeSitter wrapper for integration with TextDefinition::ParseLineX
 */
#pragma once

#include <string>
#include <vector>
#include "parsers/crystallineparser.h"
#include "../../Src/Common/UnicodeString.h"
#include <map>
#include <memory>

/**
 * @brief TreeSitter-based ParseLineX implementation
 * 
 * Can be registered as TextDefinition::ParseLineX.
 * Cookie upper 16 bits: BufferID
 * Cookie lower 16 bits: Line number
 * 
 * @param dwCookie State cookie (BufferID | LineIndex)
 * @param pszChars Line text (unused)
 * @param nLength Line length (unused)
 * @param pBuf Output buffer
 * @param nActualItems Output item count
 * @return Cookie for the next line
 */
unsigned ParseLineTreeSitter(unsigned dwCookie, int nLineIndex, const tchar_t *pszChars, 
                             int nLength, CrystalLineParser::TEXTBLOCK * pBuf, 
                             int &nActualItems, void* pContext);

/**
 * @brief Dynamically create TextDefinition for TreeSitter
 * 
 * @param pszExt File extension
 * @param pszName Language name (for display)
 * @param nBufferId Buffer ID (for context identification)
 * @return New TextDefinition if TreeSitter is available, nullptr otherwise
 * 
 * @note The returned TextDefinition must be freed by the caller using delete
 */
CrystalLineParser::TextDefinition* CreateTreeSitterTextDefinition(
    const tchar_t* pszExt, const tchar_t* pszName, int nBufferId);

/**
 * @brief Free TreeSitter TextDefinition
 * @param pDef TextDefinition to free
 */
void FreeTreeSitterTextDefinition(CrystalLineParser::TextDefinition* pDef);

void* CreateTreeSitterParseContextForDiff(const String& filePath, const std::vector<String>& lines);
void DestroyTreeSitterParseContextForDiff(void* parseContext);
bool IsTreeSitterCommentPositionForDiff(void* parseContext, int nLineIndex, int nCharPos);
