/** 
 * @file  ClipboardHistory.cpp
 *
 * @brief Implementation for Clipboard history functions
 */

#include "StdAfx.h"
#include "ClipboardHistory.h"
#include "ClipBoard.h"
#include "Concurrent.h"
#include "UniFile.h"

#if __has_include(<winrt/windows.applicationmodel.datatransfer.h>) && _MSC_VER >= 1930

#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.applicationmodel.datatransfer.h>
#include <winrt/windows.graphics.imaging.h>
#include <winrt/windows.storage.streams.h>

#if !__has_include(<winrt/windows.applicationmodel.holographic.h>)
#pragma comment(lib, "windowsapp.lib") // < Windows SDK 10.0.22000.0
#endif

using namespace winrt::Windows::ApplicationModel::DataTransfer;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Storage;

#define USE_WINRT

#endif

namespace ClipboardHistory
{
	namespace impl
	{
		std::shared_ptr<TempFile> CreateTempTextFile(const String& text)
		{
			auto pTempFile = std::make_shared<TempFile>(TempFile());
			pTempFile->Create(_T("CLP"), L".txt");
			String path = pTempFile->GetPath();
			UniStdioFile file;
			if (file.OpenCreateUtf8(path))
			{
				file.WriteString(text);
				file.Close();
			}
			return pTempFile;
		}

		String GetClipboardText()
		{
			String text;
			GetFromClipboard(text);
			return text;
		}

#ifdef USE_WINRT
		std::shared_ptr<TempFile> CreateTempBitmapFile(const DataPackageView& dataPackageView)
		{
			std::shared_ptr<TempFile> pTempFile(new TempFile());
			pTempFile->Create(_T("CLP"), _T(".png"));

			auto streamReference = dataPackageView.GetBitmapAsync().get();
			auto inputStream = streamReference.OpenReadAsync().get();
			auto decoder = BitmapDecoder::CreateAsync(inputStream).get();
			auto bitmap = decoder.GetSoftwareBitmapAsync().get();

			auto outputFile = StorageFile::GetFileFromPathAsync(pTempFile->GetPath()).get();
			auto outputStream = outputFile.OpenAsync(FileAccessMode::ReadWrite).get();
			auto encoder = BitmapEncoder::CreateAsync(BitmapEncoder::PngEncoderId(), outputStream).get();
			encoder.SetSoftwareBitmap(bitmap);
			encoder.FlushAsync().get();

			return pTempFile;
		}
#endif

		std::vector<Item> GetItems(unsigned num)
		{
			std::vector<Item> result;
#ifdef USE_WINRT
			try
			{
				auto historyItems = Clipboard::GetHistoryItemsAsync().get();
				auto items = historyItems.Items();
				for (unsigned int i = 0; i < num; ++i)
				{
					result.emplace_back();
					auto& item = result.back();
					if (i < items.Size())
					{
						try
						{
							auto dataPackageView = items.GetAt(i).Content();
							item.timestamp = winrt::clock::to_time_t(items.GetAt(i).Timestamp());
							if (dataPackageView.Contains(StandardDataFormats::Text()))
							{
								item.pTextTempFile = CreateTempTextFile(dataPackageView.GetTextAsync().get().c_str());
							}
							if (dataPackageView.Contains(StandardDataFormats::Bitmap()))
							{
								item.pBitmapTempFile = CreateTempBitmapFile(dataPackageView);
							}
							if (!item.pTextTempFile && !item.pBitmapTempFile)
							{
								item.pTextTempFile = CreateTempTextFile(_T(""));
							}
						}
						catch (const winrt::hresult_error& e)
						{
							item.pTextTempFile = CreateTempTextFile(e.message().c_str());
						}
					}
					else
					{
						if (i == 0)
							time(&item.timestamp);
						item.pTextTempFile = CreateTempTextFile(i == 0 ?
								GetClipboardText() :
								(!Clipboard::IsHistoryEnabled() ? _("Clipboard history is disabled.\r\nTo enable clipboard history, press Windows logo key + V and then click the Turn on button.") : _T("")));
					}
				}
			}
			catch (const winrt::hresult_error&)
			{
				for (unsigned int i = 0; i < num; ++i)
				{
					result.emplace_back();
					auto& item = result.back();
					if (i == 0)
						time(&item.timestamp);
					item.pTextTempFile = CreateTempTextFile(
						i == 0 ? GetClipboardText() : _("This system does not support clipboard history."));
				}
			}
#else
			for (unsigned int i = 0; i < num; ++i)
			{
				result.emplace_back();
				auto& item = result.back();
				if (i == 0)
					time(&item.timestamp);
				item.pTextTempFile = CreateTempTextFile(
					i == 0 ? GetClipboardText() : _("The 32-bit version of WinMerge does not support Clipboard Compare"));
			}
#endif
			return result;
		}
	}

	std::vector<Item> GetItems(unsigned num)
	{
		auto task = Concurrent::CreateTask([num] {
			return impl::GetItems(num);
		});
		return task.Get();
	}
}
