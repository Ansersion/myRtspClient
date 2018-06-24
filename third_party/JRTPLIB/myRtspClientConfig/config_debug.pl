#!/usr/bin/perl

&usage, exit 1 if ($#ARGV+1) != 2;

`touch cmake_config_build.debug`;

my $ConfigFile = shift;
my $CMakeListFile = shift;
my $Cmd;

open CONFIG, "<$ConfigFile" or die "Cannot open $ConfigFile: $!";
while(<CONFIG>) {
	if(/^\s*(\w+)\s*=\s*([^\s]+)\n$/) {
		$Key = $1;
		$Value = $2;
		$CompileParameter{$Key} = $Value;
		if($Value =~ /\$\((\w+)\)/) {
			$CompileParameter{$Key} = $`. $CompileParameter{$1} . $';
		}
	}
}

my $foundJthread = 0;
foreach $k (keys %CompileParameter) {
	if($k =~ /RTP_ENDIAN/) {
		if($CompileParameter{$k} =~ /LITTLE/) {
			$Endian = "LITTLE";
		}
	}
	if($k =~ /RTP_JTHREAD/) {
        $foundJthread = 1;
		if($CompileParameter{$k} =~ /ENABLE/) {
            $CompileParameter{$k} = "1";
		} else {
            $CompileParameter{$k} = "0";
        }
	}
}
if(0 == $foundJthread) {
    $CompileParameter{"RTP_JTHREAD"} = "0";
}

close CONFIG;
my $CmakeConfig = "./cmake_config.debug";
my $CmakeConfigBuild = "./cmake_config_build.debug";
open CONFIG_CMAKE, "<$CmakeConfig" or die "Cannot open $CmakeConfig: $!";
open CONFIG_CMAKE_BUILD, ">$CmakeConfigBuild" or die "Cannot open $CmakeConfigBuild: $!";
while(<CONFIG_CMAKE>) {
	if(s/JTHREAD_FOUND\s*<BOOL>\s*/JTHREAD_FOUND $CompileParameter{"RTP_JTHREAD"}/) {
    }
	print CONFIG_CMAKE_BUILD $_;
}
close CONFIG_CMAKE;
close CONFIG_CMAKE_BUILD;

$Cmd = "cat $CmakeConfigBuild $CMakeListFile > ../CMakeLists.txt";
system($Cmd);


