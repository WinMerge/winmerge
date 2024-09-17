

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for MergeAppLib.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __MergeAppLib_h__
#define __MergeAppLib_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IMergeApp_FWD_DEFINED__
#define __IMergeApp_FWD_DEFINED__
typedef interface IMergeApp IMergeApp;

#endif 	/* __IMergeApp_FWD_DEFINED__ */


#ifndef __IMergeApp_FWD_DEFINED__
#define __IMergeApp_FWD_DEFINED__
typedef interface IMergeApp IMergeApp;

#endif 	/* __IMergeApp_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMergeApp_INTERFACE_DEFINED__
#define __IMergeApp_INTERFACE_DEFINED__

/* interface IMergeApp */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMergeApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a719a330-c39a-4d8b-bf5a-8e52f4dd8da3")
    IMergeApp : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Translate( 
            /* [in] */ BSTR text,
            /* [retval][out] */ BSTR *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOption( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT varDefault,
            /* [retval][out] */ VARIANT *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveOption( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT varValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MsgBox( 
            /* [in] */ BSTR prompt,
            /* [optional][in] */ VARIANT varButtons,
            /* [optional][in] */ VARIANT varTitle,
            /* [retval][out] */ int *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InputBox( 
            /* [in] */ BSTR prompt,
            /* [optional][in] */ VARIANT varTitle,
            /* [optional][in] */ VARIANT varDefault,
            /* [retval][out] */ BSTR *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ BSTR text) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMergeAppVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMergeApp * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMergeApp * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMergeApp * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMergeApp * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMergeApp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMergeApp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMergeApp * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(IMergeApp, Translate)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Translate )( 
            IMergeApp * This,
            /* [in] */ BSTR text,
            /* [retval][out] */ BSTR *pRet);
        
        DECLSPEC_XFGVIRT(IMergeApp, GetOption)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOption )( 
            IMergeApp * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT varDefault,
            /* [retval][out] */ VARIANT *pRet);
        
        DECLSPEC_XFGVIRT(IMergeApp, SaveOption)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveOption )( 
            IMergeApp * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT varValue);
        
        DECLSPEC_XFGVIRT(IMergeApp, MsgBox)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MsgBox )( 
            IMergeApp * This,
            /* [in] */ BSTR prompt,
            /* [optional][in] */ VARIANT varButtons,
            /* [optional][in] */ VARIANT varTitle,
            /* [retval][out] */ int *pRet);
        
        DECLSPEC_XFGVIRT(IMergeApp, InputBox)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InputBox )( 
            IMergeApp * This,
            /* [in] */ BSTR prompt,
            /* [optional][in] */ VARIANT varTitle,
            /* [optional][in] */ VARIANT varDefault,
            /* [retval][out] */ BSTR *pRet);
        
        DECLSPEC_XFGVIRT(IMergeApp, LogError)
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogError )( 
            IMergeApp * This,
            /* [in] */ BSTR text);
        
        END_INTERFACE
    } IMergeAppVtbl;

    interface IMergeApp
    {
        CONST_VTBL struct IMergeAppVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMergeApp_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMergeApp_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMergeApp_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMergeApp_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMergeApp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMergeApp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMergeApp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMergeApp_Translate(This,text,pRet)	\
    ( (This)->lpVtbl -> Translate(This,text,pRet) ) 

#define IMergeApp_GetOption(This,name,varDefault,pRet)	\
    ( (This)->lpVtbl -> GetOption(This,name,varDefault,pRet) ) 

#define IMergeApp_SaveOption(This,name,varValue)	\
    ( (This)->lpVtbl -> SaveOption(This,name,varValue) ) 

#define IMergeApp_MsgBox(This,prompt,varButtons,varTitle,pRet)	\
    ( (This)->lpVtbl -> MsgBox(This,prompt,varButtons,varTitle,pRet) ) 

#define IMergeApp_InputBox(This,prompt,varTitle,varDefault,pRet)	\
    ( (This)->lpVtbl -> InputBox(This,prompt,varTitle,varDefault,pRet) ) 

#define IMergeApp_LogError(This,text)	\
    ( (This)->lpVtbl -> LogError(This,text) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMergeApp_INTERFACE_DEFINED__ */



#ifndef __MergeAppLib_LIBRARY_DEFINED__
#define __MergeAppLib_LIBRARY_DEFINED__

/* library MergeAppLib */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_MergeAppLib;
#endif /* __MergeAppLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


