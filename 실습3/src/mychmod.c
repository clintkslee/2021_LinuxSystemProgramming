#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv)
{
	int p; //vals for save permission
	if(argc!=3)
	{
		fprintf(stderr, "usage: %s (3 digit octal number) (file name)\n", argv[0]);
		exit(1);
	}
	if(access(argv[2], F_OK)!=0)
	{
		printf("No such file or directory.\n");
		exit(1);
	}
	p=strtol(argv[1], NULL, 8);
	if(chmod(argv[2], p)!=0)
	{
		printf("[%s %s %s] fail.\n", argv[0], argv[1], argv[2]);
		exit(1);
	}
	printf("[%s %s %s] success.\n", argv[0], argv[1], argv[2]);

	return 0;
}