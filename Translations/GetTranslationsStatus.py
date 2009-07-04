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
        self._projects = []
    
    @property
    def projects(self):
        return self._projects
    
    @property
    def languages(self):
        temp = []
        for project in self._projects: #For all projects...
            for language in project.languages: #For all languages...
                if language not in temp: #If language NOT in list...
                    temp.append(language)
        temp.sort()
        return temp
    
    def clear(self):
        self._projects = []
    
    def addProject(self, project):
        self._projects.append(project)
    
    def writeToXmlFile(self, xmlpath):
        xmlfile = open(xmlpath, 'w')
        xmlfile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
        xmlfile.write('<status>\n')
        xmlfile.write('  <update>%s</update>\n' % (time.strftime('%Y-%m-%d')))
        for project in self._projects: #For all projects...
            xmlfile.write('  <translations project="%s">\n' % (project.name))
            for status1 in project.status: #For all status...
                if status1.template: #If a template file...
                    xmlfile.write('    <translation template="1">\n')
                    xmlfile.write('      <language>%s</language>\n' % (status1.language))
                    xmlfile.write('      <file>%s</file>\n' % (status1.filename))
                    xmlfile.write('      <update>%s</update>\n' % (status1.updatedate[0:10]))
                    xmlfile.write('      <strings>\n')
                    xmlfile.write('        <count>%u</count>\n' % (status1.count))
                    xmlfile.write('        <translated>%u</translated>\n' % (status1.count))
                    xmlfile.write('        <fuzzy>0</fuzzy>\n')
                    xmlfile.write('        <untranslated>0</untranslated>\n')
                    xmlfile.write('      </strings>\n')
                    xmlfile.write('    </translation>\n')
                else: #If NOT a template file...
                    xmlfile.write('    <translation>\n')
                    xmlfile.write('      <language>%s</language>\n' % (status1.language))
                    xmlfile.write('      <file>%s</file>\n' % (status1.filename))
                    xmlfile.write('      <update>%s</update>\n' % (status1.updatedate[0:10]))
                    xmlfile.write('      <strings>\n')
                    xmlfile.write('        <count>%u</count>\n' % (status1.count))
                    xmlfile.write('        <translated>%u</translated>\n' % (status1.translated))
                    xmlfile.write('        <fuzzy>%u</fuzzy>\n' % (status1.fuzzy))
                    xmlfile.write('        <untranslated>%u</untranslated>\n' % (status1.untranslated))
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

class Project(object):
    def __getitem__(self, key):
        for status in self._status: #For all status...
            if status.language == key:
                return status
        return None
    
    @property
    def name(self):
        return self._name
    
    @property
    def status(self):
        return self._status
    
    @property
    def languages(self):
        temp = []
        for status in self._status: #For all status...
            temp.append(status.language)
        temp.sort()
        return temp

class Status(object):
    @property
    def filepath(self):
        return self._filepath
    
    @property
    def filename(self):
        return os.path.basename(self._filepath)
    
    @property
    def template(self):
        return self._template
    
    @property
    def language(self):
        return os.path.splitext(self.filename)[0]
    
    @property
    def count(self):
        return self._count
    
    @property
    def translated(self):
        return self._translated
    
    @property
    def untranslated(self):
        return self._untranslated
    
    @property
    def fuzzy(self):
        return self._fuzzy
    
    @property
    def updatedate(self):
        return self._updatedate
    
    @property
    def translators(self):
        return self._translators

class Translator(object):
    def __init__(self, name, mail, ismaintainer):
        self.name = name
        self.mail = mail
        self.ismaintainer = ismaintainer

class PoProject(Project):
    def __init__(self, name, potfile, podir):
        self._name = name
        self._status = []
        
        #PO files...
        for itemname in os.listdir(podir): #For all dir items...
            fullitempath = os.path.abspath(os.path.join(podir, itemname))
            if os.path.isfile(fullitempath): #If a file...
                filename = os.path.splitext(itemname)
                if str.lower(filename[1]) == '.po': #If a PO file...
                    self._status.append(PoStatus(fullitempath, False))
        
        #POT file...
        self._status.append(PoStatus(os.path.abspath(potfile), True))

class PoStatus(Status):
    def __init__(self, filepath, template):
        self._filepath = filepath
        self._template = template
        self._count = 0
        self._translated = 0
        self._untranslated = 0
        self._fuzzy = 0
        self._porevisiondate = ''
        self._potcreationdate = ''
        self._translators = []
        
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
                          self._translators.append(Translator(sName, sMail, bIsMaintainer))
              else: #If empty line...
                  iMsgStarted = 0
              
              if iMsgStarted == 0: #If NOT inside a translation...
                  if sMsgId != '':
                      self._count += 1
                      if bIsFuzzy == False: #If NOT a fuzzy translation...
                          if sMsgStr != '':
                              self._translated += 1
                          else:
                              self._untranslated += 1
                      else: #If a fuzzy translation...
                          self._fuzzy += 1
                  elif sMsgStr != '':
                      tmp = rePoRevisionDate.findall(sMsgStr)
                      if tmp: #If "PO-Revision-Date"...
                          #TODO: Convert to date!
                          self._porevisiondate = tmp[0]
                      tmp = rePotCreationDate.findall(sMsgStr)
                      if tmp: #If "POT-Creation-Date"...
                          #TODO: Convert to date!
                          self._potcreationdate = tmp[0]
                  sMsgId = ''
                  sMsgStr = ''
                  bIsFuzzy = False
          pofile.close()
    
    @property
    def updatedate(self):
        if self._template: #if template...
            return self._potcreationdate
        else: #if NOT template...
            return self._porevisiondate

class InnoSetupProject(Project):
    def __init__(self, name, templatefile, translationsdir):
        self._name = name
        self._status = []
        
        #Translations files...
        for itemname in os.listdir(translationsdir): #For all dir items...
            fullitempath = os.path.abspath(os.path.join(translationsdir, itemname))
            if os.path.isfile(fullitempath): #If a file...
                filename = os.path.splitext(itemname)
                if str.lower(filename[1]) == '.isl': #If a ISL file...
                    if filename[0] != 'English': #If NOT the English file...
                        self._status.append(InnoSetupStatus(fullitempath, False))
        
        #Template file...
        self._status.append(InnoSetupStatus(os.path.abspath(templatefile), True))

class InnoSetupStatus(Status):
    def __init__(self, filepath, template):
        self._filepath = filepath
        self._template = template
        self._count = 0
        self._translated = 0
        self._untranslated = 0
        self._fuzzy = 0
        self._updatedate = ''
        self._translators = []
    
    @property
    def language(self):
        if self._template: #if template...
            return 'English'
        else: #if NOT template...
            filename = os.path.splitext(self.filename)
            return filename[0].replace('_', '')

class ReadmeProject(Project):
    def __init__(self, name, templatefile, translationsdir):
        self._name = name
        self._status = []
        
        #Translations files...
        for itemname in os.listdir(translationsdir): #For all dir items...
            fullitempath = os.path.abspath(os.path.join(translationsdir, itemname))
            if os.path.isfile(fullitempath): #If a file...
                filename = os.path.splitext(itemname)
                if str.lower(filename[1]) == '.txt': #If a TXT file...
                    self._status.append(ReadmeStatus(fullitempath, False))
        
        #Template file...
        self._status.append(ReadmeStatus(os.path.abspath(templatefile), True))

class ReadmeStatus(Status):
    def __init__(self, filepath, template):
        self._filepath = filepath
        self._template = template
        self._count = 0
        self._translated = 0
        self._untranslated = 0
        self._fuzzy = 0
        self._updatedate = ''
        self._translators = []
    
    @property
    def language(self):
        if self._template: #if template...
            return 'English'
        else: #if NOT template...
            filename = os.path.splitext(self.filename)
            return filename[0].replace('ReadMe-', '')

def main():
    status = TranslationsStatus()
    status.addProject(PoProject('WinMerge', 'WinMerge/English.pot', 'WinMerge'))
    status.addProject(PoProject('ShellExtension', 'ShellExtension/English.pot', 'ShellExtension'))
    status.addProject(InnoSetupProject('InnoSetup', 'InnoSetup/English.isl', 'InnoSetup'))
    status.addProject(ReadmeProject('Docs/Readme', 'Docs/Readme.txt', 'Docs/Readme'))
    status.writeToXmlFile('TranslationsStatus.xml')

# MAIN #
if __name__ == "__main__":
    main()