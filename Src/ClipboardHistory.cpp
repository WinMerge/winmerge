#include "StdAfx.h"
#include "ClipboardHistory.h"
#include "ClipBoard.h"
#include "Concurrent.h"
#include "UniFile.h"

#ifdef _WIN64

#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.applicationmodel.datatransfer.h>
#include <winrt/windows.graphics.imaging.h>
#include <winrt/windows.storage.streams.h>

using namespace winrt::Windows::ApplicationModel::DataTransfer;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Storage;

#endif

namespace ClipboardHistory
{
	namespace impl
	{
		std::shared_ptr<TempFile> CreateTempTextFile(const String& text)
		{
			std::shared_ptr<TempFile> pTempFile(new TempFile());
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

#ifdef _WIN64
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
#ifdef _WIN64
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
							item.pTextTempFile = CreateTempTextFile(_T("Windows Clipboard History is disabled or empty."));
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
					{
						String text;
						GetFromClipboard(text, nullptr);
						time(&item.timestamp);
						item.pTextTempFile = CreateTempTextFile(text);
					}
					else
					{
						item.pTextTempFile = CreateTempTextFile(_T("Windows Clipboard History is disabled or empty."));
					}
				}
			}
#else
			for (unsigned int i = 0; i < num; ++i)
			{
				result.emplace_back();
				auto& item = result.back();
				if (i == 0)
				{
					String text;
					GetFromClipboard(text, nullptr);
					time(&item.timestamp);
					item.pTextTempFile = CreateTempTextFile(text);
				}
				else
				{
					item.pTextTempFile = CreateTempTextFile(_T("WinMerge 32bit version cannot access Windows Clipboard History"));
				}
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

/*
#include <windows.applicationmodel.datatransfer.h>
#include "WinRTUtils.h"
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::ApplicationModel::DataTransfer;

HRESULT GetClipboardHistoryItems(__FIVectorView_1_Windows__CApplicationModel__CDataTransfer__CClipboardHistoryItem** ppItems)
{
	if (!WinRTUtils::RoGetActivationFactory)
		return E_NOTIMPL;
	ComPtr<IClipboardStatics2> pClipboardStatics2;
	HRESULT hr = WinRTUtils::RoGetActivationFactory(
		HStringReference(RuntimeClass_Windows_ApplicationModel_DataTransfer_Clipboard).Get(),
		IID_PPV_ARGS(&pClipboardStatics2));
	if (FAILED(hr))
		return hr;
	ComPtr<IAsyncOperation<ClipboardHistoryItemsResult *>> pAsync;
	hr = pClipboardStatics2->GetHistoryItemsAsync(&pAsync);
	if (FAILED(hr))
		return hr;
	ComPtr<IClipboardHistoryItemsResult> pClipboardHistoryItemsResult;
	WinRTUtils::await(pAsync.Get(),
		static_cast<IClipboardHistoryItemsResult**>(&pClipboardHistoryItemsResult));
	return pClipboardHistoryItemsResult->get_Items(ppItems);
}

		WinRTUtils::load();
		ComPtr<__FIVectorView_1_Windows__CApplicationModel__CDataTransfer__CClipboardHistoryItem> pItems;
		if (SUCCEEDED(GetClipboardHistoryItems(&pItems)))
		{
			unsigned int size = 0;
			pItems->get_Size(&size);
			for (unsigned int i = 0; i < size; ++i)
			{
				ComPtr<IClipboardHistoryItem> pItem;
				ComPtr<IDataPackageView> pDataPackageView;
				ComPtr<IAsyncOperation<HSTRING>> pAsyncGetTextResult;
				HString text;

				pItems->GetAt(i, &pItem);
				pItem->get_Content(&pDataPackageView);
	//			pDataPackageView->get_AvailableFormats()
				pDataPackageView->GetTextAsync(&pAsyncGetTextResult);
				WinRTUtils::await(pAsyncGetTextResult.Get(), text.GetAddressOf());
				int a = 0;
			}
		}
		WinRTUtils::unload();
		*/

