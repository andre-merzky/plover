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

########################################################################
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
  my $out   = new IO::File ("> $BASE/$e_hdr") || die "Cannot create enum entry at $BASE/$e_hdr: $!\n";

  my $comm  = generator_helper::format_comment (4, $enum->{'comm'});

  if ( $comm )
  {
    $comm = "\n$comm";
    chomp $comm;
  }

#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/util/logging.hpp>
#include <saga/util/enums.hpp>

namespace saga
{
  namespace $pname
  { $comm
    enum $ename 
    {
EOT
#------------------------------------------------------------


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
      printf $out ("      %-18s = %5d$comma%s\n", $n, $e, $c);
    }
    else
    {
      printf $out ("   // %-18s = %5d$comma%s\n", $n, $e, $c);
    }
  }

#------------------------------------------------------------
  print $out <<EOT;
    };

  } // namespace $pname

} // namespace saga

#endif // $guard

EOT
#------------------------------------------------------------
    
  $out->close;

  return $e_hdr;
}
########################################################################


########################################################################
sub iface_hpp ($$$$)
{
  my $BASE  = shift;
  my $pname = shift;
  my $iname = shift;
  my $iface = shift;

  my $PNAME = uc ($pname);
  my $INAME = uc ($iname);

  my $i_hdr = "saga/api/$pname/$iname.hpp";
  my $guard = generator_helper::get_guard ($i_hdr);
  my $out   = new IO::File ("> $BASE/$i_hdr") || die "Cannot create enum entry at $BASE/$i_hdr: $!\n";


  print "iface_hpp: $pname $iname\n";

  my $comm  = generator_helper::format_comment (4, $iface->{'comm'});

  if ( $comm )
  {
    $comm = "\n$comm";
    chomp $comm;
  }


  # print Dumper \$iface if ( $iname eq "incorrect_state");

  my $impltxt = "";
  my $impls   = $iface->{'impl'} || [];
  my $bases   = $iface->{'base'} || [];

  my $first      = 1;
  my $impl_async = 0;
# my $impl_ind   = ' ' x (11 + length ($iname));
  my $impl_ind   = ' ' x 8;

  # C++ handles interfaces and base classes the same way, for inheritance :-(
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
    if ( $impl eq 'saga::async' )
    {
      $impl_async = 1;
    }

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


#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/task.hpp>
#include <saga/impl/$pname/$iname.hpp>

namespace saga
{
  namespace $pname
  { $comm
    class $iname$impltxt
    {
      private:
        saga::util::shared_ptr <saga::impl::${pname}::$iname> impl_;

      public:
EOT
#------------------------------------------------------------


  # sync methods
  {
    if ( $impl_async )
    {
      print $out "      //-------------------------\n";
      print $out "      // sync method definitions \n";
      print $out "      //-------------------------\n\n";
    }

    my $m_first = 1;
    foreach my $method ( @{$iface->{'def'}{'methods'}} )
    {
      my $mtxt = _format_method_sync_api_hpp (6, $method);

      # no empty line before first method
      if ( $m_first )
      {
        $mtxt =~ s/^\s*\n//io;
      }

      $m_first = 0;
      print $out "$mtxt\n";
    }
  }

  # async methods
  if ( $impl_async )
  {
    print $out "\n\n";
    print $out "      //--------------------------\n";
    print $out "      // async method definitions \n";
    print $out "      //--------------------------\n\n";

    my $m_first = 1;
    foreach my $method ( @{$iface->{'def'}{'methods'}} )
    {
      my $mtxt = _format_method_async_api_hpp (6, $method);

      # no empty line before first method
      if ( $m_first )
      {
        $mtxt =~ s/^\s*\n//io;
      }

      $m_first = 0;
      print $out "$mtxt\n";
    }
  }

#------------------------------------------------------------
  print $out <<EOT;
    };

  } // namespace $pname

} // namespace saga

#endif // $guard

EOT
#------------------------------------------------------------
    
  $out->close;

  return $i_hdr;
}
########################################################################

########################################################################
sub _format_method_sync_api_hpp ($$)
{
  my $ind   = shift;
  my $m     = shift;
  my $out   = "";
  my $IND   = ' ' x $ind;

  my $mname  = $m->{'name'};
  my $comm   = generator_helper::format_comment ($ind, $m->{'comm'}); 

  if ( $comm )
  {
    $out .= "\n$comm";
  }

  print " --- $mname\n";

  # print "----------------------------------\n" if ( $mname eq "get_test_obj");
  # print Dumper \$m                             if ( $mname eq "get_test_obj");
  # print "----------------------------------\n" if ( $mname eq "get_test_obj");

  # search for 'out' params - the first will determine the return value type.
  # If none is found, we have a void method.  Rember that param's index, as we
  # don't need it for the signature
  my $mtype  = "void";
  my $oparam = "";

  FORMAT_METHOD_SYNC_PARAM:
  foreach my $param ( @{$m->{'params'}} )
  {
    if ( $param->{'mode'} eq 'out' )
    {
      $oparam = $param->{'name'};
      $mtype  = $param->{'type'};
      last FORMAT_METHOD_SYNC_PARAM;
    }
  }

  my $tname = $m->{'temp'} || "";
  my $slen  = 0;

  if ( $tname )
  {
    $out .= "\n$IND" . "template <typename $tname>\n";
  }
  
  my $tmp = sprintf ("$IND%-18s  %-20s (", $mtype, $mname);
  $out .= $tmp;
  $IND = ' ' x length  ($tmp);

  # add parameter signatures
  my $noind = 1; # noi indent for first param

  foreach my $param ( @{$m->{'params'}} )
  {
    my $pname = $param->{'name'};
    my $pmode = $param->{'mode'};
    my $pdef  = $param->{'default'};
    my $ptype = $param->{'type'};

    # skip output param - already handled as return type
    unless ( $pname eq $oparam )
    {
      # out and inout params are to be passed by reference pointers
      my $pref = " ";
      unless ( $pmode eq 'in' )
      {
        $pref = "*";
      }

      my $ptxt = sprintf ("%-18s $pref %s\n", $ptype, $pname);

      unless ( $noind )
      {
        $out .= $IND;
      }

      $out .= $ptxt;
    
      $noind = 0;
    }
  }

  # remove last \n
  chomp ($out);

  # close method signature
  if ( $noind )
  {
    # no param got inserted - method has void signature)
    $out .= "void);\n";
  }
  else
  {
    $out .= ");\n";
  }

  chomp ($out);

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
  my $comm   = generator_helper::format_comment ($ind, $m->{'comm'}); 

  if ( $comm )
  {
    $out .= "\n$comm";
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
    # add the async flag to the template
    my $tmp = sprintf ("$IND" . "template <typename ASYNC,\n$IND          typename %-5s> ", $tname);
    $out .= $tmp;
  }
  else
  {
    # just the async template flag needed
    $out    .= "$IND" . "template <typename ASYNC> ";
  }
  

  # return type for async calls is always saga::task.  We still need to filter
  # out the original out par, as that will not be part of the func signature,
  # but will be returned by the task on get_result<mtype>();
  my $oparam = "";

  FORMAT_METHOD_SYNC_PARAM:
  foreach my $param ( @{$m->{'params'}} )
  {
    if ( $param->{'mode'} eq 'out' )
    {
      $oparam = $param->{'name'};
      last FORMAT_METHOD_SYNC_PARAM;
    }
  }
  $out .= "saga::task  ";

  my $tmp = sprintf ("%-20s (", $mname);
  $out .= $tmp;
  $IND = ' ' x ($ind + 38 + length ($tmp));

  # add parameter signatures
  my $noind = 1; # noi indent for first param

  foreach my $param ( @{$m->{'params'}} )
  {
    my $pname = $param->{'name'};
    my $pmode = $param->{'mode'};
    my $pdef  = $param->{'default'};
    my $ptype = $param->{'type'};

    unless ( $pname eq $oparam )
    {
      # out and inout params are to be passed by reference pointers
      my $pref = " ";
      unless ( $pmode eq 'in' )
      {
        $pref = "*";
      }

      my $ptxt = sprintf ("%-18s $pref %s\n", $ptype, $pname);

      unless ( $noind )
      {
        $out .= $IND;
      }

      $out .= $ptxt;
      $noind = 0;
    }
  }

  # remove last \n
  chomp ($out);

  # close method signature
  if ( $noind )
  {
    # no param got inserted - method has void signature)
    $out .= "void);\n";
  }
  else
  {
    $out .= ");\n";
  }

  chomp ($out);

  return $out;
}
########################################################################


1;

