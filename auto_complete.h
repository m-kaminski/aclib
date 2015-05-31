/******
 *
 * main header for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski.
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */

#ifndef __AUTO_COMPLETE_
#define __AUTO_COMPLETE_

/*
 * If you feel that you need more history entries, more possible completions
 * or longer commands, feel free to adjust following macros.
 *
 * Note that library holds static arrays of size:
 *  COMMANDLEN_MAX*MAX_NUM_COMPLETIONS*sizeof(int)
 *  and COMMANDLEN_MAX*MAX_NUM_HISTORY*sizeof(int)
 *
 * and thus if you'll increase these above common sense, memory footprint of
 * your program will grow substantially.
 */

/**! maximum length of command */
#define COMMANDLEN_MAX 4096

/**! maximum number of possible completions */
#define MAX_NUM_COMPLETIONS 2048

/**! number of history entries */
#define MAX_NUM_HISTORY 200


/**
 * Add a string onto list of possible completions
 *
 * @param[in] compl a string to which text inserted may be expanded (in aclib lingo: availabe completion)
 */
void init_completion(char*compl);

/**
 * Helper routine - must be called after init_completion and before getline_complete.
 * - it sorts completions within internal tables of aclib.
 * if no new completions are added, there is no need to call this routine between calls to
 * getline_complete
 */
void init_completions();

/**
 * Reads command from standard input.
 * Returns null terminated string (not including newline character).
 *
 * Allows auto-completion and history browsing via following keys:
 * tab - completion (if unique)
 * tab tab - print list of available completions (if not unique)
 * left arrow - move cursor left by one character
 * right arrow - move cursor left by one character
 * backspace - remove character before the cursor
 * ctrl+left arrow - move cursor left by one token (word separated by ' ', '[', '.', '/')
 * ctrl+right arrow - move cursor left by one token (word separated by ' ', '[', '.', '/')
 * up arrow - history before
 * down arrow - history after
 * ctrl+w - clear token under cursor
 * enter - finish entering command
 *
 * There is no need to free buffer returned by getline_complete, as it is static array
 * held by library
 *
 * if function returns NULL, EOF appeared with no text entered (otherwise pointer to string
 * with strlen of 0 will be returned)
 *
 * @param prompt command prompt
 */
char *getline_complete(char *prompt);

/**
 * Prints history to stdout.
 */
void print_history();

#endif
