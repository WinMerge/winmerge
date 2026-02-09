project(StackWalker-project)

cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    #set(CMAKE_BUILD_TYPE Debug)
endif()


option(StackWalker_DISABLE_TESTS  "Disable tests" OFF)


###############################
# Check compiler's capabilities
###############################

include (CheckCXXCompilerFlag)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (CMAKE_COMPILER_IS_CLANG true)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set (CMAKE_COMPILER_IS_MSVC true)
endif()

if(CMAKE_COMPILER_IS_MSVC)
    # MSVC_TOOLSET_VERSION is only available from cmake 3.12
    # --> https://cmake.org/cmake/help/v3.12/variable/MSVC_TOOLSET_VERSION.html
    # --> https://gitlab.kitware.com/KindDragon/cmake/commit/f2a61e0a4790d4d52a2412c8017be2b92e9af26f?view=inline
    if(NOT DEFINED MSVC_TOOLSET_VERSION)
        if(MSVC_VERSION GREATER_EQUAL 1920)
            set(MSVC_TOOLSET_VERSION "") # leave unknown
        elseif(MSVC_VERSION GREATER_EQUAL 1910)
            # VS 2017
            set(MSVC_TOOLSET_VERSION 141)
        elseif(MSVC_VERSION EQUAL 1900)
            # VS 2015
            set(MSVC_TOOLSET_VERSION 140)
        elseif(MSVC_VERSION EQUAL 1800)
            # VS 2013
            set(MSVC_TOOLSET_VERSION 120)
        elseif(MSVC_VERSION EQUAL 1700)
            # VS 2012
            set(MSVC_TOOLSET_VERSION 110)
        elseif(MSVC_VERSION EQUAL 1600)
            # VS 2010
            set(MSVC_TOOLSET_VERSION 100)
        elseif(MSVC_VERSION EQUAL 1500)
            # VS 2008
            set(MSVC_TOOLSET_VERSION 90)
        elseif(MSVC_VERSION EQUAL 1400)
            # VS 2005
            set(MSVC_TOOLSET_VERSION 80)
        else()
            # We don't support MSVC_TOOLSET_VERSION for earlier compiler.
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SECURE_SCL=0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_NONSTDC_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4740")
else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not supported yet")
endif()

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
set (CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
set (CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")

set(TARGET_StackWalker StackWalker)
add_library(${TARGET_StackWalker} STATIC
    Main/StackWalker/StackWalker.cpp)
target_include_directories(${TARGET_StackWalker} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Main/StackWalker>
    )

install(TARGETS "${TARGET_StackWalker}"
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(FILES "${CMAKE_SOURCE_DIR}/Main/StackWalker/StackWalker.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (MSVC_VERSION GREATER_EQUAL 1900)
    set(PDB_StackWalker "${TARGET_StackWalker}.pdb")
else()
    set(PDB_StackWalker "vc${MSVC_TOOLSET_VERSION}.pdb")
endif()
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_StackWalker}.dir/$\{CMAKE_INSTALL_CONFIG_NAME\}/${PDB_StackWalker}"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RENAME "${TARGET_StackWalker}.pdb"
    OPTIONAL)


if (StackWalker_DISABLE_TESTS)
    message(STATUS "Skipping tests")
else()
    enable_testing()

    set(TARGET_StackWalker_tests StackWalker_test)
    add_executable(${TARGET_StackWalker_tests}
        Main/StackWalker/main.cpp)
    target_link_libraries(${TARGET_StackWalker_tests} PUBLIC ${TARGET_StackWalker})

    add_test(NAME ${TARGET_StackWalker_tests} COMMAND ${TARGET_StackWalker_tests})
endif()
