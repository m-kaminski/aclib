/******
 *
 * library source file for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski. 
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Published under 2-clause BSD-style license
 * 1. Redistributions of source code must retain the above copyright notice
 * 2. Redistributions in binary form must reproduce the above copyright notice
 */
#define _POSIX_SOURCE
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
#include <search.h>
#include <signal.h>
#include <stdlib.h>
#include "special_keys.h"
#include "vt100codes.h"
#include "vt100term.h"
#include "auto_complete.h"

#define TOKENLEN_MAX 4096
#define MAX_NUM_COMPLETIONS 2048
#define MAX_NUM_HISTORY 200

static int completions[MAX_NUM_COMPLETIONS][TOKENLEN_MAX];
static size_t current_num_completions = 0; /* incremented at each completion inited*/

static int history[MAX_NUM_HISTORY][COMMANDLEN_MAX];
static size_t total_history_entries = 0;
static size_t last_history_entry = -1;

/**
 * global variable used for passing state to comparison functions
 * called by lfind etc.
 */
static size_t memsize;

#ifdef THREAD_SAFETY
#include <pthread.h>
static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
#define START_CRITICAL() pthread_mutex_lock(&global_mutex)
#define END_CRITICAL() pthread_mutex_unlock(&global_mutex)

#else
#define START_CRITICAL()
#define END_CRITICAL()
#endif

/* helper for lsearch/bsearch */
int compl_eqcompar(const void *key, const void *s2)
{
	return memcmp(key, s2, memsize*4);
}

/* helper for lsearch */
int compl_neqcompar(const void *key, const void *s2)
{
	return !memcmp(key, s2, memsize*4);
}

/**
 * find first and last completion string iterator, that matches first <u>size</u>
 * bytes from string given in parameter <u>begin</u>
 *
 * @param[in] begin string which we search for
 * @param[in] size amount of characters to match
 * @param[out] it_begin first match
 * @param[out] it_end last match
 */
void compute_completion_ranges(int* begin, int size, int **it_begin, int **it_end)
{
	memsize=size;
	*it_begin=(int*)(lfind(begin, completions,
				&current_num_completions, sizeof(completions[0]),
				compl_eqcompar));
	size_t num_negative = current_num_completions -
		(*it_begin-&completions[0][0]) / TOKENLEN_MAX;

	if (!*it_begin || !size) {
		return;
	}
	*it_end=(int*)(lfind(*it_begin, *it_begin,
				&num_negative, sizeof(completions[0]),
				compl_neqcompar));
	if (!*it_end) {
		*it_end = (&completions[current_num_completions][0]);
	}
}

/**
 * search unique completion for string given in stdinput while matching only specified
 * amount of initial characters
 *
 * @param begin begining of string
 * @param size portion of string to be matched
 * @return null terminated string of 32-bit characters - completion matched
 */
int* search_completion(int* begin, int size) {
	int *it_begin, *it_end;
	compute_completion_ranges(begin, size, &it_begin, &it_end);
	if (!it_begin || !size) {
		return 0;
	}
	size_t num_negative = (it_end-it_begin) / TOKENLEN_MAX;
	int *keypart=it_begin;

	if ( ((keypart-&completions[0][0]) / TOKENLEN_MAX == current_num_completions-1) ) {
		/* completion is unique */
		return keypart+size;
	} else {
		/* else return unique part and nothing more */
		int i;
		static int line[COMMANDLEN_MAX];
		for (i = 0 ; keypart[i]; ++i) {
			if (keypart[size+i]!=keypart[size+i+TOKENLEN_MAX*(num_negative-1)]) {
				break;
			}
			line[i]=keypart[i+size];
		}
		line[i] = 0;
		return line;
	}
}

/**
 * returns true if it is character on which to stop when shifting o
 * cursor with control or when printing out long string expansions
 */
bool is_stopkey(int c) {
	return	c=='.' ||
		c=='/' ||
		c=='[' ||
		c==' ' ;
}

/**
 * search for set of completions for given string and print all of them to standard input.
 *
 * @param begin begining of string
 * @param size portion of string to be matched
 */
void print_all_completions(int* begin, int size)
{
	int *it_begin, *it_end;
	compute_completion_ranges(begin, size, &it_begin, &it_end);
	if (!it_begin || !size) {
	printf("No completions possible. Type " SET_ATTR "help"
		       SET_ATTR " to learn about possible commands.\n",
		       ATTR_BRIGHT, ATTR_RESET);
		return;
	}
	size_t num_negative = (it_end-it_begin) / TOKENLEN_MAX;
	int i,j;
	int last_j = 0;
	for (i = 0 ; i != num_negative; ++i) {
		if (i) {
			if (!memcmp(&it_begin[i*TOKENLEN_MAX], &it_begin[(i-1) * TOKENLEN_MAX], (last_j + 1) * 4)) {
				continue;
			}
		}
		printf(SET_ATTR, ATTR_RESET);
		for (j = 0 ; it_begin[i*TOKENLEN_MAX+j]; ++j) {
			if (j==size) {
				printf(SET_ATTR, ATTR_BRIGHT);
			}
			putc_utf8(it_begin[i*TOKENLEN_MAX+j]);
			if ((is_stopkey(it_begin[i*TOKENLEN_MAX+j]))&&
			    it_begin[i*TOKENLEN_MAX+j + 1] &&
			    j>size) {
				printf(SET_ATTR, ATTR_RESET);
				printf("[more]");
				break;
			}
		}
		last_j=j;
		puts("");
	}
	printf(SET_ATTR, ATTR_RESET);
}

/**
 * print command prompt string
 */
void print_prompt(const char *prompt)
{
	printf(SET_ATTR, ATTR_BRIGHT);
	printf(SET_ATTR, FG_COLOR_BLUE);
	printf("%s",prompt);
	printf(SET_ATTR, ATTR_RESET);
}

/** 
 * Signal handler that doesn't do anything - just allows program
 * to terminate cleanly without disrupting terminal settings
 */
typedef void (*sighandler_t)(int);
void sighandler(int sig) {}

/**
 * line being acquired by getline_complete
 */
static int line[COMMANDLEN_MAX];
static int cursorpos = 0;
static int line_len = 0;
static const char *gprompt;
/* main api function */
char *getline_complete(const char *prompt)
{
	START_CRITICAL();
	memset(line,0,sizeof(line));
	gprompt=prompt;
	static char line_t[COMMANDLEN_MAX];
	sighandler_t def_handler = signal(SIGINT,sighandler);
	int current_history_entry = last_history_entry  + 1;
	int c,i;
	line_len = 0;
	cursorpos = 0;
	int cons_tabs = 0;
	if (current_history_entry>=MAX_NUM_HISTORY)
		current_history_entry = 0;
	echo_disable();
	printf(INSERT_MODE);
	print_prompt(prompt);
	END_CRITICAL();
	while (1) {
		c=decode_key_pressed();
		START_CRITICAL();
		if (c == -1) {
			puts("\nInvalid input sequence!");
			break;
		}

		if (!is_special(c)) {
			putc_utf8(c);
			memmove(&line[cursorpos + 1], &line[cursorpos],
				sizeof(int) * (line_len-cursorpos));
			line[cursorpos]=c;
			cursorpos++;
			line_len++;
			cons_tabs = 0;
		} else {

			if (c == CtrlLeft && cursorpos) {
				int moves = 0;
				do {
					cursorpos--;
					moves++;
				} while ( !(is_stopkey(line[cursorpos])) &&
					cursorpos);
				printf(CURSOR_BACKWARD_N,moves);
			}

			if (c == CtrlRight && cursorpos<line_len) {
				int moves = 0;
				do {
					cursorpos++;
					moves++;
				} while ( !(is_stopkey(line[cursorpos])) &&
					cursorpos<line_len);
				printf(CURSOR_FORWARD_N,moves);
			}

			if (c == Left && cursorpos) {
				printf(CURSOR_BACKWARD_N,1);
				cursorpos--;
			}

			if (c == Right && cursorpos<line_len) {
				printf(CURSOR_FORWARD_N,1);
				cursorpos++;
			}

			if (c == Home) {
				if (cursorpos) {
					printf(CURSOR_BACKWARD_N, cursorpos);
					cursorpos = 0;
				}
			}

			if (c == End) {
				if (cursorpos!=line_len) {
					printf(CURSOR_FORWARD_N, line_len-cursorpos);
					cursorpos=line_len;
				}
			}

			if ((c == Backspace && cursorpos) ||
			    (c == Delete && cursorpos<line_len)) {
				line_len--;
				if (c == Backspace) {
					cursorpos--;
					printf(CURSOR_BACKWARD_N,1);
				}
				memmove(&line[cursorpos],&line[cursorpos + 1],
					sizeof(int) * (line_len-cursorpos));
				printf(ERASE_END_OF_LINE);
				if (cursorpos!=line_len) {
					for (i = cursorpos ; i !=  line_len; ++i)
						putc_utf8(line[i]);
					printf(CURSOR_BACKWARD_N, line_len-cursorpos);
				}
			}

			/* kill token */
			if (c == CtrlW) {
				int kill_begin = cursorpos;
				int kill_end = cursorpos == line_len ?
					cursorpos : cursorpos + 1;
				while ( !(is_stopkey(line[kill_begin])) &&
					kill_begin) {
					kill_begin--;
				}

				while ( !(is_stopkey(line[kill_end])) &&
					kill_end<line_len) {
					kill_end++;
				}

				memmove(&line[kill_begin], &line[kill_end],
					sizeof(int) * (line_len-kill_end));
				if (cursorpos > kill_begin) {
					printf(CURSOR_BACKWARD_N, cursorpos - kill_begin);
					cursorpos = kill_begin;
				}
				line_len -= kill_end-kill_begin;
				printf(ERASE_END_OF_LINE);
				if (cursorpos!=line_len) {
					for (i = cursorpos ; i !=  line_len; ++i)
						putc_utf8(line[i]);
					printf(CURSOR_BACKWARD_N, line_len-cursorpos);
				}
			}

			/* handle history */
			if (total_history_entries && (c == Up || c == Down)) {
				if (c == Up) current_history_entry--;
				if (c == Down) current_history_entry++;

				if ((int)(current_history_entry) >= (int)(total_history_entries)) {
					current_history_entry = 0;
				}

				if ((int)(current_history_entry)<0) {
					current_history_entry =
						total_history_entries - 1;
				}

				if (cursorpos) {
					printf(CURSOR_BACKWARD_N, cursorpos);
				}

				printf(ERASE_END_OF_LINE);
				cursorpos = 0;
				line_len = 0;
				for (i = 0 ; history[current_history_entry][i]; ++i) {
					putc_utf8(history[current_history_entry][i]);
					line[cursorpos]=history[current_history_entry][i];
					cursorpos++;
					line_len++;
				}
			} /* end handling of history */

			if (c == Tab && !cursorpos) {
				cons_tabs++;
			} else if (c == Tab && cursorpos) {

				int cplpos=cursorpos;
				while (line[cplpos-1] != ' ' && cplpos) {
					cplpos--;
				}
				int *cpl = &line[cplpos];
				cpl = search_completion(cpl, cursorpos-cplpos);
				if (cpl) {
					printf(ERASE_END_OF_LINE);
					for (i = 0 ; cpl[i]; ++i) {
						putc_utf8(cpl[i]);
						memmove(&line[cursorpos + 1],&line[cursorpos],
							sizeof(int) * (line_len-cursorpos));
						line[cursorpos]=cpl[i];
						cursorpos++;
						line_len++;
					}

					if (cursorpos!=line_len) {
						for (i = cursorpos ; i !=  line_len; ++i)
							putc_utf8(line[i]);
						printf(CURSOR_BACKWARD_N, line_len-cursorpos);
					}
				}
				cons_tabs++;
			} else {
				cons_tabs = 0;
			} /* end of handling FIRST tab key (resulting in unique completion */

			/* handle tab-tab sequence, resulting in printing all 
			 * possible completions */
			if (c == Tab && cons_tabs>1) {
				printf(SET_ATTR, ATTR_BRIGHT);
				puts("\npossible completions are:");
				printf(SET_ATTR, ATTR_RESET);
				if (!cursorpos) {
					print_all_completions(line, 0);
					print_prompt(prompt);
					for (i = 0 ; i !=  line_len; ++i)
						putc_utf8(line[i]);
					if (line_len!=cursorpos)
						printf(CURSOR_BACKWARD_N, line_len-cursorpos);
					END_CRITICAL();
					continue;
				}
				int cplpos = cursorpos;
				while (line[cplpos-1]!=' ' && cplpos)
					cplpos--;
				int *cpl = &line[cplpos];
			
				print_all_completions(cpl, cursorpos-cplpos);
				print_prompt(prompt);
				for (i = 0 ; i !=  line_len; ++i)
					putc_utf8(line[i]);
				if (line_len!=cursorpos)
					printf(CURSOR_BACKWARD_N, line_len-cursorpos);
			} /* end of handling tab tab sequence */
		}
		if (c == EoF || c == Enter) {
			puts("");
			END_CRITICAL();
			break;
		}
		END_CRITICAL();
	}

	START_CRITICAL();
	/* restore replace mode, reenable local echo */
	printf(REPLACE_MODE);
	echo_enable();
	/* allow interruption */
	signal(SIGINT, def_handler);

	/* finished processing line; we may possibly add it to history */
	memset(&line[line_len], 0, sizeof(history[0])-sizeof(int)*line_len);
	if (memcmp(line, history[last_history_entry], sizeof(line))) {
		/* this command differs from last command in history */
		last_history_entry++;
		if (last_history_entry == MAX_NUM_HISTORY) {
			last_history_entry = 0;
		}
		if (total_history_entries<last_history_entry + 1) {
			total_history_entries=last_history_entry + 1;
		}
		memset(history[last_history_entry], 0 , sizeof(history[0]));
		memcpy(history[last_history_entry], line, line_len*4);
	}

	/* return 0 if string is empty and EoF character received.
	 * when string is not empty, than it may be intput without newline
	 * at its end - just process command normally
	 */
	if (!line_len && c == EoF) {
		signal(SIGINT, def_handler);
		gprompt=0;
		END_CRITICAL();
		return 0;
	}
	/* convert charset to output one */
	gprompt=0;
	trans_utf8(line_t, line);
	END_CRITICAL();
	return line_t;
}

void hide_acline()
{
	START_CRITICAL();
	printf("\r");
	printf(ERASE_END_OF_LINE);
	fflush(stdout);
}

void restore_acline()
{
	int i;
	if (!gprompt) {
		END_CRITICAL();
		return;
	}
	print_prompt(gprompt);
	for (i = 0 ; i !=  line_len; ++i)
		putc_utf8(line[i]);
	if (line_len!=cursorpos)
		printf(CURSOR_BACKWARD_N, line_len-cursorpos);
	fflush(stdout);
	END_CRITICAL();
}
/* main api function */
int init_completion(const char *compl)
{
	int i;
	if (current_num_completions == MAX_NUM_COMPLETIONS) {
		return 1;
	}
	START_CRITICAL();
	for (i = 0 ; compl[i]; ++i) {
		if (i >= TOKENLEN_MAX-1) {
			memset(&completions[current_num_completions][0], 0,
			       sizeof(completions[i]));
			END_CRITICAL();
			return 1;
		}
		completions[current_num_completions][i]=compl[i];
	}
	completions[current_num_completions][i] = 0;
	current_num_completions++;
	END_CRITICAL();
	return 0;
}

/* helper for qsort*/
int compl_lesscomp(const void *s1, const void *s2)
{
	return memcmp(s1, s2, COMMANDLEN_MAX);
}

/* main api function */
void init_completions()
{
	START_CRITICAL();
	qsort(completions, current_num_completions, sizeof(completions[0]),
	      compl_lesscomp);
	END_CRITICAL();
}

/* main api function */
int completion_exists(const char *compl)
{
	int i;
	int *it_begin;
	int tmp[TOKENLEN_MAX];
	START_CRITICAL();
	for (i = 0 ; compl[i]; ++i) {
		if (i == TOKENLEN_MAX-1) {
			break;
		}
		tmp[i]=compl[i];
	}

	tmp[i] = 0;
	memsize = i; /* compare amount of characters equivalent to lenth of compl 
		      * plus a NULL-terminator */
	it_begin = (int*)(lfind(tmp, completions,
		&current_num_completions, sizeof(completions[0]),
		compl_eqcompar));
	END_CRITICAL();
	return it_begin != NULL;
}

/* main api function */
void print_history()
{
	int i;
	START_CRITICAL();
	if (last_history_entry + 1!=total_history_entries) {
		for (i = last_history_entry + 1; i != MAX_NUM_HISTORY; ++i) {
			char buf[COMMANDLEN_MAX];
			trans_utf8(buf,history[i]);
			printf("# %s\n", buf);
		}
	}
	for (i = 0; i != last_history_entry + 1; ++i) {
		char buf[COMMANDLEN_MAX];
		trans_utf8(buf,history[i]);
		printf("# %s\n", buf);
	}
	END_CRITICAL();
}
