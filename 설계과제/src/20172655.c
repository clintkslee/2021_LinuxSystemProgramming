////20172655 lee kang san
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>

typedef struct _data {
	pthread_t *tid; //for index
	int worker;
	int m; //row
	int n; //column
	int **mat; //current state
	int **mat2; //next state
}DATA;

void* mythread(void* data);
int calonecell(int** mat, int i, int j, int m, int n); //다음 세대에 해당 칸 살았나 죽었나 판단
void buildmat(FILE* fp, int** mat, int m, int n); //파일에서 불러오기
void freemat(int** mat, int m); //매트릭스 메모리 해제

int main(int argc, char** argv)
{
	int gen=1;
	FILE* fp=fopen(argv[1], "r");
	FILE* fp2;
	FILE* fp3;

	char ch;
	int** mat, **mat2;
	char* buf=(char*)malloc(sizeof(char)*20001);
	int m=0, n=0;
	int userinput, worker;
	char filename[200];
	char tempname[20]={0,};
	clock_t start, end;
	pid_t pid, *pa, status;
	int cnt=0;
	int k, check;
	pthread_t *tid=NULL;

	while((ch=fgetc(fp))!='\n') { //열 n
		if(ch=='0' || ch=='1')
			n++;
	}
	fseek(fp, 0, SEEK_SET);
	while(fgets(buf, 20000, fp)!=NULL){ //행 m
		if(buf[0]=='\n' || strlen(buf)==0)
			break;
		m++;
	}
	fseek(fp, 0, SEEK_SET);
	free(buf);

	mat=(int**)malloc(sizeof(int*)*m); //파일에서 불러온 내용 저장할 매트릭스
	mat2=(int**)malloc(sizeof(int*)*m); //multi thread에서 다음 세대 저장용 매트릭스
	for(int i=0; i<m; i++) {
		mat[i]=(int*)malloc(sizeof(int)*n);
		mat2[i]=(int*)malloc(sizeof(int)*n);
	}

	buildmat(fp, mat, m, n);
	
	printf("=============================\n");
	printf("[ generation 0 ] %d X %d Matrix\n", m, n);
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
			printf("%d ", mat[i][j]);
		printf("\n");
	}
	fclose(fp);
	printf("=============================\n");

	sprintf(filename, "input.matrix");
	while(gen)
	{
		start = clock(); //시간 측정 시작
		fp=fopen(filename, "r"); //입력 파일 읽기
		buildmat(fp, mat, m, n);
		printf("[ generation %d ]\n", gen);
		printf(">>> Select next execution <<<\n");
		printf("PRESS 1 ... program termination\n");
		printf("PRESS 2 ... sequential execution\n");
		printf("PRESS 3 ... parallel execution(multi process)\n");
		printf("PRESS 4 ... parallel execution(multi thread)\n");
		printf("INPUT : ");
		check=scanf("%d", &userinput);
		while(check!=1 || (1>userinput || userinput>4)) {
			while(getchar()!='\n');
			printf("Wrong input.\n");
			printf("INPUT : ");
			check=scanf("%d", &userinput);
		}
		if(userinput==1) { //terminate process
			printf("(1) program termination\n");
			break;
		}
		switch (userinput) {
			case 2: //single process and thread
				printf("(2) sequential execution\n");
				sprintf(filename, "gen_%d.matrix", gen);
				fp2=fopen(filename, "wt");
				for(int i=0; i<m; i++)
				{
					for(int j=0; j<n; j++)
					{
						fprintf(fp2, "%d ", calonecell(mat, i, j, m, n));
					}
					fprintf(fp2, "\n");
				}
				break;
			case 3: //multi process
				printf("(3) parallel execution with multi process\n");
				sprintf(filename, "gen_%d.matrix", gen);
				fp2=fopen(filename, "wt");
				printf("How many child process? : ");
				check=scanf("%d", &worker);
				while(check!=1 || (1>worker)) {
					while(getchar()!='\n');
					printf("Wrong input.\n");
					printf("How many child process? : ");
					check=scanf("%d", &worker);
				}
				pa=(pid_t*)malloc(sizeof(pid_t)*worker);

				for(int i=0; i<worker; i++) {
					pid=fork();
					if(pid==0) {//child
					//	printf("child process %d [PID %d] started. (PPID %d)\n", i, getpid(), getppid());
						k=i;
						break;
					}
					else {//parent
						pa[i]=pid; //fork()시 child process가 return한 pid 수집
					}
				}
				if(pid==0) { //child
					for(int i=0; i<m; i++) {
						if(i%worker == k)
						{
							sprintf(tempname, "temp%d", i);
							fp3=fopen(tempname, "wt");
							for(int j=0; j<n; j++)
								fprintf(fp3, "%d ", calonecell(mat, i, j, m, n));
							fclose(fp3);
						}
					}
					exit(1);
				}
				int wpid;
				while((wpid=wait(&status)) > 0);
				int temp;
				for(int i=0; i<m; i++) {
					sprintf(tempname, "temp%d", i);
					fp3=fopen(tempname, "rt");
					while(fscanf(fp3, "%d", &temp)!=EOF)
						fprintf(fp2, "%d ", temp);
					fprintf(fp2, "\n");
					fclose(fp3);
					remove(tempname);
				}

				printf(">> Child process ids <<\n");

				for(int i=0; i<worker; i++) //사용한 pid 출력
					printf("Child process [%d] : PID = %d\n", i, pa[i]);
				break;
			case 4: //multi thread
				printf("(4) parallel execution with multi thread\n");
				sprintf(filename, "gen_%d.matrix", gen);
				fp2=fopen(filename, "wt");
				printf("How mant thread? : ");
				check=scanf("%d", &worker);
				while(check!=1 || (1>worker)) {
					while(getchar()!='\n');
					printf("Wrong input.\n");
					printf("How mant thread? : ");
					check=scanf("%d", &worker);
				}
				
				DATA* d=(DATA*)malloc(sizeof(DATA));
				tid=(pthread_t*)malloc(sizeof(pthread_t)*worker);
				for(int i=0; i<worker; i++) {
					int idx=i;
					d->tid=tid;
					d->worker=worker;
					d->m=m;
					d->n=n;
					d->mat=mat;
					d->mat2=mat2;
					pthread_create(&(tid[i]), NULL, mythread, (void*)d);
				}
				for(int i=0; i<worker; i++)
					pthread_join(tid[i], (void*)&status);
				//
				for(int i=0; i<m; i++) {
					for(int j=0; j<n; j++) {
						fprintf(fp2, "%d ", mat2[i][j]);
					}
					fprintf(fp2, "\n");
				}
				break;
		}
		end = clock(); //시간측정 종료
		printf("Time : %ld ms\n", (end-start)); //ms단위 수행시간 출력
		gen++;
		fclose(fp);
		fclose(fp2);
		printf("=============================\n");
	} //end while
	freemat(mat, m);
	freemat(mat2, m);
	printf("=============================\n");
	return 0;
}
void* mythread(void* data)
{
	pthread_t* tid;
	int k; //index for mod task
	int worker, m, n, **mat, **mat2;
	tid=((DATA*)data)->tid;
	worker=((DATA*)data)->worker;
	m=((DATA*)data)->m;
	n=((DATA*)data)->n;
	mat=((DATA*)data)->mat;
	mat2=((DATA*)data)->mat2;
	for(int i=0; i<worker; i++) {
		if(tid[i]==pthread_self())
			k=i;
	}	
	for(int i=0; i<m; i++) {
		if(i%worker == k) {
			for(int j=0; j<n; j++) {
				mat2[i][j]=calonecell(mat, i, j, m, n);
			}
		}
	}	
	printf("THREAD[%d] tid=%lu is finished.\n", k, tid[k]);
	return NULL;
}

int calonecell(int** mat, int i, int j, int m, int n) //다음 세대에 해당 칸 살았나 죽었나 판단
{
	int state=mat[i][j];
	int zero=0;
	int one=0;
	int nextstate=0;
	int a, b;

	for(a=i-1; a<=i+1; a++) {
		for(b=j-1; b<=j+1; b++) {
			if(0<=a && a<=m-1 && 0<=b && b<=n-1)
			{
				if(mat[a][b]==1)
					one++;
			}
		}
	}
	if(state==1) one--;
	zero=8-one;
	if(state==1) {
		if(3<=one && one<=6)
			nextstate=1;
		else
			nextstate=0;
	}	
	else if(state==0) {
		if(one==4)
			nextstate=1;
		else
			nextstate=0;
	}
	
//	printf("mat[%d][%d] cur:%d zero:%d one:%d  next:%d\n", i, j, state, zero, one, nextstate);
	
	return nextstate;
}

void buildmat(FILE* fp, int** mat, int m, int n) //파일에서 불러오기
{
	for(int i=0; i<m; i++) {
		for(int j=0; j<n; j++) {
			while(1) {
				mat[i][j]=fgetc(fp);
				if(mat[i][j]=='1' || mat[i][j]=='0') {
					mat[i][j]-='0';
					break;
				}
			}
		}
	}
}
void freemat(int** mat, int m) //매트릭스 메모리 해제
{
	for(int i=0; i<m; i++) {
		free(mat[i]);
	}
	free(mat);
}
