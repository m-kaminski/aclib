#!/bin/bash

OUTPUT=`echo -e "$1" | ../example | grep Input | cut -d "\"" -f 2`


test "$2" = "$OUTPUT"
RESULT=$?
(
    echo "Editing test test"
    echo "Test input:  $1"
    echo "Test output: $OUTPUT"
    echo "Expected:    $2"
    echo "Test result: $RESULT"
) >> test_log
exit $RESULT
