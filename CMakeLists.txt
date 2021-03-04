cmake_minimum_required(VERSION 3.10)

add_subdirectory(include)
add_subdirectory(src)

add_library(ftar STATIC ${FRANKENTAR_HEADERS} ${FRANKENTAR_SOURCES})
add_executable(frankentar src/main.c)
target_link_libraries(frankentar ftar)
