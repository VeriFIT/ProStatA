
CMAKE = cmake
CXXFLAGS += -std=c++11 -pedantic -Wall -fPIC -fno-rtti
CXXFLAGS += -O0 -g -DDEBUG
# why is problem with RTTI in clang-3.8? clang-3.4 is OK...

#LLVM_DIRECTORY = /home/nika/Desktop/programs/llvm-3.8
#LDLIBS = -L$(LLVM_DIRECTORY)/lib
#INCLUDES = -I$(LLVM_DIRECTORY)/include

LIBRARYNAME = libglobalvars

.PHONY: all local clean

# with cmake
all:
	mkdir -p ../passes_build
	cd ../passes_build && $(CMAKE) ../passes
	$(MAKE) -C ../passes_build

# or not
local: $(LIBRARYNAME).so

%.o: %.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(LIBRARYNAME).so: global_vars.o
	$(CC) -shared $(CXXFLAGS) $< -o $@ $(LDLIBS)

clean:
	if test -e ../passes_build/Makefile; then $(MAKE) clean -C ../passes_build; fi
	rm -f *~ *.o *.so
