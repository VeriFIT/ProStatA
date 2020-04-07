cmake_minimum_required (VERSION 3.0)
project (passes CXX)

# uncomment this on Darwin if linking the plug-ins fails on undefined references
if(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-flat_namespace -undefined suppress")
endif()

# Find llvm devel-headers and libs
find_package(LLVM REQUIRED CONFIG)
if(LLVM_FOUND)
    message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "LLVM binary dir: ${LLVM_TOOLS_BINARY_DIR}")
    message(STATUS "LLVM library directories: ${LLVM_LIBRARY_DIRS}")
    link_directories(${LLVM_LIBRARY_DIRS})
    message(STATUS "Including LLVM directories: ${LLVM_INCLUDE_DIRS}")
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    message(STATUS "Adding LLVM definitions: ${LLVM_DEFINITIONS}")
    add_definitions(${LLVM_DEFINITIONS})
endif()
# dont find LLVMConfig.cmake for llvm-3.4
#find_package(LLVM 3.4)
#set(LLVM_CMAKE_DIR "/usr/share/llvm/cmake")

if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "10.0")
    add_definitions(-std=c++14)
else()
    add_definitions(-std=c++11)
endif()
add_definitions(-fPIC)

if(NOT LLVM_ENABLE_RTTI)
    add_definitions (-fno-rtti)
endif()

message(STATUS "Cmake libpath ${CMAKE_LIBRARY_PATH}")

# all LLVM passes
#add_library (passes SHARED global_vars.cc lower_mem.cc lower_select.cc elim_phi.cc elim_const_expr.cc)
add_library (passes SHARED global_vars.cc elim_const_expr.cc nestedgep.cc)

# set the full path of libsl.so/.dylib
set(LIBPASSES_PATH ${PROJECT_BINARY_DIR}/libpasses${CMAKE_SHARED_LIBRARY_SUFFIX})

# helping script for testing
configure_file(${PROJECT_SOURCE_DIR}/test.sh.in  ${PROJECT_BINARY_DIR}/test.sh  @ONLY)

# clang plug-in
# or test.cc to z.B. static library passes.a
#add_library (passes_test SHARED test.cc global_vars.cc lower_mem.cc lower_select.cc)

