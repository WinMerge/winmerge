var fso = new ActiveXObject("Scripting.FileSystemObject");
var sh = new ActiveXObject("WScript.Shell");
var otf = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1);

while (!otf.AtEndOfStream) {
	WScript.Echo(sh.ExpandEnvironmentStrings(otf.ReadLine()));
}

otf.Close();

