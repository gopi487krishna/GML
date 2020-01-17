cmake_minimum_required(VERSION 2.8)
project(GML_Project)

#To add source code and setup folder structure
set(src GDML/GDML/GDML.cpp)
set(test Test/test.cpp)
set(include GDML/GDML/)
include_directories(include)

#Add Boost Unit Test Library
set(Boost_USE_STATIC_LIBS on)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})

#Add Compiler Flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-std=c++17 -stdlib=libc++ -O3 -Wall)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-std=c++17 -O3 -Wall)
endif()

#Create Executable
add_executable(exe ${src} ${test} ${include})
target_link_libraries(exe ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

#Add Support for Unit Test
enable_testing()
add_test(myTest exe)
