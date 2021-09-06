// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <shobjidl_core.h>
#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <string>
#include <vector>
#include <sstream>
#include <Shlwapi.h>
#include "../Common/RegKey.h"
#include "../Common/Constants.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

using namespace Microsoft::WRL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

class WinMergeExplorerCommandBase : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand, IObjectWithSite>
{
public:
    virtual const wchar_t* Title() = 0;
    virtual const EXPCMDFLAGS Flags() { return ECF_DEFAULT; }
    virtual const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) { return ECS_ENABLED; }
    virtual int IconId() { return 0; }

    // IExplorerCommand
    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* name)
    {
        *name = nullptr;
        auto title = wil::make_cotaskmem_string_nothrow(Title());
        RETURN_IF_NULL_ALLOC(title);
        *name = title.release();
        return S_OK;
    }
    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon)
    {
        int id = IconId();
        if (id == 0)
        {
            *icon = nullptr;
            return E_NOTIMPL;
        }
        auto modulefilename = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());
        std::wstring path = modulefilename.get() + std::wstring(L",-") + std::to_wstring(id);
        SHStrDupW(path.c_str(), icon);
        return S_OK;
    }
    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        *cmdState = State(selection);
        return S_OK;
    }
    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept try
    {
        HWND parent = nullptr;
        if (m_site)
        {
            ComPtr<IOleWindow> oleWindow;
            RETURN_IF_FAILED(m_site.As(&oleWindow));
            RETURN_IF_FAILED(oleWindow->GetWindow(&parent));
        }

        std::wostringstream title;
        title << Title();

        if (selection)
        {
            DWORD count;
            RETURN_IF_FAILED(selection->GetCount(&count));
            title << L" (" << count << L" selected items)";
        }
        else
        {
            title << L"(no selected items)";
        }

        MessageBox(parent, title.str().c_str(), L"TestCommand", MB_OK);
        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags) { *flags = Flags(); return S_OK; }
    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands) { *enumCommands = nullptr; return E_NOTIMPL; }

    // IObjectWithSite
    IFACEMETHODIMP SetSite(_In_ IUnknown* site) noexcept { m_site = site; return S_OK; }
    IFACEMETHODIMP GetSite(_In_ REFIID riid, _COM_Outptr_ void** site) noexcept { return m_site.CopyTo(riid, site); }

protected:
    ComPtr<IUnknown> m_site;
};

class SubExplorerCommandHandler final : public WinMergeExplorerCommandBase
{
public:
    const wchar_t* Title() override { return L"SubCommand"; }
};

class EnumCommands : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IEnumExplorerCommand>
{
public:
    EnumCommands()
    {
        m_commands.push_back(Make<SubExplorerCommandHandler>());
        m_current = m_commands.cbegin();
    }

    // IEnumExplorerCommand
    IFACEMETHODIMP Next(ULONG celt, __out_ecount_part(celt, *pceltFetched) IExplorerCommand** apUICommand, __out_opt ULONG* pceltFetched)
    {
        ULONG fetched{ 0 };
        wil::assign_to_opt_param(pceltFetched, 0ul);

        for (ULONG i = 0; (i < celt) && (m_current != m_commands.cend()); i++)
        {
            m_current->CopyTo(&apUICommand[0]);
            m_current++;
            fetched++;
        }

        wil::assign_to_opt_param(pceltFetched, fetched);
        return (fetched == celt) ? S_OK : S_FALSE;
    }

    IFACEMETHODIMP Skip(ULONG /*celt*/) { return E_NOTIMPL; }
    IFACEMETHODIMP Reset()
    {
        m_current = m_commands.cbegin();
        return S_OK;
    }
    IFACEMETHODIMP Clone(__deref_out IEnumExplorerCommand** ppenum) { *ppenum = nullptr; return E_NOTIMPL; }

private:
    std::vector<ComPtr<IExplorerCommand>> m_commands;
    std::vector<ComPtr<IExplorerCommand>>::const_iterator m_current;
};

class WinMergeFileDirExplorerCommandHandler : public WinMergeExplorerCommandBase
{
public:
    WinMergeFileDirExplorerCommandHandler()
    : m_dwContextMenuEnabled(0)
    {
        CRegKeyEx reg;
        if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
            m_dwContextMenuEnabled = reg.ReadDword(f_RegValueEnabled, 0);
    }
    const wchar_t* Title() override { return L"WinMerge"; }
    const EXPCMDFLAGS Flags() override
    {
        if (m_dwContextMenuEnabled & EXT_ENABLED && m_dwContextMenuEnabled & EXT_ADVANCED)
            return ECF_HASSUBCOMMANDS;
        else
            return ECF_DEFAULT;
    }
    const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) override
    {
        DWORD dwNumItems = 0;
        if (!(m_dwContextMenuEnabled & EXT_ENABLED))
            return ECS_HIDDEN;
        else if (selection && SUCCEEDED(selection->GetCount(&dwNumItems)) && dwNumItems > MaxFileCount)
            return ECS_DISABLED;
        return ECS_ENABLED;
    }
    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands)
    {
        *enumCommands = nullptr;
        auto e = Make<EnumCommands>();
        return e->QueryInterface(IID_PPV_ARGS(enumCommands));
    }
private:
    DWORD m_dwContextMenuEnabled;
};

class __declspec(uuid("90340779-F37E-468E-9728-A2593498ED32")) WinMergeFileExplorerCommandHandler final : public WinMergeFileDirExplorerCommandHandler
{
public:
    int IconId() override { return IDI_WINMERGEDIR; }
};

class __declspec(uuid("B982639B-0934-4F73-A63B-2816880CF612")) WinMergeDirExplorerCommandHandler final : public WinMergeFileDirExplorerCommandHandler
{
public:
    int IconId() override { return IDI_WINMERGE; }
};

CoCreatableClass(WinMergeFileExplorerCommandHandler)
CoCreatableClass(WinMergeDirExplorerCommandHandler)

CoCreatableClassWrlCreatorMapInclude(WinMergeFileExplorerCommandHandler)
CoCreatableClassWrlCreatorMapInclude(WinMergeDirExplorerCommandHandler)


STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}