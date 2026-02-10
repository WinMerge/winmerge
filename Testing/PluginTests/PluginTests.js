var g_cnt;
var g_testname;

var FileSys = new ActiveXObject("Scripting.FileSystemObject");
var ScriptFolder = FileSys.GetParentFolderName(WScript.ScriptFullName);
var WShell = new ActiveXObject("Wscript.Shell");

try {
  FileSys.CreateFolder(ScriptFolder + "\\result");
  FileSys.CreateFolder(ScriptFolder + "\\result\\excel");
  FileSys.CreateFolder(ScriptFolder + "\\result\\word");
  FileSys.CreateFolder(ScriptFolder + "\\result\\powerpnt");
  FileSys.CreateFolder(ScriptFolder + "\\result\\patch");
  FileSys.CreateFolder(ScriptFolder + "\\result\\patch\\result1");
  FileSys.CreateFolder(ScriptFolder + "\\result\\patch\\result2");
  FileSys.CreateFolder(ScriptFolder + "\\result\\patch\\result3");
} catch (e) { }

var PluginSettings = {
  "Files": {
    "Left": {
      "Item_0": ""
    },
    "Right": {
      "Item_0": ""
    }
  },
  "Plugins": {
    "CompareMSExcelFiles.sct": {
      "UnpackToFolder": 1,
      "UpdateLinks": 3,
      "CompareDocumentProperties": 1,
      "CompareNames": 1,
      "CompareCellValues": 1,
      "CompareWorksheetsAsImage": 1,
      "CompareWorksheetsAsHTML": 1,
      "ImageWidth": 1000,
      "ImageHeight": 3000,
      "CompareFormulas": 1,
      "CompareTextsInShapes": 1,
      "CompareHeadersAndFooters": 1,
      "CompareVBAMacros": 1
    },
    "CompareMSPowerPointFiles.sct": {
      "UnpackToFolder": 1,
      "CompareDocumentProperties": 1,
      "CompareSlideAsImage": 1,
      "CompareTextsInShapes": 1,
      "CompareTextsInNotesPage": 1,
      "CompareVBAMacros": 1
    },
    "CompareMSVisioFiles.sct": {
      "UnpackToFolder": 1,
      "ComparePageAsImage": 1,
      "CompareTextsInShapes": 1,
      "CompareVBAMacros": 1
    },
    "CompareMSWordFiles.sct": {
      "UnpackToFolder": 1,
      "CompareDocumentProperties": 1,
      "CompareBookmarks": 1,
      "CompareTextContents": 1,
      "CompareDocumentsAsHTML": 1,
      "CompareTextsInShapes": 1,
      "CompareVBAMacros": 1
    },
    "PrediffLineFilter.sct": {
      "Count": 5,
      "Enabled1": 1,
      "IgnoreCase1": 0,
      "UseRegExp1": 0,
      "Pattern1": "abc",
      "ReplaceText1": "def",
      "Enabled2": 1,
      "IgnoreCase2": 1,
      "UseRegExp2": 0,
      "Pattern2": "Ghi",
      "ReplaceText2": "Jkl",
      "Enabled3": 1,
      "IgnoreCase3": 0,
      "UseRegExp3": 1,
      "Pattern3": "(\\d+)\\.(\\d+)",
      "ReplaceText3": "\\2.\\1",
      "Enabled4": 1,
      "IgnoreCase4": 1,
      "UseRegExp4": 1,
      "Pattern4": "Mno.*Z",
      "ReplaceText4": "XxxX",
      "Enabled5": 0,
      "IgnoreCase5": 0,
      "UseRegExp5": 0,
      "Pattern5": "disabled",
      "ReplaceText5": ""
    }
  },
  "SubstitutionFilters": {
    "Values": 6,

    "Enabled00": 1,
    "CaseSensitive00": 1,
    "UseRegExp00": 0,
    "MatchWholeWordOnly00": 0,
    "Pattern00": "abc",
    "Replacement00": "def",

    "Enabled01": 1,
    "CaseSensitive01": 0,
    "UseRegExp01": 0,
    "MatchWholeWordOnly01": 0,
    "Pattern01": "Ghi",
    "Replacement01": "Jkl",

    "Enabled02": 1,
    "CaseSensitive02": 0,
    "UseRegExp02": 1,
    "MatchWholeWordOnly02": 0,
    "Pattern02": "(\\d+)\\.(\\d+)",
    "Replacement02": "\\2.\\1",

    "Enabled03": 1,
    "CaseSensitive03": 0,
    "UseRegExp03": 1,
    "MatchWholeWordOnly03": 0,
    "Pattern03": "Mno.*Z",
    "Replacement03": "XxxX",

    "Enabled04": 0,
    "CaseSensitive04": 1,
    "UseRegExp04": 0,
    "MatchWholeWordOnly04": 0,
    "Pattern04": "disabled",
    "Replacement04": "",

    "Enabled05": 1,
    "CaseSensitive05": 1,
    "UseRegExp05": 0,
    "MatchWholeWordOnly05": 1,
    "Pattern05": "word",
    "Replacement05": "XXX\\a\\b\\t\\f\\r\\n\\v\\\\"
  }
};

var MergeApp = {
  "GetOption": function (key, defvalue) {
    var ary1 = key.split("/");
    var ary2 = ary1[0].split("\\");
    if (!(ary2[0] in PluginSettings)) {
      return defvalue;
    }
    if (ary2.length > 1) {
      if (!(ary2[1] in PluginSettings[ary2[0]])) {
        return defvalue;
      }
      return PluginSettings[ary2[0]][ary2[1]][ary1[1]];
    }
    if (!(ary1[1] in PluginSettings[ary2[0]])) {
      return defvalue;
    }
    return PluginSettings[ary2[0]][ary1[1]];
  },
  "SaveOption": function (key, value) {
    //var t = typeof value === "string" ? "REG_SZ" : "REG_DWORD";
  },
  "Translate": function (text) {
    return text;
  },
  "LogError": function (text) {
    WScript.Echo(text);
  },
  "MsgBox": function (prompt, buttons, title) {
    WScript.Echo("title: " + title);
    WScript.Echo("prompt: " + prompt);
    WScript.Echo("buttons: " + buttons);
  },
  "InputBox": function (prompt, title, defaultValue) {
    WScript.Echo("title: " + title);
    WScript.Echo("prompt: " + prompt);
    WScript.Echo("default: " + defaultValue);
    return defaultValue;
  }
};
function setTestName(testname) {
  g_cnt = 0;
  g_testname = testname;
}

function getCurrentTestName() {
  return g_testname + g_cnt;
}

function assertTrue(value) {
  g_cnt++;
  if (!value) {
    throw new Error(30001, getCurrentTestName());
  }
}

function assertEquals(expected, actual) {
  g_cnt++;
  if (expected !== actual) {
    var msg = getCurrentTestName() + "\r\nexpected: \"" + expected + "\"\r\nactual: \"" + actual + "\"";
    WScript.Echo(msg);
    throw new Error(30001, msg);
  }
}

function printPluginInfo(p) {
  WScript.Echo("PluginDescription: " + p.PluginDescription);
  WScript.Echo("PluginEvent      : " + p.PluginEvent);
  WScript.Echo("PluginFileFilters: " + p.PluginFileFilters);
  WScript.Echo("PluginIsAutomatic: " + p.PluginIsAutomatic);
  try { WScript.Echo("PluginUnpackedFileExtension: " + p.PluginUnpackedFileExtension); } catch (e) {}
  try { WScript.Echo("PluginExtendedProperties: " + p.PluginExtendedProperties); } catch (e) {}
  WScript.Echo("");
}

function AITest() {
  var asciiChars = "";
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\AI.sct");
  printPluginInfo(p);

  p.PluginOnEvent(0, MergeApp);

  // AIConvertText
  p.PluginArguments = "Translate the given text into Japanese";
  WScript.Echo(p.AIConvertText("Hello World!"));
}

function EditorAddinTest() {
  var asciiChars = "";
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\editor addin.sct");
  printPluginInfo(p);

  for (var i = 0; i < 128; i++) {
    asciiChars += String.fromCharCode(i);
  }

  p.PluginOnEvent(0, MergeApp);

  // MakeUpper
  setTestName("MakeUpper");
  assertEquals("", p.MakeUpper(""));
  assertEquals(" ~0129ABCZABCZ", p.MakeUpper(" ~0129abczABCZ"));
  var expected = asciiChars.replace("abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  assertEquals(expected, p.MakeUpper(asciiChars));

  // MakeLower
  setTestName("MakeLower");
  assertEquals("", p.MakeLower(""));
  assertEquals(" ~0129abczabcz", p.MakeLower(" ~0129abczABCZ"));
  expected = asciiChars.replace("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz");
  assertEquals(expected, p.MakeLower(asciiChars));

  // ExecFilterCommand
  setTestName("ExecFilterCommand");
  p.PluginArguments = "sort";
  assertEquals("1\r\n2\r\n", p.ExecFilterCommand("2\r\n1\r\n"));
  p.PluginArguments = "echo a";
  assertEquals("a \r\n", p.ExecFilterCommand(""));
  p.PluginVariables = ScriptFolder;
  p.PluginArguments = "dir %1";
  p.ExecFilterCommand("");

  // SelectColumns
  setTestName("SelectColumns");
  p.PluginArguments = "1";
  assertEquals("a", p.SelectColumns("abc"));
  assertEquals("a\r\nd", p.SelectColumns("abc\r\ndef"));
  assertEquals("a\r\nd\r\n", p.SelectColumns("abc\r\ndef\r\n"));
  p.PluginArguments = "3";
  assertEquals("c\r\nf", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "1,3";
  assertEquals("ac\r\nd", p.SelectColumns("abc\r\nde"));
  p.PluginArguments = "1-3";
  assertEquals("abc\r\ndef", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-3";
  assertEquals("abc\r\ndef", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "1-";
  assertEquals("abc\r\ndef", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "2-";
  assertEquals("bc\r\nef", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "3-";
  assertEquals("c\r\nf", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "4-";
  assertEquals("\r\n", p.SelectColumns("abc\r\ndef"));

  p.PluginArguments = "-e ab";
  assertEquals("", p.SelectColumns(""));
  assertEquals("ab\r\n", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-e \"ab|de\"";
  assertEquals("ab\r\nde", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-e ^.";
  assertEquals("a\r\nd", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-e .*";
  assertEquals("abc\r\ndef", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-e -i \"Ab|dE\"";
  assertEquals("ab\r\nde", p.SelectColumns("abc\r\ndef"));
  p.PluginArguments = "-e -i -g ab";
  assertEquals("abAB\r\n", p.SelectColumns("abcABC\r\ndef"));
  p.PluginArguments = "-v -e \"ab|de\"";
  assertEquals("c\r\nf", p.SelectColumns("abc\r\ndef"));

  p.PluginArguments = "1-3 -e ab 2-3";
  assertEquals("abcabbc\r\ndefef", p.SelectColumns("abc\r\ndef"));

  // SelectLines
  setTestName("SelectLines");
  p.PluginArguments = "1";
  assertEquals("", p.SelectLines(""));
  assertEquals("aaa", p.SelectLines("aaa"));
  p.PluginArguments = "2";
  assertEquals("bbb", p.SelectLines("aaa\r\nbbb"));
  p.PluginArguments = "1,2";
  assertEquals("aaa\r\nbbb", p.SelectLines("aaa\r\nbbb"));
  p.PluginArguments = "1-2";
  assertEquals("aaa\r\nbbb", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\nbbb\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "1-";
  assertEquals("aaa\r\nbbb", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\nbbb\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  assertEquals("aaa\r\n\r\n\r\n", p.SelectLines("aaa\r\n\r\n\r\n"));
  p.PluginArguments = "-2";
  assertEquals("aaa\r\nbbb", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\nbbb\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "4-5";
  assertEquals("", p.SelectLines("aaa\r\nbbb"));
  assertEquals("", p.SelectLines("aaa\r\nbbb\r\n"));

  p.PluginArguments = "-e aa";
  assertEquals("", p.SelectLines(""));
  assertEquals("aaa", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "-e -i AA";
  assertEquals("aaa", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "-e .*";
  assertEquals("aaa\r\nbbb", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\nbbb\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "-e ^a";
  assertEquals("aaa", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "-e a$";
  assertEquals("aaa", p.SelectLines("aaa\r\nbbb"));
  assertEquals("aaa\r\n", p.SelectLines("aaa\r\nbbb\r\n"));
  p.PluginArguments = "-v -e a$";
  assertEquals("bbb", p.SelectLines("aaa\r\nbbb"));
  assertEquals("bbb\r\n", p.SelectLines("aaa\r\nbbb\r\n"));

  p.PluginArguments = "1 -e c$";
  assertEquals("aaa\r\nccc", p.SelectLines("aaa\r\nbbb\r\nccc"));

  // Replace
  setTestName("Replace");
  p.PluginArguments = "abc def";
  assertEquals("", p.Replace(""));
  assertEquals("def def def def", p.Replace("abc def abc def"));
  p.PluginArguments = "-e a.c XXX";
  assertEquals("", p.Replace(""));
  assertEquals("XXX def XXX def", p.Replace("abc def abc def"));
  p.PluginArguments = "-e -i A.C XXX";
  assertEquals("XXX def XXX def", p.Replace("abc def abc def"));
  p.PluginArguments = "-e ^\\d+ XXX";
  assertEquals("XXX abc\r\nXXX def", p.Replace("1000 abc\r\n1001 def"));
  p.PluginArguments = "-e (.{3}) $1\\r\\n";
  assertEquals("012\r\n345\r\n678\r\n9", p.Replace("0123456789"));
  p.PluginArguments = "-e (\\d+) \\a\\b\\t\\n\\v\\f\\r\\\\$1\\1\\0\\x21\\x7E";
  assertEquals(String.fromCharCode(0x07) + String.fromCharCode(0x08) + String.fromCharCode(0x09) + String.fromCharCode(0x0A) + String.fromCharCode(0x0B) + String.fromCharCode(0x0C) + String.fromCharCode(0x0D) + "\\012345678901234567890123456789!~", p.Replace("0123456789"));
  //
  p.PluginArguments = "-s";
  assertEquals("", p.Replace(""));
  p.PluginArguments = "-s";
  assertEquals("def def def", p.Replace("abc def abc"));
  p.PluginArguments = "-s";
  assertEquals("Jkl def Jkl", p.Replace("gHI def Ghi"));
  p.PluginArguments = "-s";
  assertEquals("2.1 def 4.3", p.Replace("1.2 def 3.4"));
  p.PluginArguments = "-s";
  assertEquals("XxxX", p.Replace("mnopqrstuvwxyz"));
  p.PluginArguments = "-s";
  assertEquals("disabled", p.Replace("disabled"));
  p.PluginArguments = "-s";
  var text = "abc def abc\r\n";
  text += "gHI def GHI\r\n";
  text += "1.2 def 3.4\r\n";
  text += "mnopqrstuvwxyz\r\n";
  text += "disabled";
  var expected =
    "def def def\r\n" +
    "Jkl def Jkl\r\n" +
    "2.1 def 4.3\r\n" +
    "XxxX\r\n" +
    "disabled";
  assertEquals(expected, p.Replace(text));
  p.PluginArguments = "-s";
  assertEquals("XXX\\a\\b\\t\\f\\r\\n\\v\\\\ wordword XXX\\a\\b\\t\\f\\r\\n\\v\\\\", p.Replace("word wordword word"));

  // ReverseColumns
  setTestName("ReverseColumns");
  assertEquals("", p.ReverseColumns(""));
  assertEquals("a", p.ReverseColumns("a"));
  assertEquals("a\r\nb", p.ReverseColumns("a\r\nb"));
  assertEquals("ba", p.ReverseColumns("ab"));
  assertEquals("ba\r\ndc", p.ReverseColumns("ab\r\ncd"));
  assertEquals("cba", p.ReverseColumns("abc"));
  assertEquals("cba\r\nfed", p.ReverseColumns("abc\r\ndef"));
  assertEquals("dcba", p.ReverseColumns("abcd"));
  assertEquals("dcba\r\nhgfe", p.ReverseColumns("abcd\r\nefgh"));

  // ReverseLines
  setTestName("ReverseLines");
  assertEquals("", p.ReverseLines(""));
  assertEquals("a", p.ReverseLines("a"));
  assertEquals("a\r\n", p.ReverseLines("a\r\n"));
  assertEquals("b\r\na", p.ReverseLines("a\r\nb"));
  assertEquals("b\r\na\r\n", p.ReverseLines("a\r\nb\r\n"));
  assertEquals("c\r\nb\r\na", p.ReverseLines("a\r\nb\r\nc"));
  assertEquals("c\r\nb\r\na\r\n", p.ReverseLines("a\r\nb\r\nc\r\n"));
  assertEquals("d\r\nc\r\nb\r\na", p.ReverseLines("a\r\nb\r\nc\r\nd"));

  // Tokenize
  setTestName("Tokenize");
  p.PluginArguments = "[^\\w]+";
  assertEquals("", p.Tokenize(""));
  assertEquals("aaa", p.Tokenize("aaa"));
  assertEquals("aaa\r\nabcd", p.Tokenize("aaa abcd"));

  // RemoveDuplicates
  setTestName("RemoveDuplicates");
  assertEquals("", p.RemoveDuplicates(""));
  assertEquals("aaa", p.RemoveDuplicates("aaa"));
  assertEquals("aaa", p.RemoveDuplicates("aaa\r\naaa"));
  assertEquals("aaa\r\n", p.RemoveDuplicates("aaa\r\naaa\r\n"));
  assertEquals("aaa\r\nbbb", p.RemoveDuplicates("aaa\r\nbbb"));
  assertEquals("aaa\r\nbbb\r\n", p.RemoveDuplicates("aaa\r\nbbb\r\n"));

  // CountDuplicates
  setTestName("CountDuplicates");
  assertEquals("", p.CountDuplicates(""));
  assertEquals("aaa\t1", p.CountDuplicates("aaa"));
  assertEquals("aaa\t2\r\n", p.CountDuplicates("aaa\r\naaa"));
  assertEquals("aaa\t2\r\n", p.CountDuplicates("aaa\r\naaa\r\n"));
  assertEquals("aaa\t1\r\nbbb\t1\r\n", p.CountDuplicates("aaa\r\nbbb"));
  assertEquals("aaa\t1\r\nbbb\t1\r\n", p.CountDuplicates("aaa\r\nbbb\r\n"));

  // Trim
  setTestName("Trim");
  assertEquals("", p.Trim(""));
  assertEquals("", p.Trim(" "));
  assertEquals("aaa", p.Trim("aaa"));
  assertEquals("aaa", p.Trim(" aaa"));
  assertEquals("aaa", p.Trim("aaa  "));
  assertEquals("aaa", p.Trim("  aaa  "));
  assertEquals("aaa", p.Trim("\t  aaa  \t"));
  assertEquals("aaa\r\nbbb", p.Trim(" aaa  \r\n  bbb "));
  assertEquals("aaa\r\nbbb\r\n", p.Trim(" aaa  \r\n  bbb \r\n"));

  // Test: Replace with double quotes (escaped as "")
  setTestName("Replace_DoubleQuotes");
  p.PluginArguments = '"""abc""" """def"""';
  assertEquals("", p.Replace(""));
  assertEquals('"def" test "def"', p.Replace('"abc" test "abc"'));
  
  // Test: Replace pattern containing double quotes
  p.PluginArguments = '"a""b""c" XXX';
  assertEquals('XXX test XXX', p.Replace('a"b"c test a"b"c'));
  
  // Test: Replace with quotes at boundaries
  p.PluginArguments = '""test"" ""result""';
  assertEquals('"result"', p.Replace('"test"'));
  assertEquals('"result" and "result"', p.Replace('"test" and "test"'));
  
  // Test: Replace with escaped quotes using regex
  p.PluginArguments = '-e """(\\w+)""" [\\1]';
  assertEquals('[test] and [data]', p.Replace('"test" and "data"'));
  
  // Test: Replace to empty string (remove pattern)
  setTestName("Replace_EmptyString");
  p.PluginArguments = 'abc ""';
  assertEquals("", p.Replace(""));
  assertEquals(" test ", p.Replace("abc test abc"));
  assertEquals("", p.Replace("abc"));
  assertEquals("", p.Replace("abcabcabc"));
  
  // Test: Remove pattern with surrounding text
  p.PluginArguments = 'remove ""';
  assertEquals("test  test", p.Replace("test remove test"));
  assertEquals("", p.Replace("remove"));
  assertEquals("before  after", p.Replace("before remove after"));
  
  // Test: Remove using regex - whitespace
  p.PluginArguments = '-e \\s+ ""';
  assertEquals("testdata", p.Replace("test data"));
  assertEquals("abc123def", p.Replace("abc   123   def"));
  assertEquals("", p.Replace("   "));
  
  // Test: Remove using regex - specific words
  p.PluginArguments = '-e \\b(TODO|FIXME):\\s* ""';
  assertEquals("This is a comment", p.Replace("TODO: This is a comment"));
  assertEquals("Fix this bug", p.Replace("FIXME: Fix this bug"));
  assertEquals("Normal text", p.Replace("Normal text"));
  
  // Test: Remove digits
  p.PluginArguments = '-e \\d+ ""';
  assertEquals("abc def", p.Replace("abc123 def456"));
  assertEquals("", p.Replace("12345"));
  
  // Test: Complex - remove quotes and their content
  p.PluginArguments = '-e """[^""]*""" ""';
  assertEquals(" and ", p.Replace('"test" and "data"'));
  
  // Test: Replace empty string to non-empty (insert)
  p.PluginArguments = '"" INSERT';
  // Note: This would insert at beginning, behavior depends on implementation
  
  // Test: Case insensitive with quotes and empty replacement
  p.PluginArguments = '-e -i """TEST""" ""';
  assertEquals(" data", p.Replace('"test" data'));
  assertEquals(" data", p.Replace('"TEST" data'));

  // Cleanup
  p.PluginOnEvent(1, MergeApp);
}

function InsertDateTimeTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\insert datetime.sct");
  printPluginInfo(p);

  WScript.Echo(p.InsertDate("date="));
  WScript.Echo(p.InsertTime("time="));
  WScript.Echo();

}

function IgnoreLeadingLineNumbersTest() {
  var changed = false;
  var text = "";
  var size;
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\IgnoreLeadingLineNumbers.sct");
  printPluginInfo(p);

  //
  text = "";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  setTestName("IgnoreLeadingLineNumbers1");
  assertEquals("", text);
  assertEquals(0, size);
  assertEquals(false, changed);

  //
  text = "n1 aaa\r\nN2 bbb\r\n";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  setTestName("IgnoreLeadingLineNumbers2");
  assertEquals(" aaa\r\n bbb\r\n", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "1 aaa\nbbb\n";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  setTestName("IgnoreLeadingLineNumbers3");
  assertEquals(" aaa\nbbb\n", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "aaa\r2 bbb\r";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  setTestName("IgnoreLeadingLineNumbers4");
  assertEquals("aaa\r bbb\r", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

}

function PrediffLineFilterTest() {
  var changed = false;
  var text = "";
  var size;
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\PrediffLineFilter.sct");
  printPluginInfo(p);

  p.PluginOnEvent(0, MergeApp);

  //
  setTestName("PrediffLineFilter");
  text = "";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("", text);
  assertEquals(0, size);
  assertEquals(false, changed);

  //
  text = "abc def abc";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("def def def", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "gHI def Ghi";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("Jkl def Jkl", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "1.2 def 3.4";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("2.1 def 4.3", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "mnopqrstuvwxyz";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("XxxX", text);
  assertEquals(4, size);
  assertEquals(true, changed);

  //
  text = "disabled";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  assertEquals("disabled", text);
  assertEquals(text.length, size);
  assertEquals(true, changed);

  //
  text = "abc def abc\r\n";
  text += "gHI def GHI\r\n";
  text += "1.2 def 3.4\r\n";
  text += "mnopqrstuvwxyz\r\n";
  text += "disabled";
  size = text.length;
  changed = false;
  var result = p.PrediffBufferW(text, size, changed);
  if (typeof result !== "string") { text = result.getItem(1); size = result.getItem(2); changed = result.getItem(3); }
  var expected =
    "def def def\r\n" +
    "Jkl def Jkl\r\n" +
    "2.1 def 4.3\r\n" +
    "XxxX\r\n" +
    "disabled";
  assertEquals(expected, text);
  assertEquals(expected.length, size);
  assertEquals(true, changed);

  p.PluginOnEvent(1, MergeApp);
}

function ApplyPatchTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\ApplyPatch.sct");
  printPluginInfo(p);
  var changed = false;
  var subcode = 0;
  p.PluginOnEvent(0, MergeApp);
  PluginSettings.Files.Left.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\file1.txt";
  PluginSettings.Files.Right.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\patch.diff";
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Patch\\patch.diff", ScriptFolder + "\\result\\patch\\result1.txt", changed, subcode);
  PluginSettings.Files.Left.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\patch.diff";
  PluginSettings.Files.Right.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\file1.txt";
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Patch\\patch.diff", ScriptFolder + "\\result\\patch\\result2.txt", changed, subcode);
  PluginSettings.Files.Left.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\dir1\\";
  PluginSettings.Files.Right.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\patch2.diff";
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Patch\\patch2.diff", ScriptFolder + "\\result\\patch\\result1\\", changed, subcode);
  PluginSettings.Files.Left.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\dir1\\";
  PluginSettings.Files.Right.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\patch3.diff";
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Patch\\patch3.diff", ScriptFolder + "\\result\\patch\\result2\\", changed, subcode);
  PluginSettings.Files.Left.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\patch3.diff";
  PluginSettings.Files.Right.Item_0 = ScriptFolder + "\\..\\Data\\Patch\\dir1\\subdir\\";
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Patch\\patch3.diff", ScriptFolder + "\\result\\patch\\result3\\", changed, subcode);
  p.PluginOnEvent(1, MergeApp);
}

function CompareMSExcelFilesTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\CompareMSExcelFiles.sct");
  printPluginInfo(p);
  var changed = false;
  var subcode = 0;
  p.PluginOnEvent(0, MergeApp);
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Office\\excel.xls", ScriptFolder + "\\result\\excel.xls.tsv", changed, subcode);
  CollectGarbage();
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Office\\excel.xls", ScriptFolder + "\\result\\excel\\", changed, subcode);
  CollectGarbage();
  p.PluginOnEvent(1, MergeApp);
}

function CompareMSWordFilesTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\CompareMSWordFiles.sct");
  printPluginInfo(p);
  var changed = false;
  var subcode = 0;
  p.PluginOnEvent(0, MergeApp);
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Office\\word.doc", ScriptFolder + "\\result\\word.doc.txt", changed, subcode);
  CollectGarbage();
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Office\\word.doc", ScriptFolder + "\\result\\word\\", changed, subcode);
  CollectGarbage();
  p.PluginOnEvent(1, MergeApp);
}

function CompareMSPowerPointFilesTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\CompareMSPowerPointFiles.sct");
  printPluginInfo(p);
  var changed = false;
  var subcode = 0;
  p.PluginOnEvent(0, MergeApp);
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Office\\powerpnt.ppt", ScriptFolder + "\\result\\powerpnt.ppt.txt", changed, subcode);
  CollectGarbage();
  p.UnpackFolder(ScriptFolder + "\\..\\Data\\Office\\powerpnt.ppt", ScriptFolder + "\\result\\powerpnt\\", changed, subcode);
  CollectGarbage();
  p.PluginOnEvent(1, MergeApp);
}

function CompareEscapedJavaPropertiesFilesTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\CompareEscapedJavaPropertiesFiles.sct");
  printPluginInfo(p);
  var changed = false;
  var subcode = 0;
  p.PluginOnEvent(0, MergeApp);
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Fileformats\\file.properties", ScriptFolder + "\\result\\file.properties.unpacked.txt", changed, subcode);
  CollectGarbage();
  p.PackFile(ScriptFolder + "\\result\\file.properties.unpacked.txt", ScriptFolder + "\\result\\file.properties.packed.txt", changed, subcode);
  CollectGarbage();
  p.PluginOnEvent(1, MergeApp);
}

// AITest();
CompareEscapedJavaPropertiesFilesTest();
EditorAddinTest();
InsertDateTimeTest();
IgnoreLeadingLineNumbersTest();
PrediffLineFilterTest();
ApplyPatchTest();
/*
CompareMSExcelFilesTest();
CompareMSWordFilesTest();
CompareMSPowerPointFilesTest();
*/

