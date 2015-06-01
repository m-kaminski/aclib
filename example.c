/**
 * This is example file for aclib - auto completion library
 *
 * Copyright (c) 2015, Maciej Kaminski.
 * WWW: maciejkaminski.pl
 * email: maciej.a.kaminski@gmail.com
 *
 * Unlike the rest of libraty (which is shared under 2-clause BSD license)
 * this file is under Public Domain and you can use it however you want.
 */
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "auto_complete.h"

#define MAX_COMMAND_LENGTH 20

void setup();

/**
 * Example program does just following things:
 * - Reads command from stdin (allowing autocompletion of commands and some other patterns)
 * - Prints it back to stdout
 * - If "history" is entered, prints command history
 * - If "help" is enteret, prints helpful message
 * - If "exit" is entered, program ends.
 * - Otherwise complains that command is not recognized
 *
 * To end program user can also type either Ctrl+C (SIGINT) or Ctrl+D (SIGSTOP) with an
 * empty command line.
 */
int main()
{
	setup();
	while (1) {
		char command[MAX_COMMAND_LENGTH];
		char *line = getline_complete("example:> ");
		if (!line)
			break;
		printf("Input string: \"%s\"\n",line);
		/* let's try simpliest way to parse a command */
		if (1 == sscanf(line, "%20s", command) ) {

			if (!strncmp(command, "history", MAX_COMMAND_LENGTH)) {
				print_history();
			} else if (!strncmp(command, "help", MAX_COMMAND_LENGTH)) {
				puts("Read the source to know what to do. Type \""
					"exit\" to end this program");
			} else if (!strncmp(command, "exit", MAX_COMMAND_LENGTH)) {
				break;
			} else {
				command[MAX_COMMAND_LENGTH-1]=0;
				printf("%s is not recognized as an internal "
				       "command.\n", command);
			}
		}
	}
	puts("Bye.");
	return 0;
}

/**
 * In this function we feed aclib with possible completions of commands, it will offer.
 */
void setup()
{
	/* add internal commands as possible completions */
	assert(!completion_exists("help"));
	init_completion("help");
	init_completion("history");
	init_completion("exit");
	assert(completion_exists("history"));

	/* add some random bullshit as possible completions - in real life program
	 * these would be possibly legal parameters to said commands.
	 */
	assert(!completion_exists("alpha.omega"));
	init_completion("alpha.omega");
	init_completion("alpha.zeta");
	init_completion("beta");
	init_completion("gamma");
	init_completion("gamma.al-z3");
	init_completion("gamma.alpha");
	init_completion("gamma.alpha.epsilon[22]");
	init_completion("gamma.alpha.epsilon[20].abc");
	init_completion("gamma.alpha.epsilon[20].def");
	init_completion("gamma.alpha.epsilon[20].gdd");
	init_completion("gamma.alpha.epsilon[20].ddds");
	init_completion("gamma.alpha.epsilon[20].aaa");
	init_completion("gamma.alpha.epsilon[20].vv");
	init_completion("gamma.alpha.epsilon[20].aa");
	init_completion("gamma.alpha.epsilon[20].ee");
	init_completion("gamma.alpha.epsilon[20].tttt");
	init_completion("gamma.alpha.epsilon[21]");
	init_completion("zeta");
	/* after adding completions, we need to sort them so that aclib will set up
	 * its internal structures. If during program runtime we add more completions,
	 * we need to call init_completions() once again.
	 */
	init_completions();
}
