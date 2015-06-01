#!/bin/bash

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
