
SAGA_SRC         = $(wildcard *.cpp)
SAGA_OBJ         = $(SAGA_SRC:%.cpp=%.o)
SAGA_BIN         = $(SAGA_SRC:%.cpp=%)

SAGA_ROOT    = $(shell pwd)/..

include $(SAGA_ROOT)/make/saga.application.mk

SAGA_CCFLAGS += -I../saga/saga/ -I../saga/
SAGA_LDFLAGS += -L../saga/saga/ -lsaga
SAGA_LDFLAGS += -L../saga/impl/ -lsaga_impl

