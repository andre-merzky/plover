
SAGA_HDR         = $(wildcard *.hpp)

SAGA_ROOT        = $(shell pwd)/../../../
SAGA_BIN         = test_shared_ptr
SAGA_BIN        += test_shareable

include $(SAGA_ROOT)/make/saga.application.mk

SAGA_CXXFLAGS += -I../
SAGA_LDFLAGS  += -L../ -lsaga_util -lpthread

