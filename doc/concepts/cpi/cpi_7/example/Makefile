
SAGA_ROOT     = $(shell pwd)/../../../../../

SAGA_BIN     += cpi_7

include $(SAGA_ROOT)/make/saga.mk

$(warning pwd: $(SAGA_PWD))

SAGA_CXXFLAGS += -I$(SAGA_PWD)/../

SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/api/filesystem/  -lsaga_api_filesystem
SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/api/async/       -lsaga_api_async

SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/impl/filesystem/ -lsaga_impl_filesystem
SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/impl/async/      -lsaga_impl_async

SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/cpi/filesystem/  -lsaga_cpi_filesystem
SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/cpi/async/       -lsaga_cpi_async

SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/engine/          -lsaga_engine

SAGA_LDFLAGS  += -L$(SAGA_PWD)/../saga/adaptor/test/    -lsaga_adaptor_test

SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/              -lsaga_util -lpthread


check:: all
	@echo ""
	valgrind --tool=memcheck --track-origins=yes --leak-check=full ./cpi_7 2>&1 | tee valgrind.log
	@echo ""

fullcheck:: all
	@echo ""
	valgrind --tool=memcheck --track-origins=yes --leak-check=full --show-reachable=yes ./cpi_7 2>&1 | tee valgrind.log
	@echo ""

run:: cpi_7
	@echo ""
	./cpi_7
	@echo ""

runall: all
	./cpi_7
	@echo ""

