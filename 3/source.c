
#include "apue.h"
#define MAX_COMMAND_LENGTH 1024
static void sig_int(int signo); // This is tne function to be invoked when signal is captured
extern char **environ;
int main(int argc,char *argv[],char *envp[])
{

    char *arg[1024];
    char buf[MAX_COMMAND_LENGTH];
    pid_t pid;
    int status;
    char *bff=buf,*t=NULL;

    if(signal(SIGINT,sig_int)==SIG_ERR)
	err_sys("signal error");
    printf("%% ");
    while(fgets(buf,MAX_COMMAND_LENGTH,stdin)!=NULL)
    {
	int counter=0;
	int last_pos=-1,cur_pos=-1;
	printf("%s\n",buf);
	if(buf[strlen(buf)-1]=='\n')
	    buf[strlen(buf)-1]='\0'; // Replace the newline with null cuz exex() need '\0' to be recongized as end of command
	/*t=strsep(&bff," ");	
	while(t)
	{
	printf("%s",t);
	arg[counter]=(char *)malloc(strlen(t)+1);
	strcpy(arg[counter++],t);
	t=strsep(&bff," ");
	}*/
	while(t=strchr(bff+cur_pos+1,' '))
	{
	cur_pos=t-bff;
	arg[counter]=(char *)malloc(cur_pos-last_pos);
	strncpy(arg[counter++],bff+last_pos+1,cur_pos-last_pos-1);
	last_pos=cur_pos;
	}
	arg[counter]=(char *)malloc(strlen(bff+cur_pos+1)+1);
	strcpy(arg[counter++],bff+cur_pos+1);
	//printf("%d %s %s",counter,arg[0],arg[counter-1]);
	   // exit(0);
	if((pid=fork())<0)
	{
	    err_sys("fork error");
	}
	else if(pid == 0) // if this is child process
	{
	    if(arg[0][0]!='/'||arg[0][0]!='.')
		{
		char tt[1024];
		strcpy(tt,arg[0]);
		strcpy(arg[0],"/bin/");
		strcat(arg[0],tt);
		}
		//Do something to pharse the pathname
	    execve(arg[0],arg,envp);
	    
	    //strcat(tt,arg[0]);
	    //execve(tt,arg)
	    err_ret("couldn't execute: %s",arg[0]);
	    exit(127);
	}
	//parent
	if((pid=waitpid(pid,&status,0))<0)
	    err_sys("waitpid error");

	printf("%% ");
    }
	exit(0);
}
void sig_int(int signo)
{
    printf("interrupt %d",signo);
}
