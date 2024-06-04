//
// SharedMemoryImpl.cpp
//
// Library: Foundation
// Package: Processes
// Module:  SharedMemoryImpl
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/SharedMemory_WIN32.h"
#include "Poco/Error.h"
#include "Poco/Exception.h"
#include "Poco/File.h"
#include "Poco/Format.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/UnWindows.h"


namespace Poco {


SharedMemoryImpl::SharedMemoryImpl(const std::string& name, std::size_t size, SharedMemory::AccessMode mode, const void*, bool):
	_name(name),
	_memHandle(INVALID_HANDLE_VALUE),
	_fileHandle(INVALID_HANDLE_VALUE),
	_size(size),
	_mode(PAGE_READONLY),
	_address(0)
{
	LARGE_INTEGER mySize;
	mySize.QuadPart = _size;
	if (mode == SharedMemory::AM_WRITE)
		_mode = PAGE_READWRITE;

	const Poco::File tmpFile(_name);
	_memHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, _mode, mySize.HighPart, mySize.LowPart, tmpFile.wpath().c_str());

	if (!_memHandle)
	{
		DWORD dwRetVal = GetLastError();
		int retVal = static_cast<int>(dwRetVal);

		if (_mode != PAGE_READONLY || dwRetVal != 5)
		{
			throw SystemException(Poco::format("Cannot create shared memory object %s [Error %d: %s]",
				_name, retVal, Error::getMessage(dwRetVal)), retVal);
		}

		_memHandle = OpenFileMappingW(PAGE_READONLY, FALSE, tmpFile.wpath().c_str());
		if (!_memHandle)
		{
			dwRetVal = GetLastError();
			throw SystemException(Poco::format("Cannot open shared memory object %s [Error %d: %s]",
				_name, retVal, Error::getMessage(dwRetVal)), retVal);
		}
	}
	map();
}


SharedMemoryImpl::SharedMemoryImpl(const Poco::File& file, SharedMemory::AccessMode mode, const void*):
	_name(file.path()),
	_memHandle(INVALID_HANDLE_VALUE),
	_fileHandle(INVALID_HANDLE_VALUE),
	_size(0),
	_mode(PAGE_READONLY),
	_address(0)
{
	DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD fileMode  = GENERIC_READ;

	if (mode == SharedMemory::AM_WRITE)
	{
		_mode = PAGE_READWRITE;
		fileMode |= GENERIC_WRITE;
	}

	_fileHandle = CreateFileW(file.wpath().c_str(), fileMode, shareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (_fileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
			throw FileNotFoundException(_name);
		throw OpenFileException("Cannot open memory mapped file", _name);
	}

	LARGE_INTEGER liFileSize = {0};
	GetFileSizeEx(_fileHandle, &liFileSize);
	if (_size == 0 || _size > liFileSize.QuadPart)
		_size = liFileSize.QuadPart;
	else
		liFileSize.QuadPart = _size;
	_memHandle = CreateFileMapping(_fileHandle, NULL, _mode, liFileSize.HighPart, liFileSize.LowPart, NULL);
	if (!_memHandle)
	{
		DWORD dwRetVal = GetLastError();
		CloseHandle(_fileHandle);
		_fileHandle = INVALID_HANDLE_VALUE;
		throw SystemException(format("Cannot map file into shared memory %s [Error %d: %s]", _name, (int)dwRetVal, Error::getMessage(dwRetVal)));
	}
	map();
}


SharedMemoryImpl::~SharedMemoryImpl()
{
	unmap();
	close();
}

namespace
{
#pragma optimize("", off)
	bool canRead(void *p)
	{
		__try
		{
			char a = *reinterpret_cast<const char *>(p);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
		return true;
	}
#pragma optimize("", on)
}


void SharedMemoryImpl::map()
{
	DWORD access = FILE_MAP_READ;
	if (_mode == PAGE_READWRITE)
		access = FILE_MAP_WRITE;
	LPVOID addr = MapViewOfFile(_memHandle, access, 0, 0, static_cast<SIZE_T>(_size));
	if (!addr)
	{
		DWORD dwRetVal = GetLastError();
		throw SystemException(format("Cannot map shared memory object %s [Error %d: %s]", _name, (int)dwRetVal, Error::getMessage(dwRetVal)));
	}
	if (_size >= 1 && !canRead(addr))
	{
		UnmapViewOfFile(addr);
		throw SystemException("Cannot map shared memory object", _name);
	}
	_address = static_cast<char*>(addr);
}


void SharedMemoryImpl::unmap()
{
	if (_address)
	{
		UnmapViewOfFile(_address);
		_address = 0;
		return;
	}
}


void SharedMemoryImpl::close()
{
	if (_memHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_memHandle);
		_memHandle = INVALID_HANDLE_VALUE;
	}

	if (_fileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_fileHandle);
		_fileHandle = INVALID_HANDLE_VALUE;
	}
}


} // namespace Poco
