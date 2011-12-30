
SAGA_ROOT     = $(shell pwd)/../../../../../

SAGA_SUBDIRS += engine api impl cpi adaptor

include $(SAGA_ROOT)/make/saga.mk

api:      impl
adaptors: cpi
impl:     cpi
cpi:      engine

