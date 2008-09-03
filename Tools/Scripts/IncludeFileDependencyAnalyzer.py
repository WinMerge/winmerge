#!/bin/env python
import re
import sys

class Depend:
  def __init__(self):
    self.__linePreprocessorPattern = re.compile('^( |\t)*#(line)? [0-9]+ "(?P<file>.*)"')
    self.__windowsFilePattern = re.compile('.*\\\\(?P<name>.*)')
    self.__unixFilePattern = re.compile('.*/(?P<name>.*)')
    self._includedFiles = set()

  def __getName(self, file):
    match = self.__windowsFilePattern.match(file)
    if match is None:
      match = self.__unixFilePattern.match(file)
    if match is None:
      return file
    else:
      return match.group("name")

  def __notBogus(self, file):
    """In g++ the first couple of lines in the pre-processed output
       look like this:
       # 1 "<built-in>"

       These lines should be ignored; they don't refer to real include files."""
    return file[0] != '<' and file[-1] != '>'

  def feed(self, lines):
    for line in lines:
      match = self.__linePreprocessorPattern.match(line)
      if match is not None:
        file = match.group('file')
        file = file.replace('\\\\','\\')
        if self.__notBogus(file):
          self._includedFiles.add(file)

  def getIncludesWithPath(self):
    return self._includedFiles

  def getIncludesWithoutPath(self):
    includes = []
    for file in self._includedFiles:
        includes.append(self.__getName(file))
    return includes

  def removeVisualStudioPlatformFiles(self):
    toRemove = []
    for file in self._includedFiles:
        if file.lower().startswith('c:\\program files\\microsoft platform sdk\\include\\'):
            toRemove.append(file)
    for file in toRemove:
        self._includedFiles.remove(file)

  def removePlatformSdkFiles(self):
    toRemove = []
    for file in self._includedFiles:
        if file.lower().startswith('c:\\program files\\microsoft visual'):
            toRemove.append(file)
    for file in toRemove:
        self._includedFiles.remove(file)

  def removeSourceFile(self):
    toRemove = []
    for file in self._includedFiles:
        if file.lower().endswith('.cpp'):
            toRemove.append(file)
        if file.lower().endswith('.c'):
            toRemove.append(file)
    for file in toRemove:
        self._includedFiles.remove(file)

def printUsage():
    print """
Usage: IncludeFileDependencyAnalyzer.py <file.i>
  where:
    <file.i>            is the preprocessor output of the compiler for a given C/C++ file.
                        Look at http://c2.com/cgi/wiki?CppDependencyAnalysis to learn how to
                        generate it."""

def main(argv):
    if len(argv) != 1:
        printUsage()
        sys.exit(-1)

    file = argv[0]
    lines = open(file).xreadlines()

    depend = Depend()
    depend.feed(lines)

    depend.removePlatformSdkFiles()
    depend.removeVisualStudioPlatformFiles()
    depend.removeSourceFile()

    dependTree = depend.getIncludesWithPath()

    print "\n".join(sorted(dependTree))

### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
