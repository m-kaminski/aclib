/******
 *
 * helper header for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski. 
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#ifndef __VT100_TERM_
#define __VT100_TERM_
/**
 * Disable local echo on terminal.
 * Also: disables canon mode therefore allowing to getc single characters
 * from stdin.
 */
void echo_disable();

/**
 * Reverts what echo_disable does
 */
void echo_enable();
#endif
