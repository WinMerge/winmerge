#
# The MIT License
# Copyright (c) 2007 Kimmo Varis
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

# This script prepares a WinMerge release
# Tasks it does:
# - cleans previous build files from folders
# - sets version number for resources
# - updates POT and PO files
# - builds WinMerge.exe and WinMergeU.exe
# - builds pcre, expat and scew (through project file)
# - builds 32-bit ShellExtension targets
# - builds user manual
# - creates per-version distribution folder
# - exports SVN sources to distribution folder
# - creates binary distribution folder

#Tasks not done (TODO?):
# - building 64-bit ShellExtension
# - creating packages from source and binary folders
# - creating installer
# - running virus check
# - creating SHA-1 hashes for distributed files

# Tools needed:
# - Python 2.5 :)
# - Subversion command line binaries

# CONFIGURATION:
# Set these variables to match your environment and folders you want to use

# Subversion binary - set this to absolute path to svn,exe
svn_binary = 'C:\\Program Files\\Subversion\\bin\\svn.exe'
# Path to VS binary to run - set this to Visual Studio IDE executable folder
vs_path = 'C:\\Program Files\\Microsoft Visual Studio .NET 2003\\Common7\\IDE'
# VS IDE executable - set this to command starting Visual Studio IDE
vs_bin = 'devenv.com'
# Relative path where to create a release folder
dist_root_folder = '/WinMerge/Releases'

# END CONFIGURATION - you don't need to edit anything below...

from subprocess import *
import os
import os.path
import sys
import getopt
import shutil

def cleanup_build():
    """Deletes all build files around folder structure"""

    print 'Delete old build files...'
    winmerge_temp = 'BuildTmp'
    if os.path.exists(winmerge_temp):
        print 'Remove folder %s' % winmerge_temp
        shutil.rmtree(winmerge_temp, True)
    else:
        print 'Skipping folder %s' % winmerge_temp
    
    try:
        print 'Remove ANSI files'
        if os.path.exists('build/mergerelease/WinMerge.exe'):
            os.remove('build/mergerelease/WinMerge.exe')
        if os.path.exists('build/mergerelease/ShellExtension.dll'):
            os.remove('build/mergerelease/ShellExtension.dll')
        print 'Remove Unicode files'
        if os.path.exists('build/mergeunicoderelease/WinMergeU.exe'):
            os.remove('build/mergeunicoderelease/WinMergeU.exe')
        if os.path.exists('build/mergeunicoderelease/ShellExtensionU.dll'):
            os.remove('build/mergeunicoderelease/ShellExtensionU.dll')
    except:
        print 'Error deleting files: '
        print sys.exc_info()[0]
        return False
    return True

# Commented out as it seems running prelink and postbuild bat files
# don't work when build is started from this script.
# So you need to build expat, scew and pcre with VS first!

#    expat_temps = ['Externals/expat/lib/Release',
#                   'Externals/expat/lib/Release-w',
#                   'Externals/expat/lib/Release_static',
#                   'Externals/expat/lib/Release_static-w',
#                   'build/expat']
#    for path in expat_temps:
#        if os.path.exists(path):
#            print 'Remove folder %s' % path
#            shutil.rmtree(path, True) 
#        else:
#            print 'Skipping folder %s' % path
#
#    scew_temps = ['Externals/scew/win32/lib',
#                  'Externals/scew/win32/obj']
#    for path in scew_temps:
#        if os.path.exists(path):
#            print 'Remove folder: %s' % path
#            shutil.rmtree(path, True) 
#
#    pcre_temps = ['Externals/pcre/bin',
#                  'Externals/pcre/dll_pcre/o',
#                  'Externals/pcre/lib_pcre/o',
#                  'Externals/pcre/lib_pcreposix/o',
#                  'Externals/pcre/pcretest/o',
#                  'build/pcre']
#    for path in pcre_temps:
#        if os.path.exists(path):
#            print 'Remove folder %s' % path
#            shutil.rmtree(path, True) 

def set_resource_version(version):
    """Sets the version number to the resource."""

    print 'Update version number to resource(s)...'
    call(['cscript', 'Src/SetResourceVersions.wsf', version])

def setup_translations():
    """Updates translation files by running scripts in Src/Languages."""

    # Scripts must be run from the directory where they reside
    curdir = os.getcwd()
    os.chdir('Src/Languages')
    call(['cscript', '/nologo', 'CreateMasterPotFile.vbs'])
    call(['cscript', '/nologo', 'UpdatePoFilesFromPotFile.vbs'])
    os.chdir(curdir)

def get_and_create_dist_folder(folder):
    """Formats a folder name for version-specific distribution folder
    and creates the folder."""

    dist_folder = os.path.join(dist_root_folder, folder)
    if os.path.exists(dist_folder):
        print 'Folder: ' + dist_folder + ' already exists!'
        print 'If you want to re-create this version, remove folder first!'
        return ''
    else:
        print 'Create distribution folder: ' + dist_folder
        os.mkdir(dist_folder)
        return dist_folder

def get_src_dist_folder(dist_folder, folder):
    """Format a source distribution folder path."""

    dist_src = os.path.join(dist_folder, folder + '-src')
    return dist_src

def svn_export(dist_src_folder):
    """Exports sources to distribution folder."""

    print 'Exporting sources to ' + dist_src_folder
    call([svn_binary, 'export', '--non-interactive', '.', dist_src_folder])

def cleanup_dlls_from_plugins(dist_src_folder):
    """Remove compiled plugin dll files from source distribution folders."""

    dll_folder = os.path.join(dist_src_folder, 'Plugins/dlls')
    files = os.listdir(dll_folder)

    print 'Removing dll files from plugin folder...'
    for cur_file in files:
        fullpath = os.path.join(dll_folder, cur_file)
        if os.path.isfile(fullpath):
            file_name, file_ext = os.path.splitext(cur_file)
            if (file_ext == '.dll'):
                os.remove(fullpath)

def build_targets():
    """Builds all WinMerge targets."""

    vs_cmd = os.path.join(vs_path, vs_bin)

    build_winmerge(vs_cmd)
    build_shellext(vs_cmd)

def build_winmerge(vs_cmd):
    """Builds WinMerge executable targets."""

    cur_path = os.getcwd()
    solution_path = os.path.join(cur_path, 'Src\\Merge.vcproj')
    #print sol_path

    # devenv Src\Merge.dsp /rebuild Release
    print 'Build WinMerge executables...'
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)
    call([vs_cmd, solution_path, '/rebuild', 'UnicodeRelease'], shell=True)

def build_shellext(vs_cmd):
    """Builds 32-bit ShellExtension."""

    cur_path = os.getcwd()
    solution_path = os.path.join(cur_path, 'ShellExtension\\ShellExtension.vcproj')

    # devenv Src\Merge.dsp /rebuild Release
    print 'Build ShellExtension dlls...'
    call([vs_cmd, solution_path, '/rebuild', 'Release MinDependency'])
    call([vs_cmd, solution_path, '/rebuild', 'Unicode Release MinDependency'])

def build_manual():
    """Builds manual's HTML Help (CHM) version."""

    curdir = os.getcwd()
    os.chdir('Docs/Users/Manual/build')
    print 'Build HTML Help (CHM) manual...' 
    call(['build_htmlhelp.bat'])
    print 'Manual build finished.'
    os.chdir(curdir)

def get_and_create_bin_folder(dist_folder, folder):
    """Formats and creates binary distribution folder."""

    bin_folder = os.path.join(dist_folder, folder + '-exe')
    print 'Create binary distribution folder: ' + bin_folder
    os.mkdir(bin_folder)
    return bin_folder

def create_bin_folders(bin_folder, dist_src_folder):
    """Creates binary distribution folders."""

    cur_path = os.getcwd()
    os.chdir(bin_folder)
    print 'Create binary distribution folder structure...'
    lang_folder = os.path.join(bin_folder, 'Languages')
    os.mkdir(lang_folder)
    doc_folder = os.path.join(bin_folder, 'Docs')
    os.mkdir(doc_folder)
    filters_folder = os.path.join(bin_folder, 'Filters')
    plugins_folder = os.path.join(bin_folder, 'MergePlugins')
    os.chdir(cur_path)

    print 'Copying files to binary distribution folder...'
    shutil.copy('build/mergerelease/WinMerge.exe', bin_folder)
    shutil.copy('build/mergeunicoderelease/WinMergeU.exe', bin_folder)

    shutil.copy('build/mergerelease/ShellExtension.dll', bin_folder)
    shutil.copy('build/mergeunicoderelease/ShellExtensionU.dll', bin_folder)
    shutil.copy('build/mergeunicoderelease/MergeLang.dll', bin_folder)
    shutil.copy('build/shellextensionx64/ShellExtensionX64.dll', bin_folder)
    shutil.copy('ShellExtension/Register.bat', bin_folder)
    shutil.copy('ShellExtension/UnRegister.bat', bin_folder)

    shutil.copy('build/pcre/pcre.dll', bin_folder)
    shutil.copy('build/expat/libexpat.dll', bin_folder)

    copy_po_files(lang_folder)
    filter_orig = os.path.join(dist_src_folder, 'Filters')
    shutil.copytree(filter_orig, filters_folder, False)

    # Copy compiled plugins dir and rename it
    plugin_dir = os.path.join(bin_folder, 'dlls')
    plugin_orig = os.path.join(dist_src_folder, 'Plugins/dlls')
    shutil.copytree(plugin_orig, plugin_dir, False)
    os.rename(plugin_dir, plugins_folder)

    shutil.copy('build/Manual/htmlhelp/WinMerge.chm', doc_folder)

    shutil.copy('Docs/Users/ReleaseNotes.html', doc_folder)
    shutil.copy('Docs/Users/ReadMe.txt', bin_folder)
    shutil.copy('Docs/Users/ChangeLog.txt', bin_folder)
    shutil.copy('Docs/Users/Contributors.txt', bin_folder)
    shutil.copy('Docs/Users/Files.txt', bin_folder)

def copy_po_files(dest_folder):
    """Copies all PO files to destination folder."""

    lang_folder = 'Src/Languages'
    files = os.listdir(lang_folder)

    print 'Copying PO files to binary folder...'
    for cur_file in files:
        fullpath = os.path.join(lang_folder, cur_file)
        if os.path.isfile(fullpath):
            file_name, file_ext = os.path.splitext(cur_file)
            if (file_ext == '.po'):
                shutil.copy(fullpath, dest_folder)

def get_and_create_runtimes_folder(dist_folder, version):
    """Formats and creates runtimes distribution folder."""

    runtimes_folder = os.path.join(dist_folder, 'Runtimes-' + version)
    print 'Create runtimes distribution folder: ' + runtimes_folder
    os.mkdir(runtimes_folder)
    return runtimes_folder

def create_runtime_folder(runtimes_folder):
    """Copy runtime files to distribution folder."""

    shutil.copy('Installer/Runtimes/mfc71.dll', runtimes_folder)
    shutil.copy('Installer/Runtimes/mfc71u.dll', runtimes_folder)
    shutil.copy('Installer/Runtimes/msvcp71.dll', runtimes_folder)
    shutil.copy('Installer/Runtimes/msvcr71.dll', runtimes_folder)

def find_winmerge_root():
    """Find WinMerge tree root folder from where to run rest of the script."""
    
    # If we find Src and Filters -subfolders we are in root 
    if os.path.exists('Src') and os.path.exists('Filters'):
        return True
    
    # Check if we are in /Tools/Scripts
    if os.path.exists('../../Src') and os.path.exists('../../Filters'):
        os.chdir('../../')
        return True
    
    return False

def usage():
    print 'WinMerge release script.'
    print 'Usage: release [-h] [-v: n]'
    print '  where:'
    print '    -h, --help print this help'
    print '    -v: n, --version= n set release version'
    print '  For example: release -v: 2.7.7.1'

def main(argv):
    version = '0.0.0.0'
    if len(argv) > 0:
        opts, args = getopt.getopt(argv, "hv:", [ "help", "version="])
        
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                usage()
                sys.exit()
            if opt in ("-v", "--version"):
                version = arg
                print "Start building WinMerge release version " + version

    # Check we are running from correct folder (and go to root if found)
    if find_winmerge_root() == False:
        print 'ERROR: Cannot find WinMerge root folder!'
        print 'The script must be run from WinMerge tree\'s root folder'
        print '(which has Src- and Filter -folders as subfolders) or from'
        print 'Tools/Scripts -folder (where this script is located).'
        sys.exit()

    if cleanup_build() == False:
        sys.exit()

    version_folder = 'WinMerge-' + version
    dist_folder = get_and_create_dist_folder(version_folder)
    if dist_folder == '':
        sys.exit(1)
    dist_src_folder = get_src_dist_folder(dist_folder, version_folder)
    svn_export(dist_src_folder)

    set_resource_version(version)
    setup_translations()
    
    build_targets()
    build_manual()
    
    dist_bin_folder = get_and_create_bin_folder(dist_folder, version_folder)
    create_bin_folders(dist_bin_folder, dist_src_folder)

    # Do the cleanup after creating binary distrib folders, as some files
    # and folders are copied from source folders to binary folders.
    cleanup_dlls_from_plugins(dist_src_folder)

    runtimes_folder = get_and_create_runtimes_folder(dist_folder, version)
    create_runtime_folder(runtimes_folder)

    print 'WinMerge release script ready!'


### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
