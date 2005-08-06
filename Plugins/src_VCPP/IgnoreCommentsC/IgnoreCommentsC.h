/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Feb 23 20:38:00 2005
 */
/* Compiler settings for C:\xtt\WinMerge-2.2.0-src\Plugins\src_VCPP\IgnoreCommentsC\IgnoreCommentsC.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IgnoreCommentsC_h__
#define __IgnoreCommentsC_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IWinMergeScript_FWD_DEFINED__
#define __IWinMergeScript_FWD_DEFINED__
typedef interface IWinMergeScript IWinMergeScript;
#endif 	/* __IWinMergeScript_FWD_DEFINED__ */


#ifndef __WinMergeScript_FWD_DEFINED__
#define __WinMergeScript_FWD_DEFINED__

#ifdef __cplusplus
typedef class WinMergeScript WinMergeScript;
#else
typedef struct WinMergeScript WinMergeScript;
#endif /* __cplusplus */

#endif 	/* __WinMergeScript_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IWinMergeScript_INTERFACE_DEFINED__
#define __IWinMergeScript_INTERFACE_DEFINED__

/* interface IWinMergeScript */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWinMergeScript;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DAFE8196-A8BD-47F1-9D0C-7299D93105E7")
    IWinMergeScript : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PluginEvent( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PluginDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PluginFileFilters( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PluginIsAutomatic( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PrediffBufferW( 
            /* [in] */ BSTR __RPC_FAR *pText,
            /* [in] */ INT __RPC_FAR *pSize,
            /* [in] */ VARIANT_BOOL __RPC_FAR *pbChanged,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbHandled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinMergeScriptVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWinMergeScript __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWinMergeScript __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWinMergeScript __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWinMergeScript __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWinMergeScript __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWinMergeScript __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWinMergeScript __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PluginEvent )( 
            IWinMergeScript __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PluginDescription )( 
            IWinMergeScript __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PluginFileFilters )( 
            IWinMergeScript __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PluginIsAutomatic )( 
            IWinMergeScript __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrediffBufferW )( 
            IWinMergeScript __RPC_FAR * This,
            /* [in] */ BSTR __RPC_FAR *pText,
            /* [in] */ INT __RPC_FAR *pSize,
            /* [in] */ VARIANT_BOOL __RPC_FAR *pbChanged,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbHandled);
        
        END_INTERFACE
    } IWinMergeScriptVtbl;

    interface IWinMergeScript
    {
        CONST_VTBL struct IWinMergeScriptVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinMergeScript_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinMergeScript_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinMergeScript_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinMergeScript_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWinMergeScript_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWinMergeScript_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWinMergeScript_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWinMergeScript_get_PluginEvent(This,pVal)	\
    (This)->lpVtbl -> get_PluginEvent(This,pVal)

#define IWinMergeScript_get_PluginDescription(This,pVal)	\
    (This)->lpVtbl -> get_PluginDescription(This,pVal)

#define IWinMergeScript_get_PluginFileFilters(This,pVal)	\
    (This)->lpVtbl -> get_PluginFileFilters(This,pVal)

#define IWinMergeScript_get_PluginIsAutomatic(This,pVal)	\
    (This)->lpVtbl -> get_PluginIsAutomatic(This,pVal)

#define IWinMergeScript_PrediffBufferW(This,pText,pSize,pbChanged,pbHandled)	\
    (This)->lpVtbl -> PrediffBufferW(This,pText,pSize,pbChanged,pbHandled)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinMergeScript_get_PluginEvent_Proxy( 
    IWinMergeScript __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWinMergeScript_get_PluginEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinMergeScript_get_PluginDescription_Proxy( 
    IWinMergeScript __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWinMergeScript_get_PluginDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinMergeScript_get_PluginFileFilters_Proxy( 
    IWinMergeScript __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWinMergeScript_get_PluginFileFilters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinMergeScript_get_PluginIsAutomatic_Proxy( 
    IWinMergeScript __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWinMergeScript_get_PluginIsAutomatic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinMergeScript_PrediffBufferW_Proxy( 
    IWinMergeScript __RPC_FAR * This,
    /* [in] */ BSTR __RPC_FAR *pText,
    /* [in] */ INT __RPC_FAR *pSize,
    /* [in] */ VARIANT_BOOL __RPC_FAR *pbChanged,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbHandled);


void __RPC_STUB IWinMergeScript_PrediffBufferW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinMergeScript_INTERFACE_DEFINED__ */



#ifndef __IgnoreCommentsCLib_LIBRARY_DEFINED__
#define __IgnoreCommentsCLib_LIBRARY_DEFINED__

/* library IgnoreCommentsCLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IgnoreCommentsCLib;

EXTERN_C const CLSID CLSID_WinMergeScript;

#ifdef __cplusplus

class DECLSPEC_UUID("85FC7BD5-AD05-4D91-A7FC-487AB4722DE8")
WinMergeScript;
#endif
#endif /* __IgnoreCommentsCLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
