#pragma once

#include <windows.h>
#include <functional>
#include <vector>
#include "UnicodeString.h"

class DropHandler : public IDropTarget
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave(void);
	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	explicit DropHandler(std::function<void(const std::vector<String>&)> callback);
	~DropHandler();

	std::function<void(const std::vector<String>&)> GetCallback() const { return m_callback; };

private:
	LONG m_cRef;
	std::function<void(const std::vector<String>&)> m_callback;
};
