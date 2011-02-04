
SAGA_ROOT    = ../

SAGA_MAK_ALL = $(wildcard *.mk)
SAGA_MAK_INT = $(wildcard *.i.mk)
SAGA_MAK_D   = $(filter-out $(SAGA_MAK_INT),$(SAGA_MAK_ALL))

include $(SAGA_ROOT)/make/saga.mk

distclean::
	@echo '      remove     internal make configs'
	@-rm -f $(SAGA_PWD)/saga.*.c.mk
	
	@echo '      remove     internal make includes'
	@-rm -f $(SAGA_PWD)/saga.*.i.mk
	@-rm -f $(SAGA_PWD)/saga.engine.i.mk
	
	@echo '      remove     make includes'
	@-rm -f $(SAGA_PWD)/saga.package.*.mk
	@-rm -f $(SAGA_PWD)/saga.engine.mk
	
	@echo '      remove     package config etc'
	@-rm -f $(SAGA_PWD)/saga.pc
	@-rm -f $(SAGA_PWD)/saga.lite.mk
	@-rm -f $(SAGA_PWD)/saga-config
	@-rm -f $(SAGA_PWD)/saga.install.sh
	@-rm -f $(SAGA_PWD)/saga.test.env.mk
	
