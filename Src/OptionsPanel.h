/** 
 * @file  OptionsPanel.h
 *
 * @brief Declaration file for OptionsPanel class.
 *
 */
#pragma once

#include "IOptionsPanel.h"
#include "TrDialogs.h"
#include <vector>
#include <functional>
#include <afxwin.h>

class COptionsMgr;

/**
 * @brief A base class for WinMerge options dialogs.
 */
class OptionsPanel : public CTrPropertyPage, public IOptionsPanel
{
public:
	OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate);

	/** 
	 * @brief Reads options values from storage to UI.
	 */
	void ReadOptions() override
	{
		ReadOptionBindings();
	}

	/** 
	 * @brief Writes options values from UI to storage.
	 */
	void WriteOptions() override
	{
		WriteOptionBindings();
	}

	/**
	 * @brief Sets options to defaults
	 */
	virtual void OnDefaults()
	{
		ResetOptionBindings();
		UpdateData(FALSE);
	}

protected:
	COptionsMgr* GetOptionsMgr() const { return m_pOptionsMgr; }

	// -------------------------------
	// Option binding support
	// -------------------------------

	struct OptionBinding
	{
		UINT nCtrlID;
		String optionID;
		void* pVar;
		void (__stdcall * pDDX)(CDataExchange*, int, void*);
		void (*ddxFunc)(CDataExchange*, OptionBinding&);
		void (*readFunc)(OptionBinding&, COptionsMgr*);
		void (*writeFunc)(OptionBinding&, COptionsMgr*);
		void (*resetFunc)(OptionBinding&, COptionsMgr*);
		std::unique_ptr<void, void(*)(void*)> readConv{ nullptr, +[](void* p) {} };
		std::unique_ptr<void, void(*)(void*)> writeConv{ nullptr, +[](void* p) {} };
	};

	std::vector<OptionBinding> m_bindings;

	template<typename T>
	static void DDX_Helper(CDataExchange* pDX, OptionBinding& b)
	{ reinterpret_cast<void(__stdcall *)(CDataExchange*, int, T&)>(b.pDDX)(pDX, b.nCtrlID, *reinterpret_cast<T*>(b.pVar)); }
	template<typename T>
	static void Read_Helper(OptionBinding& b, COptionsMgr* mgr) { *reinterpret_cast<T*>(b.pVar) = mgr->GetT<T>(b.optionID); }
	template<typename T>
	static void Write_Helper(OptionBinding& b, COptionsMgr* mgr) { mgr->SaveOption(b.optionID, *reinterpret_cast<T*>(b.pVar)); }
	template<typename T>
	static void Reset_Helper(OptionBinding& b, COptionsMgr* mgr) { *reinterpret_cast<T*>(b.pVar) = mgr->GetDefault<T>(b.optionID); }
	template<typename TVar, typename TOpt = TVar, typename ReadConv>
	static void ReadConv_Helper(OptionBinding& b, COptionsMgr* mgr)
	{
		auto conv = reinterpret_cast<ReadConv*>(b.readConv.get());
		*reinterpret_cast<TVar*>(b.pVar) = (*conv)(mgr->GetT<TOpt>(b.optionID));
	}
	template<typename TVar, typename TOpt = TVar, typename WriteConv>
	static void WriteConv_Helper(OptionBinding& b, COptionsMgr* mgr)
	{
		auto conv = reinterpret_cast<WriteConv*>(b.writeConv.get());
		mgr->SaveOption(b.optionID, (*conv)(*reinterpret_cast<TVar*>(b.pVar)));
	}
	template<typename TVar, typename TOpt = TVar, typename ReadConv>
	static void ResetConv_Helper(OptionBinding& b, COptionsMgr* mgr)
	{
		auto conv = reinterpret_cast<ReadConv*>(b.readConv.get());
		*reinterpret_cast<TVar*>(b.pVar) = (*conv)(mgr->GetDefault<TOpt>(b.optionID));
	}

	/**
	 * @brief Bind option with control + DDX function
	 */
	template<typename TVar>
	void BindOption(const String& optionID, TVar& var, UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&))
	{
		OptionBinding b{};
		b.nCtrlID = nCtrlID;
		b.optionID = optionID;
		b.pVar = &var;
		b.pDDX = reinterpret_cast<void(__stdcall*)(CDataExchange*, int, void*)>(ddxFunc);
		b.ddxFunc = &DDX_Helper<TVar>;
		b.readFunc = &Read_Helper<TVar>;
		b.writeFunc = &Write_Helper<TVar>;
		b.resetFunc = &Reset_Helper<TVar>;
		m_bindings.push_back(std::move(b));
	}

	template<typename TVar, typename TOpt = TVar, typename ReadConv, typename WriteConv>
	void BindOptionCustom(const String& optionID, TVar& var, UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&),
		ReadConv readconv, WriteConv writeconv)
	{
		OptionBinding b{};
		b.nCtrlID = nCtrlID;
		b.optionID = optionID;
		b.pVar = &var;
		b.pDDX = reinterpret_cast<void(__stdcall*)(CDataExchange*, int, void*)>(ddxFunc);
		b.ddxFunc = &DDX_Helper<TVar>;
		b.readFunc = &ReadConv_Helper<TVar, TOpt, ReadConv>;
		b.writeFunc = &WriteConv_Helper<TVar, TOpt, WriteConv>;
		b.resetFunc = &ResetConv_Helper<TVar, TOpt, ReadConv>;
		b.readConv = { new ReadConv(std::move(readconv)), [](void* p) { delete static_cast<ReadConv*>(p); } };
		b.writeConv = { new WriteConv(std::move(writeconv)), [](void* p) { delete static_cast<WriteConv*>(p); } };
		m_bindings.push_back(std::move(b));
	}

	/** @brief Call in DoDataExchange */
	void DoDataExchangeBindOptions(CDataExchange* pDX)
	{
		for (auto& b : m_bindings)
			b.ddxFunc(pDX, b);
	}

	/** @brief Read values from OptionsMgr to variables */
	void ReadOptionBindings()
	{
		for (auto& b : m_bindings)
			b.readFunc(b, m_pOptionsMgr);
	}

	/** @brief Write values from variables to OptionsMgr */
	void WriteOptionBindings()
	{
		for (auto& b : m_bindings)
			b.writeFunc(b, m_pOptionsMgr);
	}

	/** @brief Reset options to default values */
	void ResetOptionBindings()
	{
		for (auto& b : m_bindings)
			b.resetFunc(b, m_pOptionsMgr);
	}

	/** @brief Sets update handlers for dialog controls.*/
	void DoDataExchange(CDataExchange* pDX) override
	{
		CPropertyPage::DoDataExchange(pDX);
		DoDataExchangeBindOptions(pDX);
	}

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};
