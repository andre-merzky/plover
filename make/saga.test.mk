#
#   Copyright (c) 2008 João Abecasis
#
#   Distributed under the Boost Software License, Version 1.0. (See accompanying
#   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

ifndef SAGA_MK_INC_TEST
       SAGA_MK_INC_TEST = yes

SAGA_TEST_OBJ  += $(SAGA_TEST_SRC:%.cpp=%.o)
SAGA_TEST_BIN  += $(SAGA_TEST_SRC:%.cpp=%)

SAGA_TEST_RUN  += $(SAGA_TEST_BIN:%=%.run)
SAGA_TEST_OUT   = $(SAGA_TEST_RUN:%.run=%.out)
SAGA_TEST_ERR   = $(SAGA_TEST_RUN:%.run=%.err)

SAGA_OBJ       += $(SAGA_TEST_OBJ)
SAGA_OBJ       += $(SAGA_ADD_TEST_BIN_OBJ) $(SAGA_ADD_TEST_BIN_OBJC)

SAGA_CLEAN_TGT += $(SAGA_TEST_BIN)
SAGA_CLEAN_TGT += $(SAGA_TEST_RUN)
SAGA_CLEAN_TGT += $(SAGA_TEST_OUT)
SAGA_CLEAN_TGT += $(SAGA_TEST_ERR)

SAGA_BIN       += $(SAGA_TEST_BIN)

ifdef SAGA_ROOT
    include $(SAGA_ROOT)/make/saga.application.mk
else
    include $(SAGA_LOCATION)/share/saga/make/saga.application.mk
endif

# We need to run things directly from the source tree
ifeq "$(SAGA_OSTYPE)" "SAGA_APPLE"
  SAGA_LD_LIBRARY_PATH   = $(DYLD_LIBRARY_PATH)
else
  SAGA_LD_LIBRARY_PATH   = $(LD_LIBRARY_PATH)
endif

SAGA_LD_LIBRARY_PATH    += :$(SAGA_BOOST_LOCATION)/lib
SAGA_LD_LIBRARY_PATH    += :$(SAGA_ROOT)/saga/impl/engine

PACKAGE_DIRS             = $(wildcard $(SAGA_ROOT)/saga/impl/packages/*/)
SAGA_LD_LIBRARY_PATH    += $(foreach PACKAGE_DIR,$(PACKAGE_DIRS),:$(PACKAGE_DIR))

ifneq "x$(SAGA_BOOST_HAVE_SYSTEM)" "xyes"
  SAGA_LD_LIBRARY_PATH  += :$(SAGA_ROOT)/external/boost/system/libs/system/src
endif

-include $(SAGA_MAKE_INCLUDE_ROOT)/saga.test.env.mk

SAGA_LD_LIBRARY_PATH    := $(subst $(SAGA_SPACE):,:,$(SAGA_LD_LIBRARY_PATH))

ifeq "$(SAGA_OSTYPE)" "SAGA_APPLE"
  SAGA_ENVIRON          +=  SAGA_LOCATION=$(SAGA_LOCATION) DYLD_LIBRARY_PATH=$(SAGA_LD_LIBRARY_PATH) 
else
  SAGA_ENVIRON          +=  SAGA_LOCATION=$(SAGA_LOCATION)   LD_LIBRARY_PATH=$(SAGA_LD_LIBRARY_PATH) 
endif


.PHONY: check
check:: subdirs $(SAGA_TEST_RUN)


$(SAGA_TEST_RUN):: subdirs
$(SAGA_TEST_RUN):: %.run : %
	@$(ECHO) "      running    $<"
	@$(ENV) $(SAGA_ENVIRON) ./$< > $<.out 2> $<.err \
     && $(TOUCH) $@ \
     || { \
          $(ECHO)  "===================================="; \
          $(ECHO)  "=== COMMAND LINE ==================="; \
          $(ECHO)  "===================================="; \
          $(ECHO)  "$(ENV) $(SAGA_ENVIRON) ./$< > $<.out 2> $<.err"; \
          $(ECHO)  "===================================="; \
          $(ECHO)  "=== OUTPUT LOG ====================="; \
          $(ECHO)  "===================================="; \
          $(CAT)   $<.out; \
          $(ECHO)  "===================================="; \
          $(ECHO)  "=== ERROR LOG ======================"; \
          $(ECHO)  "===================================="; \
          $(CAT)   $<.err; \
          $(ECHO)  "===================================="; \
          $(TOUCH) $(SAGA_ROOT)/.saga_check_failed; \
     }

endif # guard

