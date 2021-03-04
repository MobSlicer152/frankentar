# Frankentar
Frankentar is a modified version of the tar format, originally created for use in the Purpl game engine. For more details on the structure of the format, see the file `include/frankentar.h`. This repository also includes a library for parsing and manipulating Frankentar archives to serve as a reference to any interested in reimplementing the format or just using it in their project.

## Files
This list includes the purposes of the headers in this repo
`include/read.h` - functions for reading archives
`include/util.h` - general utility functions used by the other functions
`include/write.h` - functions for writing archives

## Build instructions
Just do `cmake -S. -Bbuild && cmake --build build` to build the library and command line program.
