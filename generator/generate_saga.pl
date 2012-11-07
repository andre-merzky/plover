#!/usr/bin/perl -w 

BEGIN {
  use strict;

  use IO::File;
  use Data::Dumper;

  use generator_helper;
  use generator_parser;
  use generate_api;
# use generate_cpi;

  sub grab_idl                    (\@);
  sub idl_to_cpp                  ($ );
}

########################################################################
# global vars
my $TW   = 72;  # textwidth
my $BASE = './simple/';

########################################################################
#
# main
#
{

  ######################################################################
  # usage
  if ( scalar (@ARGV) < 1 )
  {
    print "\n\n\t usage: $0 <file.tex> [...]\n\n";
  
    exit -1;
  }
  
  ######################################################################
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
  
  ######################################################################
  #
  # convert tex      to idl 
  # convert idl      to parse tree
  # convert idl tree to cpp code
  #
  my $idl_txt  = grab_idl                    (@args_clean);  # print Dumper $idl_txt;
  my $idl_tree = generator_parser::parse_idl ($idl_txt);     # print Dumper $idl_tree;
                 idl_to_cpp                  ($idl_tree);
} # main
########################################################################



########################################################################
#
# filter IDL out of the latex sources
#
sub grab_idl (\@)
{
  my $args = shift;
  my $idl  = "";

  foreach my $tex ( @{$args} )
  {
    printf ("%-30s", $tex);

    # slurp in tex file
    open  (IN, "<$tex") || die "Cannot open input at $tex: $!\n";
    my @lines = <IN>;
    close (IN);
    chomp (@lines);

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
  }

  return $idl;
}
########################################################################


########################################################################
#
# before we do anything useful with the idl, we apply a number of magic cpp fixes
#
sub idl_to_cpp ($)
{
  my $idl_ref = shift;
  my %idl     = %{$idl_ref};

  # to avoid clashes with later use of the idl, we from now on operate on a copy
  my %cpp_idl = %idl;

  # print Dumper \$idl_ref;


  ##############################################################################
  #
  # before we can translate to cpp, we have fix a number of keywords
  #
  #   file         -> filesystem
  #   namespace    -> name_space
  #   logical_file -> replica

  # print Dumper \%cpp_idl;
  # exit;
  
  # package renames
  generator_helper::rename_package (\%cpp_idl, 'file',              'filesystem');
  generator_helper::rename_package (\%cpp_idl, 'namespace',         'name_space');
  generator_helper::rename_package (\%cpp_idl, 'logical_file',      'replica');

  # type renames
  generator_helper::rename_type    (\%cpp_idl, 'logical_file',      'entry');
  generator_helper::rename_type    (\%cpp_idl, 'logical_directory', 'directory');
  generator_helper::rename_type    (\%cpp_idl, 'string',            'std::string');
  generator_helper::rename_type    (\%cpp_idl, 'array',             'std::vector');
  generator_helper::rename_type    (\%cpp_idl, 'job_description',   'description');

  # method renames
  generator_helper::rename_method  (\%cpp_idl, 'size',              'get_size');

  # FIXME: we need size_t and ssize_t in several places
  # FIXME: the use of strings implies #include<string>
  # FIXME: array<> -> std::vector<> + #include<vector>
  # FIXME: use of saga classes -> #include<api|impl|cpi/package/class.hpp>
  # FIXME: enums need registration
  # FIXME? class names need fixing in method sigs (package missing)
  generator_helper::fix_signatures (\%cpp_idl);

  # move some classes from saga:: to saga::core

  # add includes we know about (exception, string. vector)
  if ( exists ($cpp_idl{'saga.core'}) )
  {
    if ( exists ($cpp_idl{'saga.core'}{'exception'}) )
    {
      # exception inherits from std::exception -- add to base and header includes
      push (@{$cpp_idl{'saga.core'}{'exception'}{'base'}},  "std::exception");
      push (@{$cpp_idl{'saga.core'}{'exception'}{'hincs'}}, "exception");
    }
  }

  
  ##############################################################################


  system "rm -rf $BASE/";
  
  mkdir  "$BASE/";
  mkdir  "$BASE/saga/";
  
  mkdir  "$BASE/saga/util/";
  mkdir  "$BASE/saga/api/";
  mkdir  "$BASE/saga/cpi/";
  mkdir  "$BASE/saga/impl/";
  # mkdir  "$BASE/saga/misc/";

  # copy a number of static utils into the new tree
  system ("cp -R ./cpp_template/* $BASE/saga/");
  
  # list of all package names
  my @p_hdrs = ();
  
  ########################################################################
  #
  # creater cpp sources headers for each package
  #
  foreach my $pname ( sort keys (%cpp_idl) )
  {
    # print Dumper $cpp_idl{$pname} if ( $pname eq "saga.error" );

    my $pkg = $pname;
  
    $pkg =~ s/^saga\.//;
  
    # print "$pkg\n";
  
    mkdir "$BASE/saga/api/$pkg/";
    mkdir "$BASE/saga/cpi/$pkg/";
    mkdir "$BASE/saga/impl/$pkg/";
    # mkdir "$BASE/saga/misc/$pkg/";
  
    my $p_api_header = "$BASE/saga/api/$pkg.hpp";
    my $p_cpi_header = "$BASE/saga/cpi/$pkg.hpp";
  
    my %pkg_elems    = ();
    my @p_elem_hdrs  = ();
  
    # create sources for each package member (enum, class, interface)
    foreach my $ename ( sort keys %{ $cpp_idl{$pname} } )
    {
      my $type    = $cpp_idl{$pname}{$ename}{'type'};
      my $api_hdr = "";
  
      if ( $type eq 'enum' )
      {
        $api_hdr = generate_api::enum_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      }
      elsif ( $type eq 'interface' ||
              $type eq 'class'     )
      {
        $api_hdr  = generate_api::class_hpp  ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $cpi_hdr  = generate_cpi::class_hpp  ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $cpi_src  = generate_cpi::class_cpp  ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $impl_hdr = generate_impl::class_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $impl_src = generate_impl::class_cpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $adap_hdr = generate_adap::class_hpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      # $adap_src = generate_adap::class_cpp ($BASE, $pkg, $ename, $cpp_idl{$pname}{$ename});
      }

      # api headers need to be included into the package header
      push (@p_elem_hdrs, $api_hdr);
    }
  


    # create package header, which pulls all package member headers
    my $PKG   = uc ($pkg);
    my $p_hdr = "saga/api/$pkg.hpp";
    my $guard = generator_helper::get_guard ($p_hdr);
    my $out   = new IO::File ("> $BASE/$p_hdr") 
             || die "Cannot create enum entry at $BASE/$p_hdr: $!\n";
  
    print $out generator_helper::format_guard_start (0, $guard);
    
    foreach my $p_elem_hdr ( @p_elem_hdrs )
    {
      print $out generator_helper::format_include (0, $p_elem_hdr);
    }
  
    print   $out generator_helper::format_guard_end (0, $guard);
    $out->close ();
  
    # save the package header for the api header
    push (@p_hdrs, $p_hdr);
  }
  
  
  ########################################################################
  #
  # create saga header, which pulls all package headers
  #
  {
    my $a_hdr = "saga/saga.hpp";  # FIXME: correct would be saga/api.hpp
    my $guard = generator_helper::get_guard ($a_hdr);
    my $out   = new IO::File ("> $BASE/$a_hdr") 
             || die "Cannot create saga header at $BASE/$a_hdr: $!\n";
  
    print   $out generator_helper::format_guard_start (0, $guard);

    foreach my $pname ( @p_hdrs ) 
    {
      print $out generator_helper::format_include (0, $pname);
    }

    print   $out generator_helper::format_guard_end (0, $guard);
  
    $out->close ();

  }  

}
########################################################################



########################################################################
END {

}
########################################################################

