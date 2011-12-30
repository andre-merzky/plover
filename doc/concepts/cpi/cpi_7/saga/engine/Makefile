
SAGA_ROOT     = $(shell pwd)/../../../../../../

SAGA_LIB     += saga_engine
SAGA_LIB_OBJ += engine.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../../
SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread

