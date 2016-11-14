/********************************
Unix Experiment 1: Dining Philosophers problem
Author: Xiangyi Meng
ID: 22920142203887
Date: 8th Nov, 2016
Usage: philosopher   [ -t  <time> ]
********************************/
#include "apue.h"
#include "lock.h"
#include <inttypes.h>
#define N 5
static char* forks[]={"fork0","fork1","fork2","fork3","fork4"};

void philosopher(int id);
void thinking(int id,int nsecs);
void takeFork(int id);
void eating(int id,int nsecs);
void putFork(int id);

int sustainingTime=2;
int pid;
int main(int argc,char **argv)
{
	if(argc>1)
	{
		if(argc!=3)
		{
			// Parameters are wrong
			err_quit("Usage:philosopher [-t <time>] and time should be greater than 0.");
		}
		else
		{
			if((sustainingTime=strtol(argv[2],(char **)NULL,10))==0)
			{
				err_quit("Usage:philosopher [-t <time>] and time should be greater than 0.");
			}
		}
	}
	int i;
	for(i=0;i<N;i++)
	{
		initlock(forks[i]);
	}
	for(i=0;i<N;i++)
	{
		pid=fork();
		if(pid==0) // For child process, do philo.
			philosopher(i);
	}
	wait((int *)NULL); // Wait until all of the child processes are killed.
	return 0;
}

void philosopher(int id)
{
	while(1)
	{
		thinking(id,sustainingTime);
		takeFork(id);
		eating(id,sustainingTime);
		putFork(id);
	}
}

void thinking(int id,int nsecs)
{
	//printf("Philosopher %d is thinking.\n",id);
	char str[50];
	sprintf(str,"Philosopher %d is thinking.\n",id);
	write(STDOUT_FILENO,str,strlen(str));
	sleep(nsecs);
}

void eating(int id,int nsecs)
{
	//printf("Philosopher %d is eating.\n",id);	
	char str[50];
	sprintf(str,"Philosopher %d is eating.\n",id);
	write(STDOUT_FILENO,str,strlen(str));
	sleep(nsecs);
}

// Assuming that philosophers with even id are all right-handers, and phlsphers with odd id are all left-handers.
void takeFork(int id)
{
	if(id%2==0) // Right-handers take the right fork first.
	{
		lock(forks[id+1]);
		lock(forks[id]);
	}
	else // Left-handers take the left fork first.
	{
		lock(forks[id]);
		lock(forks[id+1]);
	}
}

void putFork(int id)
{
	if(id%2==0) // Right-handers put the right fork first.
	{
		unlock(forks[id+1]);
		unlock(forks[id]);
	}
	else // Left-handers put the left fork first.
	{
		unlock(forks[id]);
		unlock(forks[id+1]);
	}
}
