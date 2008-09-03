import os
import os.path
import re
import sys
import getopt

from xml.dom import minidom
import xml.etree.cElementTree as ET

class Comparator:
    def Compare(self, other, skipRegexes):
        selfPaths = self.GetRelativePaths()
        otherPaths = other.GetRelativePaths()

        self._RemoveMatching(selfPaths, skipRegexes)
        self._RemoveMatching(otherPaths, skipRegexes)

        filesInSelfNotInOther = selfPaths.difference(otherPaths)
        filesInOtherNotInSelf = otherPaths.difference(selfPaths)

        differencesFound = False
        print ''

        if len(filesInSelfNotInOther) > 0:
            differencesFound = True
            self._PrintDifferences(filesInSelfNotInOther, self.GetName(), other.GetName())

        if len(filesInOtherNotInSelf) > 0:
            differencesFound = True
            self._PrintDifferences(filesInOtherNotInSelf, other.GetName(), self.GetName())

        if differencesFound:
            print 'Differences found'
        else:
            print 'No differences found for what we verify: File list'

    def _RemoveMatching(self, set, regexes):
        toRemove = []
        for fileName in set:
            for regex in regexes:
                if regex.match(fileName):
                    toRemove.append(fileName)
        for fileName in toRemove:
            set.remove(fileName)

    def _PrintDifferences(self, fileSet, leftName, rightName):
        print "Files in '%s' not in '%s':" % (leftName, rightName)
        filesSorted = list(fileSet)
        filesSorted.sort()
        for f in filesSorted:
            print '  ' + f
        print ''

class VS2003ProjectComparator(Comparator):
    def __init__(self, basepath):
        self._basepath = basepath
        self._projectFileName = 'Merge.vcproj'

    def GetName(self):
        return self._projectFileName

    def GetRelativePaths(self):
        """Extracts the relative paths of all files referenced in the project file."""
        files = []
        elementTree = ET.parse(os.path.join(self._basepath, self._projectFileName))
        for fileElement in elementTree.findall('//File'):
            fileName = fileElement.get('RelativePath')
            if fileName[0:2] == './' or fileName[0:2] == '.\\':
                fileName = fileName[2:]
            files.append(fileName)
        return set(files)

class VS2008ProjectComparator(Comparator):
    def __init__(self, basepath):
        self._basepath = basepath
        self._projectFileName = 'Merge.2008.vcproj'
    
    def GetName(self):
        return self._projectFileName

    def GetRelativePaths(self):
        """Extracts the relative paths of all files referenced in the project file."""
        files = []
        elementTree = ET.parse(os.path.join(self._basepath, self._projectFileName))
        for fileElement in elementTree.findall('//File'):
            files.append(fileElement.get('RelativePath'))
        return set(files)

class OnDiskFilesComparator(Comparator):
    def __init__(self, basepath):
        self._basepath = basepath
    
    def GetName(self):
        return 'disk files below ' + self._basepath

    def GetRelativePaths(self):
        """Finds all source files below the base path"""
        result = []
        for root, dirs, files in os.walk(self._basepath):
            if '.svn' in dirs:
                dirs.remove('.svn')  # don't visit .svn directories
            relativeDir = root[len(self._basepath)+1:]
            for fileName in files:
                result.append(os.path.join(relativeDir,fileName))
        return set(result)

def CreateComparator(type, basepath, fromTo):
    if type == "2003":
        return VS2003ProjectComparator(basepath)
    if type == "2008":
        return VS2008ProjectComparator(basepath)
    if type == "disk":
        return OnDiskFilesComparator(basepath)
    print "Invalid '%s' option '%s'" % (fromTo, type)
    usage()
    sys.exit()

def PrintUsage():
    print """
WinMerge project file compare script. This script compares the project files
of different versions of VisualStudio as well as the list of files on disk
and displays the discrepencies.

Usage: CompareProjectFiles [-h] [-b <basepath>] <from> <to>
  where:
    -h, --help              print this help
    -b, --base-path <ARG>   set the base path to search for files
                            defaults to ../../Src
    <from> and <to>         either of:
                             disk: files on disk
                             2003: VisualStudio 2003 project file
                             2008: VisualStudio 2008 project file
"""

def main(argv):
    basepath = '../../Src'

    opts, args = getopt.getopt(argv, "hb:", [ "help", "base-path=" ])
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            PrintUsage()
            sys.exit()
        if opt in ("-b", "--base-path"):
            basepath = arg

    if len(args) != 2:
        PrintUsage()
        sys.exit()

    fromArg, toArg = args
    
    basepath = os.path.realpath(basepath)

    fromInstance = CreateComparator(fromArg, basepath, "FROM")
    toInstance = CreateComparator(toArg, basepath, "TO")

    regexes = [re.compile(r'^ExpatMapLib\\'),
               re.compile(r'^Languages\\'),
               re.compile(r'^res\\.*\.bmp$'),]
    fromInstance.Compare(toInstance, regexes)

### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
