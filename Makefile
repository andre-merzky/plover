
# SAGA_SUBDIRS = config make generator saga doc # examples
SAGA_SUBDIRS = saga doc # examples

doc: saga

SAGA_ROOT    = $(shell pwd)

include $(SAGA_ROOT)/make/saga.mk

examples: saga

edit:
	@gvim -o \
    Makefile \
    saga/util/Makefile \
    saga/util/shared_ptr.hpp \
    saga/util/shared_ptr_tr1.hpp \
    saga/util/shared_ptr_saga.hpp \
    saga/util/demangle.hpp \
    doc/concepts/pimpl/Makefile \
    doc/concepts/pimpl/README \
    doc/concepts/pimpl/dummy_shared_ptr.hpp \
    doc/concepts/pimpl/doc_pimple_multiple_inheritance.cpp \
    doc/concepts/pimpl/doc_pimple_shared_pointer.cpp \
    doc/concepts/pimpl/doc_pimple_shared_pointer_created.cpp \
    doc/concepts/adaptor/adaptor_1/Makefile \
    doc/concepts/adaptor/adaptor_1/adaptor.hpp \
    doc/concepts/adaptor/adaptor_1/adaptor.cpp \
    doc/concepts/adaptor/adaptor_2/Makefile \
    doc/concepts/adaptor/adaptor_2/adaptor.hpp \
    doc/concepts/adaptor/adaptor_2/adaptor.cpp \
    doc/concepts/adaptor/engine/Makefile \
    doc/concepts/adaptor/engine/cpi.hpp \
    doc/concepts/adaptor/engine/adaptor_registry.hpp \
    doc/concepts/adaptor/engine/adaptor.hpp \
    doc/concepts/adaptor/engine/adaptor.cpp \
    doc/concepts/adaptor/engine/engine.hpp \
    doc/concepts/adaptor/engine/engine.cpp \
    doc/concepts/adaptor/Makefile \
    doc/concepts/adaptor/adaptor_test.cpp

