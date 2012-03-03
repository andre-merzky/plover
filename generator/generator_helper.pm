
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


1;

