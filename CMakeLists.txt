cmake_minimum_required(VERSION 3.7.0)

if (WIN32 AND NOT EXISTS "$ENV{QT_DIR}")
  message(FATAL_ERROR "The value of QT_DIR environment variable is invalid.")  
endif ()

project(JlTranslator VERSION 0.1)

set(CMAKE_BINARY_DIR "../build")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#add_subdirectory(JlUtil)
add_subdirectory(JlTranslatorTool)

#add_dependencies(JlTranslatorTool JlUtil)


