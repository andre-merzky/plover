
SAGA_ROOT     = $(shell pwd)/../../../

SAGA_BIN     += cpi_1
SAGA_BIN     += cpi_2
SAGA_BIN     += cpi_3
SAGA_BIN     += cpi_4
SAGA_BIN     += cpi_5

include $(SAGA_ROOT)/make/saga.mk

SAGA_LDFLAGS  += -L../../../saga/util/ -lsaga_util -lpthread
SAGA_CXXFLAGS += -DDEBUG

check:: all
	@echo ""
	valgrind --tool=memcheck --track-origins=yes --leak-check=full ./cpi_5 2>&1 | tee valgrind.log
	@echo ""

run:: cpi_5
	@echo ""
	./cpi_5
	@echo ""

runall: all
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

