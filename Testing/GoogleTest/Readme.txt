Quick help to get started with Google Test for WinMerge:

As name suggest, this test set uses Google's unit testing framework. So first
you need to get Google Test binaries from

http://code.google.com/p/googletest/

Once you've unzipped the binary (or compiled sources), copy the gtest.lib -file
to Testing/GoogleTest -folder in WinMerge tree. Testing projects search the
library from that folder.

Open the testing project to Visual Studio (VS 2003 and later work) and compile
either Debug- or Release-version of the executable.

Open the Command Prompt and CD to folder where executable were build, usually
[projectname]/Debug or [projectname]/Release

Run the executable.

While the testing runs it prints progress information and info about passed/
failed tests.
