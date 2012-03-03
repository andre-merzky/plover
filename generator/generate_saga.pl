#!/usr/bin/perl -w 

BEGIN {
  use strict;

  use IO::File;
  use Data::Dumper;

  use generator_helper;
  use generate_api;

  sub grab_idl                    ($   );
  sub idl_to_cpp                  ($   );
  sub parse_idl                   ($   );
}

########################################################################
# global vars
my $TW   = 72;  # textwidth
my $BASE = './simple/';

########################################################################
# usage
if ( scalar (@ARGV) < 1 )
{
  print <<EOT;
  
  usage: $0 <file.tex> [...]

EOT
  exit -1;
}

########################################################################
# make sure all args are tex files
my @args_clean;
foreach my $arg ( @ARGV )
{
  if ( $arg !~ /\.tex$/io )
  {
    print STDERR "dropping non .tex file $arg\n";
  }
  elsif ( ! -f $arg || ! -r $arg )
  {
    print STDERR "dropping inaccesible file $arg\n";
  }
  else
  {
    push (@args_clean, $arg);
  }
}

########################################################################
# convert input tex to idl to src code
my $idl_txt = "";
foreach my $arg ( @args_clean )
{
  $idl_txt .= grab_idl  ($arg);
}
my $idl_tree = parse_idl ($idl_txt);

# print Dumper $idl_tree;

idl_to_cpp ($idl_tree);

########################################################################
#
# filter IDL out of the latex sources
#
sub grab_idl ($)
{
  my $tex = shift;

  printf ("%-30s", $tex);

  # slurp in tex file
  open  (IN, "<$tex") || die "Cannot open input at $tex: $!\n";
  my @lines = <IN>;
  close (IN);
  chomp (@lines);

  my $idl    = "";
  my $is_idl = 0;

  foreach my $line ( @lines )
  {
    if ( ! $is_idl )
    {
      # keep everything in between 'myspec' sections - those are the idl
      # parts, and add the 'spec details' for documentation
      if ( $line =~ /^\s*\\begin\{myspec\}\s*$/io )
      {
        $is_idl = 1;
        print "|";
      }

      # we also keep latex subsubsection marker so that we can see what 
      # the 'spec details' parts are actually describing
      elsif ( $line =~ /^\s*\\subsubsection\*\{\s*(Class|Enum|Interface)\s+\\T\{(.*?)\}\}\s*$/io )
      {
        my $type = $1;
        my $name = $2;

        $name =~ s/\\//iog;

        $idl .= "\nSPEC DETAILS : $type : $name\n";
        print "-";
      }
    }
    else # is_idl == 1
    {
      if ( $line =~ /^\s*\\end\{myspec\}\s*$/io )
      {
        $is_idl = 0;
      }
      else
      {
        # remove leading [+!], ignore lines with leading [-]
        if ( $line =~ /^\-/io )
        {
          #ignore
        }
        else 
        {
          $line =~ s/^[+!]/ /io;
          $idl .= "$line\n";
        }
      }
    }
  }

  print "\n";

  return $idl;
}

########################################################################

sub parse_idl ($)
{
  my $txt = shift;

  while ( $txt )
  {
    # ignore text before 'package' keyword
    $txt =~ s/^(?:.*?\s)?\n\s*package/package/iosg;

    my $pname = "";

    PACKAGE:
    while ( $txt =~ /^\s*                # ignore leading space
                      package            # package keyword
                      \s+
                      (\S+)              # package name (no spaces) -> pname
                      \s*\{\s*           # package content opening bracket
                      (
                       (?:               # multiple elements are possible
                        [^\{\}]*?        # element comments, type and name
                        \{[^\}]+\}       # element content
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

      print "$pname\n";

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

        print "  $ename\n";

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
            $ecomm = $2 || "";
            $etxt  = "";

            if ( $elem{'comm'} )
            {
              $elem{'comm'} .= "\n// \n";
            }
            $elem{'comm'} .= "\n$ecomm";
          }

          # search for method definitions
          while ( $etxt =~ /^\s*
                            (?:(\/\/\s*.+?)\s+)?  # opt. comment
                            (\S+)                 # method name
                            \s*
                            (?:\<([^>]+?)\>)?     # opt. template qualifier
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


            
            push (@{$elem{'def'}{'methods'}}, \%method);

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
    my $line = "";

    LINE:
    while ( $txt =~ /^([^\n]*)\n(.*)$/iosg )
    {
      $line = $1 || "";
      $txt  = $2 || "";

      # instead of details or content, we might hit the next package
      if ( $line =~ /^\s*package\s*\S+\s*$/io )
      {
        # put the line back on front of $txt, stop right here
        $txt = "$line\n$txt";
        last LINE;
      }


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
            # if we found any detail earlier on, now we need to store it
            # away, along with any extra_comm we saw.  So we search the def
            # hash for the matching method name

            # print "dump detail for $mname ($etype $ename) in $pname\n";

            my $match_found = 0;

            if ( exists ($idl{$pname})                           && 
                 exists ($idl{$pname}{$ename})                   &&
                 exists ($idl{$pname}{$ename}{'def'})            &&
                 exists ($idl{$pname}{$ename}{'def'}{'methods'}) )
            {
              foreach my $method ( @{$idl{$pname}{$ename}{'def'}{'methods'}} )
              {
                if ( $method->{'name'} eq $mname )
                {
                  $match_found = 1;
              
                  # print "storing detail in $pname :: $ename :: $mname\n";
                  $method->{'detail'} = $mtxt;
                  $method->{'precom'} = $extra_comm;

                }
              }
            }

            if ( ! $match_found )
            {
              # we only warn on method names which contain a '*' - those need to
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

  return \%idl;
}


########################################################################
#
# before we do anything useful with the idl, we apply a number of magic cpp fixes
#
sub idl_to_cpp ($)
{
  my $idl_ref = shift;
  my %idl = %{$idl_ref};

  # to avoid clashes with later use of the idl, we from now on operate on a copy
  my %cpp_idl = %idl;
  

  ##############################################################################
  #
  # before we can translate to cpp, we have fix a number of keywords
  #
  #   file         -> filesystem
  #   namespace    -> name_space
  #   logical_file -> replica
  
  if ( exists ($cpp_idl{'saga.file'}) )
  {
    $cpp_idl{'saga.filesystem'} = $cpp_idl{'saga.file'};
    delete ($cpp_idl{'saga.file'})
  }

  if ( exists ($cpp_idl{'saga.namespace'}) )
  {
    $cpp_idl{'saga.name_space'} = $cpp_idl{'saga.namespace'};
    delete ($cpp_idl{'saga.namespace'})
  }

  if ( exists ($cpp_idl{'saga.logical_file'}) )
  {
    $cpp_idl{'saga.replica'} = $cpp_idl{'saga.logical_file'};
    delete ($cpp_idl{'saga.logical_file'})
  }

  if ( exists ($cpp_idl{'saga.core'}) )
  {
    if ( exists ($cpp_idl{'saga.core'}{'exception'}) )
    {
      # exception inherits from std::exception -- add to base and header includes
      push (@{$cpp_idl{'saga.core'}{'exception'}{'base'}}, "std::exception");
      push (@{$cpp_idl{'saga.core'}{'exception'}{'hincs'}}, "exception");
    }
  }
  ########################################################################
  
  # print Dumper %cpp_idl;
  ##############################################################################


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
  foreach my $pname ( sort keys (%cpp_idl) )
  {
    print Dumper $cpp_idl{$pname} if ( $pname eq "saga.error" );

    my $pkg = $pname;
  
    $pkg =~ s/^saga\.//;
  
    # print "$pkg\n";
  
    mkdir "$BASE/saga/api/$pkg/";
    mkdir "$BASE/saga/cpi/$pkg/";
    mkdir "$BASE/saga/impl/$pkg/";
    mkdir "$BASE/saga/misc/$pkg/";
  
    my $p_api_header = "$BASE/saga/api/$pkg.hpp";
    my $p_cpi_header = "$BASE/saga/cpi/$pkg.hpp";
  
    my %pkg_elems    = ();
    my @p_elem_hdrs  = ();
  
    foreach my $ename ( sort keys %{ $cpp_idl{$pname} } )
    {
      my $type = $cpp_idl{$pname}{$ename}{'type'};
      my $hdr  = "";
  
      if ( $type eq 'enum' )
      {
        $hdr = generate_api::enum_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      }
      elsif ( $type eq 'interface' )
      {
        $hdr = generate_api::iface_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      }
      elsif ( $type eq 'class' )
      {
        $hdr = generate_api::iface_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      }

      push (@p_elem_hdrs, $hdr);
    }
  


    # create package header, which pulls all package member headers
    my $PKG   = uc ($pkg);
    my $p_hdr = "saga/api/$pkg.hpp";
    my $guard = generator_helper::get_guard ($p_hdr);
    my $out   = new IO::File ("> $BASE/$p_hdr") || die "Cannot create enum entry at $BASE/$p_hdr: $!\n";
  
#------------------------------------------------------------
    print $out <<EOT;
  
#ifndef $guard
#define $guard
  
EOT
#------------------------------------------------------------
    
    foreach my $p_elem_hdr ( @p_elem_hdrs )
    {
#------------------------------------------------------------
      print $out <<EOT;
#include <$p_elem_hdr>
EOT
#------------------------------------------------------------
    }
  
#------------------------------------------------------------
    print $out <<EOT;
  
#endif // $guard
  
EOT
#------------------------------------------------------------
    $out->close ();
  
    # save the package header for the api header
    push (@p_hdrs, $p_hdr);
  }
  
  
  ########################################################################
  {
    # create saga header, which pulls the package headers
    my $a_hdr = "saga/saga.hpp";  # FIXME: correct would be saga/api.hpp
    my $guard = generator_helper::get_guard ($a_hdr);
    my $out   = new IO::File ("> $BASE/$a_hdr") || die "Cannot create saga header at $BASE/$a_hdr: $!\n";
  
#------------------------------------------------------------
    print $out <<EOT;
  
  #ifndef $guard
  #define $guard
  
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
  
  #endif // $guard
  
EOT
#------------------------------------------------------------
    $out->close ();
  }  
}

