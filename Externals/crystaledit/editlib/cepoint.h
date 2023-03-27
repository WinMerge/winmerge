// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#pragma once

struct CEPoint
{
	CEPoint() : x(0), y(0) {}
	CEPoint(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

inline bool operator==(const CEPoint& p1, const CEPoint& p2) { return p1.x == p2.x && p1.y == p2.y; }
inline bool operator!=(const CEPoint& p1, const CEPoint& p2) { return !(p1 == p2); }

