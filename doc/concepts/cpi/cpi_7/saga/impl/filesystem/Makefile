
SAGA_ROOT     = $(shell pwd)/../../../../../../../

SAGA_LIB     += saga_impl_filesystem
SAGA_LIB_OBJ += file.o
SAGA_LIB_OBJ += dir.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../../../
SAGA_LDFLAGS  += -L$(SAGA_PWD)/../../../saga/engine/ -lsaga_engine

SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread

