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
#include "auto_complete.h"

#define MAX_COMMAND_LENGTH 20

#define INTASTEXT(N) # N
void setup();

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
				puts("You're left alone");
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
	init_completion("alpha.omega");
	init_completion("alpha.zeta");
	init_completion("beta");
	init_completion("help");
	init_completion("history");
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
	init_completion("/dev/sda1");
	init_completion("/dev/sda2");
	init_completion("/dev/sda5");
	init_completion("/dev/sdc");
	init_completion("/dev/sdb");
	init_completion("/dev/disk/by-id");
	init_completion("/dev/disk/by-id/lvm-pv-uuid-Lw8SMJ-BLdJ-Spaf-8mwY-dqMJ-EsP0-7vxnx4");
	init_completion("/dev/disk/by-id/dm-uuid-LVM-N0r520mq8NpO45DqUIaTnXB4r58QRBZqrqi1KrLjnlJpgFyeSkYaaOrOcXKFcg3x");
	init_completion("/dev/disk/by-id/dm-name-debian--lenovo--vg-home");
	init_completion("/dev/disk/by-id/dm-uuid-LVM-N0r520mq8NpO45DqUIaTnXB4r58QRBZqVkQTgBpn50Drtlk8NnSANLl0XYpbyv2G");
	init_completion("/dev/disk/by-id/dm-name-debian--lenovo--vg-swap_1");
	init_completion("/dev/disk/by-id/dm-uuid-LVM-N0r520mq8NpO45DqUIaTnXB4r58QRBZqBRuUVx0gB4Enid4zuoa1R6RY1YTf7A9H");
	init_completion("/dev/disk/by-id/dm-name-debian--lenovo--vg-root");
	init_completion("/dev/disk/by-id/dm-uuid-CRYPT-LUKS1-d18e46606caa4a86a33b1b1064aae43c-sda5_crypt");
	init_completion("/dev/disk/by-id/dm-name-sda5_crypt");
	init_completion("/dev/disk/by-id/ata-PLDS_DVD-RW_DS8A9SH_O10A11857L1CG3B2D942");
	init_completion("/dev/disk/by-id/wwn-0x50025388a03809c1-part1");
	init_completion("/dev/disk/by-id/ata-Samsung_SSD_840_EVO_120GB_S1D5NSAF491969M-part1");
	init_completion("/dev/disk/by-id/wwn-0x50025388a03809c1-part5");
	init_completion("/dev/disk/by-id/ata-Samsung_SSD_840_EVO_120GB_S1D5NSAF491969M-part5");
	init_completion("/dev/disk/by-id/wwn-0x50025388a03809c1-part2");
	init_completion("/dev/disk/by-id/ata-Samsung_SSD_840_EVO_120GB_S1D5NSAF491969M-part2");
	init_completion("/dev/disk/by-id/wwn-0x50025388a03809c1");
	init_completion("/dev/disk/by-id/ata-Samsung_SSD_840_EVO_120GB_S1D5NSAF491969M");
	init_completion("/dev/disk/by-uuid");
	init_completion("/dev/disk/by-uuid/764abe50-58b5-4384-97b3-24aacfce690e");
	init_completion("/dev/disk/by-uuid/47c64afe-6064-40d2-a7b3-df7ed01ac9f8");
	init_completion("/dev/disk/by-uuid/e8c3db6b-580a-4eea-bad4-d27a32a6c920");
	init_completion("/dev/disk/by-uuid/9f865755-3369-42b0-ab80-ec930dfd02eb");
	init_completion("/dev/disk/by-uuid/d18e4660-6caa-4a86-a33b-1b1064aae43c");
	init_completion("char");
	init_completion("uint8_t");
	init_completion("uint32_t");
	init_completion("uint64_t");
	init_completion("int");
	init_completion("char*");
	/* after adding completions, we need to sort them so that aclib will set up
	 * its internal structures. If during program runtime we add more completions,
	 * we need to call init_completions() once again.
	 */
	init_completions();
}
