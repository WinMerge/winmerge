/**
 * @file  ccrystalrendererdirectdraw.cpp
 *
 * @brief Implementation of the CCrystalRendererDirectWrite class
 */

#ifdef _WIN64
#undef WINVER
#define WINVER 0x0a00
#include <afxwin.h>
#include "ccrystalrendererdirectwrite.h"
#include "utils/ctchar.h"
#include "resource.h"
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <vector>
#include <algorithm>
#include <utility>

struct CustomGlyphRun : public DWRITE_GLYPH_RUN
{
	CustomGlyphRun(const DWRITE_GLYPH_RUN& glyphRun, const float *customGlyphAdvances, float charHeight)
		: DWRITE_GLYPH_RUN(glyphRun)
		, sumCharWidth(0)
		, ascent(0)
	{
		glyphAdvances = new float[glyphCount];
		DWRITE_FONT_METRICS fontFaceMetrics;
		glyphRun.fontFace->GetMetrics(&fontFaceMetrics);
		for (unsigned i = 0; i < glyphCount; ++i)
		{
			const_cast<float*>(glyphAdvances)[i] = customGlyphAdvances[i];
			sumCharWidth += glyphAdvances[i];
		}
		float height = (fontFaceMetrics.ascent + fontFaceMetrics.descent) * fontEmSize / fontFaceMetrics.designUnitsPerEm;
		float scaleY = charHeight / height;
		if (1.0f > scaleY)
			fontEmSize *= scaleY;
		ascent = fontFaceMetrics.ascent * fontEmSize / fontFaceMetrics.designUnitsPerEm;
	}

	CustomGlyphRun(const CustomGlyphRun& other)
		: DWRITE_GLYPH_RUN(other)
		, sumCharWidth(other.sumCharWidth)
		, ascent(0.0f)
	{
		glyphAdvances = new float[other.glyphCount];
		for (unsigned i = 0; i < other.glyphCount; ++i)
			const_cast<float*>(glyphAdvances)[i] = other.glyphAdvances[i];
	}

	~CustomGlyphRun()
	{
		delete[] glyphAdvances;
	}

	float sumCharWidth;
	float ascent;
};

struct DrawGlyphRunParams
{
	DrawGlyphRunParams(
		float fBaselineOriginXOther,
		float fBaselineOriginYOther,
		DWRITE_MEASURING_MODE measuringModeOther,
		const DWRITE_GLYPH_RUN& glyphRunOther)
		: glyphRun(glyphRunOther)
		, fBaselineOriginX(fBaselineOriginXOther)
		, fBaselineOriginY(fBaselineOriginYOther)
		, measuringMode(measuringModeOther)
	{
		glyphRun.glyphAdvances = new FLOAT[glyphRunOther.glyphCount];
		glyphRun.glyphIndices = new UINT16[glyphRunOther.glyphCount];
		glyphRun.glyphOffsets = glyphRunOther.glyphOffsets ? new DWRITE_GLYPH_OFFSET[glyphRunOther.glyphCount] : nullptr;
		glyphRun.fontFace->AddRef();
		for (unsigned i = 0; i < glyphRunOther.glyphCount; ++i)
		{
			const_cast<float *>(glyphRun.glyphAdvances)[i] = glyphRunOther.glyphAdvances[i];
			const_cast<UINT16 *>(glyphRun.glyphIndices)[i] = glyphRunOther.glyphIndices[i];
			if (glyphRunOther.glyphOffsets)
				const_cast<DWRITE_GLYPH_OFFSET *>(glyphRun.glyphOffsets)[i] = glyphRunOther.glyphOffsets[i];
		}
	}

	DrawGlyphRunParams(const DrawGlyphRunParams &other)
		: glyphRun(other.glyphRun)
		, fBaselineOriginX(other.fBaselineOriginX)
		, fBaselineOriginY(other.fBaselineOriginY)
		, measuringMode(other.measuringMode)
	{
		glyphRun.glyphAdvances = new FLOAT[other.glyphRun.glyphCount];
		glyphRun.glyphIndices = new UINT16[other.glyphRun.glyphCount];
		glyphRun.glyphOffsets = other.glyphRun.glyphOffsets ? new DWRITE_GLYPH_OFFSET[other.glyphRun.glyphCount] : nullptr;
		glyphRun.fontFace->AddRef();
		for (unsigned i = 0; i < other.glyphRun.glyphCount; ++i)
		{
			const_cast<FLOAT *>(glyphRun.glyphAdvances)[i] = other.glyphRun.glyphAdvances[i];
			const_cast<UINT16 *>(glyphRun.glyphIndices)[i] = other.glyphRun.glyphIndices[i];
			if (other.glyphRun.glyphOffsets)
				const_cast<DWRITE_GLYPH_OFFSET *>(glyphRun.glyphOffsets)[i] = other.glyphRun.glyphOffsets[i];
		}
	}

	~DrawGlyphRunParams()
	{
		delete[] glyphRun.glyphAdvances;
		delete[] glyphRun.glyphIndices;
		delete[] glyphRun.glyphOffsets;
		glyphRun.fontFace->Release();
	}

	FLOAT fBaselineOriginX;
	FLOAT fBaselineOriginY;
	DWRITE_MEASURING_MODE measuringMode;
	DWRITE_GLYPH_RUN glyphRun;
};

class CDrawingContext
{
public:
	CDrawingContext(CRenderTarget* pRenderTarget)
	: m_pRenderTarget(pRenderTarget)
	{
	}

	CRenderTarget* m_pRenderTarget;
	std::vector<DrawGlyphRunParams> m_drawGlyphRunParams;
};

class CCustomTextRenderer : public CCmdTarget
{
	DECLARE_DYNAMIC(CCustomTextRenderer)
public:
	CCustomTextRenderer() = default;
	virtual ~CCustomTextRenderer() = default;
	IDWriteTextRenderer* Get();
public:
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(CustomTextRenderer, IDWriteTextRenderer)
	// override IDWriteTextRenderer
	STDMETHOD(DrawGlyphRun)(void*, FLOAT, FLOAT, DWRITE_MEASURING_MODE, const DWRITE_GLYPH_RUN*,
			const DWRITE_GLYPH_RUN_DESCRIPTION*, IUnknown*);
	STDMETHOD(DrawInlineObject)(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*);
	STDMETHOD(DrawStrikethrough)(void*, FLOAT, FLOAT, const DWRITE_STRIKETHROUGH*, IUnknown*);
	STDMETHOD(DrawUnderline)(void*, FLOAT, FLOAT, const DWRITE_UNDERLINE*, IUnknown*);
	// override IDWritePixelSnapping
	STDMETHOD(GetCurrentTransform)(void*, DWRITE_MATRIX*);
	STDMETHOD(GetPixelsPerDip)(void*, FLOAT*);
	STDMETHOD(IsPixelSnappingDisabled)(void*, BOOL*);
	// implementation helpers
	END_INTERFACE_PART(CustomTextRenderer)
};

inline IDWriteTextRenderer* CCustomTextRenderer::Get()
{
	return &m_xCustomTextRenderer;
}

IMPLEMENT_DYNAMIC(CCustomTextRenderer, CCmdTarget)

BEGIN_INTERFACE_MAP(CCustomTextRenderer, CCmdTarget)
	INTERFACE_PART(CCustomTextRenderer, __uuidof(IDWriteTextRenderer), CustomTextRenderer)
END_INTERFACE_MAP()

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::DrawGlyphRun(void* pClientDrawingContext,
	FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, DWRITE_MEASURING_MODE measuringMode,
	const DWRITE_GLYPH_RUN* pGlyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* pGlyphRunDescription,
	IUnknown* pClientDrawingEffect)
{
	CDrawingContext* pDrawingContext = static_cast<CDrawingContext*>(pClientDrawingContext);
	pDrawingContext->m_drawGlyphRunParams.push_back(
		DrawGlyphRunParams{fBaselineOriginX, fBaselineOriginY, measuringMode, *pGlyphRun});
	return S_OK;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::DrawInlineObject(void* pClientDrawingContext,
	FLOAT fOriginX, FLOAT fOriginY, IDWriteInlineObject* pInlineObject,
	BOOL bIsSideways, BOOL bIsRightToLeft,
	IUnknown* pClientDrawingEffect)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::DrawStrikethrough(void* pClientDrawingContext,
	FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, const DWRITE_STRIKETHROUGH* pStrikethrough,
	IUnknown* pClientDrawingEffect)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::DrawUnderline(void* pClientDrawingContext,
	FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, const DWRITE_UNDERLINE* pUnderline,
	IUnknown* pClientDrawingEffect)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::GetCurrentTransform(void* pClientDrawingContext, DWRITE_MATRIX* pTransform)
{
	CDrawingContext* pDrawingContext = static_cast<CDrawingContext*>(pClientDrawingContext);
	pDrawingContext->m_pRenderTarget->GetTransform((D2D1_MATRIX_3X2_F*)pTransform);
	return S_OK;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::GetPixelsPerDip(void* pClientDrawingContext, FLOAT* pfPixelsPerDip)
{
	CDrawingContext* pDrawingContext = static_cast<CDrawingContext*>(pClientDrawingContext);
	*pfPixelsPerDip = pDrawingContext->m_pRenderTarget->GetDpi().width / 96.0f;
	return S_OK;
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::IsPixelSnappingDisabled(void* pClientDrawingContext, BOOL* pbIsDisabled)
{
	*pbIsDisabled = FALSE;
	return S_OK;
}

STDMETHODIMP_(ULONG) CCustomTextRenderer::XCustomTextRenderer::AddRef()
{
	METHOD_PROLOGUE(CCustomTextRenderer, CustomTextRenderer);
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CCustomTextRenderer::XCustomTextRenderer::Release()
{
	METHOD_PROLOGUE(CCustomTextRenderer, CustomTextRenderer);
	return pThis->ExternalRelease();
}

STDMETHODIMP CCustomTextRenderer::XCustomTextRenderer::QueryInterface(REFIID iid, LPVOID far* ppvObj)
{
	METHOD_PROLOGUE(CCustomTextRenderer, CustomTextRenderer);
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// CCrystalRendererDirectWrite construction/destruction


CCrystalRendererDirectWrite::CCrystalRendererDirectWrite(int nRenderingMode)
	: m_pCurrentTextFormat{ nullptr }, m_charSize{}, m_lfBaseFont{}
	, m_pTextBrush(new CD2DSolidColorBrush(&m_renderTarget, D2D1::ColorF(D2D1::ColorF::Black)))
	, m_pTempBrush(new CD2DSolidColorBrush(&m_renderTarget, D2D1::ColorF(D2D1::ColorF::Black)))
	, m_pBackgroundBrush(new CD2DSolidColorBrush(&m_renderTarget, D2D1::ColorF(D2D1::ColorF::White)))
	, m_pTextRenderer(new CCustomTextRenderer())
{
  const auto props = D2D1::RenderTargetProperties(
      D2D1_RENDER_TARGET_TYPE_DEFAULT,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
      0,
      0,
      D2D1_RENDER_TARGET_USAGE_NONE,
      D2D1_FEATURE_LEVEL_DEFAULT
  );

  m_renderTarget.Create(props);
  IDWriteFactory *pWriteFactory = AfxGetD2DState()->GetWriteFactory();
  CComPtr<IDWriteRenderingParams> pTextRenderingParams, pTextRenderingParams2;
  pWriteFactory->CreateRenderingParams(&pTextRenderingParams);
  pWriteFactory->CreateCustomRenderingParams(
	  pTextRenderingParams->GetGamma(),
	  pTextRenderingParams->GetEnhancedContrast(),
	  1.0f,
	  pTextRenderingParams->GetPixelGeometry(),
	  static_cast<DWRITE_RENDERING_MODE>(nRenderingMode),
	  &pTextRenderingParams2);
  m_renderTarget.SetTextRenderingParams(pTextRenderingParams2);
  m_renderTarget.SetDpi(CD2DSizeF(96.0F, 96.0F));
}

CCrystalRendererDirectWrite::~CCrystalRendererDirectWrite ()
{
}

void CCrystalRendererDirectWrite::BindDC(const CDC& dc, const CRect& rc)
{
	m_renderTarget.BindDC(dc, rc);
}

void CCrystalRendererDirectWrite::BeginDraw()
{
	m_renderTarget.BeginDraw();
}

bool CCrystalRendererDirectWrite::EndDraw()
{
	return (SUCCEEDED(m_renderTarget.EndDraw()));
}

static D2D1_SIZE_F GetCharWidthHeight(IDWriteTextFormat *pTextFormat)
{
	CComPtr<IDWriteTextLayout> pTextLayout;
	AfxGetD2DState()->GetWriteFactory()->CreateTextLayout(L"W", 1, pTextFormat, 0, 0, &pTextLayout);
	DWRITE_TEXT_METRICS textMetrics{};
	pTextLayout->GetMetrics(&textMetrics);
	return {textMetrics.width, textMetrics.height};
}

void CCrystalRendererDirectWrite::SetFont(const LOGFONT &lf)
{
	m_lfBaseFont = lf;
	for (int nIndex = 0; nIndex < 4; ++nIndex)
	{
		bool bold = (nIndex & 1) != 0;
		bool italic = (nIndex & 2) != 0;
		m_pTextFormat[nIndex].reset(new CD2DTextFormat(&m_renderTarget,
			lf.lfFaceName[0] ? lf.lfFaceName : _T("Courier New"),
			static_cast<FLOAT>(abs(lf.lfHeight == 0 ? 11 : lf.lfHeight)),
			bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
			italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL));
		IDWriteTextFormat *pTextFormat = m_pTextFormat[nIndex]->Get();
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}
	m_pCurrentTextFormat = m_pTextFormat[0].get();
	m_pFont.reset();
	m_charSize = ::GetCharWidthHeight(m_pTextFormat[3]->Get());
}

void CCrystalRendererDirectWrite::SwitchFont(bool italic, bool bold)
{
	int nIndex = 0;
	if (bold)
		nIndex |= 1;
	if (italic)
		nIndex |= 2;
	m_pCurrentTextFormat = m_pTextFormat[nIndex].get();
}

CSize CCrystalRendererDirectWrite::GetCharWidthHeight()
{
	if (m_pTextFormat[3] == nullptr)
		SetFont(m_lfBaseFont);
	return CSize(static_cast<int>(m_charSize.width + 0.9999), static_cast<int>(m_charSize.height + 0.9999));
}

bool CCrystalRendererDirectWrite::GetCharWidth(unsigned start, unsigned end, int * nWidthArray)
{
	if (!m_pFont)
	{
		m_pFont.reset(new CFont());
		LOGFONT lfFont = m_lfBaseFont;
		lfFont.lfWeight = FW_BOLD;
		lfFont.lfItalic = TRUE;
		m_pFont->CreateFontIndirect(&lfFont);
	}
	CClientDC dc (CWnd::GetDesktopWindow());
	CFont *pOldFont = dc.SelectObject(m_pFont.get());
	bool succeeded = !!GetCharWidth32(dc.m_hDC, start, end, nWidthArray);
	dc.SelectObject(pOldFont);
	return succeeded;
}

void CCrystalRendererDirectWrite::SetTextColor(CEColor clr)
{
	m_pTextBrush->SetColor(ColorRefToColorF(clr));
}

void CCrystalRendererDirectWrite::SetBkColor(CEColor clr)
{
	m_pBackgroundBrush->SetColor(ColorRefToColorF(clr));
}

void CCrystalRendererDirectWrite::FillRectangle(const CRect &rc)
{
	m_renderTarget.FillRectangle(CD2DRectF(rc), m_pBackgroundBrush.get());
}

void CCrystalRendererDirectWrite::FillSolidRectangle(const CRect &rc, CEColor color)
{
	m_pTempBrush->SetColor(ColorRefToColorF(color));
	m_renderTarget.FillRectangle(CD2DRectF(rc), m_pTempBrush.get());
}

void CCrystalRendererDirectWrite::DrawRoundRectangle(int left, int top, int right, int bottom, int width, int height)
{
	m_renderTarget.DrawRoundedRectangle(
		D2D1_ROUNDED_RECT{ {static_cast<float>(left), static_cast<float>(top), static_cast<float>(right), static_cast<float>(bottom)},
		static_cast<float>(width), static_cast<float>(height) }, m_pTextBrush.get());
}

void CCrystalRendererDirectWrite::PushAxisAlignedClip(const CRect & rc)
{
	m_renderTarget.PushAxisAlignedClip(rc);
}

void CCrystalRendererDirectWrite::PopAxisAlignedClip()
{
	m_renderTarget.PopAxisAlignedClip();
}

void CCrystalRendererDirectWrite::DrawMarginIcon(int x, int y, int iconIndex, int iconsize)
{
	if (!m_pIconBitmap)
	{
		m_pIconBitmap.reset(new CD2DBitmap(nullptr, static_cast<UINT>(IDR_MARGIN_ICONS_PNG), _T("IMAGE")));
		m_pIconBitmap->Create(&m_renderTarget);
	}
	auto size = m_pIconBitmap->GetPixelSize();
	CD2DRectF rcSrc{static_cast<float>(iconIndex * MARGIN_ICON_SIZE), 0.0f, static_cast<float>((iconIndex + 1) * MARGIN_ICON_SIZE), static_cast<float>(MARGIN_ICON_SIZE)};
	m_renderTarget.DrawBitmap(m_pIconBitmap.get(),
		{ static_cast<float>(x), static_cast<float>(y),
		 static_cast<float>(x + iconsize), static_cast<float>(y + iconsize) },
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &rcSrc);
}

void CCrystalRendererDirectWrite::DrawMarginLineNumber(int x, int y, int number)
{
	tchar_t szNumbers[32];
	int len = wsprintf(szNumbers, _T("%d"), number);
	m_renderTarget.DrawText(szNumbers,
		{ static_cast<float>(x) - m_charSize.width * len - 4, static_cast<float>(y),
		  static_cast<float>(x), static_cast<float>(y + m_charSize.height) },
		m_pTextBrush.get(), m_pTextFormat[0].get());
}

void CCrystalRendererDirectWrite::DrawBoundaryLine(int left, int right, int y)
{
	m_pTempBrush->SetColor(ColorRefToColorF(0));
	m_renderTarget.DrawLine(
		{ static_cast<float>(left), static_cast<float>(y) },
		{ static_cast<float>(right), static_cast<float>(y) }, m_pTempBrush.get());
}

void  CCrystalRendererDirectWrite::DrawGridLine(int x1, int y1, int x2, int y2, int sourceConstantAlpha)
{
	m_pTempBrush->SetColor(ColorRefToColorF(0));
	m_renderTarget.DrawLine(
		{ static_cast<float>(x1), static_cast<float>(y1) },
		{ static_cast<float>(x2), static_cast<float>(y2) }, m_pTempBrush.get(), sourceConstantAlpha / 255.f);
}

void CCrystalRendererDirectWrite::DrawLineCursor(int left, int right, int y, int height)
{
	m_pTempBrush->SetColor(ColorRefToColorF(0));
	m_pTempBrush->SetOpacity(0.1f);
	m_renderTarget.DrawLine(
		{ static_cast<float>(left), static_cast<float>(y) },
		{ static_cast<float>(right), static_cast<float>(y) },
		m_pTempBrush.get(), static_cast<float>(1));
	m_pTempBrush->SetOpacity(1.0f);
}

void CCrystalRendererDirectWrite::DrawText(int x, int y, const CRect &rc, const tchar_t *text, size_t len, const int nWidths[])
{
	CD2DRectF rcF(rc);

	m_renderTarget.PushAxisAlignedClip(rcF);

	m_renderTarget.FillRectangle(rcF, m_pBackgroundBrush.get());

	D2D1_COLOR_F textColor = m_pTextBrush->GetColor();
	D2D1_COLOR_F backColor = m_pBackgroundBrush->GetColor();
	if (memcmp(&textColor, &backColor, sizeof(D2D1_COLOR_F)) != 0)
	{
		CDrawingContext drawingContext{ &m_renderTarget };
		CComPtr<IDWriteTextLayout> pTextLayout;
#ifdef _UNICODE
		AfxGetD2DState()->GetWriteFactory()->CreateTextLayout(text, static_cast<unsigned>(len),
#else
		USES_CONVERSION;
		AfxGetD2DState()->GetWriteFactory()->CreateTextLayout(A2W(text), static_cast<unsigned>(len),
#endif
			*m_pCurrentTextFormat,
			rcF.right - rcF.left, rcF.bottom - rcF.top, &pTextLayout);
		pTextLayout->Draw(&drawingContext, m_pTextRenderer->Get(), 0, 0);

		std::vector<float> customGlyphAdvances(len, m_charSize.width);
		for (size_t i = 0, j = 0; i < len; ++i)
		{
			if (nWidths[i] != 0)
				customGlyphAdvances[j++] = static_cast<float>(nWidths[i]);
		}

		struct DrawGlyphRunIndex { size_t i; float fBaselineOriginX; size_t glyphPos; };
		std::vector<DrawGlyphRunIndex> indices;
		for (size_t i = 0, glyphPos = 0; i < drawingContext.m_drawGlyphRunParams.size(); ++i)
		{
			indices.push_back({i, drawingContext.m_drawGlyphRunParams[i].fBaselineOriginX, glyphPos});
			glyphPos += drawingContext.m_drawGlyphRunParams[i].glyphRun.glyphCount;
		}
		std::stable_sort(indices.begin(), indices.end(),
			[](const DrawGlyphRunIndex & a, const DrawGlyphRunIndex& b) { return a.fBaselineOriginX < b.fBaselineOriginX; });

		float fBaselineOriginX = static_cast<float>(x);
		for (size_t i = 0; i < indices.size(); ++i)
		{
			if (indices[i].glyphPos >= customGlyphAdvances.size())
			{
				TRACE(_T("BUG: indices[i].glyphPos >= customGlyphAdvances.size()\n"));
				break;
			}
			DrawGlyphRunParams& param = drawingContext.m_drawGlyphRunParams[indices[i].i];
			CustomGlyphRun customGlyphRun(param.glyphRun, &customGlyphAdvances[indices[i].glyphPos], m_charSize.height);
			float fBaselineOriginY = y + customGlyphRun.ascent;
			DrawGlyphRun(&drawingContext,
				(customGlyphRun.bidiLevel & 1) ? (fBaselineOriginX + customGlyphRun.sumCharWidth) : fBaselineOriginX,
				fBaselineOriginY,
				param.measuringMode, &customGlyphRun, nullptr, nullptr);
			fBaselineOriginX += customGlyphRun.sumCharWidth;
		}
	}
	m_renderTarget.PopAxisAlignedClip();
}

STDMETHODIMP CCrystalRendererDirectWrite::DrawGlyphRun(void* pClientDrawingContext,
	FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, DWRITE_MEASURING_MODE measuringMode,
	const DWRITE_GLYPH_RUN* pGlyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* pGlyphRunDescription,
	IUnknown* pClientDrawingEffect)
{
	IDWriteFactory *pWriteFactory = AfxGetD2DState()->GetWriteFactory();
	CComQIPtr<IDWriteFactory4> pWriteFactory4(pWriteFactory);
	CComQIPtr<ID2D1DeviceContext4> pD2dDeviceContext(m_renderTarget);
	CComPtr<IDWriteColorGlyphRunEnumerator1> glyphRunEnumerator;

	DWRITE_GLYPH_IMAGE_FORMATS supportedFormats = static_cast<DWRITE_GLYPH_IMAGE_FORMATS>(
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_TRUETYPE) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_CFF) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_COLR) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_SVG) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_PNG) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_JPEG)|
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_TIFF) |
		static_cast<int>(DWRITE_GLYPH_IMAGE_FORMATS_PREMULTIPLIED_B8G8R8A8)
	);

	if (!pWriteFactory4)
	{
		m_renderTarget.DrawGlyphRun(
			{fBaselineOriginX, fBaselineOriginY},
			*pGlyphRun, m_pTextBrush.get(), measuringMode);
		return S_OK;
	}

	HRESULT hr = pWriteFactory4->TranslateColorGlyphRun(
		{fBaselineOriginX, fBaselineOriginY},
		pGlyphRun, pGlyphRunDescription, supportedFormats,
		measuringMode, nullptr, 0, &glyphRunEnumerator);
	if (hr == DWRITE_E_NOCOLOR)
	{
		m_renderTarget.DrawGlyphRun(
			{fBaselineOriginX, fBaselineOriginY},
			*pGlyphRun, m_pTextBrush.get(), measuringMode);
		return S_OK;
	}

	for (;;)
	{
		BOOL haveRun;
		glyphRunEnumerator->MoveNext(&haveRun);
		if (!haveRun)
			break;

		DWRITE_COLOR_GLYPH_RUN1 const* colorRun;
		glyphRunEnumerator->GetCurrentRun(&colorRun);

		D2D1_POINT_2F currentBaselineOrigin = D2D1::Point2F(
			colorRun->baselineOriginX,
			colorRun->baselineOriginY
		);

		switch (colorRun->glyphImageFormat)
		{
		case DWRITE_GLYPH_IMAGE_FORMATS_PNG:
		case DWRITE_GLYPH_IMAGE_FORMATS_JPEG:
		case DWRITE_GLYPH_IMAGE_FORMATS_TIFF:
		case DWRITE_GLYPH_IMAGE_FORMATS_PREMULTIPLIED_B8G8R8A8:
		{
			// This run is bitmap glyphs. Use Direct2D to draw them.
			pD2dDeviceContext->DrawColorBitmapGlyphRun(
				colorRun->glyphImageFormat, currentBaselineOrigin,
				&colorRun->glyphRun, measuringMode);
		}
		break;

		case DWRITE_GLYPH_IMAGE_FORMATS_SVG:
		{
			// This run is SVG glyphs. Use Direct2D to draw them.
			pD2dDeviceContext->DrawSvgGlyphRun(
				currentBaselineOrigin, &colorRun->glyphRun,
				m_pTextBrush->Get(),
				nullptr, 0, measuringMode);
		}
		break;

		case DWRITE_GLYPH_IMAGE_FORMATS_TRUETYPE:
		case DWRITE_GLYPH_IMAGE_FORMATS_CFF:
		case DWRITE_GLYPH_IMAGE_FORMATS_COLR:
		default:
		{
			// This run is solid-color outlines, either from non-color
			// glyphs or from COLR glyph layers. Use Direct2D to draw them.

			ID2D1Brush* layerBrush;
			if (colorRun->paletteIndex == 0xFFFF)
			{
				// This run uses the current text color.
				layerBrush = m_pTextBrush->Get();
			}
			else
			{
				// This run specifies its own color.
				m_pTempBrush->SetColor(colorRun->runColor);
				layerBrush = m_pTempBrush->Get();
			}

			// Draw the run with the selected color.
			pD2dDeviceContext->DrawGlyphRun(currentBaselineOrigin, &colorRun->glyphRun,
				colorRun->glyphRunDescription, layerBrush, measuringMode);
		}
		break;
		}
	}
	return S_OK;
}

void CCrystalRendererDirectWrite::DrawRuler(int left, int top, int width, int height, int interval, int offset)
{
	m_pTempBrush->SetColor(ColorRefToColorF(0));
	float bottom = static_cast<float>(top + height) - 0.5f;
	int prev10 = (offset / 10) * 10;
	tchar_t szNumbers[32];
	int len = wsprintf(szNumbers, _T("%d"), prev10);
	if ((offset % 10) != 0 && offset - prev10 < len)
	{
		m_renderTarget.DrawText(szNumbers + (offset - prev10),
			{ static_cast<float>(left), bottom - height + 0.5f,
			  static_cast<float>(left + (len - (offset - prev10)) * interval), bottom },
			m_pTextBrush.get(), m_pTextFormat[0].get());
	}
	for (int i = 0; i < width / interval; ++i)
	{
		float x = static_cast<float>(left + i * interval);
		if (((i + offset) % 10) == 0)
		{
			len = wsprintf(szNumbers, _T("%d"), offset + i);
			m_renderTarget.DrawText(szNumbers,
				{ x, bottom - height, x + len * interval, bottom },
				m_pTextBrush.get(), m_pTextFormat[0].get());
		}
		float tickscale = [](int i, int offset) {
			if (((i + offset) % 10) == 0)
				return 0.6f;
			else if (((i + offset) % 5) == 0)
				return 0.4f;
			else
				return 0.2f;
		}(i, offset);
		DrawGridLine(static_cast<int>(x), static_cast<int>(bottom - height * tickscale), static_cast<int>(x), static_cast<int>(bottom), 70);
	}
	DrawGridLine(static_cast<int>(left), static_cast<int>(bottom), static_cast<int>(left + width), static_cast<int>(bottom), 70);
}
#endif
