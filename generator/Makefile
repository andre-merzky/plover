
SAGA_ROOT     = $(shell pwd)/../

include $(SAGA_ROOT)/make/saga.application.mk

all:: idl cpp

idl:
	@./tex2idl.pl *.tex

cpp: idl
	@./parse_idl.pl *.idl

clean::
	@rm -f *.idl
	

test::
	@-rm *.idl
	@./tex2idl.pl   saga_core_testing.tex
	@./parse_idl.pl saga_core_testing.idl
	@du -a saga_simple/ | grep -i testing
	# @cat saga_simple/saga/api/*testing*pp
	# @cat saga_simple/saga/api/testing/*pp


distclean::
	@rm -rf saga_simple/

