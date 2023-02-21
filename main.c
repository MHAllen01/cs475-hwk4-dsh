/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include "dsh.h"
#include <ctype.h>

/**
 * Removes the newline character from a string and replaces is it with the null terminating character
 * 
 * @param	*cmdline	String to replace newline character
*/
void CleanedCmdline(char* cmdline) {
	int pos = strlen(cmdline);
	if (cmdline[pos - 1] == '\n') {
		cmdline[pos - 1] = '\0';
	}
}

int main(int argc, char **argv) {
	char cmdline[MAXBUF]; // stores user input from commmand line
	char **args; // stores args for execv

	while(1) {
		// Prompt for input
		printf("\ndsh> ");
		fgets(cmdline, MAXBUF, stdin);
		CleanedCmdline(cmdline); // Remove \n

		// Break out if "exit"
		if (strcmp(cmdline, "exit") == 0) {
			break;
		}

		// See if cmdline starts with '/', MODE 1
		if (cmdline[0] == '/') {
			// Determine if running in foreground or background
			int background = 0;
			// Found & as the final argument entered
			char *bkgrdCheck = strchr(cmdline, '&');
			if (bkgrdCheck != NULL && bkgrdCheck == (cmdline + strlen(cmdline) - 1)) {
				cmdline[strlen(cmdline) - 2] = '\0'; // Remove the ' ' and '&' characters
				background = 1;
			}

			// Get the arguments for execv
			args = split(cmdline, " ");

			// Check and run the process
			RunMode1(args, background);

			// Free args
			for (int i=0; args[i] != NULL; i++) {
				free(args[i]);
				args[i] = NULL;
			}
			free(args);
			args = NULL;
		} else {
			// Command doesn't start with '/', MODE 2
			// Determine if running in foreground or background
			int background = 0;
			// Found & as the final argument entered.
			char *bkgrdCheck = strchr(cmdline, '&');
			if (bkgrdCheck != NULL && bkgrdCheck == (cmdline + strlen(cmdline) - 1)) {
				cmdline[strlen(cmdline) - 2] = '\0'; // Remove the ' ' and '&' characters
				background = 1;
			}

			// Get the arguments for execv
			args = split(cmdline, " ");

			// 1. See if command is in current working directory and is valid
			// 2. Move on to see if anywhere else
			// 2.1 If not, it's not valid, print error
			
			// Get current working directory and store in cwd
			char cwd[MAXBUF];
			getcwd(cwd, MAXBUF);
			
			if (strcmp(args[0], "pwd") != 0 && strcmp(args[0], "cd") != 0) {
				// Concatenate user input
				strcat(cwd, "/");
				strcat(cwd, args[0]);

				//Check and run the process
				int mode2Status = RunMode2(args, cwd, background);
				if (mode2Status == 1) {
					printf("ERROR: %s not found\n", args[0]);
				} 
			} else if (strcmp(args[0], "pwd") == 0) {
				// PWD
				printf("%s\n", cwd);
			} else if (strcmp(args[0], "cd") == 0 && args[1] == NULL) {
				// CD to home directory
				int cdhomeStatus = CDHome();
				if (cdhomeStatus == 1) {
					printf("ERROR: home directory not found\n");
				}
			} else if (strcmp(args[0], "cd") == 0 && args[1] != NULL) {
				// CD to directory
				int cdStatus = CD(args, cwd);
				if (cdStatus == 1) {
					printf("ERROR: %s not found\n", args[1]);
				}
			}
			// Free args
			for (int i=0; args[i] != NULL; i++) {
				free(args[i]);
				args[i] = NULL;
			}
			free(args);
			args = NULL;
		}
	}
	return 0;
}