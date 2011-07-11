
SAGA_ROOT     = $(shell pwd)/../../../

SAGA_BIN     += cpi_1
SAGA_BIN     += cpi_2

include $(SAGA_ROOT)/make/saga.mk

SAGA_LDFLAGS += -L../../../saga/util/ -lsaga_util

check:: all
	@echo ""
	valgrind --tool=memcheck --leak-check=full ./cpi_1
	@echo ""

run: all
	@echo ""
	./cpi_1
	@echo ""
	./cpi_2
	@echo ""

