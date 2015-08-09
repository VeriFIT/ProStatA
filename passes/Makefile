
CMAKE = cmake
CXXFLAGS += -std=c++11 -pedantic -Wall -fPIC -fno-rtti
CXXFLAGS += -O0 -g -DDEBUG
# why is problem with RTTI in clang-3.8? clang-3.4 is OK...

#LLVM_DIRECTORY = /home/nika/Desktop/programs/llvm-3.8
#LDLIBS = -L$(LLVM_DIRECTORY)/lib
#INCLUDES = -I$(LLVM_DIRECTORY)/include

LIBS = libtest.so libpasses.so
#LIBS += libglobalvars.so liballinline.so  liblowermem.so liblowerselect.so

.PHONY: all local clean

# with cmake
all:
	mkdir -p ../passes_build
	cd ../passes_build && $(CMAKE) ../passes
	$(MAKE) -C ../passes_build

# or not
local: $(LIBS)

global_vars.o: global_vars.cc global_vars.hh passes.hh
lower_mem.o: lower_mem.cc lower_mem.hh passes.hh
all_inline.o: all_inline.cc all_inline.hh passes.hh
test.o: test.cc passes.hh

%.o: %.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

libglobalvars.so: global_vars.o
	$(CC) -shared $(CXXFLAGS) $< -o $@ $(LDLIBS)

# clang plug-in
libtest.so: test.o global_vars.o all_inline.o lower_mem.o
	$(CC) -shared $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# all LLVM passes
libpasses.so: global_vars.o all_inline.o lower_mem.o lower_select.o
	$(CC) -shared $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	if test -e ../passes_build/Makefile; then $(MAKE) clean -C ../passes_build; fi
	rm -f *~ *.o *.so

