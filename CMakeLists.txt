cmake_minimum_required(VERSION 3.0)
project(mingw_util CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# available with gcc 8.0
set(CMAKE_CXX_STANDARD 17)
set(PE_COMPILE_DEF USE_FILESYSTEM)
set(PE_LINK_DEF stdc++fs)

add_subdirectory(pe-parse/pe-parser-library)
add_executable(peldd
  peldd.cc
  pe-parse/pe-parser-library/src/parse.cpp
  pe-parse/pe-parser-library/src/buffer.cpp
  )
target_link_libraries(peldd pe-parser-library ${PE_LINK_DEF})
target_compile_definitions(peldd PRIVATE ${PE_COMPILE_DEF})
target_compile_options(peldd PRIVATE -Wall -Werror)