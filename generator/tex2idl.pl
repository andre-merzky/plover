#!/usr/bin/perl -w 

BEGIN {
  use strict;
}

if ( scalar (@ARGV) < 1 )
{
  print <<EOT;
  
  usage: $0 <file.tex> [...]

EOT
  exit -1;
}

foreach my $arg ( @ARGV )
{
  if ( $arg =~ /^(.+)\.tex$/ )
  {
    my $base =  $1;
    my $out  = "$base.idl";

    # print "parsing idl from $arg\n";
  
    open  (IN, "<$arg") || die "Cannot open input at $arg: $!\n";
    my @lines = <IN>;
    close (IN);

    chomp (@lines);

    my @idl = ();
    
    my $is_idl = 0;
    foreach my $line ( @lines )
    {
      if ( ! $is_idl )
      {
        # keep everything in between 'myspec' sections - those are the idl
        # parts, and the 'spec details'.
        if ( $line =~ /^\s*\\begin\{myspec\}\s*$/io )
        {
          $is_idl = 1;
        }

        # we also keep latex subsubsection marker so that we can see what 
        # the 'spec details' parts are actually describing
        elsif ( $line =~ /^\s*\\subsubsection\*\{\s*(Class|Enum|Interface)\s+\\T\{(.*?)\}\}\s*$/io )
        {
          my $type = $1;
          my $name = $2;

          $name =~ s/\\//iog;

          push (@idl, "\nSPEC DETAILS : $type : $name\n");
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
            push (@idl, $line);
          }
        }
      }
    }

    if ( scalar (@idl) )
    {
      open  (OUT, ">$out");
      print  OUT join ("\n", @idl);
      print  OUT "\n";
      close (OUT);
    }
  }
  else
  {
    print "skipping $arg (no .tex file)\n";
  }
}

