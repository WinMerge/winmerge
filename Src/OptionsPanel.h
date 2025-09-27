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

	struct IOptionBinding
	{
		virtual ~IOptionBinding() {}
		virtual void DDX(CDataExchange*) = 0;
		virtual void Read(COptionsMgr*) = 0;
		virtual void Write(COptionsMgr*) = 0;
		virtual void Reset(COptionsMgr*) = 0;
	};

	template <typename TVar>
	class OptionBinding : public IOptionBinding
	{
	public:
		OptionBinding(const String & optionID, TVar* pVar,
			UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&))
			: nCtrlID(nCtrlID), optionID(optionID), pVar(pVar), pDDX(ddxFunc) {
		}
		~OptionBinding() override {}
		void (__stdcall * pDDX)(CDataExchange*, int, TVar&);
		void DDX(CDataExchange* pDX) override { pDDX(pDX, nCtrlID, *pVar); }
		void Read(COptionsMgr* mgr) override { *pVar = mgr->GetT<TVar>(optionID); }
		void Write(COptionsMgr* mgr) override { mgr->SaveOption(optionID, *pVar); }
		void Reset(COptionsMgr* mgr) override { *pVar = mgr->GetDefault<TVar>(optionID); }
	private:
		UINT nCtrlID;
		String optionID;
		TVar* pVar;
	};

	template<typename TVar, typename TOpt>
	class OptionBindingCustom : public IOptionBinding
	{
	public:
		using ReadConvFn = TVar(*)(TOpt);
		using WriteConvFn = TOpt(*)(TVar);

		OptionBindingCustom(const String & optionID, TVar* pVar,
			UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&),
			ReadConvFn Readconv, WriteConvFn Writeconv)
			: nCtrlID(nCtrlID), optionID(optionID), pVar(pVar), pDDX(ddxFunc),
			ReadConv(Readconv), WriteConv(Writeconv) {
		}
		~OptionBindingCustom() override {}
		void (__stdcall * pDDX)(CDataExchange*, int, TVar&);
		void DDX(CDataExchange* pDX) override { pDDX(pDX, nCtrlID, *pVar); }
		void Read(COptionsMgr* mgr) override { *pVar = ReadConv(mgr->GetT<TOpt>(optionID)); }
		void Write(COptionsMgr* mgr) override { mgr->SaveOption(optionID, WriteConv(*pVar)); }
		void Reset(COptionsMgr* mgr) override { *pVar = ReadConv(mgr->GetDefault<TOpt>(optionID)); }
	private:
		UINT nCtrlID;
		String optionID;
		TVar* pVar;
		ReadConvFn  ReadConv;
		WriteConvFn WriteConv;
	};

	std::vector<std::unique_ptr<IOptionBinding>> m_bindings;

	/**
	 * @brief Bind option with control + DDX function
	 */
	template<typename TVar>
	void BindOption(const String& optionID, TVar& var, UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&))
	{
		m_bindings.emplace_back(new OptionBinding<TVar>(optionID, &var, nCtrlID, ddxFunc));
	}

	template<typename TVar, typename TOpt = TVar>
	void BindOptionCustom(const String& optionID, TVar& var, UINT nCtrlID, void(__stdcall *ddxFunc)(CDataExchange*, int, TVar&),
		TVar(*readConv)(TOpt), TOpt(*writeConv)(TVar))
	{
		m_bindings.emplace_back(new OptionBindingCustom<TVar, TOpt>(optionID, &var, nCtrlID, ddxFunc, readConv, writeConv));
	}

	/** @brief Call in DoDataExchange */
	void DoDataExchangeBindOptions(CDataExchange* pDX)
	{
		for (auto& b : m_bindings)
			b->DDX(pDX);
	}

	/** @brief Read values from OptionsMgr to variables */
	void ReadOptionBindings()
	{
		for (auto& b : m_bindings)
			b->Read(m_pOptionsMgr);
	}

	/** @brief Write values from variables to OptionsMgr */
	void WriteOptionBindings()
	{
		for (auto& b : m_bindings)
			b->Write(m_pOptionsMgr);
	}

	/** @brief Reset options to default values */
	void ResetOptionBindings()
	{
		for (auto& b : m_bindings)
			b->Reset(m_pOptionsMgr);
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
