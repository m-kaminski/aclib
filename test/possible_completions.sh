#!/bin/bash

echo "Possible completions test" >> test_log
echo -e "$1" | ../example | diff $2 -
if [[ $? -ne 0 ]]
then
    (
	echo "Expected:"
	cat $2
	echo "Got:"
	echo -e "$1" | ../example
	echo "Passed. Test result: 1"
    )>> test_log
    exit 1
else
    echo "Passed. Test result: 0" >> test_log
    exit 0
fi
