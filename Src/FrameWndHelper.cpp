#include "StdAfx.h"
#include "FrameWndHelper.h"
#include <../src/mfc/afximpl.h>

namespace FrameWndHelper
{

/**
 * @brief We must use this function before a call to SetDockState
 *
 * @note Without this, SetDockState will assert or crash if a bar from the
 * CDockState is missing in the current CMergeEditFrame.
 * The bars are identified with their ID. This means the missing bar bug is triggered
 * when we run WinMerge after changing the ID of a bar.
 */
bool EnsureValidDockState(CFrameWnd* pFrameWnd, CDockState& state)
{
	for (int i = (int)state.m_arrBarInfo.GetSize() - 1; i >= 0; i--)
	{
		bool barIsCorrect = true;
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		if (pInfo == nullptr)
			barIsCorrect = false;
		else
		{
			if (!pInfo->m_bFloating)
			{
				pInfo->m_pBar = pFrameWnd->GetControlBar(pInfo->m_nBarID);
				if (pInfo->m_pBar == nullptr)
					barIsCorrect = false; //toolbar id's probably changed	
			}
		}

		if (!barIsCorrect)
			state.m_arrBarInfo.RemoveAt(i);
	}
	return true;
}

void RemoveBarBorder(CFrameWnd* pFrameWnd)
{
	afxData.cxBorder2 = 0;
	afxData.cyBorder2 = 0;
	for (int i = 0; i < 4; ++i)
	{
		CControlBar* pBar = pFrameWnd->GetControlBar(i + AFX_IDW_DOCKBAR_TOP);
		pBar->SetBarStyle(pBar->GetBarStyle() & ~(CBRS_BORDER_ANY | CBRS_BORDER_3D));
	}
}
}
