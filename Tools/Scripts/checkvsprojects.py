'''A Python scrip to check Visual Studio project files.
At the moment the script checks that:
 - files listed in project file exist
'''

#
# The MIT License
# Copyright (c) 2010 Kimmo Varis
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import os.path
from xml.dom import minidom
import sys

def read_proj_file(filename):
    '''Open project file and check it.'''

    try:
        fread = open(filename, 'r')
    except IOError, (errno, strerror):
        print 'Cannot open file ' + filename + ' for reading'
        print 'Error: ' + strerror
        return False

    print 'Parse VS project file...'
    dom = minidom.parse(fread)
    print 'Checking project file version...',
    proj_version = determine_project_version(dom)
    if proj_version.find(',') != -1:
        proj_version = proj_version.replace(',', '.')
    if proj_version == '8.00' or proj_version == '9.00':
        print 'Detected VS ' + proj_version
        files = get_files_list(dom, os.path.dirname(filename))
        #for file in files:
        #    print 'Path: ' + file
        check_files_exist(files)
    else:
        print 'Not supported VS project file!'
        return False

    return True

def determine_project_version(domtree):
    '''Determine VS version from the project file.

    This method tries to determine VS version from the project file. If the
    version number cannot be determined 0 is returned.
    '''

    elements = domtree.getElementsByTagName('VisualStudioProject')
    if len(elements) != 1:
        return '0'

    if elements[0].hasAttribute('Version'):
        return elements[0].getAttribute('Version')
    else:
        return '0'

def get_files_list(domtree, basepath):
    '''Read list of filenames from project file.'''

    print 'Reading file list from project file...'
    files_list = []
    elements = domtree.getElementsByTagName('File')
    if len(elements) > 0:
        for element in elements:
            if element.hasAttribute('RelativePath'):
                path = element.getAttribute('RelativePath')
                path = os.path.join(basepath, path)
                path = os.path.normpath(path)
                files_list.append(path)
    return files_list

def check_files_exist(files_list):
    '''Check that files in list exist.'''

    print 'Checking that all files exist...'
    missing_files = []
    for file in files_list:
        if not os.path.exists(file):
            missing_files.append(file)
    if len(missing_files) > 0:
        print 'File not found: %s' % file

def main(argv):
    if len(argv) == 1:
        filename = argv[0]
        filename = os.path.abspath(filename)
        read_proj_file(filename)
    else:
        print 'Usage: checkvsprojects filename'
        return
    return 0
        
# MAIN
if __name__ == "__main__":
    main(sys.argv[1:])
