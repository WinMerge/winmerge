var g_cnt;
var g_testname;

var FileSys = new ActiveXObject("Scripting.FileSystemObject");
var ScriptFolder = FileSys.GetParentFolderName(WScript.ScriptFullName);

var MergeApp = {
  "Log": function (level, text) { WScript.Echo(text); }
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
    throw new Error(30001, getCurrentTestName() + "\r\nexpected: \"" + expected + "\"\r\nactual: \"" + actual + "\"");
  }
}

function CompareMSExcelFilesTest() {
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\CompareMSExcelFiles.sct");
  WScript.Echo("PluginDescription: " + p.PluginDescription);
  WScript.Echo("PluginEvent      : " + p.PluginEvent);
  WScript.Echo("PluginFileFilters: " + p.PluginFileFilters);
  WScript.Echo("PluginIsAutomatic: " + p.PluginIsAutomatic);
  try {
    WScript.Echo("PluginUnpackedFileExtension: " + p.PluginUnpackedFileExtension);
  } catch (e) {
  }

  var changed = false;
  var subcode = 0;
  p.UnpackFile(ScriptFolder + "\\..\\Data\\Office\\excel.xls", ScriptFolder + "\\result.txt", changed, subcode);
}

function EditorAddinTest() {
  var asciiChars = "";
  var p = GetObject("script: " + ScriptFolder + "\\..\\..\\Plugins\\dlls\\editor addin.sct");

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
  p.PluginArguments = "-e (\\d+) \\a\\b\\t\\n\\v\\f\\r\\\\$1\\1\\0";
  assertEquals(String.fromCharCode(0x07) + String.fromCharCode(0x08) + String.fromCharCode(0x09) + String.fromCharCode(0x0A) + String.fromCharCode(0x0B) + String.fromCharCode(0x0C) + String.fromCharCode(0x0D) + "\\0123456789\\1\\0", p.Replace("0123456789"));

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

  p.PluginOnEvent(1, MergeApp);
}

EditorAddinTest();
//CompareMSExcelFilesTest

