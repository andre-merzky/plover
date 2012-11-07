package generate_api; 

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
# write an api enum header
#
sub enum_hpp ($$$$)
{
  my $BASE  = shift;
  my $pname = shift;
  my $ename = shift;
  my $enum  = shift;

  my $PNAME = uc ($pname);
  my $ENAME = uc ($ename);

  my $e_hdr = "saga/api/$pname/$ename.hpp";
  my $guard = generator_helper::get_guard ($e_hdr);
  my $out   = new IO::File ("> $BASE/$e_hdr") 
           || die "Cannot create enum header at $BASE/$e_hdr: $!\n";


  print $out generator_helper::format_guard_start (0, $guard);
  print $out generator_helper::format_include     (0, "saga/util/logging.hpp");
  print $out generator_helper::format_include     (0, "saga/util/enums.hpp");
  print $out generator_helper::format_ns_start    (0, "saga::$pname");
  print $out generator_helper::format_comment     (4, $enum->{'comm'});
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

  print $out "    };\n";
  print $out generator_helper::format_ns_end    (4, "saga::$pname");
  print $out generator_helper::format_guard_end (0, $guard);
    
  $out->close;

  return $e_hdr;
}
########################################################################


########################################################################
#
# write an class or interface header
#
sub class_hpp ($$$$)
{
  my $BASE       = shift;
  my $pname      = shift;
  my $cname      = shift;
  my $class      = shift;

  _fix_api_ctors ($pname, $class);

  # print Dumper $class;

  my $is_async   = $class->{'async'};
  my $is_iface   = _class_is_iface  ($class);
  my $is_class   = ! $is_iface;
  my $has_base   = _class_has_base  ($class);
  my $has_iface  = _class_has_iface ($class);

  my $PNAME      = uc ($pname);
  my $CNAME      = uc ($cname);

  my $i_hdr      = "saga/api/$pname/$cname.hpp";
  my $guard      = generator_helper::get_guard ($i_hdr);
  my $out        = new IO::File ("> $BASE/$i_hdr") 
                || die "Cannot create class header at $BASE/$i_hdr: $!\n";


  print "-> $i_hdr\n";


  my $impltxt    = "";
  my $impls      = $class->{'impl'} || [];
  my $bases      = $class->{'base'} || [];

  my $first      = 1;
  my $ind        = 8;
  my $impl_ind   = ' ' x $ind;


  # C++ handles interfaces and base classes the same way, for inheritance
  if ( $has_base )
  {
    foreach my $base ( @{$bases} )
    {
      if ( $first )
      {
        $impltxt .= sprintf "\n$impl_ind: public %-20s // base", $base;
      }
      else
      {
        $impltxt .= sprintf "\n$impl_ind, public %-20s // base", $base;
      }

      $first = 0;
    }
  }

  if ( $has_iface )
  {
    foreach my $impl ( @{$impls} )
    {
      if ( $first )
      {
        $impltxt .= sprintf "\n$impl_ind: public %-20s // interface", $impl;
      }
      else
      {
        $impltxt .= sprintf "\n$impl_ind, public %-20s // interface", $impl;
      }

      $first = 0;
    }
  }

  unless ( $has_base || $has_iface )
  {
    $impltxt .= "\n$impl_ind: public virtual base::pimpl // pimpl base";
  }

  print $out generator_helper::format_guard_start (0, $guard);

  print $out generator_helper::format_include  (0, "saga/util/shareable.hpp");
  print $out generator_helper::format_include  (0, "saga/util/logging.hpp");
  print $out generator_helper::format_include  (0, "saga/util/stack_tracer.hpp");
  print $out "\n";
  print $out generator_helper::format_include  (0, "saga/api/async/task.hpp") if ( $is_async );
  print $out generator_helper::format_include  (0, "saga/impl/$pname/$cname.hpp");

  unless ( $has_base || $has_iface )
  {
    print $out generator_helper::format_include  (0, "saga/base/pimpl.hpp");
  }

  print $out "\n\n";
  print $out generator_helper::format_ns_start (0, "saga::$pname");
  print $out generator_helper::format_comment  (4, $class->{'comm'});

  print $out "    class $cname$impltxt\n";
  print $out "    {\n";
  print $out _format_private_typedefs ($ind, "saga::impl::$pname\::$class->{name}");
  print $out _format_private_ctors    ($ind, $class);
  print $out _format_protected_ctors  ($ind, $class);
  print $out _format_public_ctors     ($ind, $class);


  # sync methods
  print $out "      public:\n";
  print $out "        //-------------------------\n";
  print $out "        // api method definitions  \n";
  print $out "        //-------------------------\n\n";

  foreach my $method ( @{$class->{'def'}{'methods'}} )
  {
    if ( $method->{'type'} eq 'sync' )
    {
      print $out _format_method_sync_api_hpp ($ind, $method, $is_async);

      if ( $is_async )
      {
        print $out "\n";
        print $out _format_method_async_api_hpp ($ind, $method);
        print $out "\n";
      }

      print $out "\n";
    }
  }

  print $out "      }\n\n";
  print $out generator_helper::format_ns_end    (4, "saga::$pname");
  print $out generator_helper::format_guard_end (0, $guard);

  $out->close;

  return $i_hdr;
}
########################################################################

########################################################################
sub _format_method_sync_api_hpp ($$;$)
{
  my $ind      = shift;
  my $m        = shift;
  my $is_async = shift || 0;

  my $out      = "";
  my $IND      = ' ' x $ind;

  my $mname    = $m->{'name'};
  my $mtype    = $m->{'type'};
  my $mbody    = $m->{'body'};
  my $minit    = $m->{'init'};
  my $rtype    = $m->{'rtype'};
  my $msign    = $m->{'sign'};
  my $oparam   = $m->{'rpar'};


  # print "----------------------------------\n" if ( $mname eq "get_all_exceptions");
  # print " --- $mname\n"                        if ( $mname eq "get_all_exceptions");
  # print Dumper \$m                             if ( $mname eq "get_all_exceptions");
  # print "----------------------------------\n" if ( $mname eq "get_all_exceptions");

  $out .= generator_helper::format_m_comment ($ind, $m);

  my $tname = $m->{'temp'} || "";
  my $slen  = 0;

  if ( $tname )
  {
    $out .= "$IND" . "template <typename $tname>\n";
  }
  
  my $tmp  = "";
  my $body = "";
  
  if ( $mtype eq 'ctor' )
  {
    $tmp = sprintf ("$IND%-10s ", $mname);
  }
  elsif ( $mtype eq 'dtor' )
  {
    $tmp = sprintf ("$IND~%-10s ", $mname);
  }
  else
  {
    $tmp = sprintf ("$IND%-10s  %s  ", $rtype, $mname);
    if ( $is_async )
    {
      $body = "return ($mname <saga::async::Sync> ($msign).get_result <$rtype> ());";
    }
    else
    {
      $body = "return ( impl_->$mname ($msign) );";
    }
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
sub _format_method_async_api_hpp ($$)
{
  my $ind   = shift;
  my $m     = shift;
  my $out   = "";
  my $IND   = ' ' x $ind;

  my $mname  = $m->{'name'};
  # comment was printed for sync version already...

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

  my $tmp  = sprintf ("$IND%-10s  %s  ", 'saga::task', $mname);
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
# needs to be added for the API layer. 
#
# FIXME: also need copy ctor and assignment
#
sub _fix_api_ctors ($$)
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


    my $is_iface   = _class_is_iface  ($entry);
    my $is_class   = ! $is_iface;
    my $has_base   = _class_has_base  ($entry);
    my $has_iface  = _class_has_iface ($entry);

    foreach my $method ( @{$methods} )
    {
      if ( $method->{'name'} eq 'CONSTRUCTOR' )
      {
        # this is a public ctor
        $method->{'vis'} = 'public';

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
        $method->{'body'} = "(void) get_impl ()->constructor ($parstr);";
        $method->{'comm'} = "// ctor with session\n" . $method->{'comm'};
        chomp ($method->{'comm'});

        $method->{'init'} = sprintf ("%-20s (impl_t::create ().get_shared_ptr <impl::pimpl> ())", 
                                     'pimpl');

        foreach my $base ( @{$entry->{'base'}} ) { $method->{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $base; }
        foreach my $impl ( @{$entry->{'impl'}} ) { $method->{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $impl; }


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
      elsif ( $method->{'name'} eq 'DESTRUCTOR' )
      {
        # simply skip it
      }
      else
      {
        push (@new_methods, $method);
      }
    }


    my $sp = "saga::util::shared_ptr ";

    # add a void ctor
    my $mvis  = 'private';
    my $mcomm = '// impl->constructor needs to be called later on';

    if ( $has_base || $has_iface )
    {
      $mvis = 'public';
    }

    if ( $is_iface )
    {
      $mvis  = 'protected';
      $mcomm = '// interfaces cannot be created';
    }

    my %vctor = ('name'    => $ename,
                 'comm'    => '// void constructor',
                 'type'    => 'ctor',
                 'vis'     => $mvis,
                 'precomm' => '',
                 'init'    => sprintf ("%-20s (NULL)", "pimpl"),
                 'body'    => $mcomm,
                 'detail'  => undef,
                 'sig'     => 'void',
                 'sigd'    => 'void',
                 'sigt'    => 'void',
                 'sign'    => '',
                 'params'  => []);

    foreach my $base ( @{$entry->{'base'}} ) { $vctor{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $base; }
    foreach my $impl ( @{$entry->{'impl'}} ) { $vctor{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $impl; }

    push (@new_methods, \%vctor);


    if ( $has_base || $has_iface )
    {
      # add an impl ctor, if one needs to be passed down
      my %ictor = ('name'    => $ename,
                   'comm'    => '// impl ctor',
                   'type'    => 'ctor',
                   'vis'     => 'private',
                   'precomm' => '',
                   'detail'  => undef,
                   'sig'     => "$sp<impl_t> impl",
                   'sigd'    => "$sp<impl_t> impl",
                   'sigt'    => "$sp<impl_t>",
                   'sign'    => 'impl',
                   'init'    => sprintf ("%-20s (impl)", 'pimpl'),
                   'body'    => '// interfaces do not call impl->constructor again',
                   'params'  => [ { 'mode'    => 'in',
                                    'name'    => 'impl',
                                    'default' => undef,
                                    'type'    => "$sp<impl_t>"} ] );

      foreach my $base ( @{$entry->{'base'}} ) { $ictor{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $base; }
      foreach my $impl ( @{$entry->{'impl'}} ) { $ictor{'init'} .=  sprintf "\n%-20s (pimpl::NO_IMPL)", $impl; }

      push (@new_methods, \%ictor);

      # and a simple public dtor
      my %idtor = ('name'    => $ename,
                   'comm'    => '// dtor decrements impl ref counter',
                   'precomm' => '',
                   'detail'  => undef,
                   'init'    => undef,
                   'type'    => 'dtor',
                   'rtype'   => '',
                   'sig'     => 'void',
                   'sigd'    => 'void',
                   'sigt'    => 'void',
                   'sign'    => '',
                   'vis'     => 'public',
                   'body'    => ' ',
                   'params'  => [ ]
                 );

      push (@new_methods, \%idtor);
    }
    else
    {
      # otherwise a no-impl ctor...
      my %ictor = ('name'    => $ename,
                   'comm'    => '// no-impl ctor, passes noimpl flag on to pimpl',
                   'type'    => 'ctor',
                   'vis'     => 'protected',
                   'precomm' => '',
                   'detail'  => undef,
                   'sig'     => "base::noimpl_enum noimpl",
                   'sigd'    => "base::noimpl_enum noimpl",
                   'sigt'    => "base::noimpl_enum",
                   'sign'    => 'noimpl',
                   'init'    => sprintf ("%-20s  (noimpl)", "pimpl"),
                   'body'    => ' ',
                   'params'  => [ { 'mode'    => 'in',
                                    'name'    => 'noimpl',
                                    'default' => undef,
                                    'type'    => "base::noimpl_enum"} ] );

      push (@new_methods, \%ictor);


      # and a simple protected dtor
      my %idtor = ('name'    => $ename,
                   'comm'    => '// decrements impl ref counter',
                   'precomm' => '',
                   'detail'  => undef,
                   'init'    => undef,
                   'type'    => 'dtor',
                   'rtype'   => '',
                   'sig'     => 'void',
                   'sigd'    => 'void',
                   'sigt'    => 'void',
                   'sign'    => '',
                   'vis'     => 'protected',
                   'body'    => ' ',
                   'params'  => [ ]
                 );

      push (@new_methods, \%idtor);
    }

    # print Dumper \@new_methods;

    $cdef->{'methods'} = \@new_methods;
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

    my $len = 2;

    foreach my $param ( @{$m->{'params'}} )
    {
      my $ptype = $param->{'type'};
      my $pname = $param->{'name'};

      unless ( $pname eq $oparam )
      {
        if ( length ($ptype) > $len )
        {
          $len = length ($ptype);
        }
      }
    }

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
        $out       .= sprintf ("%-${len}s $pref %s", $ptype, $pname);
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
  

######################################################################
#
#
#
sub _format_private_typedefs($$)
{
  my $ind    = shift;
  my $itype  = shift;

  my $out   = "";
  my $todo  = 0;

  $out .= "      private:\n";
  $out .= "        // some helpful typedefs\n";
  $out .= "        typedef $itype impl_t\n";
  $out .= "\n";

  return $out;
}


######################################################################
#
#
#
sub _format_private_ctors($$)
{
  my $ind   = shift;
  my $class = shift;

  my $out   = "";
  my $todo  = 0;

  foreach my $method ( @{$class->{'def'}{'methods'}} )
  {
    if ( $method->{'type'} eq 'ctor' ||
         $method->{'type'} eq 'dtor' )
    {
      print Dumper \$method unless defined $method->{'vis'};
      if ( $method->{'vis'} eq 'private' )
      {
        $todo++;
      }
    }
  }

  if ( $todo )
  {
    $out .= "      private:\n";
    $out .= "        //--------------------------\n";
    $out .= "        // construction/destruction \n";
    $out .= "        //--------------------------\n\n";

    foreach my $method ( @{$class->{'def'}{'methods'}} )
    {
      if ( $method->{'type'} eq 'ctor' ||
           $method->{'type'} eq 'dtor' )
      {
        if ( $method->{'vis'} eq 'private' )
        {
          $out .= _format_method_sync_api_hpp ($ind, $method);
          $out .= "\n";
        }
      }
    }
  }

  $out .= "\n";

  return $out;
}


######################################################################
#
#
#
sub _format_protected_ctors($$)
{
  my $ind   = shift;
  my $class = shift;

  my $out   = "";
  my $todo  = 0;

  foreach my $method ( @{$class->{'def'}{'methods'}} )
  {
    if ( $method->{'type'} eq 'ctor' ||
         $method->{'type'} eq 'dtor' )
    {
      if ( $method->{'vis'} eq 'protected' )
      {
        $todo++;
      }
    }
  }

  if ( $todo )
  {
    $out .= "      protected:\n";
    $out .= "        //--------------------------\n";
    $out .= "        // construction/destruction \n";
    $out .= "        //--------------------------\n\n";

    foreach my $method ( @{$class->{'def'}{'methods'}} )
    {
      print Dumper \$method unless defined $method->{'type'};

      if ( $method->{'type'} eq 'ctor' ||
           $method->{'type'} eq 'dtor' )
      {
        if ( $method->{'vis'} eq 'protected' )
        {
          $out .= _format_method_sync_api_hpp ($ind, $method);
          $out .= "\n";
        }
      }
    }
  }

  $out .= "\n";

  return $out;
}

######################################################################
#
#
#
sub _format_public_ctors($$)
{
  my $ind   = shift;
  my $class = shift;

  my $out   = "";
  my $todo  = 0;

  foreach my $method ( @{$class->{'def'}{'methods'}} )
  {
    if ( $method->{'type'} eq 'ctor' ||
         $method->{'type'} eq 'dtor' )
    {
      if ( $method->{'vis'} eq 'public' )
      {
        $todo++;
      }
    }
  }

  if ( $todo )
  {
    $out .= "      public:\n";
    $out .= "        //--------------------------\n";
    $out .= "        // construction/destruction \n";
    $out .= "        //--------------------------\n\n";

    foreach my $method ( @{$class->{'def'}{'methods'}} )
    {
      if ( $method->{'type'} eq 'ctor' ||
           $method->{'type'} eq 'dtor' )
      {
        if ( $method->{'vis'} eq 'public' )
        {
          $out .= _format_method_sync_api_hpp ($ind, $method);
          $out .= "\n";
        }
      }
    }
  }

  $out .= "\n";

  return $out;
}

######################################################################
#
#
#
sub _class_is_iface ($)
{
  my $class = shift;

  if ( $class->{'type'} eq 'interface' )
  {
    return 1;
  }

  return 0;
}


######################################################################
#
#
#
sub _class_has_iface ($)
{
  my $class = shift;

  return 0 unless defined ($class);
  return 0 unless exists  ($class->{'impl'});
  return scalar ( @{$class->{'impl'}} );
}


######################################################################
#
#
#
sub _class_has_base ($)
{
  my $class = shift;

  return 0 unless defined ($class);
  return 0 unless exists  ($class->{'base'});
  return scalar ( @{$class->{'base'}} );
}

1;

