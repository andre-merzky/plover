
AC_DEFUN([AX_SAGA_CHECK_BASE],
[
  SAGA_ROOT=`pwd`
  export SAGA_ROOT
  AC_SUBST(SAGA_ROOT)

  AC_LANG(C++)
  AC_REQUIRE_CPP
  
  AC_ARG_VAR
  AC_PROG_CC
  AC_PROG_CXX
  AC_PROG_CXXCPP

  AX_SAGA_CHECK_LIBDL()
  
  SAGA_CPP=$CPP
  SAGA_CXX=$CXX
  SAGA_CC=$CC
  SAGA_LD=$CXX
  
  SAGA_CPPFLAGS=$CPPFLAGS
  SAGA_CXXFLAGS=$CXXFLAGS
  SAGA_CCFLAGS=$CFLAGS
  SAGA_LDFLAGS_EXT="$LDFLAGS $LIBADD_DL"
  
  if test "_$SAGA_CPP" = "_" ; then
    SAGA_CPP=$CXX
  fi
  
  if test "_$SAGA_LD" = "_" ; then
    SAGA_LD=$CXX
  fi
  
  AC_SUBST(SAGA_CXX)
  AC_SUBST(SAGA_CPP)
  AC_SUBST(SAGA_CC)
  AC_SUBST(SAGA_LD)
  
  AC_SUBST(SAGA_CPPFLAGS)
  AC_SUBST(SAGA_CXXFLAGS)
  AC_SUBST(SAGA_CCFLAGS)
  AC_SUBST(SAGA_LDFLAGS_EXT)
  
  AC_PATH_PROG(SAGA_CP,    cp)
  AC_PATH_PROG(SAGA_LN,    ln)
  AC_PATH_PROG(SAGA_LS,    ls)
  AC_PATH_PROG(SAGA_MV,    mv)
  AC_PATH_PROG(SAGA_RM,    rm)
  AC_PATH_PROG(SAGA_TR,    tr)
  AC_PATH_PROG(SAGA_AWK,   awk)
  AC_PATH_PROG(SAGA_CAT,   cat)
  AC_PATH_PROG(SAGA_CUT,   cut)
  AC_PATH_PROG(SAGA_ENV,   env)
  AC_PATH_PROG(SAGA_PWD,   pwd)
  AC_PATH_PROG(SAGA_RPM,   rpmbuild)
  AC_PATH_PROG(SAGA_SED,   sed)
  AC_PATH_PROG(SAGA_TAR,   tar)
  AC_PATH_PROG(SAGA_DATE,  date)
  AC_PATH_PROG(SAGA_GREP,  grep)
  AC_PATH_PROG(SAGA_ECHO,  echo)
  AC_PATH_PROG(SAGA_FIND,  find)
  AC_PATH_PROG(SAGA_GZIP,  gzip)
  AC_PATH_PROG(SAGA_TEST,  test)
  AC_PATH_PROG(SAGA_TRUE,  true)
  AC_PATH_PROG(SAGA_MKDIR, mkdir)
  AC_PATH_PROG(SAGA_TOUCH, touch)
  AC_PATH_PROG(SAGA_FALSE, false)
  
  
  ###########################################################
  # 
  # lets see who finds this...
  # inspired by : configure of kvirc 2.0
  # 
  AC_MSG_CHECKING(number of dimensions)
  AC_MSG_RESULT([3.5*pi (spatial) + 1 (temporal)])
  AC_MSG_CHECKING(type of universe)
  AC_MSG_RESULT([4 bosons, 3 fermions, 1 higgs - ok, standard universe])
  AC_MSG_CHECKING(if e=mc^2)
  AC_MSG_RESULT([ok])
  AC_MSG_CHECKING(validity of Maxwell laws on this machine)
  AC_MSG_RESULT([ok])
  #
  ###########################################################
])


AC_DEFUN([AX_SAGA_CHECK_PACKAGES],
[
  AC_ARG_WITH([packages],
              AS_HELP_STRING([--with-packages=all],
                             [comma separated list of SAGA API packages to build.
                              possible values: job, namespace, file, logicalfile, 
                              stream, rpc, advert, message_bus, all (default is all).]),
              [
                if test "$withval" = "no"; then
                  want_packages=""
                elif test "$withval" = "yes"; then
                  want_packages="all"
                else
                  want_packages=$withval
                fi
              ],
              [want_packages="all"])

  if test "x$want_packages" = "x"; then
    AC_MSG_ERROR("It makes no sense to build SAGA withpout any API packages.")
  fi

  if test "x$want_packages" = "xall"; then
    want_packages="job,namespace,file,logicalfile,stream,rpc,advert,message_bus"
  fi

  while $TRUE
  do
    this_package=`echo  $want_packages |            cut -f1  -d','`
    want_packages=`echo $want_packages | grep ',' | cut -f2- -d','`

    if test "x$this_package" == "x"; then
      break
    fi

    case "$this_package" in

      job )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_JOB="yes"
        AC_SUBST(SAGA_PACKAGE_JOB)
        ;;

      file )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_FILE="yes"
        AC_SUBST(SAGA_PACKAGE_FILE)
        ;;

      logicalfile )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_LOGICALFILE="yes"
        AC_SUBST(SAGA_PACKAGE_LOGICALFILE)
        ;;

      namespace )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_NAME_SPACE="yes"
        AC_SUBST(SAGA_PACKAGE_NAME_SPACE)
        ;;

      stream )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_STREAM="yes"
        AC_SUBST(SAGA_PACKAGE_STREAM)
        ;;

      rpc ) 
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_RPC="yes"
        AC_SUBST(SAGA_PACKAGE_RPC)
        ;;

      advert )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_ADVERT="yes"
        AC_SUBST(SAGA_PACKAGE_ADVERT)
        ;;

      message_bus )
        build_packages="$build_packages$this_package "
        SAGA_PACKAGE_MESSAGEBUS="yes"
        AC_SUBST(SAGA_PACKAGE_MESSAGEBUS)
        ;;

      * )
        AC_MSG_ERROR(unknown package type '$this_package')
        ;;

    esac

  done

  SAGA_BUILD_PACKAGES=$build_packages
  export   SAGA_BUILD_PACKAGES
  AC_SUBST(SAGA_BUILD_PACKAGES)
])

