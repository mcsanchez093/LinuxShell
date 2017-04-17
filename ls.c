#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

int get_int_len(int value){
  int l=1;
  while(value>9){ l++; value/=10; }
  return l;
}

int getSizeLength(char *dirpath, int lnkorsize) //gets the optimal length for the size column
{
	DIR *dir;
	struct dirent *dp;
	int length=0;

	struct stat fileStat;
	struct group *grp;
	struct passwd *pwd;
	char fullname[255];


	dir=opendir(dirpath);
	while ((dp = readdir(dir)) != NULL)
	{
		strcpy(fullname, dirpath); //copy dir into fullname string
		strcat(fullname, "/");
		strcat(fullname, dp->d_name); //build a string of the full path to the file
		stat(fullname, &fileStat);

		if (lnkorsize == 1)
		{
			if (get_int_len(fileStat.st_size) > length)
			{
				length=(get_int_len(fileStat.st_size));
			}
		}
		else if (lnkorsize == 0)
		{
			if (get_int_len(fileStat.st_nlink) > length)
			{
				length=(get_int_len(fileStat.st_nlink));
			}
		}
		
	}

	closedir(dir);

	return length;
}

int getyear(void)
{
	time_t time_raw_format;
    struct tm * ptr_time;
    static char timebuffer[5];
    int i;

    time(&time_raw_format);
    ptr_time = localtime(&time_raw_format);
    strftime(timebuffer, sizeof(timebuffer),"%Y",ptr_time);

	sscanf(timebuffer, "%d", &i);

    return (i);
}

int statprint(char *fullpath, char *path, int sizelink, int sizelen)
{
	struct stat fileStat;
	struct group *grp;
	struct passwd *pwd;
	struct tm lt;
	time_t t = fileStat.st_mtime;
	char timebuf[100];

	stat(fullpath, &fileStat);

	//print file permissions
	printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
	printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");

    if (sizelink==0)
    {
    	printf("%d ", fileStat.st_nlink);
    }
    else
    {
    	printf("%*d ", sizelink, fileStat.st_nlink);
    }
    

	pwd = getpwuid(fileStat.st_uid);
    printf("%5s ", pwd->pw_name);

	grp = getgrgid(fileStat.st_gid);
    printf("%5s ", grp->gr_name);
    
    if (sizelen==0)
    {
    	printf("%d ", fileStat.st_size);
    }
    else
    {
    	printf("%*d ", sizelen, fileStat.st_size);
    }
	
	localtime_r(&t, &lt);
	strftime(timebuf, sizeof(timebuf), "%b %d %k:%M", &lt);
	printf("%s ", timebuf);

	printf("%s", path);

	printf("\n");


    return 0;
}

int main(int argc, char * argv[])
{
	int argnum, parama=0, paraml=0, compare=1, keyfound=0, badkey=0, i;
	char *keyword;

	for (argnum=1; argnum<argc; argnum++)
	{
		compare=strncmp(argv[argnum], "-", 1);
		if (compare==0)
		{
			for (i=0; i<=strlen(argv[argnum]); i++)
			{
				if (argv[argnum][i] == 'a') {parama=1;}
				else if (argv[argnum][i] == 'l') {paraml=1;}
			}
		}
		else //if it isn't a setting arg
		{
			if (access(argv[argnum], F_OK) == -1)
			{
				printf("ls: cannot access '%s': No such file or directory\n", argv[argnum]);
				badkey=1;
				continue;
			}
			keyword=argv[argnum];
			keyfound=1;
		}
	}
	if ((keyfound==0) && (badkey != 1)) //If there was no arg relating to a dir/file name to grab info on, default to current dir
	{
		keyword=".";
	}
	else if ((keyfound==0) && (badkey==1))
	{
		return 1;
	}


	DIR *dir;

	if ((dir=opendir(keyword)) == NULL) //if keyword doesn't open as dir, treat it as a file
	{
		if (paraml==0)
		{
			printf("%s", keyword);
		}
		else
		{
			statprint(keyword, keyword, 0, 0);
		}

	}
	else //if keyword opens as a dir
	{
		char fullname[255];
		struct dirent *dp;
		int numitems=0;

		while ((dp = readdir(dir)) != NULL)
		{
			if ((parama==0) && (dp->d_name[0] == '.')) // if the -a param isn't set, skip entries starting w/ "."
			{}
			else {numitems++;}
		} //see how many items are in the dir

		rewinddir(dir); //rewind dir pointer to re-loop and actually store info

		char list[numitems][255];

		if(paraml==1) {printf("total %d\n", numitems);} //print the total line if in -l mode, before we loop through each dir/file entry

		int count=0;
		while ((dp = readdir(dir)) != NULL) //for each file/dir entry in the keyword dir...
		{

			if ((parama==0) && (dp->d_name[0] == '.')) // if the -a param isn't set, skip entries starting w/ "."
			{}
			else
			{
				strcpy(list[count], dp->d_name);
				count++;
			}
		}
		closedir(dir);

		
		//SORT ARRAY OF STRINGS HERE ('list')
		char temp[255];
		int j, sorti;

		
		for (sorti = 1; sorti < numitems; sorti++)
		{
			for (j = 1; j < numitems; j++)
			{
				if (strcasecmp(list[j - 1], list[j]) > 0) //compare two elements starting at j ptr
				{
					strcpy(temp, list[j - 1]);
					strcpy(list[j - 1], list[j]);	//if backwards, swap places!
					strcpy(list[j], temp);
				}
			}
		}

		//iterate through the list of filenames, print/statprint each
		int z;
		for (z=0; z<(count); z++)
		{
			strcpy(fullname, keyword); //copy dir into fullname string
			strcat(fullname, "/");
			strcat(fullname, list[z]); //build a string of the full path to the file
			if (paraml==1)
			{
				statprint(fullname, list[z], getSizeLength(keyword, 0), getSizeLength(keyword, 1));
			}
			if (paraml==0)
			{
				printf("%s ", list[z]);
			}
		}
	}
	if (paraml==0) {printf("\n");} //print line break at end if no -l

    return 0;
}