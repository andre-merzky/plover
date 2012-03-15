package generate_cpi; 

use strict; 
use warnings; 


use Exporter; 
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS); 

$VERSION     = 1.00; 
@ISA         = qw(Exporter); 
@EXPORT      = (); 
@EXPORT_OK   = ();
%EXPORT_TAGS = ();

use generator_helper;
use Data::Dumper;
use Storable qw(dclone);


########################################################################
#
# write an cpi enum header
#
sub enum_hpp ($$$$)
{
  my $BASE  = shift;
  my $pname = shift;
  my $ename = shift;
  my $enum  = shift;

  my $PNAME = uc ($pname);
  my $ENAME = uc ($ename);

  my $e_hdr = "saga/cpi/$pname/$ename.hpp";
  my $guard = generator_helper::get_guard ($e_hdr);
  my $out   = new IO::File ("> $BASE/$e_hdr") || die "Cannot create enum entry at $BASE/$e_hdr: $!\n";

  print $out generator_helper::format_guard_start (0, $guard);
  print $out generator_helper::format_include     (0, "saga/util/logging.hpp");
  print $out generator_helper::format_include     (0, "saga/util/enums.hpp");
  print $out generator_helper::format_ns_start    (0, "saga::$pname");
  print $out generator_helper::format_comment     (4, $enum->{'comm'});
  print $out "\n";
  print $out "    enum $ename\n";
  print $out "    {\n";


  my @ekeys = sort { $a <=> $b } keys ( %{$enum->{'def'}} );

  foreach my $i (  0..$#ekeys )
  {
    my $e = $ekeys[$i];

    my $n = $enum->{'def'}{$e}{'name'} || "";
    my $c = $enum->{'def'}{$e}{'comm'} || "";
    my $u = $enum->{'def'}{$e}{'used'} ||  0;

    # append comma for all but last enum
    my $comma = ',';
    if ( $i eq $#ekeys  ) { $comma = ' '; }

    # format comment as such
    if ( $c ) { $c = "    // $c"; }
  
    # is the enum used, or just a comment?
    if ( $u )
    {
      printf $out ("      %-20s = %5d$comma%s\n", $n, $e, $c);
    }
    else
    {
      printf $out ("   // %-20s = %5d$comma%s\n", $n, $e, $c);
    }
  }

  print $out "\n";
  print $out "    };\n";
  print $out generator_helper::format_ns_end    (4, "saga::$pname");
  print $out generator_helper::format_guard_end (0, $guard);

  $out->close;

  return $e_hdr;
}
########################################################################


########################################################################
#
# write an enum class or interface header
#
sub class_hpp ($$$$)
{
  my $BASE  = shift;
  my $pname = shift;
  my $cname = shift;
  my $class = shift;

  _fix_cpi_ctors    ($pname, $class);
  _fix_cpi_dtors    ($pname, $class);

  # print Dumper $class;

  my $is_async = $class->{'async'};

  my $PNAME = uc ($pname);
  my $CNAME = uc ($cname);

  my $i_hdr = "saga/cpi/$pname/$cname.hpp";
  my $guard = generator_helper::get_guard ($i_hdr);
  my $out   = new IO::File ("> $BASE/$i_hdr") || die "Cannot create enum entry at $BASE/$i_hdr: $!\n";


  print "class_hpp: $pname $cname\n";


  # print Dumper \$class if ( $cname eq "incorrect_state");

  my $impltxt = "";
  my $impls   = $class->{'impl'} || [];
  my $bases   = $class->{'base'} || [];

  my $first      = 1;
  my $ind        = 8;
  my $impl_ind   = ' ' x $ind;

  # C++ handles interfaces and base classes the same way, for inheritance
  foreach my $base ( @{$bases} )
  {
    if ( $first )
    {
      $impltxt .= "\n$impl_ind: public $base \t// base";
    }
    else
    {
      $impltxt .= "\n$impl_ind, public $base \t// base";
    }

    $first = 0;
  }

  foreach my $impl ( @{$impls} )
  {
    if ( $first )
    {
      $impltxt .= "\n$impl_ind: public $impl \t// interface";
    }
    else
    {
      $impltxt .= "\n$impl_ind, public $impl \t// interface";
    }

    $first = 0;
  }


  print $out generator_helper::format_guard_end (0, $guard);

  print $out generator_helper::format_include   (0, "saga/util/shareable.hpp");
  print $out generator_helper::format_include   (0, "saga/util/logging.hpp");
  print $out generator_helper::format_include   (0, "saga/util/stack_tracer.hpp");
  print $out generator_helper::format_include   (0, "saga/api/async/task.hpp") if ( $is_async );
  print $out generator_helper::format_include   (0, "saga/impl/$pname/$cname.hpp");
  print $out generator_helper::format_ns_start  (0, "saga::$pname");
  print $out generator_helper::format_comment   (4, $class->{'comm'});

  print $out "    class $cname$impltxt\n";
  print $out "    {\n";
  print $out "      private:\n";
  print $out "        saga::util::shared_ptr <saga::impl::${pname}::$cname> impl_;\n";
  print $out "\n";
  print $out "      public:\n";

  # ctor/dtor methods
  {
    print $out "        //--------------------------\n";
    print $out "        // construction/destruction \n";
    print $out "        //--------------------------\n\n";

    my $m_first = 1;
    foreach my $method ( @{$class->{'def'}{'methods'}} )
    {
      if ( $method->{'type'} eq 'ctor' ||
           $method->{'type'} eq 'dtor' )
      {
        my $mtxt = _format_method_sync_cpi_hpp ($ind, $method);

        # no empty line before first method
        if ( $m_first )
        {
          $mtxt =~ s/^\s*\n//io;
        }

        $m_first = 0;
        print $out "$mtxt\n";
      }
    }
  }


  # sync methods
  print $out "        //-------------------------\n";
  print $out "        // cpi method definitions  \n";
  print $out "        //-------------------------\n\n";

  foreach my $method ( @{$class->{'def'}{'methods'}} )
  {
    if ( $method->{'type'} eq 'sync' )
    {
      my $stxt .= _format_method_sync_cpi_hpp ($ind, $method);
      print $out "$stxt";

      if ( $is_async )
      {
        $method->{'comm'} = '';
        my $atxt .= _format_method_async_cpi_hpp ($ind, $method);
        print $out "\n$atxt";
      }
    }
    print $out "\n\n";
  }

  print $out "\n";
  print $out "    };\n";
  print $out generator_helper::format_ns_end    (4, "saga::$pname");
  print $out generator_helper::format_guard_end (0, $guard);

  $out->close;

  return $i_hdr;
}
########################################################################

########################################################################
sub _format_method_sync_cpi_hpp ($$)
{
  my $ind   = shift;
  my $m     = shift;
  my $out   = "";
  my $IND   = ' ' x $ind;

  my $mname  = $m->{'name'};
  my $mtype  = $m->{'type'};
  my $mbody  = $m->{'body'};
  my $minit  = $m->{'init'};
  my $rtype  = $m->{'rtype'};
  my $msign  = $m->{'sign'};
  my $oparam = $m->{'rpar'};


  my $comm   = generator_helper::format_comment ($ind, $m->{'comm'}); 

  if ( $comm )
  {
    $out .= $comm;
  }

  print " --- $mname\n";

  # print "----------------------------------\n" if ( $mname eq "get_test_obj");
  # print Dumper \$m                             if ( $mname eq "get_test_obj");
  # print "----------------------------------\n" if ( $mname eq "get_test_obj");

  my $tname = $m->{'temp'} || "";
  my $slen  = 0;

  if ( $tname )
  {
    $out .= "$IND" . "template <typename $tname>\n";
  }
  
  my $tmp  = "";
  my $body = "";
  
  if ( $mtype eq 'ctor' ||
       $mtype eq 'dtor' )
  {
    $tmp = sprintf ("$IND%-20s ", $mname);
  }
  else
  {
    $tmp = sprintf ("$IND%-20s  %-20s ", $rtype, $mname);
    $body = "return ($mname <saga::async::Sync> ($msign).get_result <$rtype> ());";
  }

  $out .= "$tmp";
  my $IND2 = ' ' x length ($tmp);

  $out .= _format_method_sig  ($IND2, $m, $body);
  $out .= _format_method_body ($IND , $m, $body);

  return $out;
}
########################################################################
#

########################################################################
sub _format_method_async_cpi_hpp ($$)
{
  my $ind   = shift;
  my $m     = shift;
  my $out   = "";
  my $IND   = ' ' x $ind;

  my $mname  = $m->{'name'};
  my $comm   = generator_helper::format_comment ($ind, $m->{'comm'}); 

  if ( $comm )
  {
    $out .= "$comm";
  }

  # print "----------------------------------\n" if ( $mname eq "get_test_obj");
  # print Dumper \$m                             if ( $mname eq "get_test_obj");
  # print "----------------------------------\n" if ( $mname eq "get_test_obj");

  # task types are specified as template parameter.  If the method itself is
  # templetized, we need to support multiple template parameters
  my $tname   = $m->{'temp'} || "";
  my $slen    = 0;

  if ( $tname )
  {
    # FIXME: warn "horribly template overload!" . Dumper \$m;
    # add the async flag to the template
    my $tmp = sprintf ("$IND" . "template <typename ASYNC,\n$IND          typename %-5s> ", $tname);
    $out .= $tmp;
  }
  else
  {
    # just the async template flag needed
    $out    .= "$IND" . "template <typename ASYNC>\n";
  }
  

  # return type for async calls is always saga::task.
  $out .= "";

  my $tmp  = sprintf ("$IND%-20s  %-20s ", 'saga::task', $mname);
  my $IND2 = ' ' x length ($tmp);


  my $body  = "";
  my $msign = $m->{'sign'};

  if ( $m->{'sigt'} eq 'void' )
  {
    $body = "return saga::async::task (impl_->$mname (ASYNC));";
  }
  else
  {
    $body = "return saga::async::task (impl_->$mname (ASYNC, $msign));";
  }

  $out    .= $tmp;
  $out    .= _format_method_sig  ($IND2, $m, $body); 
  $out    .= _format_method_body ($IND , $m, $body); 

  return $out;
}
########################################################################

########################################################################
#
# each CONSTRUCTOR needs to be replaced with a C++ c'tor, and an ctor(impl)
# needs to be added for the CPI layer
#
sub _fix_cpi_ctors ($$)
{
  my $pname = shift;
  my $entry = shift;

  my $ename = $entry->{'name'};

  if ( $entry->{'type'} eq 'interface' ||
       $entry->{'type'} eq 'class'     )
  {
    my $cdef            = $entry->{'def'};
    my $methods         = $cdef->{'methods'};
    my $have_ctor       = 0;
    my $use_session     = 0;
    my @wo_session_pars = ();
    my @new_methods     = ();

    foreach my $method ( @{$methods} )
    {
      if ( $method->{'name'} eq 'CONSTRUCTOR' )
      {
        # get list of params
        my $parstr    = "";
        my $first_par = 1;
        foreach my $par ( @{$method->{'params'}} )
        {
          # if we found a session as first parameter, save the remaining pars
          # w/o that session
          if ( $use_session )
          {
            push (@wo_session_pars, $par);
          }

          # also check if first ctor param is session
          if ( $first_par &&  $par->{'type'} eq 'session' )
          {
            $use_session = 1;
          }

          if ( $par->{'type'} ne $ename )
          {
            $parstr   .= ', ' unless $first_par;
            $parstr   .= $par->{'name'};
            $first_par = 0;
          }
        }

        # print Dumper \$method;
        # print "\n";
        $method->{'name'} = $ename;
        $method->{'init'} = "impl_ (new saga::impl::${pname}::${ename});";
        $method->{'body'} = "(void) impl_->constructor ($parstr);";
        $method->{'comm'} = "// ctor with session\n" . $method->{'comm'};
        chomp ($method->{'comm'});

        push (@new_methods, $method);

        # add a similar ctor w/o session, if needed
        if ( $use_session )
        {
          my $wo_session_ctor              = dclone ($method);
             $wo_session_ctor->{'params'}  = \@wo_session_pars;
             $wo_session_ctor->{'comm'}    = "// ctor without session";
             $wo_session_ctor->{'body'}    = "saga::core::session s (true); " . 
                                             "// use default session\n";
             $wo_session_ctor->{'body'}   .= $method->{'body'};

          push (@new_methods, $wo_session_ctor);
        }
      }
      else
      {
        push (@new_methods, $method);
      }
    }

    my $sp = "saga::util::shared_ptr ";

    # add a void ctor
    my %vctor = ('name'    => $ename,
                 'comm'    => '// void constructor',
                 'type'    => 'ctor',
                 'precomm' => '',
                 'init'    => 'impl_ (NULL)',
                 'body'    => ' ',
                 'detail'  => undef,
                 'sig'     => 'void',
                 'sigd'    => 'void',
                 'sigt'    => 'void',
                 'sign'    => '',
                 'params'  => []);

    push (@new_methods, \%vctor);


    # add an impl ctor
    my %ictor = ('name'    => $ename,
                 'comm'    => '// impl constructor',
                 'type'    => 'ctor',
                 'precomm' => '',
                 'detail'  => undef,
                 'sig'     => "$sp<saga::impl::${pname}::${ename}> impl",
                 'sigd'    => "$sp<saga::impl::${pname}::${ename}> impl",
                 'sigt'    => "$sp<saga::impl::${pname}::${ename}>",
                 'sign'    => 'impl',
                 'init'    => "impl_ (impl)",
                 'body'    => ' ',
                 'params'  => [ { 'mode'    => 'in',
                                  'name'    => 'impl',
                                  'default' => undef,
                                  'type'    => "$sp<saga::impl::${pname}::${ename}>"} ] );
    push (@new_methods, \%ictor);

    $cdef->{'methods'} = \@new_methods;

    print " === $pname\n";
  }
}
  
  

########################################################################
#
# we don't need no dtors...
#
sub _fix_cpi_dtors ($$)
{
  my $pname = shift;
  my $entry = shift;

  if ( $entry->{'type'} eq 'interface' ||
       $entry->{'type'} eq 'class'     )
  {
    my $cdef      = $entry->{'def'};
    my $methods   = $cdef->{'methods'};
    my @replaced  = ();

    foreach my $method ( @{$methods} )
    {
      if ( $method->{'name'} ne 'DESTRUCTOR' )
      {
        push (@replaced, $method);
      }
      else
      {
        # instead of the d'tor, we add a close call
        my %mclose = ('name'    => 'close',
                      'comm'    => '// close impl',
                      'precomm' => '',
                      'detail'  => undef,
                      'init'    => undef,
                      'type'    => 'sync',
                      'rtype'   => 'void',
                      'sig'     => 'void',
                      'sigd'    => 'void',
                      'sigt'    => 'void',
                      'sign'    => '',
                      'body'    => "impl_ = NULL;",
                      'params'  => [ ]
                     );
        push (@replaced, \%mclose);
      }
    }

    $cdef->{'methods'} = \@replaced;
  }
}
  

######################################################################
#
#
#
sub _format_method_sig ($$;$)
{
  my $ind  = shift;
  my $m    = shift;
  my $body = shift || $m->{'body'};
  my $out  = "";


  # FIXME: do not use refs for SAGA typed out/inout pars
  
  if ( $m->{'sigt'} eq 'void' )
  {
    # no param got inserted - method has void signature)
    $out .= "(void)";
  }
  else
  {
    $out = "(";

    # add parameter signatures
    my $first_par = 1; # noi indent for first param
    my $oparam    = $m->{'rpar'} || "";

    foreach my $param ( @{$m->{'params'}} )
    {
      my $pname = $param->{'name'};
      my $pmode = $param->{'mode'};
      my $pdef  = $param->{'default'};
      my $ptype = $param->{'type'};

      unless ( $pname eq $oparam )
      {
        # out and inout params are to be passed by reference pointers
        # FIXME: not true for saga object types, but true otherwise *sigh*

        my $pref = " ";
        # unless ( $pmode eq 'in' )
        # {
        #   $pref = "*";
        # }


        $out       .= ",\n$ind "  unless ( $first_par );
        $out       .= sprintf ("%-15s $pref %s", $ptype, $pname);
        $first_par  = 0;
      }
    }

    $out .= ")";
  }

  return $out;
}

######################################################################
#
#
#
sub _format_method_body ($$;$)
{
  my $ind   = shift;
  my $m     = shift;
  my $mbody = shift || $m->{'body'};
  my $minit =          $m->{'init'};
  my $out   = "";

  if ( $mbody )
  {
    $out .= "\n";

    if ( $minit )
    {
      my $init_first = 1;
      foreach my $iline ( split (/\n/, $minit) )
      {
        if ( $init_first )
        {
          $out .= "$ind    : $iline\n";
          $init_first = 0;
        }
        else
        {
          $out .= "$ind    , $iline\n";
        }
      }
    }

    if ( $mbody =~ /^\s*$/io )
    {
      $out .= "$ind" . "{\n$ind" . "}\n";
    }
    else
    {
      $out .= "$ind" . "{\n";
      foreach my $bline ( split (/\n/, $mbody) )
      {
        $out .= "$ind  $bline\n";
      }
      $out .= "$ind}\n";
    }
  }
  else
  {
    $out .= ';';
  }

  return $out;
}
  

1;

