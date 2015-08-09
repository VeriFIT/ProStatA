cmake_minimum_required (VERSION 2.6)
project (passes CXX)

add_definitions (-std=c++11 -fPIC -fno-rtti)

# all LLVM passes
add_library (passes SHARED global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)

# clang plug-in
# or test.cc to z.B. static library passes.a
add_library (test SHARED test.cc global_vars.cc all_inline.cc lower_mem.cc lower_select.cc)
