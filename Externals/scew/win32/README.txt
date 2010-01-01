Microsoft Visual C++ 2008
=========================

Static library
--------------

   In order to build SCEW as a static library you need to select one
of the following configurations:

  - Static Debug
  - Static Debug UTF-16
  - Static Release
  - Static Release UTF-16

   The "Debug" configurations will build the library without
optimizations. By contrast, the "Release" configuration will build the
library with optmizations enabled. The UTF-16 configurations add
support for this encoding by including the XML_UNICODE_WCHAR_T Expat
preprocessor define. This define is also needed in your application if
you want to support UTF-16.

   It is also necessary to add another Expat preprocessor define to
your application, XML_STATIC. Thus, both, Expat and SCEW will be
statically linked. This define will also enable SCEW automatic static
library inclusion for your application, so there is no need to add a
library dependency in your project properties. However, you still need
to add a library dependency (Linker options) for Expat. In your source
code, you only need to include the main SCEW header:

#include <scew/scew.h>

The output files for the static libraries follow a similar Expat
approach:

  libscew(w)(d)MT.lib           The static library

where:

  "w"  indicates the UTF-16 version.
  "d"  indicates the debug version.
  "MT" indicates Multi-threaded runtime library.

   Note that, by default, SCEW will be built for the "Multi-threaded"
(/MT) runtime library. If you want to build SCEW for another runtime
library you first need to change your project properties (Code
generation options), and then, to avoid confusion, update the output
file name (Linker options) to, for example, libscewMD.lib. Where "MD"
would be for the "Multi-threaded DLL" (/MD) runtime library.

   As SCEW will automatically try to include the correct static
library for your application, "SCEW_HOME/scew/scew.h" should be also
updated:

#define SCEW_LIB_M "MD"


Shared library
--------------

   In order to build SCEW as a shared library (DLL) you need to select
one of the following configurations:

  - Debug
  - Debug UTF-16
  - Release
  - Release UTF-16

   All the options have the same meaning as in the static library. The
only difference between both is the output files and how to use them
in your application.

The output files for the shared libraries are:

  libscew(w)(d).dll             The shared library
  libscew(w)(d).lib             The import library

where:

  "w" indicates the UTF-16 version.
  "d" indicates the debug version.

   Then, in your application, you need to include a library depency
(Linker options) to "libscew(w)(d).lib". Note that, here, we are using
the ".lib" file, not the ".dll" one.

   Finally, you just need to copy your desired SCEW DLL library in a
system path or, simply, in the directory where your executable
resides.
