
SAGA_ROOT     = $(shell pwd)/../../../../../../

SAGA_LIB     += saga_engine
SAGA_LIB_OBJ += engine.o
SAGA_LIB_OBJ += result_types.o
SAGA_LIB_OBJ += call_context.o
SAGA_LIB_OBJ += cpi_base.o
SAGA_LIB_OBJ += impl_base.o
SAGA_LIB_OBJ += func.o

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../..
SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread --Wl,--no-as-needed

