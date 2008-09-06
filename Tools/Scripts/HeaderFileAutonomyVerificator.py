import getopt
import os.path
import re
import subprocess
import sys
import tempfile

class TemporarySourceFile:
    def __init__(self, headerFile):
        self._headerFile = headerFile
        fd, self._tempFileName = tempfile.mkstemp( '.cpp', os.path.basename(headerFile), os.path.dirname(headerFile), True)
        tempFile = os.fdopen(fd,'w')
        tempFile.write('#include "')
        tempFile.write(headerFile)
        tempFile.write('"')
        tempFile.close()

    def Compile(self, commandLine):
        command = commandLine + "\"" + self._tempFileName + "\""
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        process.wait()
        if process.returncode == 0:
            print "Compilation successful"
        else:
            print "Compilation failed!"
            for line in process.stdout.readlines():
                print line
            print "Compilation failed!"
    
    def __del__(self):
        if self._tempFileName is not None:
            os.remove(self._tempFileName)


def ExtractCommandLineFromFile(fileName):
    file = open(fileName)
    try:
        return " ".join(file.readlines())
    finally:
        file.close()

def RemovePrecompiledHeadersFromCompileCommand(compileCommand):
    #TODO
    #remove: /Yu"stdafx.h" /Fp"./../BuildTmp/MergeUnicodeDebug/Merge.pch"
    return compileCommand

def FixPathSeparatorsFromCompileCommand(compileCommand):
    #TODO
    return compileCommand

def FixRelativePathsFromCompileCommand(compileCommand):
    #TODO
    return compileCommand


def PrintUsage():
    print """
This script verifies if a C++ header file is self-contained by compiling
a temporary file containing only the following line:
#include "yourfile.h"

The temporary file is compiled with the following command-line unless you
override it with the -c argument:
cl.exe /Od /I "." /I "./CompareEngines" /I "./Common" /I "./editlib" /I "./diffutils" /I "./diffutils/lib" /I "./diffutils/src" /I "../Externals/expat/lib" /I "../Externals/expat/xmlwf" /I "../Externals/scew" /I "../Externals/pcre/Win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D "HAVE_STRING_H=1" /D "PR_FILE_NAME=\"pr\"" /D "DIFF_PROGRAM=\\"diff\\"" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D "USG=1" /D "EDITPADC_CLASS=" /D "COMPILE_MULTIMON_STUBS" /D "UNICODE" /D "_CRT_SECURE_NO_DEPRECATE" /D "_AFXDLL" /D "_UNICODE" /FD /EHsc /MDd /GS /GR /Yu"stdafx.h" /Fp"./../BuildTmp/MergeUnicodeDebug/Merge.pch" /Fo"./../BuildTmp/MergeUnicodeDebug/" /Fd"./../BuildTmp/MergeUnicodeDebug/" /FR"./../BuildTmp/MergeUnicodeDebug/" /W3 /nologo /c /Zi

Note: Before you run this script, you must have set up the Visual Studio
      environment variables through the following script (or similar)
       "c:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat"

Usage: HeaderFileAutonomyVerificator.py [-h] [-c commandfile] <headerfile>
  where:
    -h, --help              print this help
    -c, --commandfile <ARG> override the default compilation command with the one
                            defined in the file <ARG>
    <headerfile>            header file to test
                            
"""

def main(argv):

    commandFile = None
    
    opts, args = getopt.getopt(argv, "hc:", [ "help", "commandfile=" ])
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            PrintUsage()
            sys.exit()
        if opt in ("-c", "--commandfile"):
            commandFile = arg

    if len(args) != 1:
        PrintUsage()
        sys.exit()

    headerFile = args[0]

    #compileCommand = r'cl.exe /Od /I "." /I "./CompareEngines" /I "./Common" /I "./editlib" /I "./diffutils" /I "./diffutils/lib" /I "./diffutils/src" /I "../Externals/expat/lib" /I "../Externals/expat/xmlwf" /I "../Externals/scew" /I "../Externals/pcre/Win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D "HAVE_STRING_H=1" /D "PR_FILE_NAME=\"pr\"" /D "DIFF_PROGRAM=\\"diff\\"" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D "USG=1" /D "EDITPADC_CLASS=" /D "COMPILE_MULTIMON_STUBS" /D "UNICODE" /D "_CRT_SECURE_NO_DEPRECATE" /D "_AFXDLL" /D "_UNICODE" /FD /EHsc /MDd /GS /GR /Yu"stdafx.h" /Fp"./../BuildTmp/MergeUnicodeDebug/Merge.pch" /Fo"./../BuildTmp/MergeUnicodeDebug/" /Fd"./../BuildTmp/MergeUnicodeDebug/" /FR"./../BuildTmp/MergeUnicodeDebug/" /W3 /nologo /c /Zi '
    compileCommand = r'cl.exe /Od /I "." /I "./CompareEngines" /I "./Common" /I "./editlib" /I "./diffutils" /I "./diffutils/lib" /I "./diffutils/src" /I "../Externals/expat/lib" /I "../Externals/expat/xmlwf" /I "../Externals/scew" /I "../Externals/pcre/Win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D "HAVE_STRING_H=1" /D "PR_FILE_NAME=\"pr\"" /D "DIFF_PROGRAM=\\"diff\\"" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D "USG=1" /D "EDITPADC_CLASS=" /D "COMPILE_MULTIMON_STUBS" /D "UNICODE" /D "_CRT_SECURE_NO_DEPRECATE" /D "_AFXDLL" /D "_UNICODE" /FD /EHsc /MDd /GS /GR /Fo"./../BuildTmp/MergeUnicodeDebug/" /Fd"./../BuildTmp/MergeUnicodeDebug/" /FR"./../BuildTmp/MergeUnicodeDebug/" /W3 /nologo /c /Zi '
    if commandFile is not None:
        compileCommand = ExtractCommandLineFromFile(commandFile)

    compileCommand = RemovePrecompiledHeadersFromCompileCommand(compileCommand)
    compileCommand = FixPathSeparatorsFromCompileCommand(compileCommand)
    compileCommand = FixRelativePathsFromCompileCommand(compileCommand)

    tempFile = TemporarySourceFile(headerFile)

    #compile and print results
    tempFile.Compile(compileCommand)
    # problems with include paths being relative to vcproj while we compile from other folder???
    
### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
