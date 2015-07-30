cmake_minimum_required (VERSION 2.6)
project (passes)

add_definitions (-std=c++11 -fno-rtti)

add_library (globalvars SHARED global_vars.cc)

set_target_properties (globalvars
  PROPERTIES
  LINKER_LANGUAGE CXX
  )
