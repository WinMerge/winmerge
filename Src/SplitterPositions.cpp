#include "pch.h"
#include "SplitterPositions.h"
#include "MergeApp.h"
#include "OptionsMgr.h"
#include <map>

static std::map<String, String> ParseSettingString(const String& name)
{
	const String settingValue = GetOptionsMgr()->GetString(name);
	std::map<String, String> map;
	for (const auto& entry : strutils::split(settingValue, ';'))
	{
		auto keyValue = strutils::split(entry, ':');
		if (keyValue.size() > 1)
			map.insert_or_assign(String(keyValue[0].data(), keyValue[0].size()), String(keyValue[1].data(), keyValue[1].size()));
	}
	return map;
}

static void SaveSettingString(const String& name, const std::map<String, String>& map)
{
	String result;
	for (const auto& [key, value] : map)
	{
		if (!result.empty())
			result += _T(';');
		result += key + _T(":") + value;
	}
	GetOptionsMgr()->SaveOption(name, result);
}

static std::vector<double> ParseSplitterPosition(const String& name, int index, int nPanes)
{
	std::map<String, String> map = ParseSettingString(name);
	const String key = strutils::format(_T("p%d%d"), nPanes, index);
	const String values = map[key];
	if (!values.empty())
	{
		auto parts = strutils::split(values, ',');
		const int expected = nPanes - 1;
		if (static_cast<int>(parts.size()) != expected)
			return {};
		std::vector<double> positions;
		positions.reserve(expected);
		for (const auto& part : parts)
		{
			const double v = tc::tcstod(String(part.data(), part.size()).c_str(), nullptr);
			if (v <= 0.0 || v >= 1.0)
				return {};
			positions.push_back(v);
		}
		return positions;
	}
	return {};
}

static String FormatSplitterPositions(const std::vector<double>& positions)
{
	String result;
	for (size_t i = 0; i < positions.size(); ++i)
	{
		if (i > 0)
			result += _T(',');
		result += strutils::format(_T("%.6g"), positions[i]);
	}
	return result;
}

namespace SplitterPositions
{
	void Load(const String& name, int nRow, int nPanes,
		std::function<void(const double*, int)> func)
	{
		const auto positions = ParseSplitterPosition(name, nRow, nPanes);
		if (!positions.empty())
			func(positions.data(), static_cast<int>(positions.size()));
	}

	void Save(const String& name, int nRow, int nPanes,
		std::function<double(int)> func)
	{
		std::map<String, String> map = ParseSettingString(name);
		const String key = strutils::format(_T("p%d%d"), nPanes, nRow);
		std::vector<double> positions;
		positions.reserve(nPanes - 1);
		for (int j = 0; j < nPanes - 1; ++j)
			positions.push_back(func(j));
		const String value = FormatSplitterPositions(positions);
		map.insert_or_assign(key, value);
		SaveSettingString(name, map);
	}

	double LoadRowPosition(const String& name)
	{
		std::map<String, String> map = ParseSettingString(name);
		String value = map[_T("row")];
		if (value.empty())
			return 0.5;
		return tc::tcstod(value.c_str(), nullptr);
	}

	void SaveRowPosition(const String& name, const double* ratio)
	{
		std::map<String, String> map = ParseSettingString(name);
		if (ratio)
		{
			map.insert_or_assign(_T("row"), strutils::format(_T("%.6g"), *ratio));
		}
		map.insert_or_assign(_T("rowCount"), ratio ? _T("2") : _T("1"));
		SaveSettingString(name, map);
	}

	int GetRowCount(const String& name)
	{
		std::map<String, String> map = ParseSettingString(name);
		String rowCount = map[_T("rowCount")];
		if (rowCount.empty())
			return 1;
		return tc::tcstol(rowCount.c_str(), nullptr, 10);
	}
}
