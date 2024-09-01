// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  TitleBarHelper.cpp
 *
 * @brief Implementation of the CTitleBarHelper class
 */

#include "StdAfx.h"
#include "TitleBarHelper.h"

void CTitleBarHelper::DrawIcon(CDC& dc)
{
}

void CTitleBarHelper::DrawButtons(CDC& dc)
{
}

CRect CTitleBarHelper::GetIconRect()
{
	return CRect{};
}

CRect CTitleBarHelper::GetButtonsRect()
{
	return CRect{};
}

void CTitleBarHelper::Update(CSize size, bool maxmized)
{
}

int CTitleBarHelper::HitTest(CPoint pt)
{
	return HTTOP;
}
