
SAGA_ROOT     = $(shell pwd)/../../../

SAGA_BIN     += cpi_test

include $(SAGA_ROOT)/make/saga.mk

SAGA_LDFLAGS += -L../../../saga/util/ -lsaga_util

check:: all
	valgrind --tool=memcheck --leak-check=full ./$(SAGA_BIN)

run: all
	/usr/bin/env LD_LIBRARY_PATH=`pwd`/adaptor/ ./$(SAGA_BIN)

