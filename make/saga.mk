
ifndef SAGA_MK_INC_INC
       SAGA_MK_INC_INC = yes

###########################################################
#
# make configuration
#
-include $(SAGA_ROOT)/make/saga.util.mk
-include $(SAGA_LOCATION)/share/saga/make/saga.util.mk

ifndef SAGA_MAKE_INCLUDE_ROOT
  $(error need either SAGA_ROOT or SAGA_LOCATION)
endif

include $(SAGA_MAKE_INCLUDE_ROOT)/saga.nothing.mk

# include the rquired make includes
ifndef SAGA_CONFDIR
  SAGA_CONFDIR = ../config
endif

TMP_REQS   = $(patsubst   package_%,package.%,$(SAGA_REQUIREMENTS))
TMP_PACKAGE_REQS   = $(filter     package.%,$(TMP_REQS))
TMP_EXTERNAL_REQS  = $(filter-out package.%,$(TMP_REQS))

TMP_MAKE_INCLUDES  = $(foreach preq,$(TMP_PACKAGE_REQS),$(SAGA_ROOT)/make/saga.$(preq).i.mk)
TMP_MAKE_INCLUDES += $(foreach ereq,$(TMP_EXTERNAL_REQS),$(SAGA_CONFDIR)/saga.config.$(ereq).c.mk)

ifneq "x$(TMP_MAKE_INCLUDES)" "x"
  ifndef SAGA_IS_CLEANING
    include $(TMP_MAKE_INCLUDES)
  endif
endif


###########################################################
#
# saga related compiler flags etc
#
SAGA_CPPINCS   += -I$(SAGA_INCLUDE_ROOT)
SAGA_MKDEP      = $(SAGA_CPP) -MM
#
###########################################################

###########################################################
#
# headers get installed under SAGA_LOCATION/include/, 
# but beneath that directory the original directory 
# hierarchy is maintained.  For example, 
#                          saga/impl/engine.hpp 
# goes into 
#   $SAGA_LOCATION/include/saga/impl/engine.hpp
#
# REL = PWD - ROOT
#

# fix for old make versions which don't know realpath
SAGA_TMP_PWD    = $(realpath $(SAGA_PWD))/
SAGA_TMP_ROOT   = $(realpath $(SAGA_ROOT))

ifeq "$(SAGA_TMP_PWD)$(SAGA_TMP_ROOT)" "/"
 SAGA_TMP_PWD  := $(shell cd $(SAGA_PWD)  && $(PWD))/
 SAGA_TMP_ROOT := $(shell cd $(SAGA_ROOT) && $(PWD))
endif

ifeq "$(SAGA_TMP_PWD)$(SAGA_TMP_ROOT)" ""
 # SAGA_TMP_PWD   = $(subst //,/,$(SAGA_PWD)/)
 # SAGA_TMP_ROOT  = $(subst //,/,$(SAGA_ROOT)/)
endif

ifneq "$(SAGA_TMP_ROOT)" ""
 SAGA_REL_PWD    := $(shell $(ECHO) $(SAGA_TMP_PWD) \
                          | $(SED) -e 's|$(SAGA_TMP_ROOT)/||g' \
                          | $(SED) -e 's|/$$||g' \
                     )
else
 SAGA_REL_PWD    := $(shell $(ECHO) $(SAGA_TMP_PWD) \
                          | $(SED) -e 's|/$$||g' \
                     )
endif

# FIXME: relation to above??
ifeq "$(realpath $(SAGA_PWD))" ""
  SAGA_REAL_PWD  = $(SAGA_PWD)
  SAGA_REAL_ROOT = $(SAGA_ROOT)
else
  SAGA_REAL_PWD  = $(realpath $(SAGA_PWD))
  SAGA_REAL_ROOT = $(realpath $(SAGA_ROOT))
endif


ifneq "$(SAGA_REL_PWD)" ""
  SAGA_REL_PWD := $(SAGA_REL_PWD)/
endif
#
###########################################################

###########################################################
#
# main targets: all, install, subdirs
#
ifndef SAGA_BIN_OBJ
 SAGA_BIN_OBJ += $(SAGA_BIN:%=%.o)
endif

SAGA_ALL_OBJ  += $(sort $(SAGA_LIB_OBJ)  $(SAGA_OBJ)  $(SAGA_ADD_LIB_OBJ) $(SAGA_ADD_BIN_OBJ) $(SAGA_BIN_OBJ))
SAGA_ALL_OBJC += $(sort $(SAGA_LIB_OBJC) $(SAGA_OBJC) $(SAGA_ADD_LIB_OBJC) $(SAGA_ADD_BIN_OBJC))

SAGA_ALL_PRE  += $(SAGA_ALL_OBJ:%.o=%.ii) 
SAGA_ALL_PREC += $(SAGA_ALL_OBJC:%.o=%.i) 


# build both dynamic and static libs
ifdef SAGA_LIB
  ifndef SAGA_IS_CLEANING
    SAGA_LIBNAME = lib$(SAGA_LIB).$(SAGA_LEXT)
    SAGA_ARCNAME = lib$(SAGA_LIB).$(SAGA_AEXT)
  else
    SAGA_LIBNAME_CLEAN = lib$(SAGA_LIB).$(SAGA_LEXT)
    SAGA_ARCNAME_CLEAN = lib$(SAGA_LIB).$(SAGA_AEXT)
  endif
endif

# sometimes, we need _only_ the static lib
ifdef SAGA_ARC
  ifndef SAGA_IS_CLEANING
    SAGA_ARCNAME = lib$(SAGA_ARC).$(SAGA_AEXT)
  else
    SAGA_ARCNAME_CLEAN = lib$(SAGA_ARC).$(SAGA_AEXT)
  endif
endif

# sometimes, we need _only_ the dynamic lib
ifdef SAGA_DYLIB
  ifndef SAGA_IS_CLEANING
    SAGA_LIBNAME = lib$(SAGA_DYLIB).$(SAGA_LEXT)
  endif
endif

# decide if we use static or dynamic libs (default: dynamic libs)
ifeq "$(SAGA_USE_STATIC_LIBS)" "yes"
  # we add dynamic libs below, for those libs which are only available as such
  SAGA_LDFLAGS_EXT := $(SAGA_S_LIBS) $(SAGA_LDFLAGS_EXT)
  SAGA_LDFLAGS_EXT := $(SAGA_S_LIBS) $(SAGA_LDFLAGS_EXT)
endif

SAGA_USE_LDFLAGS_BIN  += $(SAGA_LDFLAGS) $(SAGA_LDFLAGS_EXT) $(SAGA_LDFLAGS_SYS)
SAGA_USE_LDFLAGS_LIB  += $(SAGA_LDFLAGS) $(SAGA_LDFLAGS_EXT) $(SAGA_LDFLAGS_SYS)

# for cleaning targets, make sure to clean make and config dirs last, so as to
# not bar make paths due to missing prerequsites etc
ifdef SAGA_IS_CLEANING
  SAGA_SUBDIRS_ORIG  := $(SAGA_SUBDIRS)
  SAGA_SUBDIRS       := $(filter-out config make, $(SAGA_SUBDIRS_ORIG))
  SAGA_SUBDIRS_AFTER := $(filter     make config, $(SAGA_SUBDIRS_ORIG))
  # $(warning -----------$(SAGA_SUBDIRS_BEFORE))
  # $(warning -----------$(SAGA_SUBDIRS))
  # $(warning -----------$(SAGA_SUBDIRS_AFTER))
endif


all:: error makefiles depend subdirs $(SAGA_ALL_OBJ) $(SAGA_ALL_OBJC) $(SAGA_LIBNAME) $(SAGA_ARCNAME)

# SHS: SHellScript
all:: $(SAGA_BIN) $(SAGA_SHS) $(SAGA_MAK) 

check:: all subdirs

SAGA_CONF_IN  = $(wildcard configure.in)
SAGA_CONF     = $(wildcard configure)
SAGA_M4      += $(wildcard *.m4) 
SAGA_M4_DEP  += $(wildcard $(SAGA_ROOT)/config/*.m4) 
SAGA_M4_DEP  += $(wildcard config/*.m4) 
SAGA_M4_INST += $(filter saga%,$(SAGA_M4))


.PHONY: $(SAGA_SUBDIRS_BEFORE) $(SAGA_SUBDIRS) $(SAGA_SUBDIRS_AFTER)
.PHONY: subdirs_before subdirs subdirs_after

subdirs_after:: subdirs
subdirs::       subdirs_before 

subdirs_before:: $(SAGA_SUBDIRS_BEFORE)
subdirs::        $(SAGA_SUBDIRS) $(SAGA_SUBDIRS_BEFORE) $(SAGA_SUBDIRS_AFTER)
subdirs_after::  $(SAGA_SUBDIRS_AFTER)

ifdef SAGA_IS_CLEANING
  $(SAGA_SUBDIRS_AFTER): $(SAGA_SUBDIRS)
else
  $(SAGA_SUBDIRS): $(SAGA_SUBDIRS_AFTER)
endif

$(SAGA_SUBDIRS_BEFORE) $(SAGA_SUBDIRS) $(SAGA_SUBDIRS_AFTER):
	@$(ECHO) " -->  $(SAGA_REL_PWD)$@/"
	@$(MAKE) SAGA_DEEP=1 -C $@ $(MAKECMDGOALS)
	@$(ECHO) " <--  $(SAGA_REL_PWD)$@/"


configure:: $(SAGA_CONF_IN) $(SAGA_M4) $(SAGA_M4_DEP)
ifneq "$(SAGA_CONF_IN)" ""
	@$(ECHO) "      creating   configure"
 ifdef SAGA_ROOT
	@autoconf -B $(SAGA_PWD)/config -B $(SAGA_REAL_ROOT)/config
 else
	@autoconf -B $(SAGA_PWD)/config -B $(SAGA_LOCATION)/share/saga/config
 endif
	@$(RM) -r autom4te.cache
endif

.PHONY: makefiles
makefiles:: makeconfigs

makeconfigs:: $(SAGA_MAK_C) subdirs
ifdef SAGA_MAK_C
	@$(CP) $(SAGA_MAK_C) $(SAGA_ROOT)/make
endif

$(SAGA_MAK_C):
	@$(ECHO) ""
	@$(ECHO) " ================================= "
	@$(ECHO) "  you need to run configure first  "
	@$(ECHO) " ================================= "
	@$(ECHO) ""
	@$(FALSE)

Makefile.nok:: Makefile.ok
ifndef SAGA_DEEP
 ifeq "$(SAGA_CONF)" ""
	@$(MAKE) -C . configure
 endif
	@$(ECHO) "      running    configure"
	@./configure
endif

Makefile.ok::
ifndef SAGA_DEEP
 ifeq "$(SAGA_CONF)" ""
	@$(MAKE) -C . configure
 endif
	@$(ECHO) "      running    configure"
	@./configure
endif


install:: all
 ifndef SAGA_DONT_INSTALL
  ifneq "$(SAGA_HDR)" ""
	 @$(ECHO) "      installing   headers"
	 @$(MKDIR)                       $(SAGA_HDR_ROOT)/$(SAGA_HDR_DIR)
	 @$(CP)          $(SAGA_HDR)     $(SAGA_HDR_ROOT)/$(SAGA_HDR_DIR)
  endif
  ifneq "$(SAGA_LIBNAME)" ""
	 @$(ECHO) "      installing   lib" 
	 @$(MKDIR)                       $(SAGA_LIB_ROOT)/$(SAGA_LIB_DIR)
	 @$(CP)          $(SAGA_LIBNAME) $(SAGA_LIB_ROOT)/$(SAGA_LIB_DIR)
  endif
  ifneq "$(SAGA_ARCNAME)" ""
	 @$(ECHO) "      installing   lib (static)" 
	 @$(MKDIR)                       $(SAGA_LIB_ROOT)/$(SAGA_LIB_DIR)
	 @$(CP)          $(SAGA_ARCNAME) $(SAGA_LIB_ROOT)/$(SAGA_LIB_DIR)
  endif
  ifneq "$(SAGA_BIN)" ""
	 @$(ECHO) "      installing   bin" 
	 @$(MKDIR)                       $(SAGA_BIN_ROOT)/$(SAGA_BIN_DIR)
	 @$(CP)          $(SAGA_BIN)     $(SAGA_BIN_ROOT)/$(SAGA_BIN_DIR)
  endif
  ifneq "$(SAGA_SHS)" ""
	 @$(ECHO) "      installing   scripts"
	 @$(MKDIR)                       $(SAGA_SHS_ROOT)/$(SAGA_SHS_DIR)
	 @$(CP)          $(SAGA_SHS)     $(SAGA_SHS_ROOT)/$(SAGA_SHS_DIR)
  endif
  ifneq "$(SAGA_INI)" ""
	 @$(ECHO) "      installing   inis"
	 @$(MKDIR)                       $(SAGA_INI_ROOT)
	 @$(CP)          $(SAGA_INI)     $(SAGA_INI_ROOT)
  endif
  ifneq "$(SAGA_MAK)" ""
	 @$(ECHO) "      installing   makefiles"
	 @$(MKDIR)                       $(SAGA_MAK_ROOT)
	 @$(CP)          $(SAGA_MAK)     $(SAGA_MAK_ROOT)
  endif        
  ifneq "$(SAGA_MAK_D)" ""
	 @$(ECHO) "      installing   make includes"
	 @$(MKDIR)                       $(SAGA_MAK_ROOT)
	 @$(CP)          $(SAGA_MAK_D)   $(SAGA_MAK_ROOT)
  endif
  ifneq "$(SAGA_M4_INST)" ""
	 @$(ECHO) "      installing   configure macros"
	 @$(MKDIR)                       $(SAGA_M4_ROOT)
	 @$(CP)          $(SAGA_M4_INST) $(SAGA_M4_ROOT)
  endif
 endif # SAGA_DONT_INSTALL
#
###########################################################

###########################################################
#
# Handle dependencies for all cpp sources
#
SAGA_DEPEND  = $(addprefix .depend/,$(SAGA_SRC:.cpp=.d))
SAGA_DEPEND += $(addprefix .depend/,$(SAGA_CSRC:.c=.d))

# include dependencies if we have any
ifneq "$(SAGA_DEPEND)" ""
 ifeq "$(filter depend clean distclean libclean depclean confclean configure makefiles, $(MAKECMDGOALS))" ""
  -include $(SAGA_DEPEND)
 endif
endif

# 'make depend' re-creates dependencies if needed
.PHONY: depend 
depend:: error subdirs $(SAGA_DEPEND)


# dependencies depend on source file, and get 
# re-created automatically
.depend/%.d:: %.cpp
	@$(ECHO) "      check        $<"
	@$(MKDIR)    .depend
	@$(SAGA_MKDEP) $(SAGA_CPPFLAGS) $(SAGA_CPPINCS) $(SAGA_CXXFLAGS) $< > $@

.depend/%.d:: %.c
	@$(ECHO) "      check        $<"
	@$(MKDIR)    .depend
	@$(SAGA_MKDEP) $(SAGA_CPPFLAGS) $(SAGA_CPPINCS) $(SAGA_CCFLAGS) $< > $@
#
###########################################################

###########################################################
#
# main rule set
#
ifeq "$(SAGA_DEBUG_MAKE)" "yes"
$(SAGA_ALL_PRE):  %.ii : %.cpp
	@$(ECHO) "      precompiling $@"
	@$(SAGA_CXX)    $(SAGA_CPPFLAGS) $(SAGA_CPPDEFS) $(SAGA_CPPINCS) -c -o $@ $<

$(SAGA_ALL_PREC): %.i : %.c
	@$(ECHO) "      precompiling $@"
	@$(SAGA_CC)     $(SAGA_CPPFLAGS) $(SAGA_CPPDEFS) $(SAGA_CPPINCS) -c -o $@ $<

$(SAGA_ALL_OBJ):  %.o : %.ii
	@$(ECHO) "      compiling    $@"
	@$(SAGA_CXX)    $(SAGA_CXXFLAGS) -c -o $@ $<

$(SAGA_ALL_OBJC): %.o : %.i
	@$(ECHO) "      compiling    $@"
	@$(SAGA_CC)     $(SAGA_CCFLAGS) -c -o $@ $<
else
$(SAGA_ALL_OBJ):  %.o : %.cpp
	@$(ECHO) "      compiling    $@"
	@$(SAGA_CXX)    $(SAGA_CPPDEFS) $(SAGA_CPPINCS) $(SAGA_CXXFLAGS) -c -o $@ $<

$(SAGA_ALL_OBJC): %.o : %.c
	@$(ECHO) "      compiling    $@"
	@$(SAGA_CC)     $(SAGA_CPPDEFS) $(SAGA_CPPINCS) $(SAGA_CCFLAGS) -c -o $@ $<
endif

$(SAGA_BIN):      % : %.o $(SAGA_ADD_BIN_OBJ) $(SAGA_ADD_BIN_OBJC)
	@$(ECHO) "      binlinking   $@"
	@$(SAGA_LD_BIN) -o $@ $^ $(SAGA_USE_LDFLAGS_BIN)

$(SAGA_ARCNAME):  $(SAGA_LIB_OBJ) $(SAGA_ADD_LIB_OBJ) $(SAGA_LIB_OBJC) $(SAGA_ADD_LIB_OBJC)
	@$(ECHO) "      liblinking   $@ (static)"
	@$(SAGA_AR)     $(SAGA_ARFLAGS) $@ $^

$(SAGA_LIBNAME):  $(SAGA_LIB_OBJ) $(SAGA_ADD_LIB_OBJ) $(SAGA_LIB_OBJC) $(SAGA_ADD_LIB_OBJC)
	@$(ECHO) "      liblinking   $@"
	@$(SAGA_LD_LIB) -o $@ $^ $(SAGA_USE_LDFLAGS_LIB)
#
###########################################################

###########################################################
#
# cleanup rules
#
SAGA_CLEAN_TGT     += $(wildcard autom4te.cache)
SAGA_CLEAN_TGT     += $(wildcard config.log)
SAGA_CLEAN_TGT     += $(wildcard config.summary)
SAGA_CLEAN_TGT     += $(wildcard confdefs.h)
SAGA_CLEAN_TGT     += $(SAGA_LIBNAME_CLEAN)
SAGA_CLEAN_TGT     += $(SAGA_ARCNAME_CLEAN)
SAGA_CLEAN_TGT     += $(SAGA_LIBNAME)
SAGA_CLEAN_TGT     += $(SAGA_ARCNAME)
SAGA_CLEAN_TGT     += $(SAGA_BIN) 
SAGA_CLEAN_TGT     += $(SAGA_ALL_OBJ)
SAGA_CLEAN_TGT     += $(SAGA_ALL_OBJC)
SAGA_CLEAN_TGT     += $(SAGA_ALL_PRE)
SAGA_CLEAN_TGT     += $(SAGA_ALL_PREC)

SAGA_DISTCLEAN_TGT += $(wildcard config.status)
SAGA_DISTCLEAN_TGT += $(SAGA_MAK)
SAGA_DISTCLEAN_TGT += $(SAGA_MAK_I)
SAGA_DISTCLEAN_TGT += $(SAGA_MAK_C)

SAGA_CONFCLEAN_TGT += $(SAGA_CONF)

SAGA_DEPCLEAN_TGT  += $(wildcard .depend)

clean:: subdirs
ifneq "$(SAGA_CLEAN_TGT)" ""
	@$(ECHO) "      clean"
	@$(RM) -r  $(SAGA_CLEAN_TGT)
endif
	
libclean:: subdirs
	@$(ECHO) "      clean libs"
ifneq "$(SAGA_LIBNAME_CLEAN)" ""
	@$(RM)          $(SAGA_LIBNAME_CLEAN)
endif
ifneq "$(SAGA_ARCNAME_CLEAN)" ""
	@$(RM)          $(SAGA_ARCNAME_CLEAN)
endif

distclean:: depclean clean
ifneq "$(SAGA_DISTCLEAN_TGT)" ""
	@$(ECHO) "      clean all"
	@$(RM) -r  $(SAGA_DISTCLEAN_TGT)
endif

confclean:: distclean
ifneq "$(SAGA_CONFCLEAN_TGT)" ""
	@$(ECHO) "      clean confs"
	@$(RM) -r  $(SAGA_CONFCLEAN_TGT)
endif

depclean:: subdirs
ifneq "$(SAGA_DEPCLEAN_TGT)" ""
	@$(ECHO) "      clean deps"
	-@$(RM) -r $(SAGA_DEPCLEAN_TGT)
endif
#
###########################################################

endif # guard

