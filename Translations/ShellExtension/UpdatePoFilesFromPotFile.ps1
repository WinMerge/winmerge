Add-Type -Language CSharp @"
/**
* This script updates the language PO files from the master POT file.
*
* Copyright (C) 2007-2008 by Tim Gerundt
* Released under the "GNU General Public License"
*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

public class PoFileUpdater
{
    public static void Main(string[] args)
    {
        DateTime startTime = DateTime.Now;

        Console.WriteLine("Updating PO files from POT file...");

        string sDir = Directory.GetCurrentDirectory();
        string sPotFile = Path.Combine(sDir, "English.pot");
        var oEnglishPotContent = GetContentFromPoFile(sPotFile);
        if (oEnglishPotContent.Count == 0)
        {
            Console.Error.WriteLine("Error reading content from English.pot");
            return;
        }
        bool bPotChanged = GetArchiveBit(sPotFile);
        string[] oLanguages = args;
        if (oLanguages.Length == 0) { oLanguages = Directory.GetFiles(sDir); }
        foreach (string sLanguage in oLanguages) //For all languages...
        {
            if (Path.GetExtension(sLanguage).ToLower() == ".po")
            {
                if (bPotChanged || (GetArchiveBit(sLanguage))) //If update necessary...
                {
                    Console.WriteLine(sLanguage);
                }
                var oLanguagePoContent = GetContentFromPoFile(sLanguage);
                if (oLanguagePoContent.Count > 0) //If content exists...
                {
                    CreateUpdatedPoFile(sLanguage, oEnglishPotContent, oLanguagePoContent);
                }
                SetArchiveBit(sLanguage, false);
            }
        }

        double seconds = (DateTime.Now - startTime).TotalSeconds;
        Console.WriteLine("All PO files updated, after " + seconds + " second(s).");
    }

    ////
    // ...
    class CSubContent
    {
        public string sMsgCtxt2;
        public string sMsgId2;
        public string sMsgStr2;
        public string sTranslatorComments;
        public string sExtractedComments;
        public string sReferences;
        public string sFlags;
    }

    ////
    // ...
    static IDictionary<string, CSubContent> GetContentFromPoFile(string sPoPath)
    {
        var reMsgCtxt = new Regex(@"^msgctxt ""(.*)""", RegexOptions.IgnoreCase);
        var reMsgId = new Regex(@"^msgid ""(.*)""", RegexOptions.IgnoreCase);
        var reMsgContinued = new Regex(@"^""(.*)""", RegexOptions.IgnoreCase);
        var oContent = new Dictionary<string, CSubContent>();

        int iMsgStarted = 0;
        string sMsgCtxt = "";
        string sMsgId = "";
        Match oMatch = null;
        var oSubContent = new CSubContent();
        string[] lines = File.ReadAllLines(sPoPath, System.Text.Encoding.UTF8);
        for (int i = 0; i < lines.Length; i++) //For all lines...
        {
            string sLine = lines[i].Trim();
            if (sLine.Length != 0) //If NOT empty line...
            {
                if (sLine.Substring(0, 1) != "#") //If NOT comment line...
                {
                    if (reMsgCtxt.IsMatch(sLine)) //If "msgctxt"...
                    {
                        iMsgStarted = 1;
                        oMatch = reMsgCtxt.Match(sLine);
                        sMsgCtxt = oMatch.Groups[1].Value;
                        oSubContent.sMsgCtxt2 = sLine + "\n";
                    }
                    else if (reMsgId.IsMatch(sLine)) //If "msgid"...
                    {
                        iMsgStarted = 2;
                        oMatch = reMsgId.Match(sLine);
                        sMsgId = oMatch.Groups[1].Value;
                        oSubContent.sMsgId2 = sLine + "\n";
                    }
                    else if (sLine.Length >= 8 && sLine.Substring(0, 8) == "msgstr \"") //If "msgstr"...
                    {
                        iMsgStarted = 3;
                        oSubContent.sMsgStr2 = sLine + "\n";
                    }
                    else if (reMsgContinued.IsMatch(sLine)) //If "msgctxt", "msgid" or "msgstr" continued...
                    {
                        if (iMsgStarted == 1)
                        {
                            sMsgCtxt = sMsgCtxt + oMatch.Groups[1].Value;
                            oSubContent.sMsgCtxt2 = oSubContent.sMsgCtxt2 + sLine + "\n";
                        }
                        else if (iMsgStarted == 2)
                        {
                            oMatch = reMsgContinued.Match(sLine);
                            sMsgId = sMsgId + oMatch.Groups[1].Value;
                            oSubContent.sMsgId2 = oSubContent.sMsgId2 + sLine + "\n";
                        }
                        else if (iMsgStarted == 3)
                        {
                            oSubContent.sMsgStr2 = oSubContent.sMsgStr2 + sLine + "\n";
                        }
                    }
                }
                else //If comment line...
                {
                    iMsgStarted = -1;
                    string sLeft = (sLine.Length < 2) ? sLine : sLine.Substring(0, 2);
                    switch (sLeft)
                    {
                        case "#~": //Obsolete message...
                            iMsgStarted = 0;
                            break;
                        case "#.": //Extracted comment...
                            oSubContent.sExtractedComments = oSubContent.sExtractedComments + sLine + "\n";
                            break;
                        case "#:": //Reference...
                            oSubContent.sReferences = oSubContent.sReferences + sLine + "\n";
                            break;
                        case "#,": //Flag...
                            oSubContent.sFlags = oSubContent.sFlags + sLine + "\n";
                            break;
                        default: //Translator comment...
                            oSubContent.sTranslatorComments = oSubContent.sTranslatorComments + sLine + "\n";
                            break;
                    }
                }
            }
            else if (iMsgStarted != 0) //If empty line AND there is pending translation...
            {
                iMsgStarted = 0; //Don't process same translation twice
                if (sMsgId == "") { sMsgId = "__head__"; }
                if (oContent.ContainsKey(sMsgCtxt + sMsgId) == false) //If the key is NOT already used...
                {
                    oContent.Add(sMsgCtxt + sMsgId, oSubContent);
                }
                sMsgCtxt = "";
                oSubContent = new CSubContent();
            }
        }
        if (iMsgStarted != 0) //If there is pending translation...
        {
            if (oContent.ContainsKey(sMsgCtxt + sMsgId) == false) //If the key is NOT already used...
            {
                oContent.Add(sMsgCtxt + sMsgId, oSubContent);
            }
        }
        return oContent;
    }

    ////
    // ...
    static void CreateUpdatedPoFile(string sPoPath, IDictionary<string, CSubContent> oEnglishPotContent, IDictionary<string, CSubContent> oLanguagePoContent)
    {
        //--------------------------------------------------------------------------------
        // Backup the old PO file...
        //--------------------------------------------------------------------------------
        string sBakPath = "sPoPath.bak";
        if (File.Exists(sBakPath))
        {
            File.Delete(sBakPath);
        }
        File.Move(sPoPath, sBakPath);
        //--------------------------------------------------------------------------------

        using (var oPoFile = new StreamWriter(sPoPath, false, new System.Text.UTF8Encoding(false)))
        {
            var oLanguage = oLanguagePoContent["__head__"];
            oPoFile.Write(oLanguage.sTranslatorComments);
            oPoFile.Write(oLanguage.sMsgId2);
            oPoFile.Write(oLanguage.sMsgStr2);
            oPoFile.Write("\n");
            var i = 0;
            foreach (var sKey in oEnglishPotContent.Keys) //For all English content...
            {
                if (sKey != "__head__")
                {
                    var oEnglish = oEnglishPotContent[sKey];
                    if (oLanguagePoContent.ContainsKey(sKey)) //If translation exists...
                    {
                        oLanguage = oLanguagePoContent[sKey];
                    }
                    else //If translation NOT exists...
                    {
                        oLanguage = oEnglish;
                    }
                    oPoFile.Write(oLanguage.sTranslatorComments);
                    oPoFile.Write(oEnglish.sExtractedComments);
                    oPoFile.Write(oEnglish.sReferences);
                    oPoFile.Write(oLanguage.sFlags);
                    oPoFile.Write(oLanguage.sMsgCtxt2);
                    oPoFile.Write(oLanguage.sMsgId2);
                    oPoFile.Write(oLanguage.sMsgStr2);
                    if (i < oEnglishPotContent.Count - 1)
                        oPoFile.Write("\n");
                }
                i++;
            }
        }
    }

    ////
    // ...
    static bool GetArchiveBit(string sFilePath)
    {
        if (File.Exists(sFilePath)) //If the file exists...
        {
            if (File.GetAttributes(sFilePath).HasFlag(FileAttributes.Archive)) //If archive bit set...
            {
                return true;
            }
        }
        return false;
    }

    ////
    // ...
    static void SetArchiveBit(string sFilePath, bool bValue)
    {
        if (File.Exists(sFilePath)) //If the file exists...
        {
            if (File.GetAttributes(sFilePath).HasFlag(FileAttributes.Archive)) //If archive bit set...
            {
                if (bValue == false)
                {
                    File.SetAttributes(sFilePath, File.GetAttributes(sFilePath) & ~FileAttributes.Archive);
                }
            }
            else //If archive bit NOT set...
            {
                if (bValue == true)
                {
                    File.SetAttributes(sFilePath, File.GetAttributes(sFilePath) | FileAttributes.Archive);
                }
            }
        }
    }
}
"@

[PoFileUpdater]::Main($args)
