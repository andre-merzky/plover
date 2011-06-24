#!/usr/bin/perl -w 

BEGIN {
  use strict;

  use IO::File;
  use Data::Dumper;

  sub create_enum_file  ($$$);
  sub create_iface_file ($$$);
  sub format_comment    ($$ );
}


########################################################################
if ( scalar (@ARGV) < 1 )
{
  print "\n\n\tusage: $0 <file.idl> [...]\n\n";
  exit -1;
}



########################################################################
# global vars
my $TW   = 72;  # textwidth
my $BASE = './saga_simple/';
my %idl  = ();


########################################################################
foreach my $arg ( @ARGV )
{
  if ( $arg =~ /^(.+)\.idl$/ )
  {
    my $txt  = "";  # remaining idl text

    print " file \t\t: $arg\n";

    open  ( IN, "<$arg") || die "Cannot open $arg: $!\n";
    while (<IN> ) { $txt .= $_; }
    close ( IN);

    # ignore text before 'package' keyword
    $txt =~ s/^(?:.*\s)?package/package/iosg;

    # before we can translate to cpp, we have fix a number of keywords
    #
    #   namespace    -> name_space
    #   logical_file -> replica
    
    $txt =~ s/\bnamespace\b/name_space/og;
    $txt =~ s/\blogical_file\b/replica/og;

    my $pname = "";

PACKAGE:
    while ( $txt =~ /^\s*                # ignore leading space
                      package            # package keyword
                      \s+
                      (\S+?)             # package name (no spaces) -> pname
                      \s*\{\s*           # package content opening bracket
                      (
                       (?:               # multiple elements are possible
                        .*?              # element comments, type and name
                        \{.+\}           # element content
                       )+                # multiple elements are possible
                      )                  # -> ptxt
                      \s*\}\s*           # package content closing bracket
                      (.*)               # remaining text after package -> txt
                      $/xios )      
    {
         $pname = $1;
      my $ptxt  = $2;
         $txt   = $3;

      my $eidx  = 0; # element index

      print "  package \t: $pname\n";

      # split ptxt into the individual elements

      # print "\n~~~$ptxt~~~\n";

      while ( $ptxt =~ /^
                        \s*
                        (\/\/.*?)?             # opt comment
                        \s*
                        (class|interface|enum) # element type      
                        \s+
                        (\S+)                  # element name
                        \s*
                        (?::\s*([^\{\}]*?))?   # optional qualifiers
                        \s*
                        \{(.+?)\}              # element idl txt
                        \s*
                        (.*)                   # remainder
                        $/xios )
      {
        my $ecomm   = $1 || "";
        my $etype   = $2;
        my $ename   = $3;
        my $equal   = $4 || "";
        my $etxt    = $5;
           $ptxt    = $6 || "";

        my %elem = ();

        $elem{'name'} = $ename;
        $elem{'type'} = $etype;
        $elem{'idx'}  = $eidx;
        $elem{'comm'} = $ecomm;
        $eidx++;
  

        # parse inheritance qualifiers
        $equal =~ s/^\s*\/\/.*$//iomg;
        my @equallines = split (/\n/, $equal);
        foreach my $equalline ( @equallines ) {
          if ( $equalline =~ /^\s*implements\s+(\S+?)\s*$/io ) {
            push (@{$elem{'impl'}}, $1);
          }
          elsif ( $equalline =~ /^\s*extends\s+(\S+?)\s*$/io ) {
            push (@{$elem{'base'}}, $1);
          }
          else {
            die "Cannot parse qualifier in line '$equalline'\n";
          }
        }



        if ( $etype eq "class"     ||
             $etype eq "interface" )
        {
          my $midx = 0;

          # tagging interfaces my have *only* comments, if at all
          if ( $etxt =~ /^
                         ([\s\n]*?)
                         ((?:\s*?\/\/[^\n]*\n)+)
                         ([\s\n]*?)
                         $
                         /ioxs )
          {
            print " ### 1 $1 ### 2 $2 ### 3 $3 ###\n";
            $ecomm = $2 || "";
            $etxt  = "";
            $elem{'comm'} .= "\n$ecomm";
          }

          # search for method definitions
          while ( $etxt =~ /^\s*
                            (?:(\/\/\s*.+?)\s+)?  # opt. comment
                            (\S+)                 # method name
                            \s*
                            (\<[^>]+?\>)?         # opt. template qualifier
                            \s*
                            \(([^\)]+)\)          # signature
                            ;                     # end of method
                            \s*(.*)               # remainder
                            $/iosx )
          {
            my $mcomm = $1 || "";  # comment before method def?
            my $mname = $2;
            my $mtemp = $3 || "";
            my $mtxt  = $4;
               $etxt  = $5 || "";

            my %method  = ();

            $method{'name'} = $mname;
            $method{'comm'} = $mcomm;
            $method{'temp'} = $mtemp;
            $method{'idx'}  = $midx;
            $midx++;

            if ( $mtxt =~ /^\s*void\s*$/ios )
            {
              $method{'is_void'} = 1;
              $mtxt = "";
            }
            else
            {
              $method{'is_void'} = 0;

              while ( $mtxt =~ /^
                                  \s*(in|out|inout)   # param modifier
                                  \s+(\S.+?)          # param type
                                  \s+(\S+)            # param name
                               (?:\s*=\s*(\S.*?))?    # opt: param default
                                  \s*(?:,\s*(.*?))?   # remaining params
                                  $/iosx )
              {
                my $parammode = $1;
                my $paramtype = $2;
                my $paramname = $3;
                my $paramdef  = $4;
                   $mtxt      = $5 || "";

                push (@{$method{'params'}}, {'name'    => $paramname, 
                                             'type'    => $paramtype, 
                                             'mode'    => $parammode,
                                             'default' => $paramdef});
              }
            }


            
            $elem{'def'}{$mname} = \%method;

            if ( $mtxt !~ /^\s*$/o ) {
              die "Could not fully parse method $mname: '$mtxt'\n";
            }
          }


          # there may be metric and attribute definitions, too
          # (metrics are just attributes which can be watched)
          if ( $etxt =~ /^\s*\/\/\s*(Metrics|Metric|Attributes)(\s*\(?extensible\)?)?:\s*(.*?)$/ios )
          {
            my $type = $1;
            my $text = $2 || "";
               $etxt = $3 || "";

            my $typekey    = "";
            my $extensible = 1;

            if ( $type =~ /^Metric/io )
            {
              $typekey = 'met';
            }
            elsif ( $type eq 'Attributes' )
            {
              $typekey = 'att';
            }
            else
            {
              die "parse error: expected attributes or metric, not $type\n";
            }

            if ( $text eq "" )
            {
              $extensible = 0;
            }

            # ignore empty lines
            while ( $etxt =~ /^\s*\/\/\s*(\/\/.*)$/ios )
            {
              $etxt = $1 || "";
            }

            if ( $etxt =~ /^\s*\/\/\s*no\s+(?:attributes|metrics)\s+pre\-defined\s*(.*)$/ios )
            {
              $etxt = $1 || "";
            }
              
                           
            while ( $etxt =~ /^\s*\/\/\s*name: \s*(.+?)\s*
                               \s*\/\/\s*desc: \s*(.+?)\s*
                               \s*\/\/\s*mode: \s*(.+?)(?:,\s*(optional))?\s*
                            (?:\s*\/\/\s*unit: \s*(.+?)\s*)?
                               \s*\/\/\s*type: \s*(.+?)\s*
                               \s*\/\/\s*value:\s*(.+?)\s*
                            (?:\s*\/\/\s*notes:\s*(.+?)\s*)?
                           ((?:\s*\/\/\s*)|
                            (?:\s*\/\/\s*\/\/.*))?
                             $/iosx )
            {
              my $attname = $1;
              my $attdesc = $2;
              my $attmode = $3;
              my $attopt  = $4 || "";
              my $attunit = $5 || "";
              my $atttype = $6;
              my $attval  = $7;
              my $attnote = $8 || "";
                  $etxt   = $9 || "";

              $attdesc =~ s/\s*\/\/\s*/ /iosg;

              $attnote =~ s/\s*\/\/\s*([^\s-])/ $1/iosg;
              $attnote =~ s/\s*\/\/\s*-/\n-/iosg;

              my %att = ('name'  => $attname, 
                         'desc'  => $attdesc, 
                         'mode'  => $attmode, 
                         'opt'   => $attopt , 
                         'unit'  => $attunit, 
                         'type'  => $atttype, 
                         'val'   => $attval ,
                         'note'  => $attnote);

              $elem{$typekey}{$attname} = \%att;

              while ( $etxt =~ /^\s*\/\/\s*(\/\/.*)$/ios )
              {
                $etxt = $1;
              }
            }
          }

          # remove trailing empty comment lines

          $etxt =~ s/^\/\///iog;

          if ( $etxt !~ /^\s*$/o ) {
            die "Could not fully parse $etype $ename: '$etxt'\n";
          }
        }
        elsif ( $etype eq "enum" )
        {
          my %enum = ();

          while ( $etxt !~ /^\s*$/o ) 
          {
            if ( $etxt =~ /^
                           \s*
                           (\S+?)     # enum key
                           \s*=\s*    # = 
                           (\d+)      # enum val
                           \s*,?      # ,
                           (?:\n|\s*\/\/\s*(.*?)\n)? # optional comment
                           (.*)       # remainder
                           $/iosx )
            {
              my $enumkey  = $1;
              my $enumval  = $2;
              my $enumcomm = $3 || "";
                 $etxt     = $4 || "";

              if ( exists ($enum{$enumval}) )
              {
                die "enum value $enumval defined twice in $pname:$ename\n";
              }

              $enum{$enumval}{'name'} = $enumkey; 
              $enum{$enumval}{'comm'} = $enumcomm;
              $enum{$enumval}{'used'} = 1;
            }
            elsif ( $etxt =~ /^
                              \s*
                              \/\/             # comment marker
                              \s*              # 
                              (\d+)            # enum val
                              \s*,?            # ,
                              \s*reserved\sfor #
                              \s+(\S+?)        # 
                              \s*\n 
                              (.*)             # remainder
                              $/iosx )
            {
              my $enumval  = $1;
              my $enumkey  = $2;
              my $enumcomm = "reserved";
                 $etxt     = $3 || "";

              if ( exists ($enum{$enumval}) )
              {
                die "enum value $enumval defined twice in $pname:$ename\n";
              }

              $enum{$enumval}{'name'} = $enumkey; 
              $enum{$enumval}{'comm'} = $enumcomm;
              $enum{$enumval}{'used'} = 0;
            }
            else
            {
              die "Could not fully parse enum $ename: '$etxt'\n";
            }

          } # while

          $elem{'def'} = \%enum;
        }

        $idl{$pname}{$ename} = \%elem;
      }

      if ( $ptxt !~ /^\s*$/o ) {
        die "Could not fully parse package $pname: $ptxt\n";
      }
    }


    # we haqve the idl package now fully parsed.  The remainder of the file will
    # contain specification details, which belong to the last parsed package.
    # We need to infer the respective elements from the subsection though, which
    # are now replaced in the IDL with lines like
    # 
    # SPEC DETAILS : Enum: state
    # 

    my $expect = "detail"; # line type we expect next
    my $etype  = "";       # entry type for details
    my $ename  = "";       # entry name for details
    # note that pname is still known here, and points to the last parsed package

    # keep detail vars around
    my $mname  = "";
    my $mpurp  = "";
    my $mform  = "";
    my $min    = "";
    my $mout   = "";
    my $minout = "";
    my $mpre   = "";
    my $mpost  = "";
    my $mperm  = "";
    my $mthrow = "";
    my $mnotes = "";
    my $mtxt   = ""; # simply keep complete text, formatted as is

    # From here on we start parsing lline by line, as the remainder of the text
    # should be rather uniform
    my @lines  = split (/\n/, $txt);
    chomp (@lines);

    LINE:
    foreach my $line ( @lines )
    {
      # print "$expect \t: $line\n";

      # waiting for details to start, we can either really find a detail
      # starting, or a new header
      if ( $expect eq "detail" )
      {
        # ignore empty lines right now
        if ( $line =~ /^\s*$/io )
        {
          next LINE;
        }

        # accept new headers (new element details follow)
        if ( $line =~ /^SPEC DETAILS : (\S+) : (\S+)$/io )
        {
          $etype = lc($1);
          $ename = lc($2);

          # print "scan details for $pname $etype $ename\n";

          $expect = "detail";
          next LINE;
        }
        

        if ( $line  =~ /^\s+\-\s+(\S+)\s*$/io )
        {
          # found starting detail.
          $mname  = $1;
          $mtxt  .= "$line\n";

          # but we complain if we did not see a header first
          if ( "$ename-$etype" eq "-" )
          {
            die "expected header, but found detail: $line\n";
          }
          
          # now wait for its contents
          $expect = "content";
          next LINE;
        }

        # we sometimes see additonal comment lines
        # warn "expected detail or header, not '$line'\n";
        $extra_comm .= "$line\n";
        next LINE;
      }


      # waiting for more 'content', and only finish on new line
      if ( $expect eq "content" )
      {
        if ( $line =~ /^\s*$/io )
        {
          if ( $mtxt !~ /^\s*$/io )
          {
            # if we found any detail earlier on, this is where we need to store it
            # away, along with any extra_comm we saw.
            # print "dump detail for $mname ($etype $ename) in $pname\n";

            if ( ! exists ($idl{$pname})                        || 
                 ! exists ($idl{$pname}{$ename})                ||
                 ! exists ($idl{$pname}{$ename}{'def'})         ||
                 ! exists ($idl{$pname}{$ename}{'def'}{$mname}) )
            {
              # we ignore missing  method names which contain a '*' - those need to
              # be handled though! FIXME
              if ( $mname !~ /\*/io )
              {
                die "no such method '$mname' for $pname :: $ename\n" unless ( defined ( $tmpmethod ) );
              }
              else
              {
                warn "no such method '$mname' for $pname :: $ename\n" unless ( defined ( $tmpmethod ) );
              }
            }
            else
            {
              # print "storing detail in $pname :: $ename :: $mname\n";
              $idl{$pname}{$ename}{'def'}{$mname}{'detail'} = $mtxt;
              $idl{$pname}{$ename}{'def'}{$mname}{'precom'} = $extra_comm;
            }
          }

          # empty line: wait for new detail (or header)
          $mtxt       = "";
          $extra_comm = "";
          $expect     = "detail";
          next LINE;
        }

        $mtxt  .= "$line\n";
        $expect = "content";
        next LINE;
      }

      # if we did not see a 'next LINE', we failed...
      die "error: parsing for '$expect', but found '$line'\n";
    }
  }
  else
  {
    print "skipping $arg (idl package found)\n";
  }
}


open  (OUT, ">idl.dump") || die "Cannot open idl.dump: $!\n";
print  OUT Dumper \%idl;
close (OUT);



########################################################################

system "rm -rf $BASE/";

mkdir  "$BASE/";
mkdir  "$BASE/saga/";

mkdir  "$BASE/saga/util/";
mkdir  "$BASE/saga/api/";
mkdir  "$BASE/saga/cpi/";
mkdir  "$BASE/saga/impl/";
mkdir  "$BASE/saga/misc/";


# list of all package names
my @p_hdrs = ();

########################################################################
foreach my $pname ( sort keys (%idl) )
{
  my $pkg    = $pname;

  $pkg =~ s/^saga\.//;

  print "$pkg\n";

  mkdir "$BASE/saga/api/$pkg/";
  mkdir "$BASE/saga/cpi/$pkg/";
  mkdir "$BASE/saga/impl/$pkg/";
  mkdir "$BASE/saga/misc/$pkg/";

  my $p_api_header = "$BASE/saga/api/$pkg.hpp";
  my $p_cpi_header = "$BASE/saga/cpi/$pkg.hpp";

  my %pkg_elems    = ();
  my @p_elem_hdrs  = ();

  foreach my $ename ( keys %{ $idl{$pname} } )
  {
    my $type = $idl{$pname}{$ename}{'type'};

    push (@{$pkg_elems{$type}}, $ename);
  }

  # print Dumper (\%pkg_elems);

  foreach my $ename ( @{$pkg_elems{'enum'}} )
  {
    print "  enum   \t $ename\n";
    my $p_elem_hdr = create_enum_file ($pkg, $ename, $idl{$pname}{$ename});
    push (@p_elem_hdrs, $p_elem_hdr);
  }

  foreach my $iname ( @{$pkg_elems{'interface'}} )
  {
    print "  iface  \t $iname\n";
    my $p_elem_hdr = create_iface_file ($pkg, $iname, $idl{$pname}{$iname});
    push (@p_elem_hdrs, $p_elem_hdr);
  }

  foreach my $cname ( @{$pkg_elems{'class'}} )
  {
    print "  class  \t $cname\n";
  }


  # create package header, which pulls all package member headers
  my $PKG   = uc ($pkg);
  my $guard = "SAGA_API_$PKG\_HPP_";
  my $p_hdr = "saga/api/$pkg.hpp";
  my $out   = new IO::File ("> $BASE/$p_hdr") || die "Cannot create enum entry
  at $BASE/$p_hdr: $!\n";

#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/saga/hpp>

EOT
#------------------------------------------------------------
  
  foreach my $p_elem_hdr ( @p_elem_hdrs )
  {
#------------------------------------------------------------
    print $out <<EOT;
#include <$p_elem_hdr>
EOT
  }

#------------------------------------------------------------
  print $out <<EOT;

#endif $guard

EOT
#------------------------------------------------------------
  $out->close ();

  # save the package header for the api header
  push (@p_hdrs, $p_hdr);
}


########################################################################
{
  # create saga header, which pulls are package headers
  my $guard = "SAGA_API_HPP_";
  my $a_hdr = "saga/saga_api.hpp";
  my $out   = new IO::File ("> $BASE/$a_hdr") || die "Cannot create enum entry at $BASE/$a_hdr: $!\n";

#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/saga.hpp>

EOT
#------------------------------------------------------------
  
  foreach my $pname ( @p_hdrs )
  {
#------------------------------------------------------------
    print $out <<EOT;
#include <$pname>
EOT
  }

#------------------------------------------------------------
  print $out <<EOT;

#endif $guard

EOT
#------------------------------------------------------------
  $out->close ();
}

########################################################################
sub create_enum_file ($$$)
{
  my $pname = shift;
  my $ename = shift;
  my $enum  = shift;

  my $PNAME = uc ($pname);
  my $ENAME = uc ($ename);

  my $guard = "SAGA_API_$PNAME\_$ENAME\_HPP_";
  my $e_hdr = "saga/api/$pname/$ename.hpp";
  my $out   = new IO::File ("> $BASE/$e_hdr") || die "Cannot create enum entry at $BASE/$e_hdr: $!\n";

  my $comm  = $enum->{'comm'};

  if ( $comm )
  {
    $comm = "\n" . format_comment (4, $comm);
  }

#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/saga.hpp>

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
      printf $out ("      %-15s = %6d$comma%s\n", $n, $e, $c);
    }
    else
    {
      printf $out ("   // %-15s = %6d$comma%s\n", $n, $e, $c);
    }
  }

#------------------------------------------------------------
  print $out <<EOT;
    };

  } // namespace $pname

} // namespace saga

#endif $guard

EOT
#------------------------------------------------------------
    
  $out->close;

  return $e_hdr;
}


########################################################################
sub create_iface_file ($$$)
{
  my $pname = shift;
  my $iname = shift;
  my $iface = shift;

  my $PNAME = uc ($pname);
  my $INAME = uc ($iname);

  my $guard = "SAGA_API_$PNAME\_$INAME\_HPP_";
  my $i_hdr = "saga/api/$pname/$iname.hpp";
  my $out   = new IO::File ("> $BASE/$i_hdr") || die "Cannot create enum entry at $BASE/$i_hdr: $!\n";

  my $comm  = $iface->{'comm'};

  if ( $comm )
  {
    $comm = "\n" . format_comment (4, $comm);
  }

  # print Dumper \$iface;

#------------------------------------------------------------
  print $out <<EOT;

#ifndef $guard
#define $guard

#include <saga/saga.hpp>

namespace saga
{
  namespace $pname
  { $comm
    class $iname
    {
EOT
#------------------------------------------------------------


#  my @ekeys = sort { $a <=> $b } keys ( %{$enum->{'def'}} );
#
#  foreach my $i (  0..$#ekeys )
#  {
#    my $e = $ekeys[$i];
#
#    my $n = $enum->{'def'}{$e}{'name'} || "";
#    my $c = $enum->{'def'}{$e}{'comm'} || "";
#    my $u = $enum->{'def'}{$e}{'used'} ||  0;
#
#    # append comma for all but last enum
#    my $comma = ',';
#    if ( $i eq $#ekeys  ) { $comma = ' '; }
#
#    # format comment as such
#    if ( $c ) { $c = "    // $c"; }
#  
#    # is the enum used, or just a comment?
#    if ( $u )
#    {
#      printf $out ("      %-15s = %6d$comma%s\n", $n, $e, $c);
#    }
#    else
#    {
#      printf $out ("   // %-15s = %6d$comma%s\n", $n, $e, $c);
#    }
#  }

#------------------------------------------------------------
  print $out <<EOT;
    };

  } // namespace $pname

} // namespace saga

#endif $guard

EOT
#------------------------------------------------------------
    
  $out->close;

  return $i_hdr;
}


########################################################################
sub format_comment ($$)
{
  my $ind   = shift;
  my $comm  = shift;
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

  chomp ($out);

  return $out;
}

########################################################################

