cmake_minimum_required(VERSION 3.10)
project(CPPCraftDemo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CPPCraftDemo CPPCraftDemo/CPPCraftDemo.cpp)

# Make the CPPCraftDemo subfolder available as an include path so that
# stdafx.h (and targetver.h) are found when the source file does #include "stdafx.h"
target_include_directories(CPPCraftDemo PRIVATE CPPCraftDemo)
