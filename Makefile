
SAGA_SUBDIRS = config make saga examples

SAGA_ROOT    = $(shell pwd)

include $(SAGA_ROOT)/make/saga.mk

examples: saga

