#!/usr/bin/perl

&usage, exit 1 if ($#ARGV+1) != 2;

`touch cmake_config_build.debug`;

my $ConfigFile = shift;
my $CMakeListFile = shift;
my $Cmd;

my $CmakeConfig = "./cmake_config.debug";

$Cmd = "cat $CmakeConfig $CMakeListFile > ../CMakeLists.txt";
system($Cmd);


