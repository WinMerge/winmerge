//
// FPEnvironment_WIN32.cpp
//
// Library: Foundation
// Package: Core
// Module:  FPEnvironment
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/FPEnvironment_WIN32.h"


namespace Poco {


FPEnvironmentImpl::FPEnvironmentImpl()
{
	 ::_controlfp_s(&_env, 0, 0);
}


FPEnvironmentImpl::FPEnvironmentImpl(const FPEnvironmentImpl& env)
{
	_env = env._env;
}


FPEnvironmentImpl::~FPEnvironmentImpl()
{
	::_controlfp_s(&_env, _env, _MCW_RC);
}


FPEnvironmentImpl& FPEnvironmentImpl::operator = (const FPEnvironmentImpl& env)
{
	_env = env._env;
	return *this;
}


void FPEnvironmentImpl::keepCurrentImpl()
{
	::_controlfp_s(&_env, 0, 0);
}


void FPEnvironmentImpl::clearFlagsImpl()
{
	_clearfp();
}


bool FPEnvironmentImpl::isFlagImpl(FlagImpl flag)
{
	return (_statusfp() & flag) != 0;
}


void FPEnvironmentImpl::setRoundingModeImpl(RoundingModeImpl mode)
{
	unsigned _env;
	::_controlfp_s(&_env, mode, _MCW_RC);
}


FPEnvironmentImpl::RoundingModeImpl FPEnvironmentImpl::getRoundingModeImpl()
{
	unsigned _env;
	return RoundingModeImpl(::_controlfp_s(&_env, 0, 0) & _MCW_RC);
}


} // namespace Poco
