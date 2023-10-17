#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct save{
	char* name;
	time_t modtime;
};

void printi(struct stat *st, struct dirent *d);
void printl(struct stat *st, struct dirent *d);
void printil(struct stat *st, struct dirent *d);
void printt(int cnt, struct save* arr);
void bbsort(int cnt, struct save* arr);
char type(mode_t mode);
char* perm(mode_t mode);

int main(int argc, char** argv)
{
	DIR *dp;
	char *dir;
	struct stat check; //checkfile or directory
	struct stat st;
	struct dirent *d;
	char path[1024];
	
	/////////////////////////////////////
	
	if(argc==1) //myls
	{
		dir=".";
		dp=opendir(dir);
		while((d=readdir(dp))!=NULL)
		{
			if(d->d_name[0]=='.')	continue; //except invisible file
			printf("%s  ", d->d_name);
		}
		putchar('\n');
	}
	
	/////////////////////////////////////
	
	else if(argc==2) //myls with 2 arguments 
	{
		stat(argv[1], &st);
		if(S_ISREG(st.st_mode)) //myls file_name
		{
			printf("%s\n", argv[1]);
		}
		else if(S_ISDIR(st.st_mode)) //myls directory_name
		{
			dir=argv[1];
			dp=opendir(dir);
			while((d=readdir(dp))!=NULL)
			{
				if(d->d_name[0]=='.')	continue; //except invisible file
				printf("%s  ", d->d_name);
			}
			putchar('\n');
			closedir(dp);
		}
		else if(argv[1][0]=='-') //myls -options(i, l, t, il, lt)
		{
			struct save arr[100];
			int cnt=0;
			dir=".";
			dp=opendir(dir);
			while((d=readdir(dp))!=NULL)
			{
				if(d->d_name[0]=='.')	continue; //except invisible file
				sprintf(path, "%s/%s", dir, d->d_name);
				lstat(path, &st); 
				if(strcmp(argv[1],"-i")==0)
					printi(&st, d);
				else if(strcmp(argv[1],"-l")==0)
					printl(&st, d);
				else if(strcmp(argv[1],"-t")==0)
				{
					arr[cnt].name=d->d_name;
					arr[cnt].modtime=st.st_mtime;
					cnt++;
				}
				else if(strcmp(argv[1],"-il")==0)
					printil(&st, d);
			}
			if(cnt!=0)
			{
				bbsort(cnt, arr);
				printt(cnt, arr); //sort for -t option
			}
		}
	}
	return 0;;
}
void printi(struct stat *st, struct dirent *d)
{
	printf("%8lu  %s\n", st->st_ino, d->d_name);
}
void printl(struct stat *st, struct dirent *d)
{
	printf("%c%s", type(st->st_mode), perm(st->st_mode));
	printf("%2ld ", st->st_nlink);
	printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%5ld ", st->st_size);
	printf("%.12s ", ctime(&st->st_mtime)+4);
	printf("%s\n",d->d_name);
}
void bbsort(int cnt, struct save* arr)
{
	char* tname;
	time_t tval;
	for(int i=0; i<cnt; i++)
	{
		for(int j=0; j<cnt-1; j++)
		{
			if(arr[j].modtime<arr[j+1].modtime)
			{
				tval=arr[j].modtime;
				arr[j].modtime=arr[j+1].modtime;
				arr[j+1].modtime=tval;
				tname=arr[j].name;
				arr[j].name=arr[j+1].name;
				arr[j+1].name=tname;
			}
		}
	}
}
void printt(int cnt, struct save* arr)
{
	for(int i=0; i<cnt; i++)
		printf("%.12s %s\n", ctime(&(arr[i].modtime))+4, arr[i].name);
}
void printil(struct stat *st, struct dirent *d)
{
	printf("%7lu ", st->st_ino);
	printf("%c%s", type(st->st_mode), perm(st->st_mode));
	printf("%2ld ", st->st_nlink);
	printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%5ld ", st->st_size);
	printf("%.12s ", ctime(&st->st_mtime)+4);
	printf("%s\n",d->d_name);
}

char type(mode_t mode)
{
	if(S_ISREG(mode))	return('-');
	if(S_ISDIR(mode))	return('d');
	if(S_ISCHR(mode))	return('c');
	if(S_ISBLK(mode))	return('b');
	if(S_ISLNK(mode))	return('l');
	if(S_ISFIFO(mode))	return('p');
	if(S_ISSOCK(mode))	return('s');
}

char* perm(mode_t mode)
{
	int i;
	static char perms[10]="---------";
	for(i=0; i<3; i++)
	{
		if(mode&(S_IRUSR>>i*3))	perms[i*3]='r';
		if(mode&(S_IWUSR>>i*3))	perms[i*3+1]='w';
		if(mode&(S_IXUSR>>i*3))	perms[i*3+2]='x';
	}
	return(perms);
}