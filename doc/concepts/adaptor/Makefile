
SAGA_ROOT     = $(shell pwd)/../../../

SAGA_SUBDIRS += adaptor_1
SAGA_SUBDIRS += adaptor_2

SAGA_BIN     += adaptor_test

include $(SAGA_ROOT)/make/saga.mk

check:: all
	valgrind --tool=memcheck --leak-check=full ./adaptor_test

run: all
	/usr/bin/env LD_LIBRARY_PATH=`pwd`/adaptor/ ./$(SAGA_BIN)

