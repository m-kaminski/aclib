/**
 * Test script aclib - auto completion library
 * This is part of automated (unit) test suite for library
 *
 * Purpose of this test is to check how library reacts to insane amount of completions
 *
 * Copyright (c) 2015, Maciej Kaminski.
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#include <stdint.h>
#include "../auto_complete.h"

int main(int argc, char **argv)
{
	int how_many;
	int i;
	sscanf(argv[1], "%d", &how_many);
	for (i=0 ; i!=how_many ; ++i)
	{
		int res;
		char tmp[40];
		sprintf(tmp,"%d",i);
		res = init_completion(tmp);
		if (i<MAX_NUM_COMPLETIONS && res) {
			return 1; /* init completion failed while it should succeed */
		}
		if (i>=MAX_NUM_COMPLETIONS && !res) {
			return 1; /* init completion did not fail while it should fail */
		}
	}
	return 0;
}
