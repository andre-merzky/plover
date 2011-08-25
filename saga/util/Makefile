
SAGA_ROOT        = $(shell pwd)/../..

SAGA_SUBDIRS     = test

SAGA_HDR         = $(wildcard *.hpp)
SAGA_ARC         = saga_util
SAGA_LIB_OBJ    += assert.o  
SAGA_LIB_OBJ    += demangle.o  
SAGA_LIB_OBJ    += scoped_lock.o  
SAGA_LIB_OBJ    += shareable.o

include $(SAGA_ROOT)/make/saga.application.mk

test: $(SAGA_ARCNAME)

