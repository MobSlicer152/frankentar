cmake_minimum_required(VERSION 3.10)

project(frankentar VERSION 0.0.1)

# Debug by default
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)

include_directories(include snappy)
add_subdirectory(include)
add_subdirectory(src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-discarded-qualifiers")

add_library(ftar STATIC ${FRANKENTAR_HEADERS} ${FRANKENTAR_SOURCES})
target_link_libraries(ftar snappy)
add_executable(frankentar src/main.c)
target_link_libraries(frankentar ftar)
