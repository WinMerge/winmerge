MakeResDll

This program invokes a resource compiler and a linker to produce 
a language DLL from an RC source file.

It searches the registry to find, in order of preference, 
Visual Studio .NET 2003, Visual Studio .NET, Visual Studio 6,
and Visual Studio 5.

It takes include and library paths for Visual Studio 6 or 5,
but it doesn't pick them up for Visual Studio .NET versions
(if you want to help fix this, please feel free!)


If multiple versions are present, and you wish to override the
normal precedence order, then you must set the following three
keys in this fashion (this example forces use of MSVC6 installed
in the default location):


HKCU\Software\\Thingamahoochie\\MakeResDll\\Settings
 VcBaseFolder = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin
 RCExe = C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\rc.exe
 LinkExe = C:\Program Files\Microsoft Visual Studio\vc98\bin\link.exe
