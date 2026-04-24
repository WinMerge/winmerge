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
#include "locality.h"
#include "unicoder.h"

#if __has_include(<winrt/windows.applicationmodel.datatransfer.h>) && _MSC_VER >= 1930

#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.applicationmodel.datatransfer.h>
#include <winrt/windows.graphics.imaging.h>
#include <winrt/windows.storage.streams.h>

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

		String FormatDescription(time_t timestamp)
		{
			int64_t t = timestamp;
			String timestr = t == 0 ? _T("---") : locality::TimeString(&t);
			return strutils::format(_("Clipboard at %s"), timestr);
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

		std::vector<Item> GetItems(unsigned maxCount, unsigned ensureMinCount)
		{
			if (ensureMinCount > maxCount)
				return {};
			std::vector<Item> result;
#ifdef USE_WINRT
			try
			{
				auto historyItems = Clipboard::GetHistoryItemsAsync().get();
				auto items = historyItems.Items();
				for (unsigned int i = 0; i < maxCount; ++i)
				{
					if (i < items.Size())
					{
						result.emplace_back();
						auto& item = result.back();
						try
						{
							auto dataPackageView = items.GetAt(i).Content();
							item.timestamp = winrt::clock::to_time_t(items.GetAt(i).Timestamp());
							item.description = FormatDescription(item.timestamp);

							if (dataPackageView.Contains(StandardDataFormats::Text()))
							{
								String text = dataPackageView.GetTextAsync().get().c_str();
								item.pTextTempFile = CreateTempTextFile(text);

								// Extract first line as preview text
								String line = text;
								size_t newlinePos = line.find_first_of(_T("\r\n"));
								if (newlinePos != String::npos)
									line = line.substr(0, newlinePos);
								strutils::replace_chars(line, _T("\t"), _T(" "));
								if (line.length() > MAX_PATH)
									item.previewText = line.substr(0, MAX_PATH) + _T("...");
								else
									item.previewText = line;
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
							String errorMsg = e.message().c_str();
							item.pTextTempFile = CreateTempTextFile(errorMsg);
							item.previewText = errorMsg;
						}
					}
					else if (i < ensureMinCount)
					{
						result.emplace_back();
						auto& item = result.back();
						if (i == 0)
							time(&item.timestamp);
						item.description = FormatDescription(item.timestamp);

						String text = i == 0 ?
							GetClipboardText() :
							(!Clipboard::IsHistoryEnabled() ? _("Clipboard history disabled.\r\nEnable: Windows logo key + V, then Turn on.") : _T(""));
						item.pTextTempFile = CreateTempTextFile(text);
						item.previewText = text;
					}
				}
			}
			catch (const winrt::hresult_error&)
			{
				for (unsigned int i = 0; i < ensureMinCount; ++i)
				{
					result.emplace_back();
					auto& item = result.back();
					if (i == 0)
						time(&item.timestamp);
					item.description = FormatDescription(item.timestamp);

					String text = i == 0 ? GetClipboardText() : _("This system does not support clipboard history.");
					item.pTextTempFile = CreateTempTextFile(text);
					item.previewText = text;
				}
			}
#else
			for (unsigned int i = 0; i < ensureMinCount; ++i)
			{
				result.emplace_back();
				auto& item = result.back();
				if (i == 0)
					time(&item.timestamp);
				item.description = FormatDescription(item.timestamp);

				String text = i == 0 ? GetClipboardText() : _("32-bit WinMerge does not support Clipboard Compare");
				item.pTextTempFile = CreateTempTextFile(text);
				item.previewText = text;
			}
#endif
			return result;
		}
	}

	std::vector<Item> GetItems(unsigned maxCount, unsigned ensureMinCount)
	{
		auto task = Concurrent::CreateTask([maxCount, ensureMinCount] {
			return impl::GetItems(maxCount, ensureMinCount);
		});
		return task.Get();
	}
}
