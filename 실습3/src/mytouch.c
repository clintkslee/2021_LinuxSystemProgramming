#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	struct stat s;
	if(argc!=2)
	{
		fprintf(stderr, "usage: %s (file name)\n", argv[0]);
		exit(1);
	}
	if(access(argv[1], F_OK)!=0)
	{
		int fd;
		printf("No such file or directory. Make new file with permission 664.\n");
		fd=open(argv[1], O_CREAT|O_EXCL|O_RDWR, 0664);
		fstat(fd, &s);
		printf("size of new file is %ld\n.:", s.st_size);
		exit(1);
	}
	if(stat(argv[1], &s)<0)
	{
		printf("stat() failed.\n");
		exit(1);
	}
	printf("FILENAME : %s\n", argv[1]);
	printf("atime : %ld %.12s\n", s.st_atime, ctime(&(s.st_atime))+4);
	printf("mtime : %ld %.12s\n", s.st_mtime, ctime(&(s.st_mtime))+4);
	printf("ctime : %ld %.12s\n", s.st_ctime, ctime(&(s.st_ctime))+4);
	
	time(&(s.st_atime));
	time(&(s.st_mtime));
	time(&(s.st_ctime));

	printf("touched!\n");
	printf("atime : %ld %.12s\n", s.st_atime, ctime(&(s.st_atime))+4);
	printf("mtime : %ld %.12s\n", s.st_mtime, ctime(&(s.st_mtime))+4);
	printf("ctime : %ld %.12s\n", s.st_ctime, ctime(&(s.st_ctime))+4);
	
	return 0;
}