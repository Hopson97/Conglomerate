cmake_minimum_required(VERSION 3.1)

project(glom)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_LIBRARIES -lstdc++fs)

#Add files
set(SOURCES 
    src/Header.cpp
    src/HeaderSorter.cpp 
    src/Main.cpp 
    src/Util.cpp 
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#Define the executable
add_executable(
    ${PROJECT_NAME} 
    ${SOURCES} 
)


#libstdc++fs