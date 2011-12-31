
SAGA_ROOT     = $(shell pwd)/../../../../../../../

SAGA_LIB     += saga_impl_async
SAGA_LIB_OBJ += task.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../../../
SAGA_LDFLAGS  += -L$(SAGA_PWD)/../../../saga/engine/ -lsaga_engine

SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread

