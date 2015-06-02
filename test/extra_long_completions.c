/**
 * Test script aclib - auto completion library
 * This is part of automated (unit) test suite for library
 *
 * Purpose of this test is to check how library reacts to insanely long completion texts
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
	int i;
	int how_many=MAX_NUM_COMPLETIONS*2;
	char tmp[COMMANDLEN_MAX*2];
	memset(tmp, 'a', sizeof(tmp));
	tmp[sizeof(tmp)-2]=0;
	for (i=0 ; i!=how_many ; ++i)
	{
		int res;
		res = completion_exists(tmp);
		if (res) {
			return 1; /* init completion did not fail while it should fail */
		}
		res = init_completion(tmp);
		if (!res) {
			return 1; /* init completion did not fail while it should fail */
		}
	}
	
	tmp[COMMANDLEN_MAX]=0;
	for (i=0 ; i!=how_many ; ++i)
	{
		int res;
		res = init_completion(tmp);
		if (!res) {
			return 1; /* init completion did not fail while it should fail */
		}
	}

	how_many=MAX_NUM_COMPLETIONS;
	tmp[COMMANDLEN_MAX-1]=0;
	for (i=0 ; i!=how_many ; ++i)
	{
		int res;
		res = init_completion(tmp);
		if (res) {
			return 1; /* init completion did not fail while it should fail */
		}
	}
	
	return 0;
}
