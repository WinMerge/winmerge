#!/usr/bin/python
# -*- coding: utf-8 -*-;

# The MIT License
# 
# Copyright (c) 2009 Tim Gerundt <tim@gerundt.de>
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

# Python script to get the status of the translations

import os
import os.path
import string
import re
import time

class TranslationsStatus(object):
    def __init__(self):
        self.__projects = []
    
    @property
    def projects(self):
        return self.__projects
    
    def clear(self):
        self.__projects = []
    
    def addProject(self, project):
        self.__projects.append(project)
    
    def addProject(self, name, potfile, podir):
        self.__projects.append(TranslationsStatusProject(name, potfile, podir))
    
    def writeToXmlFile(self, xmlpath):
        xmlfile = open(xmlpath, 'w')
        xmlfile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
        xmlfile.write('<status>\n')
        xmlfile.write('  <update>%s</update>\n' % (time.strftime('%Y-%m-%d')))
        for project in self.__projects: #For all projects...
            xmlfile.write('  <translations project="%s">\n' % (project.name))
            for status1 in project.status: #For all status...
                xmlfile.write('    <translation>\n')
                xmlfile.write('      <language>%s</language>\n' % (status1.language))
                xmlfile.write('      <file>%s</file>\n' % (status1.filename))
                if status1.filetype == 'PO': #If a PO file...
                    xmlfile.write('      <update>%s</update>\n' % (status1.porevisiondate[0:10]))
                    xmlfile.write('      <strings>\n')
                    xmlfile.write('        <count>%u</count>\n' % (status1.count))
                    xmlfile.write('        <translated>%u</translated>\n' % (status1.translated))
                    xmlfile.write('        <fuzzy>%u</fuzzy>\n' % (status1.fuzzy))
                    xmlfile.write('        <untranslated>%u</untranslated>\n' % (status1.untranslated))
                    xmlfile.write('      </strings>\n')
                else: #If a POT file...
                    xmlfile.write('      <update>%s</update>\n' % (status1.potcreationdate[0:10]))
                    xmlfile.write('      <strings>\n')
                    xmlfile.write('        <count>%u</count>\n' % (status1.count))
                    xmlfile.write('        <translated>%u</translated>\n' % (status1.count))
                    xmlfile.write('        <fuzzy>0</fuzzy>\n')
                    xmlfile.write('        <untranslated>0</untranslated>\n')
                    xmlfile.write('      </strings>\n')
                if status1.translators: #If translators exists...
                    xmlfile.write('      <translators>\n')
                    for translator in status1.translators: #For all translators...
                        if (translator.ismaintainer): #If maintainer...
                            xmlfile.write('        <translator maintainer="1">\n')
                        else: #If NOT maintainer...
                            xmlfile.write('        <translator>\n')
                        xmlfile.write('          <name>%s</name>\n' % (translator.name))
                        if (translator.mail): #If mail address exists...
                            xmlfile.write('          <mail>%s</mail>\n' % (translator.mail))
                        xmlfile.write('        </translator>\n')
                    xmlfile.write('      </translators>\n')
                xmlfile.write('    </translation>\n')
            xmlfile.write('  </translations>\n')
        xmlfile.write('</status>\n')
        xmlfile.close()

class TranslationsStatusProject(object):
    def __init__(self, name, potfile, podir):
        self.__name = name
        self.__status = []
        
        #PO files...
        for itemname in os.listdir(podir): #For all dir items...
            fullitempath = os.path.abspath(os.path.join(podir, itemname))
            if os.path.isfile(fullitempath): #If a file...
                filename = os.path.splitext(itemname)
                if str.lower(filename[1]) == '.po': #If a PO file...
                    self.__status.append(PoStatus(fullitempath))
        
        #POT file...
        self.__status.append(PoStatus(os.path.abspath(potfile)))
    
    @property
    def name(self):
        return self.__name
    
    @property
    def status(self):
        return self.__status

class PoStatus(object):
    def __init__(self, filepath):
        self.__filepath = filepath
        self.__count = 0
        self.__translated = 0
        self.__untranslated = 0
        self.__fuzzy = 0
        self.__porevisiondate = ''
        self.__potcreationdate = ''
        self.__translators = []
        
        if os.access(filepath, os.R_OK): #If PO(T) file can read...
          reMsgId = re.compile('^msgid "(.*)"$', re.IGNORECASE)
          reMsgStr = re.compile('^msgstr "(.*)"$', re.IGNORECASE)
          reMsgContinued = re.compile('^"(.*)"$', re.IGNORECASE)
          reTranslator = re.compile('^# \* (.*)$', re.IGNORECASE)
          rePoRevisionDate = re.compile('PO-Revision-Date: ([0-9 :\+\-]+)', re.IGNORECASE)
          rePotCreationDate = re.compile('POT-Creation-Date: ([0-9 :\+\-]+)', re.IGNORECASE)
          
          iMsgStarted = 0
          sMsgId = ''
          sMsgStr = ''
          bIsFuzzy = False
          bIsMaintainer = False
          
          pofile = open(filepath, 'r')
          for line in pofile: #For all lines...
              line = line.strip()
              if line: #If NOT empty line...
                  if line[0] != '#': #If NOT comment line...
                      if reMsgId.findall(line): #If "msgid"...
                          iMsgStarted = 1
                          tmp = reMsgId.findall(line)
                          sMsgId = tmp[0]
                      elif reMsgStr.findall(line): #If "msgstr"...
                          iMsgStarted = 2
                          tmp = reMsgStr.findall(line)
                          sMsgStr = tmp[0]
                      elif reMsgContinued.findall(line): #If "msgid" or "msgstr" continued...
                          tmp = reMsgContinued.findall(line)
                          if iMsgStarted == 1:
                              sMsgId = sMsgId + tmp[0]
                          elif iMsgStarted == 2:
                              sMsgStr = sMsgStr + tmp[0]
                  else: #If comment line...
                      iMsgStarted = -1
                      if line.startswith('#,'): #If "Reference" line...
                          if line.find('fuzzy') > -1: #If "fuzzy"...
                              bIsFuzzy = True
                      elif line.startswith('# Maintainer:'): #If maintainer list starts...
                          bIsMaintainer = True
                      elif line.startswith('# Translators:'): #If translators list starts...
                          bIsMaintainer = False
                      elif reTranslator.findall(line): #If translator/maintainer...
                          translator = reTranslator.findall(line)
                          if re.findall('\<(.*)\>', translator[0]): #If mail address exists...
                              tmp = re.findall('(.*) \<(.*)\>', translator[0])
                              sName = tmp[0][0]
                              sMail = tmp[0][1]
                          else: #If mail address NOT exists...
                              sName = translator[0]
                              sMail = ''
                          self.__translators.append(PoTranslator(sName, sMail, bIsMaintainer))
              else: #If empty line...
                  iMsgStarted = 0
              
              if iMsgStarted == 0: #If NOT inside a translation...
                  if sMsgId != '':
                      self.__count += 1
                      if bIsFuzzy == False: #If NOT a fuzzy translation...
                          if sMsgStr != '':
                              self.__translated += 1
                          else:
                              self.__untranslated += 1
                      else: #If a fuzzy translation...
                          self.__fuzzy += 1
                  elif sMsgStr != '':
                      tmp = rePoRevisionDate.findall(sMsgStr)
                      if tmp: #If "PO-Revision-Date"...
                          self.__porevisiondate = tmp[0]
                      tmp = rePotCreationDate.findall(sMsgStr)
                      if tmp: #If "POT-Creation-Date"...
                          self.__potcreationdate = tmp[0]
                  sMsgId = ''
                  sMsgStr = ''
                  bIsFuzzy = False
          pofile.close()
    
    @property
    def filepath(self):
        return self.__filepath
    
    @property
    def filename(self):
        return os.path.basename(self.__filepath)
    
    @property
    def filetype(self):
        filename = os.path.splitext(self.__filepath)
        if str.lower(filename[1]) == '.po': #If a PO file...
            return 'PO'
        elif str.lower(filename[1]) == '.pot': #If a POT file...
            return 'POT'
        return None
    
    @property
    def language(self):
        return os.path.splitext(self.filename)[0]
    
    @property
    def count(self):
        return self.__count
    
    @property
    def translated(self):
        return self.__translated
    
    @property
    def untranslated(self):
        return self.__untranslated
    
    @property
    def fuzzy(self):
        return self.__fuzzy
    
    @property
    def porevisiondate(self):
        return self.__porevisiondate
    
    @property
    def potcreationdate(self):
        return self.__potcreationdate
    
    @property
    def translators(self):
        return self.__translators

class PoTranslator(object):
    def __init__(self, name, mail, ismaintainer):
        self.name = name
        self.mail = mail
        self.ismaintainer = ismaintainer

def main():
    status = TranslationsStatus()
    status.addProject('WinMerge', 'WinMerge/English.pot', 'WinMerge')
    status.addProject('ShellExtension', 'ShellExtension/English.pot', 'ShellExtension')
    status.writeToXmlFile('TranslationsStatus.xml')

# MAIN #
if __name__ == "__main__":
    main()