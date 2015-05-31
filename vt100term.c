/******
 *
 * helper library source file for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski. 
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "vt100codes.h"
#include "special_keys.h"

static struct termios term;

void echo_disable()
{
	tcgetattr(1, &term);
	term.c_cc[VMIN] = 1;
	term.c_lflag &= ~(ECHO | ICANON); // disable echo on terminal
	tcsetattr(1, 0, &term);
}

void echo_enable()
{
	term.c_lflag |= ECHO | ICANON; // reenable echo on terminal
	tcsetattr(1, 0, &term);
}
