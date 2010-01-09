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

# Copyright (c) 2010 Kimmo Varis <kimmov@winmerge.org>

# $Id$

# A script for cleaning up Visual Studio backup files that get created when
# solution/project files are updated to new version of VS.

import getopt
import os
import shutil
import sys

# The version of the script
scriptversion = 0.1

def cleanupfolder(folder):
    reportfolder = os.path.join(folder, r'_UpgradeReport_Files')
    if os.path.exists(reportfolder):
        shutil.rmtree(reportfolder)

    logfile = r'UpgradeLog{0}.XML'
    logfile = os.path.join(folder, logfile.format(''))
    if os.path.exists(logfile):
        os.remove(logfile)

    index = 2;
    existed = True
    while existed == True:
        logfile = os.path.join(folder, logfile.format(index))
        if os.path.exists(logfile):
            os.remove(logfile)
        else:
            existed = False
        index += 1

def usage():
    '''Print script usage information.'''

    print 'cleanup_backups.py - version ' + str(scriptversion)
    print 'Script to cleanup VS solution/project update backup/log files.'
    print 'Usage: cleanup_backups.py [-h] [path]'
    print 'Where:'
    print '  -h, --help Print this help.'
    print '  path folder where solution/project file is.'
    print 'For example: cleanup_backups.py src'

def main(argv):
    rootfolder = ''
    if len(argv) > 0:
        opts, args = getopt.getopt(argv, 'h', ['help'])

        for opt, arg in opts:
            if opt in ('-h', '--help'):
                usage()
                sys.exit()
         
        if len(args) == 1:
            rel_path = args[0]
            rootfolder = os.path.abspath(rel_path)

    # If not root path given, use current folder as root path
    if rootfolder == '':
        rootfolder = os.getcwd()

    if not os.path.exists(rootfolder):
        print 'ERROR: Cannot find path: ' + rootfolder
        sys.exit()

    print 'Removing backups from folder: ' + rootfolder
    cleanupfolder(rootfolder)

# MAIN #
if __name__ == "__main__":
    main(sys.argv[1:])
