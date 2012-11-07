
SHELL = /bin/bash
SAGA_ROOT     = $(shell pwd)/../

include $(SAGA_ROOT)/make/saga.application.mk

all:: gen 

gen:
	@./generate_saga.pl saga_core_*.tex
	@dot -Tpng -O saga.dot

test1:: distclean
	@./generate_saga.pl   saga_core_object.tex
	@echo ------------------------------
#	@cat saga_simple/saga/api/*object*pp
	@echo ------------------------------
	@for file in saga_simple/saga/api/object/object_type.hpp; do \
		echo ------- $$file -----------; \
		cat $$file; \
		done
	@echo ------------------------------
	@for file in saga_simple/saga/api/object/object.hpp; do \
		echo ------- $$file -----------; \
		cat $$file; \
		done
	@echo ------------------------------


test2:: distclean
	@./generate_saga.pl   saga_core_error.tex
#	@echo ------------------------------
#	@du -a saga_simple/ | grep -e '\..pp'
	@echo ----- saga_simple/saga/api/error/exception.hpp -------------------------
	@cat saga_simple/saga/api/error/exception.hpp
#	@echo ----- saga_simple/saga/api/error/incorrect_state.hpp -------------------------
#	@cat saga_simple/saga/api/error/incorrect_state.hpp
#	@echo ----- saga_simple/saga/api/error.hpp -------------------------
#	@cat saga_simple/saga/api/error.hpp


distclean::
	@rm -rf saga_simple/

