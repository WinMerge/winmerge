// Based on code from https://github.com/microsoft/AppModelSamples/tree/master/Samples/SparsePackages
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
#include <memory>
#include <Shlwapi.h>
#include "../Common/WinMergeContextMenu.h"
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
    WinMergeExplorerCommandBase(WinMergeContextMenu* pContextMenu) : m_pContextMenu(pContextMenu) {}
    virtual const wchar_t* Title() = 0;
    virtual const EXPCMDFLAGS Flags() { return ECF_DEFAULT; }
    virtual const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) { return ECS_ENABLED; }
    virtual const int IconId() { return 0; }
    virtual const int Verb() { return 0; }

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
        const int id = IconId();
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

        if (selection)
            m_pContextMenu->InvokeCommand(Verb());
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
    WinMergeContextMenu* m_pContextMenu;
};

class SubExplorerCommandHandler final : public WinMergeExplorerCommandBase
{
public:
    SubExplorerCommandHandler(WinMergeContextMenu *pContextMenu, const MenuItem& menuItem)
        : WinMergeExplorerCommandBase(pContextMenu)
        , m_menuItem(menuItem)
    {
    }
    const wchar_t* Title() override
    {
        return m_menuItem.text.c_str();
    }
    const int Verb() override
    {
        return m_menuItem.verb;
    }
    const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) override
    {
        return m_menuItem.enabled ? ECS_ENABLED : ECS_DISABLED;
    }
private:
    MenuItem m_menuItem;
};

class EnumCommands : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IEnumExplorerCommand>
{
public:
    explicit EnumCommands(WinMergeContextMenu* pContextMenu)
    {
        for (auto& menuItem : pContextMenu->GetMenuItemList())
            m_commands.push_back(Make<SubExplorerCommandHandler>(pContextMenu, menuItem));
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
        : m_contextMenu(wil::GetModuleInstanceHandle())
        , WinMergeExplorerCommandBase(&m_contextMenu)
    {
    }
    const wchar_t* Title() override { return L"WinMerge"; }
    const int Verb() override { return WinMergeContextMenu::CMD_COMPARE; }
    const EXPCMDFLAGS Flags() override
    {
        if ((m_contextMenu.GetContextMenuEnabled() & (WinMergeContextMenu::EXT_ENABLED | WinMergeContextMenu::EXT_ADVANCED)) == (WinMergeContextMenu::EXT_ENABLED | WinMergeContextMenu::EXT_ADVANCED))
            return ECF_HASSUBCOMMANDS;
        else
            return ECF_DEFAULT;
    }
    const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) override
    {
        std::vector<std::wstring> paths;
        DWORD dwNumItems = 0;
        selection->GetCount(&dwNumItems);
        for (DWORD i = 0; i < dwNumItems; ++i)
        {
            ComPtr<IShellItem> psi;
            wil::unique_cotaskmem_string pszFilePath;
            if (selection &&
                SUCCEEDED(selection->GetItemAt(i, &psi)) &&
                SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
            {
                paths.push_back(pszFilePath.get());
            }
        }
        m_contextMenu.UpdateMenuState(paths);
        if (m_contextMenu.GetMenuState() == WinMergeContextMenu::MENU_HIDDEN)
            return ECS_HIDDEN;
        return ECS_ENABLED;
    }
    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands)
    {
        *enumCommands = nullptr;
        auto e = Make<EnumCommands>(&m_contextMenu);
        return e->QueryInterface(IID_PPV_ARGS(enumCommands));
    }
private:
    WinMergeContextMenu m_contextMenu;
};

class __declspec(uuid("90340779-F37E-468E-9728-A2593498ED32")) WinMergeFileExplorerCommandHandler final : public WinMergeFileDirExplorerCommandHandler
{
public:
    const int IconId() override { return IDI_WINMERGEDIR; }
};

class __declspec(uuid("B982639B-0934-4F73-A63B-2816880CF612")) WinMergeDirExplorerCommandHandler final : public WinMergeFileDirExplorerCommandHandler
{
public:
    const int IconId() override { return IDI_WINMERGE; }
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