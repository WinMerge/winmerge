#
# The MIT License
# Copyright (c) 2008 Kimmo Varis
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

# $Id$

# This script removes additional data that TortoiseSVN adds to the patch
# files. After running this script for TortoiseSVN patch the patch file
# can be applied with GNU patch.
#
# This patch removes:
# - Index -lines  containing filename to patch
# - Index-line following separator line
# - Revision numbers from actual patch filename lines
#
# Original patch file is kept with .bak filename extension.

import getopt
import os
import shutil
import sys

def process_patch(filename, backup):
    '''Process one patch file. Create a bak file from original file and
    temp file for processing. After successfully processing rename temp
    file to original file.
    
    filename: patch file's filename
    backup: create a backup file?
    return: True if conversion succeeded, False otherwise
    '''
    
    outfile = filename + '.tmp'
    bakfile = filename + '.bak'
    
    if not os.path.exists(filename):
        print 'Patch file ' + filename + ' not found!'
        return False

    if backup == True:
        shutil.copyfile(filename, bakfile)
    
    ret = clean_patch(filename, outfile)
    if ret == True:
        shutil.move(outfile, filename)
    else:
        os.remove(outfile)
        if os.path.exists(bakfile):
            os.remove(bakfile)
        return False
    return True

def clean_patch(infile, outfile):
    '''Remove TortoiseSVN specifics from a patch file.
    
    infile: filename of patch file to convert
    outfile: new file to create for converted file
    return: True if the conversion succeeds, False otherwise
    '''

    try:
        f = open(infile, 'r')
    except IOError, (errno, strerror):
        print 'Cannot open file ' + infile + ' for reading'
        print 'Error: ' + strerror
        return False

    try:
        f2 = open(outfile, 'w')
    except IOError, (errno, strerror):
        print 'Cannot open file ' + outfile + ' for writing'
        print 'Error: ' + strerror
        return False

    sepline = False # next line *should* be a separator line
    infolines = 0 # GNU patch filename lines
    linenum = 0
    parse_err = False
    for line in f:
        linenum += 1

        # TSVN patches have additional 'Index' -lines with following
        # separator line. We remove these additional lines
        if line.startswith('Index: '):
            sepline = True
            continue
        if sepline == True:
            if not line.startswith('===='):
                print 'Unexpected line found in line ', linenum
                print 'This is not a TSVN patch file?'
                parse_err = True
                break
            sepline = False
            infolines = 2
            continue
        
        # Remove revision number (in parentheses) from filename lines
        # E.g. line
        # --- filename.c (revision 323)
        # is stripped to:
        # --- filename.c
        if infolines > 0:
            if line.startswith('---') or line.startswith('+++'):
                index = line.rfind('(')
                index2 = line.rfind(')')
                if index != -1 and index2 != -1:
                    line = line[:index].rstrip() + line[index2 + 1:]
                infolines -= 1
            else:
                print 'Unexpected line found in line ', linenum
                print 'This is not a TSVN patch file?'
                parse_err = True
                break
        
        f2.write(line)
        
    f.close()
    f2.close()

    if parse_err == True:
        return False
    return True

def usage():
    print 'TortoiseSVN patch file cleanup script.'
    print 'This script removes TSVN specific additions from patch files so that'
    print 'GNU patch can apply them.'
    print 'Usage: tsvn_patch [-b] filename'
    print '  where:'
    print '    -b, --nobak Do not create a backup file of original file'

def main(argv):
    filename = ""
    createbaks = True
    if len(argv) > 0:
        opts, args = getopt.getopt(argv, "hb", [ "help", "nobak"])
        
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                usage()
                sys.exit()
            if opt in ("-b", "--nobak"):
                createbaks = False

        if len(args) == 1:
            filename = args[0]
        else:
            usage()
            sys.exit()
    else:
        usage()
        sys.exit()

    ret = False
    if len(filename) > 0:
        ret = process_patch(filename, createbaks)
        
    if ret == True:
        print 'Patch file ' + filename + ' converted successfully!'

### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
