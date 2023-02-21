typedef enum
{
	CMD_EXT,	// external command
	CMD_CD,		// cd
	CMD_PWD,	// pwd
	CMD_HIST, // history
	CMD_EXIT,	// exit
	CMD_ECHO
} cmd_t;

/**
 * Checks whether given command string is a builtin
 * @param cmd	 A command string, such as "/bin/ls" or "exit"
 * @return CMD_EXT if the given command string is an external commamd;
 * Otherwise, return the build-in code (from the constants defined above)
 */
cmd_t chkBuiltin(char *cmd);
