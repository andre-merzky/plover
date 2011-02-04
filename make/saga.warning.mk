
SAGA_MK_INC_WARNING = yes

.PHONY: warning
warning::
ifdef SAGA_MAKE_WARNING
	$(warning  -- $(SAGA_MAKE_WARNING))
endif

