#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define INFINITE 1000000000
int main(int argc, char** argv)
{
	int timelimit=INFINITE;
	pid_t pid;
	int index;
	if(argc==2)
		timelimit=atoi(argv[1]);
	while(timelimit)
	{
		if((pid=fork())==0)
			execl("/bin/date", "date", (char*)0);
		if(timelimit<=5)
			printf("PROCESS WILL BE KILLED IN %d...\n", timelimit);
		timelimit--;
		sleep(1);
	}
	raise(SIGKILL);
	return 0;
}
