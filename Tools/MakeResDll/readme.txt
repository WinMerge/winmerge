MakeResDll

This program invokes a resource compiler and a linker to produce 
a language DLL from an RC source file.

It searches the registry to find, in order of preference:
 Visual Studio .NET 2005
 Visual Studio .NET 2003
 Visual Studio .NET (2002)
 Visual Studio 6
 Visual Studio 5 

It reads paths from the registry to find the locations for
the RC compiler, the linker, and the include and library paths
(which it passes on the commandline when it calls the RC
compiler and linker).

You may force it to use a particular version of MakeResDll by
invoking it with the /ui switch, which will bring up a dialog
allowing you to choose the version of Visual Studio desired.

This actually simply sets the registry value "VcVersion" 
under key HKCU\Software\\Thingamahoochie\\MakeResDll\\Settings
as follows:
 MSVC5: VcVersion=5
 MSVC6: VcVersion=6
 MSVC.Net 2002: VcVersion=Net
 MSVC.Net 2003: VcVersion=Net2003
 MSVC.Net 2005: VcVersion=Net2005

If you wish to override the paths used for some arcane reason,
these settings may be set -- the following example shows it
set to use the default locations for MSVC6:

HKCU\Software\\Thingamahoochie\\MakeResDll\\Settings
 VcBaseFolder = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin
 RCExe = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\rc.exe
 LinkExe = C:\Program Files\Microsoft Visual Studio\vc98\bin\link.exe


The paths used by the program were largely deduced by examining
the vcvars32.bat files that ship with the various versions, at, eg: 
 
 C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat
 C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat
