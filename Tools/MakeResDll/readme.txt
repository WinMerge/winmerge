MakeResDll

This program invokes a resource compiler and a linker to produce 
a language DLL from an RC source file.

It searches the registry to find, in order of preference, 
Visual Studio .NET 2003, Visual Studio .NET, Visual Studio 6,
and Visual Studio 5.

This does not set the include and library paths correctly,
so before invoking this one must have these paths set.
If these were not set at installation time (it is an option),
these may typically be set by invoking a batch file such as:

 C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat

or

 C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat


If multiple versions are present, and you wish to override the
normal precedence order, then you must set the following three
keys in this fashion (this example forces use of MSVC6 installed
in the default location):


HKCU\Software\\Thingamahoochie\\MakeResDll\\Settings
 VcBaseFolder = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin
 RCExe = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\rc.exe
 LinkExe = C:\Program Files\Microsoft Visual Studio\vc98\bin\link.exe
