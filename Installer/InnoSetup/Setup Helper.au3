AutoItSetOption("MustDeclareVars", 1)

Dim $strAction
Dim $strCommand

;If a user is running this .exe file then...
If $CMDLine[0] = 0 Then
 	;Let the user know they shouldn't run this:
	msgbox(4144, "Setup - WinMerge", "Users: " & @CR & "This application is part of WinMerge's installation and removal processes.  It is not meant to be run externally by users." & @CRLF & @CRLF & "Developers:" & @CRLF & @TAB & "/i - Restarts Explorer to initialize the ShellExtension.dll as Explorer's starts" & @CRLF & @TAB & "/u - Closes Explorer, deletes ShellExtension.dll, and then restarts explorer")

	;Terminates the program
	exit
Else
	;Determines what the user's first command-line input parameter was
    $strCommand = StringUpper($CMDLine[1])
    
    msgbox(4144, "Setup Helper", "[$strCommand]=[" & $strCommand & "]")

    
	;Analyzes the command-line input parameter
	Select
		;If we received /i then...
	    Case $strCommand = "/I"
	        ;If the ShellExtension.dll file exists on the user's system then...
	        If FileExists(@ScriptDir + "\ShellExtension.dll") = 1 Then
	            ;The installer is trying to upgrade ShellExtension.dll
				$strAction="update"
	        Else
		        ;The installer is trying to install ShellExtension.dll
				$strAction="install"
			EndIf
			
		;If we received /u then...
	    Case $strCommand = "/U"
	        ;The uninstaller is trying to uninstall ShellExtension.dll
	        $strAction="uninstall"
	        
		;If the user specified an invalid bit of syntax then...
	    Case Else
	        ;Let the user know they shouldn't run this:
     		 msgbox(4144, "Setup - WinMerge", "Users: " & @CR & "This application is part of WinMerge's installation and removal processes.  It is not meant to be run externally by users." & @CRLF & @CRLF & "Developers:" & @CRLF & @TAB & "/i - Restarts Explorer to initialize the ShellExtension.dll as Explorer's starts" & @CRLF & @TAB & "/u - Closes Explorer, deletes ShellExtension.dll, and then restarts explorer")

			;Terminates the program
			exit
	EndSelect
EndIf



msgbox(4144, "Setup - WinMerge", "In order to " & $strAction & " the WinMerge Explorer context menu support all instances of Explorer must be restarted.  This is a non-destructive process, although some of your current folder viewing windows may be discarded.  Please finish any and all work in your Explorer windows before clicking [OK].")

;Kill Explorer until there's no longer any copies running
Do
	ProcessClose("Explorer.exe")
Until ProcessExists("Explorer.exe") = 0



;If the user properly specified the /u switch then...
If $strCommand = "/U" Then

	;Deletes the shell extension file
	FileDelete(@ScriptDir & "\ShellExtension.dll")

	;If the user's WinMerge directory is empty then...
	If FileExists(@ScriptDir & "\*.*") = 0 Then
	
	    ;Deletes the WinMerge directory
	    DirRemove(@ScriptDir, 1)
	Endif
EndIf


Run(@WindowsDir & "\Explorer.exe", "", @SW_HIDE)
WinWait("My Documents")
WinClose("My Documents")

;If the user properly specified the /u switch then...
If $strCommand = "/U" Then

	;If the file exists then the Exit Code will be 1 signifying an error
	Exit(FileExists(@ScriptDir & "\ShellExtension.dll"))
EndIf



