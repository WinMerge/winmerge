#include "StdAfx.h"
#include "MyReBar.h"

BEGIN_MESSAGE_MAP(CMyReBar, CReBar)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CMyReBar::CMyReBar()
{
}

static inline bool IsHighContrastEnabled()
{
	HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
	return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

BOOL CMyReBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, GetSysColor(
		IsHighContrastEnabled() ? COLOR_BTNFACE : COLOR_3DHIGHLIGHT));
	return TRUE;
}

void CMyReBar::LoadStateFromString(const CString& state)
{
	constexpr int BARCOUNT_MAX = 4;
	unsigned count = 0;
	unsigned id[BARCOUNT_MAX]{};
	int cx[BARCOUNT_MAX]{-1, -1};
	_stscanf_s(state, _T("%u,%u,%d,%u,%d,%u,%d,%u,%d"), &count, &id[0], &cx[0], &id[1], &cx[1], &id[2], &cx[2], &id[3], &cx[3]);
	count = (count > BARCOUNT_MAX) ? BARCOUNT_MAX : count;
	CReBarCtrl& rebar = GetReBarCtrl();
	const unsigned nBarCount = rebar.GetBandCount();
	for (unsigned i = 0; i < count; ++i)
	{
		for (unsigned j = i; j < nBarCount; ++j)
		{
			REBARBANDINFO rbi{ sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_SIZE | RBBIM_STYLE };
			rebar.GetBandInfo(j, &rbi);
			if (id[i] != 0 && id[i] == static_cast<unsigned>(GetWindowLong(rbi.hwndChild, GWL_ID)))
			{
				if (j != i)
					rebar.MoveBand(j, i);
				if (cx[i] < 0)
				{
					rbi.fStyle |= RBBS_BREAK;
					rbi.cx = -cx[i];
				}
				else
				{
					rbi.fStyle &= ~RBBS_BREAK;
					rbi.cx = cx[i];
				}
				rbi.fMask &= ~RBBIM_CHILD;
				rebar.SetBandInfo(i, &rbi);
			}
		}
	}
}

CString CMyReBar::GenerateStateString()
{
	CReBarCtrl& rebar = GetReBarCtrl();
	const unsigned nCount = rebar.GetBandCount();
	CString state;
	state.AppendFormat(_T("%d"), nCount);
	for (unsigned i = 0; i < nCount; ++i)
	{
		REBARBANDINFO rbi{ sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_SIZE | RBBIM_STYLE };
		rebar.GetBandInfo(i, &rbi);
		state.AppendFormat(_T(",%u,%d"),
			GetWindowLong(rbi.hwndChild, GWL_ID),
			((rbi.fStyle & RBBS_BREAK) != 0) ? -static_cast<int>(rbi.cx) : rbi.cx);
	}
	return state;
}