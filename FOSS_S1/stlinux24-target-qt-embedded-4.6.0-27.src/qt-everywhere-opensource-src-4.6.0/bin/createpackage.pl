#!/usr/bin/perl
#############################################################################
##
## Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the S60 port of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial Usage
## Licensees holding valid Qt Commercial licenses may use this file in
## accordance with the Qt Commercial License Agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Nokia.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
## $QT_END_LICENSE$
##
#############################################################################

############################################################################################
#
# Convenience script for creating signed packages you can install on your phone.
#
############################################################################################

use strict;

# use a command-line parsing module
use Getopt::Long;
# Use file name parsing module
use File::Basename;
# Use File::Spec services mainly rel2abs
use File::Spec;
# use CWD abs_bath, which is exported only on request
use Cwd 'abs_path';


sub Usage() {
    print <<ENDUSAGESTRING;

==============================================================================================
Convenience script for creating signed packages you can install on your phone.

Usage: createpackage.pl [options] templatepkg target-platform [certificate key [passphrase]]

Where supported optiobns are as follows:
     [-i|install]            = Install the package right away using PC suite
     [-p|preprocess]         = Only preprocess the template .pkg file.
     [-c|certfile=<file>]    = The file containing certificate information for signing.
                               The file can have several certificates, each specified in
                               separate line. The certificate, key and passphrase in line
                               must be ';' separated. Lines starting with '#' are treated 
                               as a comments. Also empty lines are ignored. The paths in 
                               <file> can be absolute or relative to <file>.
Where parameters are as follows:
     templatepkg             = Name of .pkg file template
     target                  = Either debug or release
     platform                = One of the supported platform
                               winscw | gcce | armv5 | armv6 | armv7
     certificate             = The certificate file used for signing
     key                     = The certificate's private key file
     passphrase              = The certificate's private key file's passphrase

Example:
     createpackage.pl fluidlauncher_template.pkg release-armv5
     
Example with certfile:
     createpackage.pl -c=mycerts.txt fluidlauncher_template.pkg release-armv5
     
     Content of 'mycerts.txt' must be something like this:
        # This is comment line, also the empty lines are ignored
        rd.cer;rd-key.pem
        .\\cert\\mycert.cer;.\\cert\\mykey.key;yourpassword
        X:\\QtS60\\selfsigned.cer;X:\\QtS60\\selfsigned.key

If no certificate and key files are provided, either a RnD certificate or
a self-signed certificate from Qt installation root directory is used.
==============================================================================================

ENDUSAGESTRING

    exit();
}

# Read given options
my $install = "";
my $preprocessonly = "";
my $certfile = "";

unless (GetOptions('i|install' => \$install, 'p|preprocess' => \$preprocessonly, 'c|certfile=s' => \$certfile)){
    Usage();
}

my $certfilepath = abs_path(dirname($certfile));

# Read params to variables
my $templatepkg = $ARGV[0];
my $targetplatform = lc $ARGV[1];

my @tmpvalues = split('-', $targetplatform);
my $target = $tmpvalues[0];
my $platform = $tmpvalues[1];;

# Convert visual target to real target (debug->udeb and release->urel)
$target =~ s/debug/udeb/i;
$target =~ s/release/urel/i;

my $certificate = $ARGV[2];
my $key = $ARGV[3];
my $passphrase = $ARGV[4];

# Generate output pkg basename (i.e. file name without extension)
my $pkgoutputbasename = $templatepkg;
$pkgoutputbasename =~ s/_template\.pkg/_$targetplatform/g;
$pkgoutputbasename = lc($pkgoutputbasename);

# Store output file names to variables
my $pkgoutput = lc($pkgoutputbasename.".pkg");
my $unsigned_sis_name = $pkgoutputbasename."_unsigned.sis";
my $signed_sis_name = $pkgoutputbasename.".sis";

# Store some utility variables
my $scriptpath = dirname(__FILE__);
my $certtext = $certificate;
my $certpath = $scriptpath;
$certpath =~ s-^(.*[^\\])$-$1\\-o;          # ensure path ends with a backslash
$certpath =~ s-/-\\-go;                     # for those working with UNIX shells
$certpath =~ s-bin\\$-src\\s60installs\\-;  # certificates are one step up in hierarcy

# Check some pre-conditions and print error messages if needed
unless (length($templatepkg) && length($platform) && length($target)) {
    print "\nError: Template PKG filename, platform or target is not defined!\n";
    Usage();
}

# Check template exist
stat($templatepkg);
unless( -e _ ) {
    print "\nError: Package description file '$templatepkg' does not exist!\n";
    Usage();
}

# Check certifcate preconditions and set default certificate variables if needed
if (length($certificate)) {
    unless(length($key)) {
        print "\nError: Custom certificate key file parameter missing.!\n";
        Usage();
    }
} else {
    #If no certificate is given, check default options
    $certtext = "RnD";
    $certificate = $certpath."rd.cer";
    $key = $certpath."rd-key.pem";

    stat($certificate);
    unless( -e _ ) {
        $certtext = "Self Signed";
        $certificate = $certpath."selfsigned.cer";
        $key = $certpath."selfsigned.key";
    }
}

# Read the certificates from file to two dimensional array
my @certificates;
if (length($certfile)) {
    open CERTFILE, "<$certfile" or die $!;
    while(<CERTFILE>){
        s/#.*//;                            # ignore comments by erasing them
        next if /^(\s)*$/;                  # skip blank lines
        chomp;                              # remove trailing newline characters
        my @certinfo = split(';', $_);      # split row to certinfo
        
        # Trim spaces
        for(@certinfo) {
            s/^\s+//;
            s/\s+$//;
        }        
        
        # Do some validation
        unless(scalar(@certinfo) >= 2 && scalar(@certinfo) <= 3 && length($certinfo[0]) && length($certinfo[1]) ) {    
            print "\nError: $certfile line '$_' does not contain valid information!\n";
            Usage();            
        }   

        push @certificates, [@certinfo];    # push data to two dimensional array
    }
}

# Remove any existing .sis packages
unlink $unsigned_sis_name;
unlink $signed_sis_name;
unlink $pkgoutput;

# Preprocess PKG
local $/;
# read template file
open( TEMPLATE, $templatepkg) or die "Error '$templatepkg': $!\n";
$_=<TEMPLATE>;
close (TEMPLATE);

# replace the PKG variables
s/\$\(PLATFORM\)/$platform/gm;
s/\$\(TARGET\)/$target/gm;

#write the output
open( OUTPUT, ">$pkgoutput" ) or die "Error '$pkgoutput' $!\n";
print OUTPUT $_;
close OUTPUT;

if ($preprocessonly) {
    exit;
}

# Create SIS.
system ("makesis $pkgoutput $unsigned_sis_name");

# Sign SIS with certificate info given as an argument.
system ("signsis $unsigned_sis_name $signed_sis_name $certificate $key $passphrase");

# Check if creating signed SIS Succeeded
stat($signed_sis_name);
if( -e _ ) {
    print ("\nSuccessfully created $signed_sis_name using certificate: $certtext!\n");

    # Sign with additional certificates & keys
    for my $row ( @certificates ) {
        # Get certificate absolute file names, relative paths are relative to certfilepath
        my $abscert = File::Spec->rel2abs( $row->[0], $certfilepath);
        my $abskey = File::Spec->rel2abs( $row->[1], $certfilepath);

        system ("signsis $signed_sis_name $signed_sis_name $abscert $abskey $row->[2]");
        print ("\tAdditionally signed the SIS with certificate: $row->[0]!\n");
    }
    
    # remove temporary pkg and unsigned sis
    unlink $pkgoutput;
    unlink $unsigned_sis_name;

    # Install the sis if requested
    if ($install) {
        print ("\nInstalling $signed_sis_name...\n");
        system ("$signed_sis_name");
    }
} else {
    # Lets leave the generated PKG for problem solving purposes
    print ("\nSIS creation failed!\n");
}


#end of file
