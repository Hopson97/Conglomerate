cmake_minimum_required(VERSION 3.1)

project(glom)
set (CMAKE_CXX_STANDARD 14)

#Add files
file(GLOB_RECURSE SRC 
    "src/*.cpp"
    "src/*.h"
)

#Define the executable
add_executable(
    ${PROJECT_NAME} 
    ${SRC} 
)

#Define the filters for IDEs
source_group("src" FILES ${SRC})