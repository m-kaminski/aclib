#!/bin/bash

gcc test_characters.c ../aclib.a -o test_characters.exe -g -fprofile-arcs -ftest-coverage

OUTPUT=`echo -e "$1" | ./test_characters.exe `

test "$2" = "$OUTPUT"

RESULT=$?
(
    echo "Editing test"
    echo "Test input:  $1"
    echo "Test output: $OUTPUT"
    echo "Expected:    $2"
    echo "Test result: $RESULT"
) >> test_log
exit $RESULT
