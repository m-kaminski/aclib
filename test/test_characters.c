/**
 * Test script aclib - auto completion library
 * This is part of automated (unit) test suite for library
 *
 * Purpose of this test processing of special characters from xterm
 *
 * Copyright (c) 2015, Maciej Kaminski.
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../special_keys.h"

int main()
{
	int i = decode_key_pressed();
	const char *kn = get_key_name(i);
	puts(kn);
	return 0;
}
