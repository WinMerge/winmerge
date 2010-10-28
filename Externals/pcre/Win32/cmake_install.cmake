# Install script for directory: D:/Work/WinMerge/WinMerge_SVN/Externals/pcre

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files/PCRE")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcred.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcre.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcre.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcre.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcred.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcre.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcre.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcre.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcreposixd.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcreposix.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcreposix.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcreposix.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcreposixd.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcreposix.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcreposix.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcreposix.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcrecppd.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcrecpp.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcrecpp.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcrecpp.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Debug/pcrecppd.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcrecpp.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcrecpp.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcrecpp.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcregrep.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcregrep.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcregrep.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcregrep.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcretest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcretest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcretest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcretest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcrecpp_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcrecpp_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcrecpp_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcrecpp_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcre_scanner_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcre_scanner_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcre_scanner_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcre_scanner_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcre_stringpiece_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/Release/pcre_stringpiece_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/MinSizeRel/pcre_stringpiece_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/RelWithDebInfo/pcre_stringpiece_unittest.exe")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/pcre.h"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/pcreposix.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/pcrecpp.h"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/pcre_scanner.h"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/pcrecpparg.h"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/pcre_stringpiece.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre-config.1"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcregrep.1"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcretest.1"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE FILES
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcreapi.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrebuild.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrecallout.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrecompat.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrecpp.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrematching.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrepartial.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrepattern.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcreperform.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcreposix.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcreprecompile.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcresample.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcrestack.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcresyntax.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_compile.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_compile2.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_config.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_copy_named_substring.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_copy_substring.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_dfa_exec.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_exec.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_free_substring.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_free_substring_list.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_fullinfo.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_get_named_substring.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_get_stringnumber.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_get_stringtable_entries.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_get_substring.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_get_substring_list.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_info.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_maketables.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_refcount.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_study.3"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/pcre_version.3"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/pcre/html" TYPE FILE FILES
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/index.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre-config.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcreapi.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrebuild.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrecallout.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrecompat.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrecpp.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcredemo.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcregrep.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrematching.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrepartial.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrepattern.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcreperform.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcreposix.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcreprecompile.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcresample.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcrestack.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcresyntax.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcretest.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_compile.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_compile2.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_config.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_copy_named_substring.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_copy_substring.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_dfa_exec.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_exec.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_free_substring.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_free_substring_list.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_fullinfo.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_get_named_substring.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_get_stringnumber.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_get_stringtable_entries.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_get_substring.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_get_substring_list.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_info.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_maketables.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_refcount.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_study.html"
    "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/doc/html/pcre_version.html"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win32/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
