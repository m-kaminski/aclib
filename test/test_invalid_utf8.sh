#!/bin/bash
# Test script aclib - auto completion library
# This is part of automated (unit) test suite for library
#
# Purpose of this test is to check if invalid utf8 sequences are properly reported
#
# Copyright (c) 2015, Maciej Kaminski.
# Published under 2-clause BSD-style license
# 1. Redistributions of source code must retain the above copyright notice
# 2. Redistributions in binary form must reproduce the above copyright notice

if [[ -n "$2" ]] ; then
    echo -e "$1" | ../example | grep "Invalid input sequence"
    RESULT=$?
    if [[ $RESULT -eq 0 ]] ; then
	RESULT=1;
    else
	RESULT=0
    fi
else
    echo -e "$1" | ../example | grep "Invalid input sequence"
    RESULT=$?
fi
(
    echo "Invalid UTF handling test"
    echo "Test input:  $1"
if [[ -n "$2" ]] ; then
    echo "Expected:    it to be a valid input sequence"
else
    echo "Expected:    it to be an invalid input sequence"
fi
    echo "Test result: $RESULT"
) >> test_log
exit $RESULT
