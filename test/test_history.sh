#!/bin/bash
# Test script aclib - auto completion library
# This is part of automated (unit) test suite for library
#
# Purpose of this test script is to check if history contents matches the expectation
#
# Copyright (c) 2015, Maciej Kaminski.
# Published under 2-clause BSD-style license
# 1. Redistributions of source code must retain the above copyright notice
# 2. Redistributions in binary form must reproduce the above copyright notice

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
