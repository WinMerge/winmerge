INCLUDES=-I../../Src -I../../Src/Common -I../../Src/diffutils -I../../Src/diffutils/lib -I../../Src/diffutils/src -I../../Src/CompareEngines -I../../Externals/boost -I../../Externals/Poco/Foundation/include -I../../Externals/Poco/XML/include -I../../Externals/Poco/Util/include

CFLAGS=-g -pg -DHAVE_CONFIG_H -DREGEX_MALLOC -D__NT__ $(INCLUDES)
CXXFLAGS=-g -pg -std=gnu++11 $(INCLUDES)
TARGET=FolderCompare.exe

OBJS=\
../../Src/Common/coretools.o \
../../Src/Common/lwdisp.o \
../../Src/Common/multiformatText.o \
../../Src/Common/OptionsMgr.o \
../../Src/Common/RegKey.o \
../../Src/Common/RegOptionsMgr.o \
../../Src/Common/ShellFileOperations.o \
../../Src/Common/UnicodeString.o \
../../Src/Common/UniFile.o \
../../Src/Common/unicoder.o \
../../Src/Common/varprop.o \
../../Src/Common/version.o \
../../Src/Common/ExConverter.o \
../../Src/CompareEngines/ByteComparator.o \
../../Src/CompareEngines/ByteCompare.o \
../../Src/CompareEngines/BinaryCompare.o \
../../Src/CompareEngines/DiffUtils.o \
../../Src/CompareEngines/TimeSizeCompare.o \
../../Src/diffutils/lib/cmpbuf.o \
../../Src/diffutils/src/analyze.o \
../../Src/diffutils/src/context.o \
../../Src/diffutils/src/Diff.o \
../../Src/diffutils/src/ed.o \
../../Src/diffutils/src/ifdef.o \
../../Src/diffutils/src/io.o \
../../Src/diffutils/src/normal.o \
../../Src/diffutils/src/side.o \
../../Src/diffutils/src/util.o \
../../Src/diffutils/GnuVersion.o \
../../Src/charsets.o \
../../Src/codepage.o \
../../Src/codepage_detect.o \
../../Src/CompareOptions.o \
../../Src/CompareStats.o \
../../Src/ConflictFileParser.o \
../../Src/DiffContext.o \
../../Src/DiffFileData.o \
../../Src/DiffFileInfo.o \
../../Src/DiffItem.o \
../../Src/DiffItemList.o \
../../Src/DiffList.o \
../../Src/DiffThread.o \
../../Src/DiffWrapper.o \
../../Src/DirItem.o \
../../Src/DirScan.o \
../../Src/DirTravel.o \
../../Src/Environment.o \
../../Src/FileFilter.o \
../../Src/FileFilterHelper.o \
../../Src/FileFilterMgr.o \
../../Src/FileTextEncoding.o \
../../Src/FileTransform.o \
../../Src/FileVersion.o \
../../Src/FilterCommentsManager.o \
../../Src/FilterList.o \
../../Src/FolderCmp.o \
../../Src/LineFiltersList.o \
../../Src/locality.o \
../../Src/markdown.o \
../../Src/MergeCmdLineInfo.o \
../../Src/MovedBlocks.o \
../../Src/MovedLines.o \
../../Src/OptionsDef.o \
../../Src/PatchHTML.o \
../../Src/PathContext.o \
../../Src/paths.o \
../../Src/Plugins.o \
../../Src/PluginManager.o \
../../Src/ProjectFile.o \
../../Src/stringdiffs.o \
../../Src/TempFile.o \
../../Src/UniMarkdownFile.o \
misc.o \
FolderCompare.o

$(TARGET): $(OBJS) $(POCOLIBS)
	$(CXX) -pg $(OBJS) -L../../Externals/poco/lib/MinGW/ia32 -lPocoUtil -lPocoXML -lPocoFoundation -lversion -lshlwapi -luuid -lole32 -loleaut32 -lIphlpapi -o $(TARGET)	

clean:
	$(RM) $(OBJS) $(TARGET)



