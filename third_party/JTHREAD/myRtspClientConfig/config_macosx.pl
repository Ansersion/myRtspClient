#!/usr/bin/perl

&usage, exit 1 if ($#ARGV+1) != 2;

`touch cmake_config_build.macosx`;

my $ConfigFile = shift;
my $CMakeListFile = shift;
my $Cmd;

$Cmd = "cp $CMakeListFile ../CMakeLists.txt";
system($Cmd);


