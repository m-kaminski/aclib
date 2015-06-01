#!/bin/bash
# Test script aclib - auto completion library
# This is part of automated (unit) test suite for library
#
# Run gcov on library source files
#
# Copyright (c) 2015, Maciej Kaminski.
# Published under 2-clause BSD-style license
# 1. Redistributions of source code must retain the above copyright notice
# 2. Redistributions in binary form must reproduce the above copyright notice

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

for i in `find $DIR/../*.c`
do
    if [[ $i =~ .*example.c ]]
    then
	echo "Not running gcov analysis for sample"
    else
	echo $i
	gcov $i
    fi
done
