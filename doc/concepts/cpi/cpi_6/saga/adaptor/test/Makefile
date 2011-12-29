
SAGA_ROOT     = $(shell pwd)/../../../../../../../

SAGA_LIB     += saga_adaptor_test

SAGA_LIB_OBJ += filesystem_adaptor_0.o
SAGA_LIB_OBJ += filesystem_adaptor_1.o
SAGA_LIB_OBJ += task_adaptor_0.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../../
SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread

