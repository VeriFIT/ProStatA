cmake_minimum_required (VERSION 2.6)

# The following misbehaved for LLVM-3.5 and LLVM-3.6 on Debian
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


project (passes CXX)

add_definitions (-std=c++11 -fPIC -fno-rtti)

set(LLVM_INC_DIR
	"/usr/include/llvm-3.7"
	"/usr/include/llvm-c-3.7"
	)

message(STATUS "Including LLVM directories: ${LLVM_INC_DIR}")
include_directories(${LLVM_INC_DIR})

message(STATUS "Adding LLVM definitions: ${LLVM_DEFINITIONS}")
add_definitions(${LLVM_DEFINITIONS})

# all LLVM passes
add_library (passes SHARED global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)

# clang plug-in
# or test.cc to z.B. static library passes.a
add_library (passes_test SHARED test.cc global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)
