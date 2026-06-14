#pragma once

#include "ISyntaxParser.h"
#include "ISyntaxParserFactory.h"
#include <vector>
#include <memory>

using ParseFunc = unsigned (*)(unsigned, const tchar_t*, int, std::vector<LangServices::TEXTBLOCK>*);

/**
 * @brief Adapter that wraps legacy CrystalEdit line-based parsers.
 *
 * This class adapts the traditional ParseLineX function pointer style
 * to the ISyntaxParser interface, managing per-line cookie state internally.
 */
class CrystalLineSyntaxParser : public LangServices::ISyntaxParser
{
public:
	/**
	 * @brief Construct a parser adapter for a specific text type.
	 * @param textType The language/format type to parse.
	 */
	explicit CrystalLineSyntaxParser(LangServices::LanguageId textType);
	virtual ~CrystalLineSyntaxParser() = default;

	// ISyntaxParser interface implementation
	void Invalidate() override { InvalidateFromLine(0); }
	void SetTextBuffer(LangServices::ITextBuffer* pTextBuffer) override;
	LangServices::ITextBuffer* GetTextBuffer() const override { return m_pTextBuffer; }
	std::vector<LangServices::TEXTBLOCK> ParseLine(int nLineIndex) override;
	void NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType) override;
	LangServices::LanguageId GetParserType() const override;
	bool FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const override;

private:
	/**
	 * @brief Get or compute the parser cookie for a specific line.
	 * @param nLineIndex Zero-based line index.
	 * @return The cookie value for this line.
	 */
	unsigned GetLineCookie(int nLineIndex);

	/**
	 * @brief Invalidate cached parse state from a specific line onward.
	 * @param nStartLine Zero-based index of the first line to invalidate.
	 */
	void InvalidateFromLine(int nStartLine);

	LangServices::ITextBuffer* m_pTextBuffer;                    ///< Text buffer interface
	ParseFunc m_ParseLineX;                        ///< Pointer to the legacy line parser function
	LangServices::LanguageId m_textType;                ///< Parser type
	LangServices::TextDefinition* m_pTextDef;           ///< Parser definition
	std::vector<unsigned> m_ParseCookies;          ///< Per-line parser state cookies
};

class CrystalLineSyntaxParserFactory : public LangServices::ISyntaxParserFactory
{
public:
	static CrystalLineSyntaxParserFactory& GetInstance()
	{
		static CrystalLineSyntaxParserFactory instance;
		return instance;
	}

	CrystalLineSyntaxParserFactory() {};
	virtual ~CrystalLineSyntaxParserFactory() = default;

	bool IsSupported(LangServices::LanguageId type) const;
	virtual std::shared_ptr<LangServices::ISyntaxParser> Create(LangServices::LanguageId type) const override
	{
		if (!IsSupported(type))
			return nullptr;
		return std::make_shared<CrystalLineSyntaxParser>(type);
	}
};

