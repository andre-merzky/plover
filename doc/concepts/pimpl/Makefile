
SAGA_HDR      = $(wildcard *.hpp)

SAGA_ROOT     = $(shell pwd)/../../../

SAGA_BIN     += doc_pimple_multiple_inheritance
SAGA_BIN     += doc_pimple_shared_pointer
SAGA_BIN     += doc_pimple_shared_pointer_created

include $(SAGA_ROOT)/make/saga.application.mk

# SAGA_CXXFLAGS += -DSAGA_USE_TR1
# SAGA_LDFLAGS  += -L../../../saga/util -l saga_util

check:: all
	valgrind --tool=memcheck --leak-check=full ./doc_pimple_shared_pointer

