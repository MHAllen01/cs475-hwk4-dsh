/*
 * dsh.h
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */

#define MAXBUF 256

// TODO: Your function prototypes below
char** split(char *str, char *delim);
int CD(char **args, char *cwd);
int CDHome();
void RunMode1(char **args, int background);
int RunMode2(char **args, char *cwd, int background);