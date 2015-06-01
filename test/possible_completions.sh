#!/bin/bash

echo "Possible completions test" >> test_log
echo -e "alp\t\t" | ../example | diff template_output -
if [[ $? -ne 0 ]]
then
    (
	echo "Expected:"
	cat template_output
	echo "Got:"
	echo -e "alp\t\t" | ../example
	echo "Passed. Test result: 1"
    )>> test_log
    exit 1
else
    echo "Passed. Test result: 0" >> test_log
    exit 0
fi
