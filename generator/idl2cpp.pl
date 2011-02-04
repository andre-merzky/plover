#!/usr/bin/perl -w 

BEGIN {
  use strict;

  use Data::Dumper;
}

if ( scalar (@ARGV) < 1 )
{
  print <<EOT;
  
  usage: $0 <file.idl> [...]

EOT
  exit -1;
}

my %idl = ();


foreach my $arg ( @ARGV )
{
  if ( $arg =~ /^(.+)\.idl$/ )
  {
    my $txt  = "";  # remaining idl text

    print " file \t\t: $arg\n";

    open  ( IN, "<$arg");
    while (<IN> ) { $txt .= $_; }
    close ( IN);

    # ignore text before 'package' keyword
    $txt =~ s/^(?:.*\s)?package/package/iosg;

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
                        (\/\/.*?\s\s\s\s)?     # opt comment
                        (class|interface|enum) # element type      
                        \s+
                        (\S+)                  # element name
                        \s*
                        ([^\{\}]*?)?           # optional qualifiers
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

        $equal =~ s/^\s*\/\/.*$//iomg;

        # validate comment
        my @ecommlines = split (/\n/, $ecomm);
        foreach my $ecommline ( @ecommlines ) {
          if ( $ecommline =~ /^\s*\/\/\s*(.*)$/io ) {
            $ecomm .= $1;
          }
          elsif ( $ecommline =~ /^\s*$/io ) {
            # ignore
          }
          else {
            die "Cannot parse package text in line '$ecommline'\n";
          }
        }


        print "   element \t: $ename ($etype)\n";

        my %elem = ();

        $elem{'name'} = $ename;
        $elem{'type'} = $etype;
        $elem{'comm'} = $ecomm;
        $elem{'idx'}  = $eidx;
        $eidx++;
  
        if ( $etype eq "class"     ||
             $etype eq "interface" )
        {
          # grab comments
          while ( $etxt =~ /^\s*\/\/\s*(.+?)\n(.*)$/ios )
          {
            my $comm = $1 || "";
               $etxt = $2 || "";

            $elem{'comm'} .= $comm;
          }

          my $midx = 0;

          # search for method definitions
          while ( $etxt =~ /^\s*
                            (?:\/\/\s*(.+?)\s+)?  # opt. comment
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


          if ( $etxt =~ /^\s*\/\/\s*(\S.*?)$(.*)/iom )
          {
            # some num vals are only added as comments, if inherited from
            # elsewhere
            $enum{'comm'} .= $1;
            $etxt          = $2 || "";
          }
          else
          {
            while ( $etxt =~ /^\s*
                               (\S+)      # enum key
                               \s*=\s*    # = 
                               (\S+)      # enum val
                               \s*,?      # ,
                               (?:\/\/\s*(.+?)\n)? # optional comment
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
            }
          }

          if ( $etxt !~ /^\s*$/o ) {
            die "Could not fully parse enum $ename: '$etxt'\n";
          }

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

          print "scan details for $pname $etype $ename\n";

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
        warn "expected detail or header, not '$line'\n";
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
            print "dump detail for $mname ($etype $ename) in $pname\n";

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
              print "storing detail in $pname :: $ename :: $mname\n";
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


