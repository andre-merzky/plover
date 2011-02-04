
ifndef SAGA_MK_NOTHING
       SAGA_MK_NOTHING = yes

.DEFAULT_GOAL := all
DEFAULT_GOAL::   all


.PHONY: nothing
nothing::
	@$(TRUE)

all::            nothing
subdirs_before:: nothing
subdirs_after::  nothing
check::          nothing
clean::          nothing
confclean::      nothing
configure::      nothing
depclean::       nothing
depend::         nothing
deps::           nothing
distclean::      nothing
error::          nothing
forceinstall::   nothing
install::        nothing
libclean::       nothing
makefiles::      nothing
warning::        nothing

Makefile.ok::    nothing
Makefile.nok::   nothing 

.PHONY: error
.PHONY: check

endif # guard

