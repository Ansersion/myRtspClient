#!/usr/bin/perl

&usage, exit 1 if ($#ARGV+1) != 1;

my $ConfigFile = shift;
my %CompileParameter = {};
my $Key;
my $Value;

open CONFIG, "<$ConfigFile" or die "Cannot open $ConfigFile: $!";
while(<CONFIG>) {
	if(/(\w+)\s*=\s*([^\s]+)\n$/) {
		$Key = $1;
		$Value = $2;
		$CompileParameter{$Key} = $Value;
		if($Value =~ /\$\((\w+)\)/) {
			$CompileParameter{$Key} = $`. $CompileParameter{$1} . $';
		}
	}
}
close CONFIG;
my $CmakeConfig = "./cmake_config.armlinux";
my $CmakeConfigBuild = "./cmake_config_build.armlinux";
open CONFIG_CMAKE, "<$CmakeConfig" or die "Cannot open $CmakeConfig: $!";
open CONFIG_CMAKE_BUILD, ">$CmakeConfigBuild" or die "Cannot open $CmakeConfigBuild: $!";
while(<CONFIG_CMAKE>) {
	if(s/TOOLCHAIN_DIR\s*<SET_VALUE>\s*/TOOLCHAIN_DIR $CompileParameter{"CROSS_COMPILE_DIR"}/) {
	} elsif(s/CMAKE_C_COMPILER\s*<SET_VALUE>\s*/CMAKE_C_COMPILER $CompileParameter{"C_COMPILER"}/) {
	} elsif(s/CMAKE_CXX_COMPILER\s*<SET_VALUE>\s*/CMAKE_CXX_COMPILER $CompileParameter{"CPLUSPLUS_COMPILER"}/) {
	}
	print CONFIG_CMAKE_BUILD $_;
}
close CONFIG_CMAKE;
close CONFIG_CMAKE_BUILD;

sub usage {
	print "perl config_armlinux.pl config.<os-platform>\n";
}
