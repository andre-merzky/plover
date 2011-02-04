
ifndef SAGA_MK_INC_DEFAULT
       SAGA_MK_INC_DEFAULT = yes

###########################################################
# 
# default config file for make.  
#
# This config file should contain enough information to 
# get parts of the make system running BEFORE configure 
# was called.  It does not need to enable actual source
# compilation.
#
###########################################################

###########################################################
#
# Tools
#
CP                         = cp -f
LN                         = ln -s
LS                         = ls
MV                         = mv -f
RM                         = rm -f
AWK                        = awk
CAT                        = cat
CUT                        = cut
ENV                        = env
PWD                        = pwd
SED                        = sed
DATE                       = date
GREP                       = grep
ECHO                       = echo
FIND                       = find
TRUE                       = true
MKDIR                      = mkdir -p
TOUCH                      = touch
FALSE                      = false

MAKE                      += --no-print-directory
#
############################################################

endif # guard

