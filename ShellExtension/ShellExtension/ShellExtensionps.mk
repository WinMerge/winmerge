
ShellExtensionps.dll: dlldata.obj ShellExtension_p.obj ShellExtension_i.obj
	link /dll /out:ShellExtensionps.dll /def:ShellExtensionps.def /entry:DllMain dlldata.obj ShellExtension_p.obj ShellExtension_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ShellExtensionps.dll
	@del ShellExtensionps.lib
	@del ShellExtensionps.exp
	@del dlldata.obj
	@del ShellExtension_p.obj
	@del ShellExtension_i.obj
