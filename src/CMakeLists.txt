configure_file(
    config.h.cmake 
    config.h
)

add_executable(peldd
  Utility.hpp
  peldd.hpp
  Arguments.cpp
  PathCache.cpp
  Traverser.cpp 
  main.cpp
)

set_target_properties(peldd PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

target_link_libraries(peldd pe-parser-library)