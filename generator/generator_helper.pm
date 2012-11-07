
package generator_helper; 

use strict; 
use warnings; 


use Exporter; 
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS); 

$VERSION     = 1.00; 
@ISA         = qw(Exporter); 
@EXPORT      = (); 
@EXPORT_OK   = ();
%EXPORT_TAGS = ();

use Data::Dumper;

########################################################################
#
# split textlist
#
#
# This method splits the following text
#
#   - this is list item a
#   - this is long list 
#item b
#   - this is long list 
#        item c
# - list item 4
#
#    - list item 5
#
# into a list which contains one item per entry (no '-')
#
sub split_textlist ($)
{
  my $txt   = shift;
  my @out   = ();
  my $item  = "";

  my @lines = split (/\s\n\s*/, $txt );

  foreach my $line ( @lines )
  {
    if ( $line =~ /^\s*-\s*(.*?)\s*$/o )
    {
      my $tmp  = $1 || "";

      push (@out, $item) if ( $item );

      $item = $tmp;
    }
    elsif ( $line =~ /^\s*([^-].*?)\s*$/o )
    {
      $item .= " $1";
    }
    else
    {
      # ignore empty lines
    }
  }

  # keep trailing item
  push (@out, $item) if ( $item );

  return @out;
}
#
########################################################################

########################################################################
#
# split textdict
#
#
# This method splits the following text
#
#   a: this is list item a
#   b: this is long list 
#item b
#   c: this is long list 
#        item c
# d: item 4
#
#    e: item 5
#
# into a list which contains one item per entry (no '-')
#
sub split_textdict ($)
{
  my $txt   = shift;
  my @out   = ();
  my $item  = "";

  my @lines = split (/\s\n\s*/, $txt );

  foreach my $line ( @lines )
  {
    if ( $line =~ /^\s*(\S+)\s*:\s+(.*?)\s*$/o )
    {
      my $key = $1;
      my $val = $2;

      push (@out, $item) if ( $item );

      $item = "$key : $val";
    }
    elsif ( $line =~ /^\s*(\S.*?)\s*$/o )
    {
      $item .= " $1";
    }
    else
    {
      # ignore empty lines
    }
  }

  # keep trailing item
  push (@out, $item) if ( $item );

  return @out;
}
#
########################################################################

########################################################################
sub get_guard ($)
{
  my $str  = shift;

  $str =~ tr/a-zA-Z0-9/_/cs;  # change non-alphasnums to single underscore

  return uc ($str);           #  return uppercase version
}


########################################################################
sub format_include ($$)
{
  my $ind  = shift;
  my $inc  = shift;


  return "#" . ( ' ' x $ind ) . "include <$inc>\n";   
}


########################################################################
sub format_guard_start ($$)
{
  my $ind   = shift;
  my $guard = shift;


  return "\n#" . ( ' ' x $ind ) . "ifndef $guard\n" .
           "#" . ( ' ' x $ind ) . "define $guard\n\n";
}


########################################################################
sub format_guard_end ($$)
{
  my $ind   = shift;
  my $guard = shift;


  return "\n#" . ( ' ' x $ind ) . "endif $guard\n\n";
}


########################################################################
sub format_ns_start ($$)
{
  my $ind  = shift;
  my $name = shift;

  my @ns   = split (/::/, $name);
  my $out  = "";

  foreach my $ns ( @ns )
  {
    $out .= ( ' ' x $ind ) . "namespace $ns\n";
    $out .= ( ' ' x $ind ) . "{\n";
    $ind += 2;
  }

  return $out;
}


########################################################################
sub format_ns_end ($$)
{
  my $ind  = shift;
  my $name = shift;

  my @ns   = split (/::/, $name);
  my $out  = "";

  foreach my $ns ( @ns )
  {
    $out .= ( ' ' x $ind ) . "} // namespace $ns\n";
    $ind -= 2;

    if ( $ind < 0 )
    {
      die "unbalanced namespaces?\n";
    }
  }

  return $out;
}


########################################################################
#
# reformat a C++ comment to have correct indentation 
# (// starting at given line indent)
#
sub format_comment ($$)
{
  my $ind    = shift;
  my $comm   = shift;

  if ( ! $comm )
  {
    return "";
  }

  my $out   = "";

  my $IND   = ' ' x $ind;
  my @lines = split (/\n/, $comm);
  my $start = 1; 

  COMM_LINE:
  foreach my $line ( @lines ) 
  {
    # remove leading empty lines

    if ( $start )
    {
      if ( $line =~ /^\s*(?:\/\/)?\s*$/o )
      {
        # ignore
        next COMM_LINE;
      }
      
      # non-empty line - comment starts
      $start = 0;
    }

    if ( $line =~ /^\s*\/\/(.*)$/o )
    {
      my $txt = $1 || " ";

      $out .= "$IND//$txt\n";
    }
    elsif ( $line =~ /^\s*$/o )
    {
      # ignore empty lines
    }
    else
    {
      die "can't parse comment line '$line'\n";
    }
  }

  return $out;
}
########################################################################

########################################################################
#
# reformat a C++ comment to have correct indentation 
# (// starting at given line indent)
#
sub format_m_comment ($$)
{
  my $ind  = shift;
  my $m    = shift;

  my $out  = format_comment ($ind, $m->{'comm'});
     $out .= format_detail  ($ind, $m, $out);

  return $out;
}
########################################################################

########################################################################
#
# reformat a C++ comment from method specification details
# (// starting at given line indent)
#
sub format_detail ($$;$)
{
  my $ind = shift;
  my $m   = shift;
  my $old = shift || "";

  if ( ! $m->{'detail'} )
  {
    return "";
  }

  my $out    = "";
  my $IND    = ' ' x $ind;
     $IND   .= '/' x 3;
  my $IND2   = ' ' x $ind;
     $IND2  .= '/' x (80 - $ind);
  my %detail = %{$m->{'detail'}};

  if ( $old )
  {
    $out .= "$IND// \n";
  }

  $out .= "$IND2\n";
  $out .= "$IND \n";

  if ( exists $detail{'Purpose'} )
  {
    $out .= "$IND \\brief: $detail{Purpose}\n";
    $out .= "$IND \n";
  }

  # we don't really need 'format'
  # if ( exists $detail{'Format'} )
  # {
  #   $out .= "$IND Format: $detail{Format}\n";
  # }

  if ( exists $detail{'Inputs'} )
  {
    my @itemlist = split_textdict ($detail{'Inputs'});

    foreach my $item ( @itemlist )
    {
      if ( $item =~ /^(\S*)\s*:\s*(\S.*?)\s*$/o )
      {
        my $par = $1;
        my $doc = $2;

        $out .= "$IND \\param[in] $par $doc\n";
      }
      else
      {
        die "Cannot parse parameter doc string '$item'\n";
      }
    }
  }

  if ( exists $detail{'InOuts'} )
  {
    my @itemlist = split_textdict ($detail{'InOuts'});

    foreach my $item ( @itemlist )
    {
      if ( $item =~ /^(\S*)\s*:\s*(\S.*?)\s*$/o )
      {
        my $par = $1;
        my $doc = $2;

        $out .= "$IND \\param[in,out] $par $doc\n";
      }
      else
      {
        die "Cannot parse parameter doc string '$item'\n";
      }
    }
  }

  if ( exists $detail{'Outputs'} )
  {
    my @itemlist = split_textdict ($detail{'Outputs'});

    foreach my $item ( @itemlist )
    {
      if ( $item =~ /^(\S*)\s*:\s*(\S.*?)\s*$/o )
      {
        my $par = $1;
        my $doc = $2;

        if ( $m->{'rpar'} ne $par )
        {
          warn "Warning: inconsistent out parameter name for $m->{name}: $m->{rpar} != $par\n";
        }

        $out .= "$IND \\param[out] $par $doc\n";
      }
      else
      {
        die "Cannot parse parameter doc string '$item'\n";
      }
    }
  }

  $out .= "$IND \n";
  $out .= "$IND2\n";

  # FIXME: continue work here

  return $out;
}
########################################################################

########################################################################
#
# rename a key in hash
#
sub rename_package ($$$)
{
  my $idl = shift;
  my $old = shift;
  my $new = shift;

  # first, rename package
  if ( exists $idl->{"$old"} )
  {
    # print "=== package $old -> $new\n";
    $idl->{"$new"} = delete $idl->{"$old"};
  }

  # we also need to rename any argument or retval types which use the package
  # name
  foreach my $pkg (keys (%{$idl}) )
  {
    my $phash = $idl->{$pkg};
    foreach my $entry ( keys (%{$idl->{$pkg}}) )
    {
      my $ehash = $phash->{$entry};

      if ( $ehash->{'type'} eq 'class' )
      {
        my $cdef = $ehash->{'def'};

        foreach my $method ( @{$cdef->{'methods'}} )
        {
          foreach my $param ( @{$method->{'params'}} )
          {
            if ( $param->{'type'} =~ /\b($old)\b::/ )
            {
              # print " === $param->{'type'} \t\t\t ($old -> $new)\n";
              $param->{'type'} =~ s/\b($old)\b::/$new\::/g;
            }
          }
        }
      }
    }
  }


  # finally, we need to rename types of inherited interfaces and classes
  foreach my $pkg (keys (%{$idl}) )
  {
    my $phash = $idl->{$pkg};
    foreach my $entry ( keys (%{$idl->{$pkg}}) )
    {
      my $ehash = $phash->{$entry};

      if ( $ehash->{'type'} eq 'class' )
      {
        foreach my $base ( @{$ehash->{'base'}} )
        {
          if ( $base =~ /\b($old)\b::/ )
          {
            # print " === $base \t\t\t ($old -> $new)\n";
            $base =~ s/\b($old)\b::/$new\::/g;
          }
        }

        foreach my $impl ( @{$ehash->{'impl'}} )
        {
          if ( $impl =~ /\b($old)\b::/ )
          {
            # print " === $impl \t\t\t ($old -> $new)\n";
            $impl =~ s/\b($old)\b::/$new\::/g;
          }
        }
      }
    }
  }
}

########################################################################
#
# fix incomplete signatrure names
# url -> saga::core::url
#
sub fix_signatures ($)
{
  my $idl = shift;

  # first, create a registry of knowm saga types
  my %reg = ();
  foreach my $pname (keys (%{$idl}) )
  {
    my $phash = $idl->{$pname};

    foreach my $entry ( keys (%{$idl->{$pname}}) )
    {
      my $key = "$phash->{$entry}->{'name'}";
      my $val = "saga::$pname\::$key";

      $reg{$key} = $val;
    }
  }

  # now, got through the packges again, and through all interfaces and classes,
  # and all methods, and fix the names as needed.
  foreach my $pname (keys (%{$idl}) )
  {
    print " === $pname \n";
    my $phash = $idl->{$pname};
    foreach my $entry ( keys (%{$idl->{$pname}}) )
    {
      my $ehash = $phash->{$entry};

      if ( $ehash->{'type'} eq 'class'     ||
           $ehash->{'type'} eq 'interface' )
      {
        foreach my $r ( keys (%reg) )
        {
          # print " === $pname $r -> $reg{$r}\n";
          foreach my $method ( @{$ehash->{'def'}->{'methods'}} )
          {
            # print " === $pname $method->{name}\n";
            $method->{'rtype'} =~ s/\b($r)\b/$reg{$r}/g;

            foreach my $par ( @{$method->{'params'}} )
            {
              $par->{'type'} =~ s/\b($r)\b/$reg{$r}/g;
            }
          }
        }
      }
    }
  }
}

########################################################################
#
# rename a method name
#
sub rename_method ($$$)
{
  my $idl = shift;
  my $old = shift;
  my $new = shift;

  foreach my $pkg (keys (%{$idl}) )
  {
    my $phash = $idl->{$pkg};
    foreach my $entry ( keys (%{$idl->{$pkg}}) )
    {
      my $ehash = $phash->{$entry};

      if ( $ehash->{'type'} eq 'class' )
      {
        my $cdef = $ehash->{'def'};

        foreach my $method ( @{$cdef->{'methods'}} )
        {
          if ( $method->{'name'} eq $old )
          {
            $method->{'name'} = $new;
          }
        }
      }
    }
  }
}

########################################################################
#
# rename a type in method sigs
#
sub rename_type ($$$)
{
  my $idl = shift;
  my $old = shift;
  my $new = shift;

  # rename any argument or retval types which use the package
  # name
  foreach my $pkg (keys (%{$idl}) )
  {
    my $phash = $idl->{$pkg};
    foreach my $entry ( keys (%{$idl->{$pkg}}) )
    {
      my $ehash = $phash->{$entry};

      if ( $ehash->{'type'} eq 'class' )
      {
        my $cdef = $ehash->{'def'};

        foreach my $method ( @{$cdef->{'methods'}} )
        {
          if ( $method->{'rtype'} =~ /\b($old)/ )
          {
            print " === $method->{rtype}\n";
            $method->{'rtype'} =~ s/\b($old)\b/$new/g;
          }

          foreach my $param ( @{$method->{'params'}} )
          {
            if ( $param->{'type'} =~ /\b($old)\b/ )
            {
              # print " === $param->{'type'} \t\t\t ($old -> $new)\n";
              $param->{'type'} =~ s/\b($old)$/$new/g;
            }
          }
        }
      }
    }
  }
}
#
########################################################################
 
1;

