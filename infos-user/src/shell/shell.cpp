/* SPDX-License-Identifier: MIT */

#include <infos.h>

template <class T>
static void memcpy(const T* from, T* to, int size){
	for (int i=0;i<size;++i)
		to[i]=from[i];
}
template <class T>
static void strcpy(const T* from, T* to, int size = 1<<sizeof(T)-1){
	for (int i=0;i<size && *to; ++i)
		to[i]=from[i];
}
constexpr int MaxHistoryNumber = 15;
constexpr char UpArrow = 17;
constexpr char DownArrow = 18;
static char commandHistory[MaxHistoryNumber][128];
static int historyIndex = -1;
static int historyNumber = 0;

static void run_command(const char *cmd)
{
	strcpy<char>(cmd, commandHistory[(++historyIndex==15)?0:historyIndex], 128);
	++historyNumber;

	char prog[64];
	int n = 0;
	while (*cmd && *cmd != ' ' && n < 63) {
		prog[n++] = *cmd++;
	}
	
	prog[n] = 0;
	if (n<63-5 && prog[0]!='/'){
		for (int j=n;j>=0;--j)
			prog[j+5]=prog[j];
		const char* usrPath="/usr/";
		memcpy(usrPath, prog, 5);
	}
	
	if (*cmd) cmd++;
	
	HPROC pcmd = exec(prog, cmd);
	if (is_error(pcmd)) {
		printf("error: unable to run command '%s'\n", cmd);
	} else {
		wait_proc(pcmd);
	}
}

int main(const char *cmdline)
{
	printf("This is the InfOS shell.  Path resolution is not-yet-implemented, so you\n"
			"must type the command exactly, e.g. try typing: /usr/ls.\n\n");
	
	printf("Use the cat program to view the README: /usr/cat /usr/docs/README\n\n");
	
	while (true) {
		printf("> ");

		char command_buffer[128];
		int n = 0;
		int checkHistoryIndex = 0;

		while (n < 127) {
			char c = getch();
			if (c == 0) continue;
			if (c == '\n') break;
			if (c == '\b') {
				if (n > 0) {
					command_buffer[--n] = 0;
					printf("\b");
				}
			} else {
				command_buffer[n++] = c;
				printf("%c", c);
			}
			if (c == UpArrow) {
				if (!historyNumber) continue;
				if (++checkHistoryIndex <= historyNumber) {
					if (checkHistoryIndex == MaxHistoryNumber)
						--checkHistoryIndex;
					int index = historyIndex - checkHistoryIndex;
					if (index >= MaxHistoryNumber)
						index -= MaxHistoryNumber;
					printf("%s", commandHistory[index]);
					// infos::kernel::syslog::messagef(infos::kernel::LogLevel::INFO, commandHistory[index]);
					strcpy<char>(commandHistory[index], command_buffer, 128);
				}
			}
			else if (c == DownArrow) {
				if (!historyNumber) continue;
				if (--checkHistoryIndex >= 0) {
					if (!checkHistoryIndex)
						++checkHistoryIndex;
					int index = historyIndex - checkHistoryIndex;
					if (index >= MaxHistoryNumber)
						index -= MaxHistoryNumber;
					printf("%s", commandHistory[index]);
					strcpy<char>(commandHistory[index], command_buffer, 128);
				}
			}
		}

		printf("\n");
		if (n == 0) continue;
		
		command_buffer[n] = 0;
		
		if (strcmp("exit", command_buffer) == 0) break;
		run_command(command_buffer);
	}
	
	return 0;
}
