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
