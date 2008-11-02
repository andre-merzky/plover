#  Copyright (c) 2005-2007 Andre Merzky (andre@merzky.net)
# 
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# vim: ft=make

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

