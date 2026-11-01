cmake_minimum_required(VERSION 3.16)
project(UnifiedEngineeringWorkbench VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(Launch src/main.cpp src/thermal.cpp src/imports.cpp src/performance.cpp
    src/hardware.cpp src/backprop.cpp)
target_include_directories(Launch PRIVATE include)

if(MSVC)
    target_compile_options(Launch PRIVATE /W4 /permissive- /O2)
else()
    target_compile_options(Launch PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()
enable_testing()
add_test(NAME workbench_tests COMMAND Launch test)
