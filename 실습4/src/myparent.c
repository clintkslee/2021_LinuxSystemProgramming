#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
int main(int argc, char** argv)
{
	int numofchild=atoi(argv[1]);
	pid_t pid, *pa;
	int index, wpid, status;
	pa=(pid_t*)malloc(sizeof(pid_t)*(numofchild+1));
	for(int i=1; i<=numofchild; i++)
	{
		pid=fork();
		if(pid==0) //child
		{
			index=i;
			break;
		}
		else { //parent
			pa[i]=pid;
		}
	}
	if(pid!=0) //parent
		printf("Created %d child.\n", numofchild);
	
	if(pid==0) //child
	{
		while(1)
		{
			sleep(5);
			printf("I'm Child %d PID = %d\n", index, getpid());
		}
	}
	else //parent
	{
		sleep(4);
		while(numofchild)
		{
			sleep(1);
			kill(pa[numofchild], SIGKILL);
			numofchild--;
		}
		printf(">>> All child process has been killed. <<<\n");
		printf("GOOD BYE.\n");
		raise(SIGKILL);
	}
	return 0;
}

