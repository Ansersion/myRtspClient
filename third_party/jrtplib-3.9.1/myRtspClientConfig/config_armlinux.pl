#!/usr/bin/perl

&usage, exit 1 if ($#ARGV+1) != 2;

my $ConfigFile = shift;
my $CMakeListFile = shift;
my $CMakeListFile_Tmp = "CMakeLists.txt_tmp";
my $CMakeListFile_Build = "CMakeLists.txt_build";
my %CompileParameter = {};
my $Key;
my $Value;
my $Endian = "BIG";
my $Cmd;

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

foreach $k (keys %CompileParameter) {
	if($k =~ /RTP_ENDIAN/) {
		if($CompileParameter{$k} =~ /LITTLE/) {
			$Endian = "LITTLE";
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

$Cmd = "cat $CmakeConfigBuild $CMakeListFile > $CMakeListFile_Tmp";
system($Cmd);

open CMAKE_LIST_FILE_TMP, "<$CMakeListFile_Tmp" or die "Cannot open $CMakeListFile_Tmp: $!";
open CMAKE_LIST_FILE_BUILD, ">$CMakeListFile_Build" or die "Cannot open $CMakeListFile_Build: $!";

while(<CMAKE_LIST_FILE_TMP>) {
	my $l = $_;
	if($Endian =~ "LITTLE") {
		if($l =~ "#define RTP_BIG_ENDIAN") {
			$l = "# $l";
		}
	}
	print CMAKE_LIST_FILE_BUILD $l;
}

close CMAKE_LIST_FILE_TMP;
close CMAKE_LIST_FILE_BUILD;
$Cmd = "cp $CMakeListFile_Build ../CMakeLists.txt";
system($Cmd);
unlink $CMakeListFile_Tmp, $CMakeListFile_Build;

sub usage {
	print "perl config_armlinux.pl config.<os-platform>\n";
}
