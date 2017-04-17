#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

int copyfile(char *src, char *dst)
{
	FILE *fpsrc, *fpdst;
	char c;

	if ((fpsrc = fopen(src, "rb")) == NULL)
	{
		perror("cp: couldn't read file");
		return 1;
	}
	if ((fpdst = fopen(dst, "wb")) == NULL)
	{
		perror("cp: couldn't create new file");
		return 1;
	}

	size_t n, m;	//this block of code from https://stackoverflow.com/questions/5263018/copying-binary-files
	unsigned char buff[8192];
	do {
    	n = fread(buff, 1, sizeof buff, fpsrc);
    	if (n) m = fwrite(buff, 1, n, fpdst);
    	else   m = 0;
	} while ((n > 0) && (n == m));
	if (m) perror("copy");
	
	fclose(fpsrc);
	fclose(fpdst);
	return 0;
}

int main (int argc, char *argv[])
{
	int rparam=0;
	char *src, *dst;

	if ((strncmp(argv[1], "-r", 2)) == 0)
	{
		if (argc>4) {printf("Too Many Arguments Provided\n"); return 1;}
		rparam=1;
		src=argv[2];
		dst=argv[3];
	}
	else
	{
		if (argc>3) {printf("Too Many Arguments Provided\n"); return 1;}
		src=argv[1];
		dst=argv[2];
	}

	if (access(src, F_OK) == -1)
	{
		printf("cp: cannot access '%s': No such file or directory\n", src); return 1;
	}

	DIR *dir;
	if ((dir=opendir(src)) == NULL) //if keyword doesn't open as dir, must be a file
	{
		copyfile(src, dst); //should probably assess the return value of this
	}
	else //if it is a dir
	{
		if (!rparam) {printf("cp: omitting directory %s\n", src); return 1;} //if it is a dir and -r isn't set, quit
	} 

	return 0;
}