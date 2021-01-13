cmake_minimum_required(VERSION 3.0)
project(PEUtil CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTESION)

add_subdirectory(pe-parse/pe-parser-library)

add_executable(peldd
  src/Utility.hpp
  src/peldd.hpp
  src/Arguments.cpp
  src/PathCache.cpp
  src/Traverser.cpp 
  main.cpp
)
target_include_directories(peldd PRIVATE src)

target_link_libraries(peldd pe-parser-library)
