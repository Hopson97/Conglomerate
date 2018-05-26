# Conglomerate

Combines a C++ project into a single .cpp file.

This is done by sorting the header files by their dependancies (#include directives from this project), and then spitting them out into a .cpp file, and then putting the source files below.

Any local includes are discarded from the output, as they would not be needed in the conglomerate file (library includes are untouched).

An example of this can be seen for this project [here](https://github.com/Hopson97/Conglomerate/blob/master/glom_output)

# Building

A C++ compiler that supports C++17 is needed

Use CMake to generate your files for building.

`cmake CMakeLists.txt`

# Usage

Add the executable to your path variables.

Open a C++ project root in your favourite terminal and type `glom`. This will will run the program and create and output file in `glom_output/out.cpp`.


### Command line options

| Name        | Details                             | Example       |
|-------------|-------------------------------------|---------------|
| `-o <name>` | Specify the name of the output file | `-o main.cpp` |
