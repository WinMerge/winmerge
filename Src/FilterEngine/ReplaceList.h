/**
 * @file ReplaceList.h
 * @brief Declaration file for ReplaceList functions
 */
#pragma once

#include <memory>
#include <vector>
#include "FilterExpressionNodes.h"
#include "UnicodeString.h"

namespace ReplaceList
{
	std::shared_ptr<std::vector<ValueType2>> LoadList(const FilterExpression& ctxt, const String& path);
	std::shared_ptr<std::vector<ValueType2>> LoadRegexList(const FilterExpression& ctxt, const String& path);
}