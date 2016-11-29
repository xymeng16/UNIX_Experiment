
#include "apue.h"
#define MAX_COMMAND_LENGTH 1024
static void sig_int(int signo); // This is tne function to be invoked when signal is captured
static void sig_alrm(int signo);
static void sig_quit(int signo);
extern char **environ;
pid_t pid;
struct sigaction	act, oact;
int main(int argc,char *argv[],char *envp[])
{

    char *arg[1024];
    char buf[MAX_COMMAND_LENGTH];
    int status;
    //char *bff=buf,*t=NULL;

    if(signal(SIGINT,sig_int)==SIG_ERR)
		err_sys("signal error");
	if(signal(SIGQUIT,sig_quit)==SIG_ERR)
		err_sys("Quit error");
	
	act.__sigaction_u.__sa_handler = sig_alrm;	// 信号SIGALRM的处理函数
	sigemptyset(&act.sa_mask);
	act.sa_mask |= SIGQUIT;		// 在处理信号SIGALRM时，屏蔽信号SIGQUIT
	act.sa_flags = 0;
#ifdef SA_RESTART		// 如果定义了该常量，则系统默认不重启，应改为重启
		act.sa_flags |= SA_RESTART;
#endif
		sigaction(SIGALRM, &act, &oact);
	printf("Father pid:%d\n",getpid());
    printf("%% ");
	while(fgets(buf,MAX_COMMAND_LENGTH,stdin)!=NULL)
    {
	//int counter=0;
	//int last_pos=-1,cur_pos=-1;
	//printf("%s\n",buf);
	//if(buf[strlen(buf)-1]=='\n')
	//    buf[strlen(buf)-1]='\0'; // Replace the newline with null cuz exex() need '\0' to be recongized as end of command
	/*t=strsep(&bff," ");	
	  while(t)
	{
	printf("%s",t);
	arg[counter]=(char *)malloc(strlen(t)+1);
	strcpy(arg[counter++],t);
	t=strsep(&bff," ");
	}*/
	//while(t=strchr(bff+cur_pos+1,' '))
	//{
	//cur_pos=t-bff;
	//arg[counter]=(char *)malloc(cur_pos-last_pos);
	//strncpy(arg[counter++],bff+last_pos+1,cur_pos-last_pos-1);
	//last_pos=cur_pos;
	//}
	//arg[counter]=(char *)malloc(strlen(bff+cur_pos+1)+1);
	//strcpy(arg[counter++],bff+cur_pos+1);
	//printf("%d %s %s",counter,arg[0],arg[counter-1]);
	   // exit(0);
	alarm(atoi(argv[2]));
	if((pid=fork())<0)
	{
	    err_sys("fork error");
	}
	else if(pid == 0) // if this is child process
	{
		printf("Child pid:%d\n",getpid());
	    //if(arg[0][0]!='/'||arg[0][0]!='.')
		//{
		//char tt[1024];
		//strcpy(tt,arg[0]);
		//strcpy(arg[0],"/bin/");
		//strcat(arg[0],tt);
		//}
		//Do something to pharse the pathname
	    //execve(arg[0],arg,envp);
	    execl("/bin/sh", "sh", "-c", buf, (char *) 0);
	    //strcat(tt,arg[0]);
	    //execve(tt,arg)
	    err_ret("couldn't execute: %s",buf);
	    exit(127);
	}
	//parent
	if((pid=waitpid(pid,&status,0))<0)
	    err_sys("waitpid error");
	alarm(0);
	printf("%% ");
    }
	exit(0);
}
void sig_int(int signo)
{
    printf("interrupt %d",signo);
}
void sig_alrm(int signo)
{
	if(pid)
	{
		printf("Kill pid:%d\n",pid);
		kill(pid,SIGKILL);
		//alarm(0);
	}
}
void sig_quit(int signo)
{
	if(pid)
	{
		printf("Kill pid:%d\n",pid);
		kill(pid,SIGKILL);
		sigset_t	pendmask;
		sigemptyset(&pendmask);
		sigpending(&pendmask);		// 获得未决信号集合
		pid = 0;		// 表示当前无正在执行的用户命令
		alarm(0);	// 清除闹钟
		if(sigismember(&pendmask, SIGALRM)) {	// 存在未决信号SIGALRM
				signal(SIGALRM, SIG_IGN);			// 清除未决信号SIGALRM
					sigaction(SIGALRM, &act, NULL);	// 恢复原来的处理方法
		}
	}
}
