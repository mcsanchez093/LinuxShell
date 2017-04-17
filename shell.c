/*
Scott Rainville
Sean Cashman
Dolan Metcalf
CSI230
Shell Source Code

I certify that this is entirely my own work, except where I have
given fully - documented references to the work of others. I
understand the definition and consequences of plagiarism and
acknowledge that the assessor of this assignment may, for the purpose
of assessing this assignment :

-Reproduce this assignment and provide
a copy to another member of academic staff; and / or -Communicate a
opy of this assignment to a plagiarism checking service (which may
then retain a copy of this assignment on its database for the purpose
of future plagiarism checking)
*/

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char arg[LINE_MAX];
	int i, x, exit=0,numargs;
	char *cwd, dirbuff[PATH_MAX +1];
	char *initcwd, initdirbuff[PATH_MAX +1];

	initcwd = getcwd(initdirbuff, PATH_MAX+1); //get the initial current wording dir - AKA where this exec resides.

	do
	{
		cwd = getcwd(dirbuff, PATH_MAX+1);

		numargs=1;
		printf("LameShell:%s$ ", dirbuff);
		fgets(arg, LINE_MAX, stdin);
		if (!strncmp("exit\n", arg, strlen(arg))) {break;}

		for (i=0; i<=strlen(arg); i++)
		{
			if (arg[i]==' ')
			{
				if (arg[i-1] != ' ')
					numargs++;
			}
		}

		char *args[numargs];
		const char s[2] = " ";
		args[0] = strtok(arg, s);
		if (numargs==1) //if this is the only arg...
			args[0][strlen(args[0])-1] = '\0'; //strip the line break char from the end of the arg

		for (x=1; x<numargs; x++)
		{
			args[x] = strtok(NULL, s);
			if (x==numargs-1) //if it's the last arg we're assessing...
				args[x][strlen(args[x])-1] = '\0'; //strip the line break char from the end of the arg
		}

		//we now have an array of pointers, "args", essentially the same as argv[] would be, and numargs [argc]
		exit=scaninput(numargs, args, initcwd);

	} while (!exit);

	return 0;
}

// This function scans the args the user inputted and does stuff with them. Built in functions, like cd, go here.
int scaninput(int numargs, char *args[], char *initcwd)
{
	args[numargs] = NULL;

	if (strlen(args[0]) == 0)
		return 0;

	if (!strncmp("exit\n", args[0], strlen(args[0])))
		if (!strncmp("exit\n", args[0], 4))
			return 1;

	if (!strncmp(args[0], "cd", strlen(args[0])))
	{
		if ((chdir(args[1])) != NULL)
			perror("cd");
		return 0;
	}

	//at this point, the command has been intercepted if it's exit, nothing, or call to built-in function
	
	char cmdloca[255];
	strcpy(cmdloca, initcwd);

	if (!strncmp(args[0], "ls", strlen(args[0])))
	{
		if (!strncmp(args[0], "ls", 2))
			strcat(cmdloca, "/ls");
	}
	else if (!strncmp(args[0], "groups", strlen(args[0])))
	{
		if (!strncmp(args[0], "groups", 6))
			strcat(cmdloca, "/groups");
	}
	else if (!strncmp(args[0], "cp", strlen(args[0])))
	{
		if (!strncmp(args[0], "cp", 2))
			strcat(cmdloca, "/cp");
	}

	else
	{
		FILE *fp;
		char cmd[255]="/usr/bin/which ";
		char path[1035];

		strcat(cmd, args[0]);
		fp = popen(cmd, "r");

		if (fgets(path, sizeof(path)-1, fp) != NULL)
		{
			path[strlen(path)-1] = '\0';
			strcpy(cmdloca, path);
		}
		else
		{
			printf("%s: command not found\n", args[0]);
			return 0;
		}
		pclose(fp);
	}

	int pid=fork();
	if (pid==0)
	{
		execv(cmdloca, args);
		perror("exec2: execv failed");
		exit(EXIT_SUCCESS);
	}
	else
	{
		int status;
		waitpid(pid, &status, 0);
	}
	return 0;
}

/*
CUSTOM EXECUTABLES ARE TO BE PLACED IN THE SAME DIR AS THE SHELL PROGRAM
*/