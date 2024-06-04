// Copyright (c) 2023 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
#pragma once

#include <cstdint>

class CEColor
{
public:
	CEColor() : m_value(0) {}
	CEColor(uint32_t value) : m_value(value) {}
	CEColor(uint8_t r, uint8_t g, uint8_t b) : m_value((b << 16) | (g << 8) | r) {}
	inline uint8_t GetBVal() const { return (m_value >> 16 & 0xff); }
	inline uint8_t GetGVal() const { return (m_value >> 8) & 0xff; }
	inline uint8_t GetRVal() const { return (m_value) & 0xff; }
	inline operator uint32_t() const { return m_value; }
	static CEColor GetDarkenColor(CEColor a, float l)
	{
		const uint8_t R = static_cast<uint8_t>(a.GetRVal() * l);
		const uint8_t G = static_cast<uint8_t>(a.GetGVal() * l);
		const uint8_t B = static_cast<uint8_t>(a.GetBVal() * l);
		return { R, G, B };
	}
	static CEColor GetIntermediateColor(CEColor a, CEColor b, float ratio)
	{
		const uint8_t R = static_cast<int8_t>((a.GetRVal() - b.GetRVal()) * ratio) + b.GetRVal();
		const uint8_t G = static_cast<int8_t>((a.GetGVal() - b.GetGVal()) * ratio) + b.GetGVal();
		const uint8_t B = static_cast<int8_t>((a.GetBVal() - b.GetBVal()) * ratio) + b.GetBVal();
		return { R, G, B };
	}
private:
	uint32_t m_value;
};

