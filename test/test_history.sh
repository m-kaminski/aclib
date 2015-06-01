#!/bin/bash

OUTPUT=`echo -e "$1" | ../example | grep \# | cut -d " " -f 2`

EXPECTATION=`echo -e "$2"`

test "$EXPECTATION" = "$OUTPUT"

RESULT=$?
(
    echo "History content test"
    echo "Test input:"
    echo "$1"
    echo "Test output: $OUTPUT"
    echo "Expected:"
    echo "$EXPECTATION"
    echo "Test result:"
    echo "$RESULT"
) >> test_log
exit $RESULT
