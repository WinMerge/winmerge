#if !defined(AFX_WCDEFINES_H__E2EFA8F0_B9CD_41AB_98FD_812C963B7ACC__INCLUDED_)
#define AFX_WCDEFINES_H__E2EFA8F0_B9CD_41AB_98FD_812C963B7ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////

#define WC_BUTTONA              "Button"	// ansi
#define WC_BUTTONW              L"Button"	// wide

#ifdef UNICODE
#define WC_BUTTON               WC_BUTTONW
#else
#define WC_BUTTON               WC_BUTTONA
#endif

//////////////////////////

#define WC_STATICA              "Static"	// ansi
#define WC_STATICW              L"Static"	// wide

#ifdef UNICODE
#define WC_STATIC               WC_STATICW
#else
#define WC_STATIC               WC_STATICA
#endif

//////////////////////////

#define WC_EDITA              "Edit"	// ansi
#define WC_EDITW              L"Edit"	// wide

#ifdef UNICODE
#define WC_EDIT               WC_EDITW
#else
#define WC_EDIT               WC_EDITA
#endif

//////////////////////////

#define WC_COMBOBOXA              "ComboBox"	// ansi
#define WC_COMBOBOXW              L"ComboBox"	// wide

#ifdef UNICODE
#define WC_COMBOBOX               WC_COMBOBOXW
#else
#define WC_COMBOBOX               WC_COMBOBOXA
#endif

//////////////////////////

#define WC_COMBOLBOXA              "ComboLBox"	// ansi
#define WC_COMBOLBOXW              L"ComboLBox"	// wide

#ifdef UNICODE
#define WC_COMBOLBOX               WC_COMBOLBOXW
#else
#define WC_COMBOLBOX               WC_COMBOLBOXA
#endif

//////////////////////////

#define WC_LISTBOXA              "ListBox"	// ansi
#define WC_LISTBOXW              L"ListBox"	// wide

#ifdef UNICODE
#define WC_LISTBOX               WC_LISTBOXW
#else
#define WC_LISTBOX               WC_LISTBOXA
#endif

//////////////////////////

#ifndef WC_SCROLLBARA

#define WC_SCROLLBARA              "Scrollbar"	// ansi
#define WC_SCROLLBARW              L"Scrollbar"	// wide

#ifdef UNICODE
#define WC_SCROLLBAR               WC_SCROLLBARW
#else
#define WC_SCROLLBAR               WC_SCROLLBARA
#endif

#endif

//////////////////////////

#define WC_TOOLBARA              "toolbarwindow32"	// ansi
#define WC_TOOLBARW              L"toolbarwindow32"	// wide

#ifdef UNICODE
#define WC_TOOLBAR               WC_TOOLBARW
#else
#define WC_TOOLBAR               WC_TOOLBARA
#endif

//////////////////////////

#define WC_SPINA              "msctls_updown32"	// ansi
#define WC_SPINW              L"msctls_updown32"	// wide

#ifdef UNICODE
#define WC_SPIN               WC_SPINW
#else
#define WC_SPIN               WC_SPINA
#endif

//////////////////////////

#define WC_PROGRESSA              "msctls_progress32"	// ansi
#define WC_PROGRESSW              L"msctls_progress32"	// wide

#ifdef UNICODE
#define WC_PROGRESS               WC_PROGRESSW
#else
#define WC_PROGRESS               WC_PROGRESSA
#endif

//////////////////////////

#define WC_SLIDERA              "msctls_trackbar32"	// ansi
#define WC_SLIDERW              L"msctls_trackbar32"	// wide

#ifdef UNICODE
#define WC_SLIDER               WC_SLIDERW
#else
#define WC_SLIDER               WC_SLIDERA
#endif

//////////////////////////

#define WC_HOTKEYA              "msctls_hotkey32"	// ansi
#define WC_HOTKEYW              L"msctls_hotkey32"	// wide

#ifdef UNICODE
#define WC_HOTKEY               WC_HOTKEYW
#else
#define WC_HOTKEY               WC_HOTKEYA
#endif

//////////////////////////

#define WC_SHELLDLLDEFVIEWA              "SHELLDLL_DefView"	// ansi
#define WC_SHELLDLLDEFVIEWW              L"SHELLDLL_DefView"	// wide

#ifdef UNICODE
#define WC_SHELLDLLDEFVIEW               WC_SHELLDLLDEFVIEWW
#else
#define WC_SHELLDLLDEFVIEW               WC_SHELLDLLDEFVIEWA
#endif

//////////////////////////

#define WC_STATUSBARA              "msctls_statusbar32"	// ansi
#define WC_STATUSBARW              L"msctls_statusbar32"	// wide

#ifdef UNICODE
#define WC_STATUSBAR               WC_STATUSBARW
#else
#define WC_STATUSBAR               WC_STATUSBARA
#endif

//////////////////////////

#define WC_ANIMATEA              "SysAnimate32"	// ansi
#define WC_ANIMATEW              L"SysAnimate32"	// wide

#ifdef UNICODE
#define WC_ANIMATE               WC_ANIMATEW
#else
#define WC_ANIMATE               WC_ANIMATEA
#endif

//////////////////////////

#define WC_RICHEDITA              "Richedit"	// ansi
#define WC_RICHEDITW              L"Richedit"	// wide

#ifdef UNICODE
#define WC_RICHEDIT               WC_RICHEDITW
#else
#define WC_RICHEDIT               WC_RICHEDITA
#endif

//////////////////////////

#define WC_RICHEDIT20A              "RichEdit20W"	// ansi
#define WC_RICHEDIT20W              L"RichEdit20W"	// wide

#ifdef UNICODE
#define WC_RICHEDIT20               WC_RICHEDIT20W
#else
#define WC_RICHEDIT20               WC_RICHEDIT20A
#endif

//////////////////////////

#define WC_DATETIMEPICKA              "SysDateTimePick32"	// ansi
#define WC_DATETIMEPICKW              L"SysDateTimePick32"	// wide

#ifdef UNICODE
#define WC_DATETIMEPICK               WC_DATETIMEPICKW
#else
#define WC_DATETIMEPICK               WC_DATETIMEPICKA
#endif

//////////////////////////

#define WC_MONTHCALA              "SysMonthCal32"	// ansi
#define WC_MONTHCALW              L"SysMonthCal32"	// wide

#ifdef UNICODE
#define WC_MONTHCAL               WC_MONTHCALW
#else
#define WC_MONTHCAL               WC_MONTHCALA
#endif

//////////////////////////

#define WC_REBARA              "ReBarWindow32"	// ansi
#define WC_REBARW              L"ReBarWindow32"	// wide

#ifdef UNICODE
#define WC_REBAR               WC_REBARW
#else
#define WC_REBAR               WC_REBARA
#endif

//////////////////////////

#define WC_DIALOGBOXA              "#32770"	// ansi
#define WC_DIALOGBOXW              L"#32770"	// wide

#ifdef UNICODE
#define WC_DIALOGBOX               WC_DIALOGBOXW
#else
#define WC_DIALOGBOX               WC_DIALOGBOXA
#endif

//////////////////////////

#define WC_MDICLIENTA              "MDIClient"	// ansi
#define WC_MDICLIENTW              L"MDIClient"	// wide

#ifdef UNICODE
#define WC_MDICLIENT               WC_MDICLIENTW
#else
#define WC_MDICLIENT               WC_MDICLIENTA
#endif

//////////////////////////

#define WC_TOOLTIPSA              "tooltips_class32"	// ansi
#define WC_TOOLTIPSW              L"tooltips_class32"	// wide

#ifdef UNICODE
#define WC_TOOLTIPS               WC_TOOLTIPSW
#else
#define WC_TOOLTIPS               WC_TOOLTIPSA
#endif

//////////////////////////

#define WC_THUMBNAILVIEWA              "ThumbnailVwExtWnd32"	// ansi
#define WC_THUMBNAILVIEWW              L"ThumbnailVwExtWnd32"	// wide

#ifdef UNICODE
#define WC_THUMBNAILVIEW               WC_THUMBNAILVIEWW
#else
#define WC_THUMBNAILVIEW               WC_THUMBNAILVIEWA
#endif

//////////////////////////

#define WC_IEA              "Internet Explorer_Server"	// ansi
#define WC_IEW              L"Internet Explorer_Server"	// wide

#ifdef UNICODE
#define WC_IE               WC_IEW
#else
#define WC_IE               WC_IEA
#endif

//////////////////////////

#define WC_SHDOCOBJVWA              "Shell DocObject View"	// ansi
#define WC_SHDOCOBJVWW              L"Shell DocObject View"	// wide

#ifdef UNICODE
#define WC_SHDOCOBJVW               WC_SHDOCOBJVWW
#else
#define WC_SHDOCOBJVW               WC_SHDOCOBJVWA
#endif

//////////////////////////

#define WC_SHELLEMBEDDINGA              "Shell Embedding"	// ansi
#define WC_SHELLEMBEDDINGW              L"Shell Embedding"	// wide

#ifdef UNICODE
#define WC_SHELLEMBEDDING               WC_SHELLEMBEDDINGW
#else
#define WC_SHELLEMBEDDING               WC_SHELLEMBEDDINGA
#endif

//////////////////////////

#define WC_CONTROLBARA              "AfxControlBar"	// ansi
#define WC_CONTROLBARW              L"AfxControlBar"	// wide

#ifdef UNICODE
#define WC_CONTROLBAR               WC_CONTROLBARW
#else
#define WC_CONTROLBAR               WC_CONTROLBARA
#endif

//////////////////////////

#define WC_MFCDIALOGBARA              "MfcDialogBar"	// ansi
#define WC_MFCDIALOGBARW              L"MfcDialogBar"	// wide

#ifdef UNICODE
#define WC_MFCDIALOGBAR               WC_MFCDIALOGBARW
#else
#define WC_MFCDIALOGBAR               WC_MFCDIALOGBARA
#endif

//////////////////////////

#define WC_MFCVIEWA              "MfcView"	// ansi
#define WC_MFCVIEWW              L"MfcView"	// wide

#ifdef UNICODE
#define WC_MFCVIEW               WC_MFCVIEWW
#else
#define WC_MFCVIEW               WC_MFCVIEWA
#endif

//////////////////////////

#define WC_MFCMDICHILDA              "MfcFrameMDIChild"	// ansi
#define WC_MFCMDICHILDW              L"MfcFrameMDIChild"	// wide

#ifdef UNICODE
#define WC_MFCMDICHILD               WC_MFCMDICHILDW
#else
#define WC_MFCMDICHILD               WC_MFCMDICHILDA
#endif

//////////////////////////

#define WC_MFCMDIFRAMEA              "MfcFrameMDI"	// ansi
#define WC_MFCMDIFRAMEW              L"MfcFrameMDI"	// wide

#ifdef UNICODE
#define WC_MFCMDIFRAME               WC_MFCMDIFRAMEW
#else
#define WC_MFCMDIFRAME               WC_MFCMDIFRAMEA
#endif

//////////////////////////

#define WC_MFCSPLITTERA              "MfcSplitter"	// ansi
#define WC_MFCSPLITTERW              L"MfcSplitter"	// wide

#ifdef UNICODE
#define WC_MFCSPLITTER               WC_MFCSPLITTERW
#else
#define WC_MFCSPLITTER               WC_MFCSPLITTERA
#endif

//////////////////////////

#define WC_MFCFRAMEA              "MfcFrame"	// ansi
#define WC_MFCFRAMEW              L"MfcFrame"	// wide

#ifdef UNICODE
#define WC_MFCFRAME               WC_MFCFRAMEW
#else
#define WC_MFCFRAME               WC_MFCFRAMEA
#endif

//////////////////////////

#define WC_MFCMINIDOCKFRAMEA          "MfcFrameMiniDock"	// ansi
#define WC_MFCMINIDOCKFRAMEW          L"MfcFrameMiniDock"	// wide

#ifdef UNICODE
#define WC_MFCMINIDOCKFRAME           WC_MFCMINIDOCKFRAMEW
#else
#define WC_MFCMINIDOCKFRAME           WC_MFCMINIDOCKFRAMEA
#endif

//////////////////////////

#define WC_MFCMINIFRAMEA              "MfcFrameMini"	// ansi
#define WC_MFCMINIFRAMEW              L"MfcFrameMini"	// wide

#ifdef UNICODE
#define WC_MFCMINIFRAME               WC_MFCMINIFRAMEW
#else
#define WC_MFCMINIFRAME               WC_MFCMINIFRAMEA
#endif

//////////////////////////

#define WC_MFCWNDA		          "MfcWnd"	// ansi
#define WC_MFCWNDW              L"MfcWnd"	// wide

#ifdef UNICODE
#define WC_MFCWND               WC_MFCWNDW
#else
#define WC_MFCWND               WC_MFCWNDA
#endif

//////////////////////////

#define WC_MENUA              "#32768"	// ansi
#define WC_MENUW              L"#32768"	// wide

#ifdef UNICODE
#define WC_MENU               WC_MENUW
#else
#define WC_MENU               WC_MENUA
#endif

//////////////////////////

#endif