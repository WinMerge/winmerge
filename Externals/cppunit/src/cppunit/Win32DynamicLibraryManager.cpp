#include <cppunit/Portability.h>

#if defined(CPPUNIT_HAVE_WIN32_DLL_LOADER)
#include <cppunit/plugin/DynamicLibraryManager.h>

#define WIN32_LEAN_AND_MEAN 
#define NOGDI
#define NOUSER
#define NOKERNEL
#define NOSOUND
#define NOMINMAX
#define BLENDFUNCTION void    // for mingw & gcc  
#include <windows.h>


CPPUNIT_NS_BEGIN


DynamicLibraryManager::LibraryHandle 
DynamicLibraryManager::doLoadLibrary( const std::string &libraryName )
{
  return ::LoadLibrary( libraryName.c_str() );
}


void 
DynamicLibraryManager::doReleaseLibrary()
{
  ::FreeLibrary( (HINSTANCE)m_libraryHandle );
}


DynamicLibraryManager::Symbol 
DynamicLibraryManager::doFindSymbol( const std::string &symbol )
{
  return (DynamicLibraryManager::Symbol)::GetProcAddress( 
     (HINSTANCE)m_libraryHandle, 
     symbol.c_str() );
}


std::string 
DynamicLibraryManager::getLastErrorDetail() const
{
  LPVOID lpMsgBuf;
  ::FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      NULL 
  );

  std::string message = (LPCTSTR)lpMsgBuf;

  // Display the string.
//  ::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

  // Free the buffer.
  ::LocalFree( lpMsgBuf );

  return message;
}


CPPUNIT_NS_END


#endif // defined(CPPUNIT_HAVE_WIN32_DLL_LOADER)
