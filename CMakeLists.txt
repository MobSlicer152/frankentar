cmake_minimum_required(VERSION 3.10)

include_directories(include)
add_subdirectory(include)
add_subdirectory(src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-discarded-qualifiers")

add_library(ftar STATIC ${FRANKENTAR_HEADERS} ${FRANKENTAR_SOURCES})
add_executable(frankentar src/main.c)
target_link_libraries(frankentar ftar)
