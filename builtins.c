#include <string.h>
#include "builtins.h"

/**
 * Checks whether given command string is a builtin
 * @param cmd
 * @return positive code if true, CMD_EXT otherwise
 */
cmd_t chkBuiltin(char *cmd) {
	if (!strcmp(cmd, "cd"))
		return CMD_CD;
	else if (!strcmp(cmd, "exit"))
		return CMD_EXIT;
	else if (!strcmp(cmd, "pwd"))
		return CMD_PWD;
	else if (!strcmp(cmd, "echo"))
		return CMD_ECHO;
	return CMD_EXT;
}
