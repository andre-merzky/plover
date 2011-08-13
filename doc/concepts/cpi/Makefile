
SAGA_ROOT     = $(shell pwd)/../../../

SAGA_BIN     += cpi_1
SAGA_BIN     += cpi_2
SAGA_BIN     += cpi_3
SAGA_BIN     += cpi_4
SAGA_BIN     += cpi_5

include $(SAGA_ROOT)/make/saga.mk

SAGA_LDFLAGS  += -L../../../saga/util/ -lsaga_util
SAGA_CXXFLAGS += -std=c++0x 

check:: all
	@echo ""
	valgrind --tool=memcheck --leak-check=full ./cpi_5
	@echo ""

run: all
	@echo ""
	./cpi_1
	@echo ""
	./cpi_2
	@echo ""
	./cpi_3
	@echo ""
	./cpi_4
	@echo ""
	./cpi_5
	@echo ""

