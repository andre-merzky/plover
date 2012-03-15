
package generator_parser; 

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
sub parse_idl ($)
{
  my $txt      = shift;

  my %idl      = ();
  my @dot_impl = ();
  my @dot_ext  = ();

  my @packages = split ( /\s*package\s+saga\./ios, $txt);

  PACKAGE:
  foreach my $package ( @packages )
  {
    if ( $package =~ /^\s*$/io )
    {
      print " === next\n";
      next PACKAGE;
    }

    $package = "package saga.$package";

    my $pname = "";

    my $i = 0;

    if ( $package =~ /\s*^package        # package keyword
                      \s+
                      (saga\.\S+)        # package name (no spaces) -> pname
                      \s*\{\s*           # package content opening bracket
                      (
                       (?:               # multiple elements are possible
                        [^\{\}]*?        # element comments, type and name
                        \{[^\}]+\}       # element content
                       )+                # multiple elements are possible
                      )                  # -> ptxt
                      \s*\}\s*           # package content closing bracket
                      (.*)               # remaining text after package -> dtxt
                      $/xis )      
    {
         $pname = $1;
      my $ptxt  = $2;
      my $dtxt  = $3 || "--"; # details

      my $eidx  = 0; # element index

      $i ++;

      # $txt =~ /saga/g;

      $pname =~ s/^saga\.//iog;
      print "pname: $pname\n";

      # split ptxt into the individual elements
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

        print " $ename\n";

        my %elem = ();

        $elem{'name'}  = $ename;
        $elem{'type'}  = $etype;
        $elem{'idx'}   = $eidx;
        $elem{'comm'}  = $ecomm;
        $elem{'async'} = 0;
        $eidx++;
    
        # parse inheritance qualifiers
        # along the way, try to find info about async inheritance, either in the
        # qualifiers or in the comments
        if ( $equal =~ /\bfrom\s+\S*\s+saga::async\b/io )
        {
          $elem{'async'} = 1;
        }
        
        $equal =~ s/^\s*\/\/.*$//iomg;
        my @equallines = split (/\n/, $equal);
            # $pname =~ s/^core\.//iog;

        foreach my $equalline ( @equallines ) 
        {
          if ( $equalline =~ /^\s*implements\s+(?:saga::)?(\S+?)\s*$/io ) 
          {
            my $base = $1;
            push (@{$elem{'impl'}}, $base);

            my $tmp = sprintf " %-30s -> \"%s\"\n", "\"${pname}::$ename\"", $base;
            push (@dot_impl,  $tmp);

            if ( $base eq 'saga::async' )
            {
              $elem{'async'} = 1;
            }
          }
          elsif ( $equalline =~ /^\s*extends\s+(?:saga::)?(\S+?)\s*$/io ) 
          {
            my $base = $1;
            push (@{$elem{'base'}}, $base);

            my $tmp = sprintf " %-30s -> \"%s\"\n", "\"${pname}::$ename\"", $base;
            push (@dot_ext,  $tmp);

            if ( $base eq 'saga::async' )
            {
              $elem{'async'} = 1;
            }
          }
          else 
          {
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
            my $msig  = $4;
               $etxt  = $5 || "";

            my %method  = ();

            my $msig_orig = $msig;

            $method{'name'}    = $mname;
            $method{'comm'}    = $mcomm;
            $method{'temp'}    = $mtemp;
            $method{'idx'}     = $midx;
            $method{'sig'}     = '';      # call sig (w/o default specs)
            $method{'sigd'}    = '';      # call sig (w/  default specs)
            $method{'sigt'}    = '';      # call sig types
            $method{'sign'}    = '';      # call sig names
            $method{'rtype'}   = 'void';  # type of return parameter
            $method{'rpar'}    = '';      # name or return parameter


            if ( $mname =~ /^CONSTRUCTOR$/io )
            {
              $method{'type'}    = 'ctor';
            }
            elsif ( $mname =~ /^DESTRUCTOR$/io )
            {
              $method{'type'}    = 'dtor';
            }
            else
            {
              $method{'type'} = 'sync';
            }

            if ( $msig !~ /^\s*void\s*$/ios )
            {
              my $first_par     = 1;
              my $first_out_par = 1;

              while ( $msig =~ /^
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
                my $paramdef  = $4 || undef;
                   $msig      = $5 || "";

                my $add_to_sig = 1;
                my $push_par   = 1;

                if ( $parammode =~ /^out$/io )
                {
                  if ( $first_out_par )
                  {
                    if ( $method{'type'} ne 'ctor' )
                    {
                      $method{'rtype'}   = $paramtype;
                      $add_to_sig        = 0;
                      $first_out_par     = 0;
                      $push_par          = 0;
                    }

                    $method{'rpar'} = $paramname;
                  }
                }

                if ( $add_to_sig )
                {
                  my $sep = ", ";
                  if ( $first_par )
                  {
                    $sep = "";
                  }

                  my $def = "";

                  if ( defined $paramdef )
                  {
                    $def = " = $paramdef";
                  }

                  $method{'sig'}  .= "$sep$paramtype $paramname";
                  $method{'sigd'} .= "$sep$paramtype $paramname$def";
                  $method{'sigt'} .= "$sep$paramtype";
                  $method{'sign'} .= "$sep$paramname";
                }


                if ( $push_par )
                {
                  push (@{$method{'params'}}, {'name'    => $paramname, 
                                               'type'    => $paramtype, 
                                               'mode'    => $parammode,
                                               'default' => $paramdef});
                }

                $first_par = 0;
              }
            }

            if ( $method{'sigt'} eq '' )
            {
              $method{'sigt'} = 'void';
            }


            push (@{$elem{'def'}{'methods'}}, \%method);

            if ( $msig !~ /^\s*(?:void\s*)?$/o ) {
              die "Could not fully parse method $mname: '$msig'\n";
            }

            $midx++;

          } # done parsing methods



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

      my $extra_comm   = "";

      # From here on we start parsing lline by line, as the remainder of the text
      # should be rather uniform
      my $line = "";

      LINE:
      while ( $dtxt =~ /^([^\n]*)\n(.*)$/iosg )
      {
        $line = $1 || "";
        $dtxt = $2 || "";

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
                    # FIXME: comment out detail for dumper readibility
                    $method->{'detail'} = ""; # , $mtxt;
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
                  die "no such method '$mname' for $pname :: $ename\n";
                }
                else
                {
                  warn "No such method '$mname' for $pname :: $ename\n";
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
    else
    {
      die "Cannot parse package: --$package--\n";
    }
  }

  open (ODOT, ">saga.dot") || die "cannot open saga.dot: $!\n";
  print ODOT "digraph E {\n";
  print ODOT "edge [style=dotted]\n";
  foreach my $d ( @dot_impl ) { print ODOT $d; }
  print ODOT "edge [style=solid]\n";
  foreach my $d ( @dot_ext ) { print ODOT $d; }
  print ODOT "}\n";
  close (ODOT);

  return \%idl;
}

1;

