cmake_minimum_required (VERSION 2.6)

# The following misbehaved for LLVM-3.5 and LLVM-3.6 on Debian
find_package(LLVM REQUIRED CONFIG)

# dont find LLVMConfig.cmake for llvm-3.4
#find_package(LLVM 3.4)
#set(LLVM_CMAKE_DIR "/usr/share/llvm/cmake")

if(NOT LLVM_FOUND)
	set(LLVM_LIBRARY_DIRS "")
	set(LLVM_INCLUDE_DIRS "")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


project (passes CXX)

add_definitions (-std=c++11 -fPIC)

if(NOT LLVM_ENABLE_RTTI)
    add_definitions (-fno-rtti)
endif()

message(STATUS "LLVM library directories: ${LLVM_LIBRARY_DIRS}")
link_directories(${LLVM_LIBRARY_DIRS})

message(STATUS "Including LLVM directories: ${LLVM_INCLUDE_DIRS}")
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

message(STATUS "Adding LLVM definitions: ${LLVM_DEFINITIONS}")
add_definitions(${LLVM_DEFINITIONS})

message(STATUS "Cmake libpath ${CMAKE_LIBRARY_PATH}")

# all LLVM passes
add_library (passes SHARED global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)

# clang plug-in
# or test.cc to z.B. static library passes.a
add_library (passes_test SHARED test.cc global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)

