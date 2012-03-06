
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
sub get_guard ($)
{
  my $str  = shift;

  $str =~ tr/a-zA-Z0-9/_/cs;  # change non-alphasnums to single underscore

  return uc ($str);           #  return uppercase version
}
########################################################################

########################################################################
#
# reformat a C++ comment to have correct indentation 
# (// starting at given line indent)
#
sub format_comment ($$)
{
  my $ind   = shift;
  my $comm  = shift;

  if ( ! $comm )
  {
    return "";
  }


  my $out   = "";
  my $IND   = ' ' x $ind;
  my @lines = split (/\n/, $comm);

  foreach my $line ( @lines ) 
  {
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
# rename a key in hash
#
sub rename_package ($$$)
{
  my $idl = shift;
  my $old = shift;
  my $new = shift;

  # first, rename package
  if ( exists $idl->{"saga.$old"} )
  {
    print "=== package $old -> $new\n";
    $idl->{"saga.$new"} = delete $idl->{"saga.$old"};
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
              $param->{'type'} =~ s/\b($old)\b::/$new/g;
            }
          }
        }
      }
    }
  }
}

########################################################################
#
# rename a type in method signs
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
          foreach my $param ( @{$method->{'params'}} )
          {
            if ( $param->{'type'} =~ /\b($old)$/ )
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

1;

