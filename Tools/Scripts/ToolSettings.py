#
# The MIT License
# Copyright (c) 2007-2010 Kimmo Varis
# Copyright (c) 2008 Matthias Mayer
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

# $Id: ToolSettings.py 6827 2009-06-05 21:20:24Z kimmov $

# This is a tools configuration script. It handles compiling, version control
# etc tool configuration for scripts calling them. It allows e.g switching the
# compiler by changing value in the ini file.

import os
import os.path
import platform
import sys
import ConfigParser

class ToolSettings:
    def __init__(self):
        # Set default values. Need to make difference for 64-bit Windows paths
        self.vs_path = ''
        if platform.architecture()[0] == '64bit':
            self.svn_binary = r'C:\Program Files (x86)\Subversion\bin\svn.exe'
            self.innosetup_path = r'C:\Program Files (x86)\Inno Setup 5'
            self.nsis_path = r'C:\Program Files (x86)\NSIS'
            self.vs_path7 = r'C:\Program Files (x86)\Microsoft Visual Studio .NET 2003'
            self.vs_path8 = r'C:\Program Files (x86)\Microsoft Visual Studio 8'
            self.vs_path9 = r'C:\Program Files (x86)\Microsoft Visual Studio 9.0'
            self.vs_path10 = r'C:\Program Files (x86)\Microsoft Visual Studio 10.0'
        else:
            self.svn_binary = r'C:\Program Files\Subversion\bin\svn.exe'
            self.innosetup_path = r'C:\Program Files\Inno Setup 5'
            self.nsis_path = r'C:\Program Files\NSIS'
            self.vs_path7 = r'C:\Program Files\Microsoft Visual Studio .NET 2003'
            self.vs_path8 = r'C:\Program Files\Microsoft Visual Studio 8'
            self.vs_path9 = r'C:\Program Files\Microsoft Visual Studio 9.0'
            self.vs_path10 = r'C:\Program Files\Microsoft Visual Studio 10.0'

        # Checkout sources from local workspace or from SVN server (give URL)
        self.source = 'workspace'
        self.vs_version = 2008

    def create_ini(self, filename):
        config = ConfigParser.RawConfigParser()
        sect = 'RUNTIME'
        if os.path.exists('Tools.ini'):
            config.readfp(open(filename))
        if not config.has_section(sect):
            config.add_section(sect)
        if not config.has_option(sect, 'type'):
            config.set(sect, 'type', 'VSXXXX')
        if not config.has_option(sect, 'VSStudio'):
            config.set(sect, 'VSStudio', self.vs_version)
        if not config.has_option(sect, 'Source'):
            config.set(sect, 'Source', self.source)
        if not config.has_option(sect, 'svn_binary'):
            config.set(sect, 'svn_binary', self.svn_binary)
        if not config.has_option(sect, 'vs_path7'):
            config.set(sect, 'vs_path7', self.vs_path7)
        if not config.has_option(sect, 'vs_path8'):
            config.set(sect, 'vs_path8', self.vs_path8)
        if not config.has_option(sect, 'vs_path9'):
            config.set(sect, 'vs_path9', self.vs_path9)
        if not config.has_option(sect, 'vs_path10'):
            config.set(sect, 'vs_path10', self.vs_path10)
        if not config.has_option(sect, 'innosetup_path'):
            config.set(sect, 'innosetup_path', self.innosetup_path)
        if not config.has_option(sect, 'nsis_path'):
            config.set(sect, 'nsis_path', self.nsis_path)

        # Writing our configuration file to 'Tools.ini'
        with open(filename, 'w') as configfile:
            config.write(configfile)

    def read_ini(self, filename):
        config = ConfigParser.RawConfigParser()
        if not os.path.exists(filename):
            # If the config file didn't exist, we create a new file and ask
            # user to edit the config and re-run the script. This is because
            # our defaults probably don't match user's environment.
            self.create_ini(filename)
            print 'New configuration file created: ' + filename
            print 'Please edit the file to match your configuration and re-run the script.'
            sys.exit()

        config.readfp(open(filename))
        self.svn_binary = config.get('RUNTIME', 'svn_binary')
        self.vs_path7 = config.get('RUNTIME', 'vs_path7')
        self.vs_path8 = config.get('RUNTIME', 'vs_path8')
        self.vs_path9 = config.get('RUNTIME', 'vs_path9')
        self.vs_path10 = config.get('RUNTIME', 'vs_path10')
        self.innosetup_path = config.get('RUNTIME', 'innosetup_path')
        self.nsis_path = config.get('RUNTIME', 'nsis_path')
        self.source = config.get('RUNTIME', 'Source') 
        self.vs_version = config.getint('RUNTIME', 'VSStudio')

        if self.vs_version == 2003:
            self.vs_path = self.vs_path7
        elif self.vs_version == 2005:
            self.vs_path = self.vs_path8
        elif self.vs_version == 2008:
            self.vs_path = self.vs_path9
        elif self.vs_version == 2010:
            self.vs_path = self.vs_path10
