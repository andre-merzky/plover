
SAGA_ROOT     = $(shell pwd)/../../../../../

SAGA_BIN     += cpi_6

include $(SAGA_ROOT)/make/saga.mk

SAGA_CXXFLAGS += -I$(SAGA_PWD)

SAGA_LDFLAGS  += -L$(SAGA_PWD)/api/dir/  -lsaga_api_dir
SAGA_LDFLAGS  += -L$(SAGA_PWD)/api/file/ -lsaga_api_file
SAGA_LDFLAGS  += -L$(SAGA_PWD)/api/task/ -lsaga_api_task

SAGA_LDFLAGS  += -L$(SAGA_PWD)/cpi/dir/  -lsaga_cpi_dir
SAGA_LDFLAGS  += -L$(SAGA_PWD)/cpi/file/ -lsaga_cpi_file
SAGA_LDFLAGS  += -L$(SAGA_PWD)/cpi/task/ -lsaga_cpi_task

SAGA_LDFLAGS  += -L$(SAGA_ROOT)/saga/util/ -lsaga_util -lpthread


check:: all
	@echo ""
	valgrind --tool=memcheck --track-origins=yes --leak-check=full ./cpi_6 2>&1 | tee valgrind.log
	@echo ""

run:: cpi_6
	@echo ""
	./cpi_6
	@echo ""

runall: all
	./cpi_6
	@echo ""

