#!/bin/bash
usage() {
	echo "Usage: $0 <test_suit_tmp.cpp>"
	exit 1
}

if [ $# -ne 1 ] 
then
	usage $*
fi

ls | grep -v my_gtest_main.cc |grep -v $1| grep cpp$ | xargs -I '{}' mv '{}' ./backup/
