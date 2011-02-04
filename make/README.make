
 ---------------------------------------------------------------------- 
|                                                                      |
| This README referes to Unix type systems which use make for building |
| SAGA.  Windows user (i.e. non-cygwin Windows user) can use the MS    |
| Developers Studio build environment.                                 |
|                                                                      |
 ---------------------------------------------------------------------- 


SAGA uses a gnu-make based build system.  It includes a number of
makefiles throughout the source tree, and in the $SAGA_ROOT/make/
directory.

All these make includes get installed into
$SAGA_LOCATION/share/saga/make/.

If SAGA_ROOT is set in a Makefile, the make includes from the referred
saga source tree are used.  If it is not set, the make includes are
used from the installation tree in SAGA_LCOATION, which defaults to
/usr/local.

The following Makefile stub should provide a good starting point for
building SAGA applications. 


 ----------------------------------------------------------------------
|                                                                      |
| SAGA_SRC  = $(wildcard *.cpp)                                        |
| SAGA_BIN  = $(SRC:%.cpp=%)                                           |
|                                                                      |
| include $(SAGA_LOCATION)/share/saga/make/saga.application.mk         |
|                                                                      |
 ----------------------------------------------------------------------


This stub loads the make rules etc. needed to build the application.
If the application needs additional include directories or libraries,
use the following syntax _after_ the make includes:


 ----------------------------------------------------------------------
|                                                                      |
| SAGA_CPPFLAGS += -I/opt/super/include                                |
| SAGA_LDFLAGS  += -L/opt/super/lib -lsuper                            |
|                                                                      |
 ----------------------------------------------------------------------


Of course it is possible to build SAGA applications with custom
Makefiles.  The includes can, however, be used to obtain the SAGA
specific compiler and linker flags:


 ----------------------------------------------------------------------
|                                                                      |
| SRC      = $(wildcard *.cpp)                                         |
| OBJ      = $(SRC:%.cpp=%.o)                                          |
| BIN      = $(SRC:%.cpp=%)                                            |
|                                                                      |
| CXX      = gcc                                                       |
| CXXFLAGS = -c -O3 -pthreads -I/opt/mpi/include                       |
|                                                                      |
| LD       = $(CXX)                                                    |
| LDFLAGS  = -L/opt/mpi/lib/ -lmpi                                     |
|                                                                      |
| include $(SAGA_LOCATION)/make/saga.engine.mk                         |
| include $(SAGA_LOCATION)/make/saga.package.file.mk                   |
|                                                                      |
|                                                                      |
| .default: $(BIN)                                                     |
|                                                                      |
| $(BIN): % : %.cpp                                                    |
|                                                                      |
| $(OBJ): %.o : %.cpp                                                  |
|   $(CXX) $(CXXFLAGS) $(SAGA_CXXFLAGS) -o $@ $<                       |
|                                                                      |
| $(BIN): % : %.o                                                      |
|   $(LD)  $(LDFLAGS)  $(SAGA_LDFLAGS)  -o $@ $<                       |
|                                                                      |
 ----------------------------------------------------------------------


SAGA_CXXFLAGS and SAGA_LDFLAGS contain only those options and settings
which are absolutely required to use SAGA.  You may want to use
'make -n' to print what the resulting make commands are, in order to
debug eventual incompatibilities between SAGA compiler and linker
flags, and your own ones.

Yet another option is to use the output of saga-config for makefiles:

 ----------------------------------------------------------------------
|                                                                      |
| SRC       = $(wildcard *.cpp)                                        |
| OBJ       = $(SRC:%.cpp=%.o)                                         |
| BIN       = $(SRC:%.cpp=%)                                           |
|                                                                      |
| CXX       = gcc                                                      |
| CPPFLAGS  = -I/opt/mpi/include                                       |
| CXXFLAGS  = -c -O3 -pthreads                                         |
|                                                                      |
| LD        = $(CXX)                                                   |
| LDFLAGS   = -L/opt/mpi/lib/ -lmpi                                    |
|                                                                      |
| CPPFLAGS += $(shell $(SAGA_LOCATION)/bin/saga-config --cppflags)     |
| CXXFLAGS += $(shell $(SAGA_LOCATION)/bin/saga-config --cxxflags)     |
| LDFLAGSS += $(shell $(SAGA_LOCATION)/bin/saga-config --ldflags)      |
|                                                                      |
| .default: $(BIN)                                                     |
|                                                                      |
| $(BIN): % : %.cpp                                                    |
|                                                                      |
| $(OBJ): %.o : %.cpp                                                  |
|   $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<                            |
|                                                                      |
| $(BIN): % : %.o                                                      |
|   $(LD)  $(LDFLAGS) -o $@ $<                                         |
|                                                                      |
 ----------------------------------------------------------------------

