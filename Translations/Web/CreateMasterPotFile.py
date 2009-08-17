#!/usr/bin/python
# -*- coding: utf-8 -*-;

# Python script to create the POT file for the website
#
# Copyright 2009 Tim Gerundt <tim@gerundt.de>
#
# This file is part of WinMerge. WinMerge is free software under the terms of the
# GNU General Public License. You should have received a copy of the license
# along with WinMerge.  If not, see <http://www.gnu.org/licenses/>.
#
# $Id$

from os import sep
from os.path import abspath, isfile, join, splitext, walk
from time import strftime
import string
import re

def getPhpFiles(path):
    ''' Get all php files from a folder and his subfolders '''
    phpfiles = []
    walk(path, walkPhpFiles, phpfiles)
    return phpfiles

def walkPhpFiles(phpfiles, dirpath, itemnames):
    ''' Helper function for getPhpFiles() '''
    for itemname in itemnames: #For all dir items...
        fullitempath = abspath(join(dirpath, itemname))
        if isfile(fullitempath): #If a file...
            filename = splitext(itemname)
            if str.lower(filename[1]) == '.php' or str.lower(filename[1]) == '.inc': #If a PHP file...
                phpfiles.append(fullitempath)

def getTranslationsFromPhpFile(filepath, translations):
    ''' Get the translations from a php file '''
    rGettext = re.compile('_e?\([\'](.+?)[\']', re.DOTALL)
    rGettextTestMultiLine = re.compile('_e?\([\']([^\'\n]+)\n')
    
    phpfile = open(filepath, 'r')
    lines = phpfile.readlines()
    phpfile.close()
    
    i = 0
    for line in lines: #For all lines...
        #--------------------------------------------------------------------------------
        # Multi-line translations...
        #--------------------------------------------------------------------------------
        tmps = rGettextTestMultiLine.findall(line)
        if tmps: #If found a multi-line gettext function...
            tmps = rGettext.findall("".join(lines[i:]))
            for tmp in tmps: #For all translations...
                if string.find(tmp, '\n') > 0: #If a multi-line translation...
                    translation = string.replace(tmp, '\n', '\\n')
                    if translation in translations: #If the translation is already exists...
                        translations[translation] += [(filepath, i)]
                    else: #If the translation is NOT already exists...
                        translations[translation] = [(filepath, i)]
                    break #Use only the FIRST multi-line translation!
        #--------------------------------------------------------------------------------
        
        #--------------------------------------------------------------------------------
        # Normal translations...
        #--------------------------------------------------------------------------------
        i += 1
        tmps = rGettext.findall(line)
        if tmps: #If found a gettext function...
            for tmp in tmps: #For all translations...
                translation = tmp
                if translation in translations: #If the translation is already exists...
                    translations[translation] += [(filepath, i)]
                else: #If the translation is NOT already exists...
                    translations[translation] = [(filepath, i)]
        #--------------------------------------------------------------------------------

def main():
    translations = {}
    php_dir = abspath('../../Web')
    php_files = getPhpFiles(php_dir)
    for php_file in php_files: #For all php files...
        getTranslationsFromPhpFile(php_file, translations)
    
    potfile = open('en-US.pot', 'w')
    potfile.write('# This file is part from WinMerge <http://winmerge.org/>\n')
    potfile.write('# Released under the "GNU General Public License"\n')
    potfile.write('#\n')
    potfile.write('# ID line follows -- this is updated by SVN\n')
    potfile.write('# $' + 'Id' + '$\n')
    potfile.write('#\n')
    potfile.write('msgid ""\n')
    potfile.write('msgstr ""\n')
    potfile.write('"Project-Id-Version: WinMerge\\n"\n')
    potfile.write('"Report-Msgid-Bugs-To: http://bugs.winmerge.org\\n"\n')
    potfile.write('"POT-Creation-Date: %s\\n"\n' % strftime('%Y-%m-%d %H:%M+0000'))
    potfile.write('"PO-Revision-Date: \\n"\n')
    potfile.write('"Last-Translator: \\n"\n')
    potfile.write('"Language-Team: English <winmerge-translate@lists.sourceforge.net>\\n"\n')
    potfile.write('"MIME-Version: 1.0\\n"\n')
    potfile.write('"Content-Type: text/plain; charset=utf-8\\n"\n')
    potfile.write('"Content-Transfer-Encoding: 8bit\\n"\n')
    potfile.write('"X-Poedit-Language: English\\n"\n')
    potfile.write('"X-Poedit-SourceCharset: utf-8\\n"\n')
    potfile.write('"X-Poedit-Basepath: ../../Web/\\n"\n')
    potfile.write('\n')
    for translation in translations: #For all translations...
        references = translations[translation]
        for reference in references: #For all references...
            potfile.write('#: %s:%u\n' % (reference[0].replace(php_dir + sep, ''), reference[1]))
        potfile.write('#, c-format\n')
        potfile.write('msgid "%s"\n' % (translation.replace('"', '\\"')))
        potfile.write('msgstr ""\n')
        potfile.write('\n')
    potfile.close()

# MAIN #
if __name__ == "__main__":
    main()