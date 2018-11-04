Quick help to get started with Google Test for WinMerge
=======================================================

As name suggest, this test set uses Google's unit testing framework. So first
you need to get Google Test sources from:

<http://code.google.com/p/googletest/>
[download the .zip file for Windows]

Unzip and compile Google Test library
-------------------------------------

Unzip the Google Test sources to some folder. Pick a folder you can keep around
(not some temp folder) because you'll need the header files every time you
compile unit tests.

Open solution file to Visual Studio. The file is in subfolder `msvc\gtest.sln`

Build the 'Release' target of 'gtest' project. You don't need to build other
projects or targets. Indeed, 'rebuild all' for the solution does not work.

Copy the gtest.lib file from `msvc\Release` subfolder to `Testing\GoogleTest`-
folder in WinMerge source tree.

Setup Visual Studio for Google Test
-----------------------------------

As mentioned earlier Google Test header files are needed to compile the tests.
As the Google Test was unzipped to separate folder we must first tell VS where
to find those headers.

Add the `gtest-[version]\include` -folder to Visual Studio's include file
folders:

* Open Options -dialog in Visual Studio
* Select Projects and solutions
* Select VC++ Directories
* Select Include files
* Add `gtest-[version]\include` -folder as *LAST* folder in that list

Open the test project into Visual Studio and compile
----------------------------------------------------

Open the testing project to Visual Studio (VS 2003 and later work) and compile
Release -target of the project.

Run the tests
-------------

Open the Command Prompt and CD to folder where executable were build, usually
`[projectname]/Release`

Run the executable.

While the testing runs it prints progress information and info about passed/
failed tests.
