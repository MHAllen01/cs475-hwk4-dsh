/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include "builtins.h"

//char **history; // Global array that stores the history of user inputs


// TODO: Your function definitions (declarations in dsh.h)


/**
 * Splits a String if the given delimiter is met.
 * Tokenizes each subsiquent String.
 * 
 * @param   *str    A String to tokenize
 * @param   *delim  A delimiter to search for in the input String
 * @return  **char  A 2D array of the tokenized Strings
*/
char** split(char* str, char* delim) {
    int NUMTOKENS = 1; // Number of occurrences of the delimiter in the string

    // Calculate NUMTOKENS
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == *delim) {
            // Delimiter found, increment
            NUMTOKENS++;
        }
    }

    // Initialize the new array of substrings
    char** substrings = (char**)malloc(NUMTOKENS * sizeof(char*));

    // Instantiate each position in the substrings array
    for (int i = 0; i < NUMTOKENS; i++) {
        substrings[i] = (char*)malloc(strlen(str));
    }

    // Put each String into the substring array as an individual token
    char* token = strtok(str, delim);

    int tokCount = 0;

    while (token != NULL) {
        strcpy(substrings[tokCount], token);
        token = strtok(NULL, delim);
        tokCount++;
    }

    // Nullify final element of the substring array
    substrings[NUMTOKENS] = NULL;

    return substrings;
    
    // Free substrings
    for (int i = 0; i < NUMTOKENS; i++) {
        free(substrings[i]);
        substrings[i] = NULL;
    }
    free(substrings);
    substrings = NULL;
}

/**
 * Changes the current working directory of the user based on the directory they want to access
 *
 * @param   **args  String array of arguments to pass into the execv function
 * @param   *cwd    String of the current working directory
 * @return  0       If no errors, otherwise, 1
*/
int CD(char **args, char *cwd) {
    // CD was ran
    // See if args[1] starts with ../
    if (strncmp(args[1], "..", 2) == 0 || strncmp(args[1], "../", 3) == 0) {
        // CD into parent directory

        chdir(args[1]);
        return 0;
    }  else if (args[1] != NULL) {
        // CD into folder

        // Temporary path to check if CD into is valid
        char *temppath = (char*) malloc(strlen(cwd) + strlen(args[1]) + 2);
        strcpy(temppath, cwd);
        strcat(temppath, "/");
        strcat(temppath, args[1]);

        if (access(temppath, F_OK | X_OK) == 0) {
            // Path is correct
            strcpy(cwd, temppath);
            chdir(cwd);

            // Free temppath
            free(temppath);
            // No errors
            return 0;
        } else {
            // Free temppath
            free(temppath);
            temppath = NULL;
            // Error
            return 1;
        }
    }
    // Error
    return 1;
}

/**
 * Changes the current working directory to the HOME directory
 * 
 * @return  0   If no errors, otherwise, 1
*/
int CDHome() {
    // CD with no path specified
    char *homevar = getenv("HOME");
    if (homevar != NULL) {
        chdir(homevar);
        // No errors
        return 0;
    } else {
        return 1;
    }
}

/**
 * Checks the first argument, the command, and runs if valid.
 * Also determines if the process should run in the background or foreground.
 * Runs the search and process for Mode 1 (path is provided)
 * 
 * @param   **args      String array of arguments to pass into the execv function
 * @param   background  Integer value if the process should run background or foreground
 * @return  0           If no errors, otherwise, 1
*/
void RunMode1(char **args, int background) {
	// Directory path is valid
	if (access(args[0], F_OK | X_OK) == 0) {
		// File exists and is executable
			
		pid_t pid = fork();
		if (pid != 0) {
            // Parent
            if (background == 0) {
                // Run in background
                wait(NULL);
            }
        } else if (pid == 0) {
            // Child
            execv(args[0], args);

            // If execv returns, there was an error
            printf("ERROR: %s not found\n", args[0]);
            exit(1);
        }
	} else {
		// File doesn't exist
		printf("\n%s: no such file or directory\n", args[0]);
	}
}

/**
 * Checks the first argument, the command, and runs if valid.
 * Also determines if the process should run in the background or foreground.
 * Runs the search and process for Mode 2 (path is not provided)
 * 
 * @param   **args      String array of arguments to pass into the execv function
 * @param   *cwd        String of the current working directory
 * @param   background  Integer value if the process should run background or foreground
*/
int RunMode2(char **args, char *cwd, int background) {
    if (access(args[0], F_OK | X_OK) == 0) {
		// File exists and is executable
		pid_t pid = fork();
		if (pid != 0) {
			// Parent process
			if (background == 0) {
				// Child executes in background
				wait(NULL);
			} 
		} else {
			// Child process
			execv(args[0], args);
					
			// Error case:
			printf("ERROR: %s not found\n", args[0]);
			exit(1);
		}
        // No errors
        return 0;
	} else {
	    // Go search elsewhere for the command
		// Get all PATH paths
		char *pathvar = getenv("PATH");
        char *tempstr = (char*) malloc(MAXBUF);

        strcpy(tempstr, pathvar);

		// Split up all paths into a String array similar to args
		char **paths = split(tempstr, ":");

		// Concatenate the user input in args[0] to all paths that were delimited
		for (int i=0; paths[i] != NULL; i++) {

            // Concatenate the separator and the filename to the buffer
            strcat(paths[i], "/");
            strcat(paths[i], args[0]);

            if(access(paths[i], F_OK | X_OK) == 0) {
                // File found, run it
                pid_t pid = fork();
		        if (pid != 0) {
                    // Parent
                    if (background == 0) {
                        // Run in backgroundground
                        wait(NULL);
                    }
                } else if (pid == 0) {
                    // Child
                    execv(paths[i], args);

                    // If execv returns, there was an error
                    printf("ERROR: %s not found\n", args[0]);
                    exit(1);
                } else {
                    break;
                }
                // Free paths
                free(tempstr);
                tempstr = NULL;
                for (int i=0; paths[i] != NULL; i++) {
                    free(paths[i]);
                    paths[i] = NULL;
                }
                free(paths);
                paths = NULL;
                pathvar = NULL;


                // No errors
                return 0;
            }
		}
        // Free paths
        for (int i=0; paths[i] != NULL; i++) {
            free(paths[i]);
            paths[i] = NULL;
        }
        free(paths);
        paths = NULL;
        // Error
        return 1;
	}
}