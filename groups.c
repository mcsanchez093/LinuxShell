/*
Scott Rainville
CSI-230-04
Recreation of UNIX "groups" command
Usage: './groups [usernames]'

Certification of Authenticity :

I certify that this is entirely my own work, except where I have given fully -
documented references to the work of others. I understand the definition
and consequences of plagiarism and acknowledge that the assessor of this assignment
may, for the purpose of assessing this assignment :

-Reproduce this assignment and provide a copy to another member of academic staff; and / or
-Communicate a copy of this assignment to a plagiarism checking service (which may then retain
a copy of this assignment on its database for the purpose of future plagiarism checking)
*/

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char * argv[])
{
	FILE* fp;
	int argnum=1, charnum;
	char *username;

	FILE* userfp;
	char usrline[255];
	int cmp, exists;

	if ((fp = fopen("/etc/group", "r")) == NULL)
	{
		perror("Couldn't open file");
		return 1;
	}

	if (argc < 2)	//if the username arg wasn't specified
	{
		username=getenv("USER");					//Gets the environment variable for USER - basically gets username
		if(username==NULL){return EXIT_FAILURE;}	//If getting username didn't work, exit program
		getgroups(username);
	}

	userfp = fopen("/etc/passwd", "r");											// This all is for determining if the user exists,
	while (argnum<argc)														// By opening up /etc/passwd and seeing if the provided arg is in there for each arg provided.
	{																			// If any of the args doesn't exist in /etc/passwd, exits the program.
		exists=0;																//
		while (fgets(usrline, sizeof(usrline), userfp) != NULL)					//
		{																		//
			charnum=0;															//
			for(; usrline[charnum]!=':' && usrline[charnum]!='\n'; charnum++){}	//counts number of characters before : or endline
			if (strlen(argv[argnum]) == charnum)								//If the arg and the username length from /etc/passwd are the same...
			{																	//		|
				cmp = strncmp(usrline, argv[argnum], charnum);					//		v
				if (cmp == 0)													//compare the username in /etc/passwd to the arg
				{																//
					exists=1;													//
					break;														//If the username has been found, continue on.
				}																//
			}																	//
		}																		//
		if (exists!=1)															//
		{																		//If username doesn't exist, exits program.
			printf("groups: '%s': no such user\n", argv[argnum]);				//
			return 1;															//
		}
		else
		{
			getgroups(argv[argnum]);
		}

		argnum++;
		rewind(userfp);
	}
	fclose(userfp);
}

int getgroups(char *username)
{
	FILE* fp;
	char line[255], group[32];
	int secnum, compare, charnum=0, len, usrcmp, userchar;

	if ((fp = fopen("/etc/group", "r")) == NULL)
	{
		perror("Couldn't open file");
		return 1;
	}
	printf("%s : ", username);
	//if (argnum>1){printf("\n");} //Just print a line break if this isn't the first user supplied

	while (fgets(line, sizeof(line), fp) != NULL) //loops once for each line in the file
	{
	    //printf("Entire Line: %s", line);
	    charnum=0;
	    userchar=0;

	    memset(group, '\0', sizeof(group)); //clears out the 'group' string for each line in the file

	    for (secnum=0; secnum<=3; secnum++) //breaks up the string into four loops, each separated by the colon.
	   	{
	    	for(; line[charnum]!=':' && line[charnum]!='\n'; charnum++){} //just counts char until it hits a : or end
	    	if (secnum==0) //if the first quadrant of the line...
	    	{
	    		strncpy(group, line, charnum); //copies only the group part of line into the group string

	    		usrcmp = strncmp(group, username, strlen(username)); //compares the group string to the arg - this is for the group in the file designated to the user itself w/no listed members.
	    		if ((usrcmp==0) && strlen(username) == strlen(group))
	    		{
	    			printf("%s ", group);
	    		}
	    	}
	    	if (secnum==2){len=charnum;} //returns end of 3rd section so user portion array pointer (*user) knows where to start
			charnum++;
	    }

	    compare=1;
		char *user = line + len +1; //makes a pointer that begins just past where the 3rd quadrant left off, ignoring non username data.
		if (strlen(user) > 1)
		{
			while ((user[userchar-1] != '\0') && (compare != 0))
			{
					compare = strncmp(user, username, strlen(username)); 	//compares the arg to the entire user string from the file.
																				//Only looks at the number of bytes equal to the arg's length.
				if ((compare==0) && (user[userchar-1] == ',' || user[userchar-1] == ':') && (user[strlen(username)+1] == '\0' || user[strlen(username)+1] == ','))//if the comparison above is true and the preceding character is either a : or ,
				{
					printf("%s ", group); //print out the group name from the beginning of the line.
				}
				++user; //moves the 'user' pointer up by one byte to move up one character.
			}
		}
	}
	//rewind(fp); //resets the file pointer to the beginning of the file for >1 arg
	fclose(fp);
	printf("\n");
	return 0;
}
