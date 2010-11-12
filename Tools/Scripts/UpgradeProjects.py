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

# Copyright (c) 2008-2010 Kimmo Varis <kimmov@winmerge.org>

# $Id$

# A script for upgrading VS solution- and project-files from VS2003 to newer
# VS versions. This script calls VS binary to upgrade every file listed. If the
# file is solution file, then all projects in the solution are also upgraded.
#
# For more info about using VS command line, see:
# http://www.devsource.com/c/a/Using-VS/Working-at-the-Visual-Studio-Command-Line/1/

import getopt
import os
import subprocess
import sys

import cleanup_backups
import fix_manifest
import ToolSettings

# The version of the script
script_version = 0.4

# global settings class instance
tools = ToolSettings.ToolSettings()

solutions = [r'Externals\expat\expat.sln',
    r'WinMerge.sln']

projects = [r'Externals\scew\win32\scew.vcproj',
    r'Externals\pcre\Win32\pcre.vcproj',
    r'ShellExtension\ShellExtension.vcproj']

# These projects need the manifest file fix
manifest_projects = [r'Src\Merge.vcproj']

def upgrade_projects(root_path):
    vs_binary = os.path.join(tools.vs_path, 'Common7/IDE')
    vs_binary = os.path.join(vs_binary, 'devenv.com')
    
    for solution in solutions:
        sol_file = os.path.join(root_path, solution)
        print 'Upgrading VS solution file: ' + sol_file
        subprocess.call([vs_binary, sol_file, '/Upgrade'], shell = True)
        cleanup(sol_file)

    for project in projects:
        proj_file = os.path.join(root_path, project)
        print 'Upgrading project file: ' + proj_file
        subprocess.call([vs_binary, proj_file, '/Upgrade'], shell = True)
        cleanup(proj_file)

def fix_proj_manifests(root_path):
    for project in manifest_projects:
        proj_file = os.path.join(root_path, project)
        fix_manifest.process_project_file(proj_file)

def cleanup(updatefile):
    folder = os.path.dirname(updatefile)
    cleanup_backups.cleanupfolder(folder)

def usage():
    '''Print script usage information.'''

    print 'UpgradeProjects.py - version ' + str(script_version)
    print 'Script to upgrade VS solution/project files.'
    print 'Usage: UpgradeProjects.py [-h] [path]'
    print 'Where:'
    print '  -h, --help Print this help.'
    print '  path Root path for WinMerge.'
    print 'For example: UpgradeProjects.py ../..'
    print '  which upgrades current project when run from Scripts folder.'

def main(argv):
    root_path = ''
    if len(argv) > 0:
        opts, args = getopt.getopt(argv, 'h', ['help'])

        for opt, arg in opts:
            if opt in ('-h', '--help'):
                usage()
                sys.exit()
         
        if len(args) == 1:
            rel_path = args[0]
            root_path = os.path.abspath(rel_path)

    # If not root path given, use current folder as root path
    if root_path == '':
        root_path = os.getcwd()

    if not os.path.exists(root_path):
        print 'ERROR: Cannot find path: ' + root_path
        sys.exit()

    tools.read_ini('Tools.ini')

    print 'Upgrading VS solution- and project-file in folder: ' + root_path
    upgrade_projects(root_path)
    fix_proj_manifests(root_path)

# MAIN #
if __name__ == "__main__":
    main(sys.argv[1:])

