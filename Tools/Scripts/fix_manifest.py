#############################################################################
##    License (GPLv2+):
##    This program is free software; you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful, but
##    WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##    General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program; if not, write to the Free Software
##    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#############################################################################

# Copyright (c) 2008 Kimmo Varis <kimmov@winmerge.org>

# $Id$

# fix_manifest.py
# A Python script to add external manifest rules to the VS project files.
# This script originates from Frhed project (http://frhed.sourceforge.net) so
# please submit modifications/improvements to originating project also.

import getopt
import os
import shutil
import sys
from xml.dom import minidom


# The version of the script
script_version = 0.1

manifest_path = r'$(InputDir)\res\$(TargetFileName).manifest'

def process_project_file(filename):
  '''Process a given project file.

     The method opens the original file for reading. And writes changes to new
     file. Only after succesful modifications the original file is overwritten.
  '''

  outfile = filename + '.bak'
  try:
    #print 'Opening file ' + filename + ' for parsing...'
    fread = open(filename, 'r')
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

  print 'Parse VS project file...'
  dom = minidom.parse(fread)
  print 'Checking project file version...',
  proj_version = determine_project_version(dom)
  if proj_version == '8.00' or proj_version == '9.00':
    print 'Detected VS ' + proj_version
    succeeded = add_manifest(dom)
    if succeeded:
      xml_str = dom.toprettyxml()
      fwrite.write(xml_str)
      fwrite.close()
  else:
    print 'Not supported VS project file!'
    return False

  shutil.move(outfile, filename)
  return True

def determine_project_version(domtree):
  '''Determine VS version from the project file.

     This method tries to determine VS version from the project file. If the
     version number cannot be determined 0 is returned.'''

  elements = domtree.getElementsByTagName('VisualStudioProject')
  if len(elements) != 1:
    return '0'

  if elements[0].hasAttribute('Version'):
    return elements[0].getAttribute('Version')
  else:
    return '0'

def add_manifest(domtree):
  '''Add external manifest file rule to the VS8 and VS9 project files.

     This method adds external manifest file rules to every build configurations
     of the project file.'''

  elements = domtree.getElementsByTagName('VisualStudioProject')
  if len(elements) != 1:
    print 'Invalid project file - ' + len(elements) + ' projects found!'
    return False

  elem = elements[0]
  config_elems = elem.getElementsByTagName('Configuration')
  for celem in config_elems:
    handle_config_elem(celem)
  return True

def handle_config_elem(element):
  '''Modify Manifest Tool element in project file.

     This method finds the correct Manifest Tool element from the project file
     and makes needed modifications to it.'''

  tool_elements = element.getElementsByTagName('Tool')
  if len(tool_elements) == 0:
    print 'No Tool elements found - invalid project file?'
    return False

  for telem in tool_elements:
    if telem.hasAttribute('Name'):
      name = telem.getAttribute('Name')
      if name == 'VCManifestTool':
        handle_tool_element(telem)
  return True

def handle_tool_element(element):
  '''Adds new attributes to the element.'''

  element.setAttribute('AdditionalManifestFiles', manifest_path)
  element.setAttribute('EmbedManifest', 'true')

def usage():
  '''Print script usage information.'''

  print 'fix_manifest.py - version ' + str(script_version)
  print 'Script to fix manifest file definition in VS8 and VS9 project files.'
  print 'Usage: fix_manifest.py [-h] filename'
  print 'Where:'
  print '  filename is absolute or relative path to the project file.'
  print '  -h, --help Print this help.'

def main(argv):
  filename = ''
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
    ret = process_project_file(filename)
    if ret == True:
      print 'Manifest file fixed to project file successfully!'
  else:
    print 'ERROR: Could not find file: ' + filename


# MAIN #
if __name__ == "__main__":
    main(sys.argv[1:])
