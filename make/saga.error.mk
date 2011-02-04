
ifndef SAGA_MK_INC_ERROR

SAGA_MK_INC_ERROR = yes

.PHONY: error
error::
ifdef SAGA_MAKE_ERROR
	$(error $(SAGA_MAKE_ERROR))
endif

endif # guard

