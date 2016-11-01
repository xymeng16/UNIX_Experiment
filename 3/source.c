#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 1024

void sig_int(int signo); // This is the function to be invoked when signal is captured

int main(void)
{
    char buf[MAX_COMMAND_LENGTH];
    pid_t pid;
    int status;

    if(signal(SIGINT,sig_int)==SIG_ERR)
	fprintf(stderr,"signal error");
    printf("%%");
    while(fgets(buf,MAX_COMMAND_LENGTH,stdin)!=NULL)
    {
	if(buf[strlen(buf)-1]=='\n')
	    buf[strlen(buf)-1]='\0'; // Replace the newline with null cuz exex() need '\0' to be recongized as end of command
	    
	if((pid=fork())<0)
	{
	    fprintf(stderr,"fork error");
	}
	else if(pid == 0) // if this is child process
	{
	    execlp(buf,buf,(char *)NULL);
	    fprintf(stderr,"couldn't execute: %s\n",buf);
	    exit(127);
	}

	if((pid=waitpid(pid,&status,0))<0)
	    fprintf(stderr,"waitpid error");

	printf("%%");
    }
}
void sig_int(int signo)
{
    printf("interrupt %d",signo);
}
