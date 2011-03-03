
SAGA_ROOT         = $(shell pwd)/../../../../

SAGA_LIB         += saga_adaptor_2
SAGA_ADD_LIB_OBJ += adaptor.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS    += -I../
SAGA_LDFLAGS     += -L../../../../saga/util/ -lsaga_util

run:

