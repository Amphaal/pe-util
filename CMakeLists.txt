cmake_minimum_required(VERSION 3.0)
project(PEUtil CXX)


if(NOT PEUTIL_DEFAULT_SEARCH_PATH_32)
    SET(PEUTIL_DEFAULT_SEARCH_PATH_32 "/usr/i686-w64-mingw32/sys-root/mingw/bin")
endif()

if(NOT PEUTIL_DEFAULT_SEARCH_PATH)
    SET(PEUTIL_DEFAULT_SEARCH_PATH    "/usr/x86_64-w64-mingw32/sys-root/mingw/bin")
endif()

message("Default search path for 32bits binaires : [${PEUTIL_DEFAULT_SEARCH_PATH_32}]")
message("Default search path for 64bits binaires : [${PEUTIL_DEFAULT_SEARCH_PATH}]")

option(PEUTIL_FORCE_FS_WINDOWS_SEPARATOR "Forces usage of Windows-style separator (slash)" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTESION)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(pe-parse/pe-parser-library)
add_subdirectory(src)