SCEW Examples (Microsoft Visual C++ 2008)
=========================================

   It is possible to build the SCEW examples for Windows using the
project files provided under these directories. There is a Visual C++
2008 project file for each example, as well as a Microsoft Visual
Studio Solution that loads all of them.

   Once you load the solution or a concrete project file, you will
need to update the properties of the project you want to build. It is
necessary to add an additional 'Include' directory pointing to the
root directory where you installed SCEW and an additional 'Library'
directory pointing to the SCEW Windows library directory. For example:

      Include directory
        SCEW top level, e.g. "c:\lib\scew-1.0.0"

      Library directory
        SCEW win32 lib, e.g. "c:\lib\scew-1.0.0\win32\lib"

Note: By default, all the examples use the SCEW and Expat static
libraries. So, it is necessary to create them before you try to build
the examples.
