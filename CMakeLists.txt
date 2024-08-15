cmake_minimum_required(VERSION 3.26)
project(game_of_life LANGUAGES CXX)

set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .exe and .dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .exe and .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .so and .dylib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .lib and .a

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
add_executable(game_of_life ${sources})

set(CMAKE_CXX_STANDARD 20)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)

add_compile_options(-ffast-math)
add_compile_options(-fno-stack-protector)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-g0)
  if(MSVC)
    add_compile_options(/O3)
  else()
    add_compile_options(-O3)
  endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g3)
  if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_INIT "-fsanitize=undefined")
  endif()
else()
  message("Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

add_subdirectory(${LIB_DIR}/sfml)
target_link_libraries(game_of_life PRIVATE sfml-graphics)


add_custom_target(copy_res
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_EXECUTABLE_OUTPUT_DIRECTORY}/res
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_EXECUTABLE_OUTPUT_DIRECTORY}/res
)
add_dependencies(game_of_life copy_res)
