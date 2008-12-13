# The MIT License
#
# Copyright (c) 2008 Kimmo Varis <kimmov@winmerge.org>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# $Id$

# SetVersions.py
# A Python script to set various component's version numbers in the project.

# This script originates from CheckSum Tool project at
# (http://checksumtool.sourceforge.net). Please submit fixes and updates to the
# script for the originating project so other projects using the script can use
# them too.

import ConfigParser
import getopt
import os
import shutil
import sys

# The version of the script
script_version = 0.8

def get_product_version(filename):
  '''Get product version used in archive paths etc.
     TODO: add new section. At the moment this gets executable version.
  '''

  config = ConfigParser.ConfigParser()
  config.readfp(open(filename))

  # First try RC file, then define-macro
  version = ''
  if config.has_section('Executable') and \
      config.has_option('Executable', 'version'):
    version = config.get('Executable', 'version')
  else:
    print 'Executable version not found, using default.'

  # Remove quotation marks
  if len(version) > 0:
    if version[0] == '"':
      version = version[1:]
    if version[len(version) - 1] == '"':
      version = version[:len(version) - 1]
  return version

def process_NSIS(filename, config, sect):
  '''Process NSIS section in the ini file.'''

  if config.has_option(sect, 'version') and \
      config.has_option(sect, 'path') and \
      config.has_option(sect, 'description'):

    ver = config.get(sect, 'version')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
  else:
    print 'ERROR: NSIS section does not have all required options!'
    return False
  
  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  
  inidir = os.path.dirname(filename)
  nsisfile = os.path.join(inidir, file)
  
  ret = set_NSIS_ver(nsisfile, ver)
  return ret

def set_NSIS_ver(file, version):
  '''Set version into NSIS installer file.'''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Replace PRODUCT_ VERSION macro value with new value
  for line in fread:
    if line.startswith('!define PRODUCT_VERSION'):
      ind = line.find('\"')
      ind2 = line.rfind('\"')
      if ind != -1 and ind2 != -1:
        line = line[:ind] + version + line[ind2 + 1:]
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  
  return True

def process_AssemblyCs(filename, config, sect):
  '''Process C# AssemblyInfo section in the ini file.'''

  if config.has_option(sect, 'version') and \
      config.has_option(sect, 'path') and \
      config.has_option(sect, 'description'):

    ver = config.get(sect, 'version')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
  else:
    print 'ERROR: Assembly info section does not have all required options!'
    return False
  
  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  
  inidir = os.path.dirname(filename)
  nsisfile = os.path.join(inidir, file)
  
  ret = set_CSAssembly_ver(nsisfile, ver)
  return ret

def set_CSAssembly_ver(file, version):
  '''Set version into C# Assembly Info file.'''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Replace AssemblyVersion value with new value
  for line in fread:
    if line.startswith('[assembly: AssemblyVersion'):
      replace_ver_in_quotes(line, version)
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  return True

def process_WinRC(filename, config, sect):
  '''Process Windows RC file section in the ini file.'''

  if config.has_option(sect, 'version') and \
       config.has_option(sect, 'path') and \
       config.has_option(sect, 'description'):

    ver = config.get(sect, 'version')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
  else:
    print 'ERROR: RC file section does not have all required options!'
    return False
  
  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  
  inidir = os.path.dirname(filename)
  rcfile = os.path.join(inidir, file)
  
  ret = set_WinRC_ver(rcfile, ver)
  return ret

def set_WinRC_ver(file, version):
  '''Set version into Windows RC file. Currently we set both fileversion and
     productversion to same number.
     TODO: Allow separate file- and product -versions.
  '''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Add ending NULL to the version string and replace dots with commas
  verInd = version.rfind('\"')
  if verInd != -1:
    versionNul = version[:verInd] + '\\0' + version[verInd:]

  # Replace version defines with new value
  ret = True
  for line in fread:
    if line.startswith(' FILEVERSION'):
      line = replace_rc_ver_at_end(line, version)
    if line.startswith(' PRODUCTVERSION'):
      line = replace_rc_ver_at_end(line, version)
    if line.find('VALUE \"FileVersion\"') != -1:
      line = replace_ver_in_quotes(line, versionNul)
    if line.find('VALUE \"ProductVersion\"') != -1:
      line = replace_ver_in_quotes(line, versionNul)
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  return ret

def process_InnoSetup(filename, config, sect):
  '''Process Innosetup script file section in the ini file.'''

  if config.has_option(sect, 'version') and \
      config.has_option(sect, 'path') and \
      config.has_option(sect, 'description') and \
      config.has_option(sect, 'macro'):

    ver = config.get(sect, 'version')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
    macro = config.get(sect, 'macro')
  else:
    print 'ERROR: InnoSetup section does not have all required options!'
    return False
  
  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  if len(macro):
    print '  Macro: ' + macro
  else:
    print '  ERROR: You must set macro name in the INI file.'
    return False
  
  inidir = os.path.dirname(filename)
  innofile = os.path.join(inidir, file)
  
  ret = set_InnoSetup_ver(innofile, ver, macro)
  return ret

def set_InnoSetup_ver(file, version, macro):
  '''Set version into InnoSetup script. If the macro setting was found
     from the INI file replace only that macro's value. Currently the script
     supports only replacing initial macro value.

     TODO: support other means (not just macro) to set version.
  '''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Replace version macro value with new value
  macroline = '#define ' +  macro
  for line in fread:
    if line.startswith(macroline):
      line = line[:len(macroline)] + ' ' + version + '\n'
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  return True

def process_CDefine(filename, config, sect):
  '''Read version number information for setting it into C/C++ #defines.'''

  if config.has_option(sect, 'version') and \
      config.has_option(sect, 'path') and \
      config.has_option(sect, 'description'):

    ver = config.get(sect, 'version')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
  else:
    print 'ERROR: C Define section does not have all required options!'
    return False
  
  # Macro names to use
  if config.has_option(sect, 'define-major'):
    major = config.get(sect, 'define-major')
  if config.has_option(sect, 'define-minor'):
    minor = config.get(sect, 'define-minor')
  if config.has_option(sect, 'define-subrelease'):
    subrel = config.get(sect, 'define-subrelease')
  if config.has_option(sect, 'define-buildnumber'):
    buildnum = config.get(sect, 'define-buildnumber')

  if len(major) == 0 and len(minor) == 0 and len(subrel) == 0:
   print '  ERROR: You must set at least one of major/minor/subrelease version numbers.'
   return False

  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  print '  Macros:'
  print '    Major: ' + major
  print '    Minor: ' + minor
  print '    Subrelease: ' + subrel
  print '    Buildnumber: ' + buildnum

  inidir = os.path.dirname(filename)
  cfile = os.path.join(inidir, file)

  ret = set_CDefine_ver(cfile, ver, major, minor, subrel, buildnum)
  return ret

def set_CDefine_ver(file, version, major, minor, subrelease, buildnumber):
  '''Set version into C/C++ file with version number #define lines. Each
     part of the version number has own line and own #define macro name.
  '''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Init to zero so missing version numbers are zero
  maj_ver = 0
  min_ver = 0
  sub_ver = 0
  build_ver = 0

  ind = version.find('.')
  if ind == -1:
    # Only major versio number given, e.g. "5"
    maj_ver = int(version[1:len(version) - 1])
  else:
    maj_ver = int(version[1:ind])

  if ind != -1:
    ind2 = version.find('.', ind + 1)
    if ind2 != -1:
      # Set minor number
      min_ver = int(version[ind + 1:ind2])
      ind = ind2
    else:
      # Only major and minor given, e.g. "2.7"
      min_ver = int(version[ind + 1:len(version) - 1])
      ind = ind2

  if ind != -1:
    ind2 = version.find('.', ind + 1)
    if ind2 != -1:
      # Set sub release number and build number "x.x.2.8"
      sub_ver = int(version[ind + 1:ind2])
      build_ver = int(version[ind2 + 1:len(version) - 1])
    else:
      # Only major, minor and sub number given, e.g. "4.2.8"
      sub_ver = int(version[ind + 1:len(version) - 1])

  # Replace version macro values with new values
  for line in fread:
    macroline = '#define '
    if line.startswith(macroline + major):
      line = line[:len(macroline + major)] + ' ' + str(maj_ver) + '\n'
    if line.startswith(macroline + minor):
      line = line[:len(macroline + minor)] + ' ' + str(min_ver) + '\n'
    if line.startswith(macroline + subrelease):
      line = line[:len(macroline + subrelease)] + ' ' + str(sub_ver) + '\n'
    if line.startswith(macroline + buildnumber):
      line = line[:len(macroline + buildnumber)] + ' ' + str(build_ver) + '\n'
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  return True

def process_DocBook(filename, config, sect):
  '''Process DocBook section in the ini file.'''

  if config.has_option(sect, 'versionstring') and \
      config.has_option(sect, 'path') and \
      config.has_option(sect, 'description') and \
      config.has_option(sect, 'tag'):

    ver = config.get(sect, 'versionstring')
    file = config.get(sect, 'path')
    desc = config.get(sect, 'description')
    tag = config.get(sect, 'tag')
  else:
    print 'ERROR: DocBook section does not have all required options!'
    return False
  
  print '%s : %s' % (sect, desc)
  print '  File: ' + file
  print '  Version: ' + ver
  print '  Tag: ' + tag
  
  inidir = os.path.dirname(filename)
  manualfile = os.path.join(inidir, file)
  
  ret = set_DocBook_ver(manualfile, ver, tag)
  return ret

def set_DocBook_ver(file, version, tag):
  '''Set revision of DocBook manual.'''

  outfile = file + '.bak'
  try:
    fread = open(file, 'r')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + file + ' for reading'
    print 'Error: ' + strerror
    return False

  try:
    fwrite = open(outfile, 'w')
  except IOError, (errno, strerror):
    print 'Cannot open file ' + infile + ' for writing'
    print 'Error: ' + strerror
    fread.close()
    return False

  # Replace text inside releaseinfo tag with new text
  begintag = '<' + tag + '>'
  endtag = '</' + tag + '>'
  for line in fread:
    begin_ind = line.find(begintag)
    end_ind = line.find(endtag)
    if begin_ind != -1 and end_ind != -1:
      line = line[:begin_ind + len(begintag)] + version + line[end_ind:]
    fwrite.write(line)

  fread.close()
  fwrite.close()
  
  shutil.move(outfile, file)
  
  return True

def replace_rc_ver_at_end(line, version):
  '''Replace plain version number at the end of the line in RC file.
     Also make sure we have four numbers.
  '''

  version = version.strip('\"')
  version = version.replace('.', ',')

  # Make sure there are four numbers, add zeros if necessary
  commas = version.count(',')
  if commas < 3:
    add = 3 - commas
    for i in range(0, add):
      version = version + ',0'

  ind = line.rfind(' ')
  if ind != -1:
    line = line[:ind + 1] + version + '\n'
  return line

def replace_ver_in_quotes(line, version):
  '''Replace version number in line in quotes with given version string.
     Eg. if line is 'Version "1.2.3"' and version is "4.5.6" the return value is
     'Version "3.4.5"'
  '''

  # Find quote marks between which we set the version number
  lineInd2 = line.rfind('\"')
  lineInd1 = line.rfind('\"', 0, lineInd2)
  if lineInd1 != -1 and lineInd2 != -1:
    line = line[:lineInd1] + version + line[lineInd2 + 1:]
  return line

def process_versions(filename):
  '''Process all sections found from the given ini file.'''

  config = ConfigParser.ConfigParser()
  config.readfp(open(filename))

  ret = False
  sectlist = config.sections()
  print 'Setting versions:'
  for sect in sectlist:
    vertype = config.get(sect, 'type')
    if vertype == 'NSIS':
      ret = process_NSIS(filename, config, sect)
    if vertype == 'CS-AssemblyInfo':
      ret = process_AssemblyCs(filename, config, sect)
    if vertype == 'WinRC':
      ret = process_WinRC(filename, config, sect)
    if vertype == 'InnoSetup':
      ret = process_InnoSetup(filename, config, sect)
    if vertype == 'C-Define':
      ret = process_CDefine(filename, config, sect)
    if vertype == 'Docbook':
      ret = process_DocBook(filename, config, sect)
  return ret

def usage():
  '''Print script usage information.'''
  print 'SetVersions.py - version ' + script_version
  print 'Script to set program component version numbers.'
  print 'Usage: SetVersions.py [-h] filename'
  print 'Where:'
  print '  filename is absolute or relative path to the ini file.'
  print '  -h, --help Print this help.'

def main(argv):
  filename = ''
  createbaks = True
  if len(argv) > 0:
    opts, args = getopt.getopt(argv, 'h', ['help'])

    for opt, arg in opts:
      if opt in ('-h', '--help'):
        usage()
        sys.exit()

    if len(args) == 1:
      filename = args[0]
    else:
      usage()
      sys.exit()
  else:
    usage()
    sys.exit()
  
  filename = os.path.abspath(filename)
  if os.path.exists(filename):
    ret = process_versions(filename)
    if ret == True:
      print 'Version numbers set successfully!'
  else:
    print 'ERROR: Could not find file: ' + filename

# MAIN #
if __name__ == "__main__":
    main(sys.argv[1:])
