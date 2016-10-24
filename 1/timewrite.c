/********************************
Unix Experiment 1: Compare sync write with async write
Author: Xiangyi Meng
ID: 22920142203887
Date: 27th Sep, 2016
Usage: timewrite <outfile> [sync](optional)
********************************/
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>

#define STD_FILE_SIZE 516581760
#define TICK_PER_SEC sysconf(_SC_CLK_TCK) // The ticks per second. Tick is used for time-counting in UNIX.

char buf[STD_FILE_SIZE];
struct tms time_begin;
struct tms time_end;

int main(int argc, char **argv)
{
	int buf_size=256; // The size of the write buffer
	int cur_pos=-1; // The current position of the pointer in buffer
	int dest_file;

	// Read all the content from stdin into buffer. 
	if((read(STDIN_FILENO,buf,STD_FILE_SIZE))!=STD_FILE_SIZE)
	{
		fprintf(stderr,"Read Error!\n");
		exit(-1);
	}

	if(argc>2)
	{
		printf("Sync write test.\n");
		if((dest_file=open(argv[1], O_WRONLY | O_SYNC | O_CREAT, S_IWUSR))<0)
		{
			fprintf(stderr,"Output file open error!");
			exit(-1);
		}
		printf("Open input file successfully!\n");

	}
	else
	{
		printf("Async write test.\n");
		if((dest_file=open(argv[1], O_WRONLY | O_CREAT, S_IWUSR))<0)
		{
			fprintf(stderr,"Output file open error!\n");
			exit(-1);
		}
		printf("Open input file successfully!\n");
	}  
	//printf("USER CPU\tSYS CPU\tCLOCK TIME\tLOOP TIMES\n");
	printf("-----------------------------------------------\n");
	printf("USER CPU    SYS CPU    CLOCK TIME    LOOP TIMES\n");
	printf("-----------------------------------------------\n");
	while(buf_size<=131072) // The size of buffer ranges from 256 bytes to 128k bytes.
	{
		int loop_counter=0;
		clock_t begin,end;
		
		cur_pos=0;
		lseek(dest_file,0,SEEK_SET);
		begin=times(&time_begin);
		while(cur_pos<STD_FILE_SIZE) // While the process of write isn't end.
		{
			loop_counter++;
			if(STD_FILE_SIZE-cur_pos<buf_size) // For the condition that the remainder length of 
			// buffer cannot cover a total buf_size
			{
			    if(write(dest_file,buf+cur_pos,STD_FILE_SIZE-cur_pos-1)!=STD_FILE_SIZE-cur_pos-1)
			    {
			        fprintf(stderr,"Write Error!");
			        exit(-1);
			    }
			}
			else 
			    {
			        if(write(dest_file,buf+cur_pos,buf_size)!=buf_size)
			        {
			    	fprintf(stderr,"Write Error!");
			    	exit(-1);
			        }
			    }
			cur_pos+=buf_size;
		}
		end=times(&time_end);
		printf("%-12.2lf%-11.2lf%-14.2lf",\
			(double)(time_end.tms_utime-time_begin.tms_utime)	/TICK_PER_SEC,\
			(double)(time_end.tms_stime-time_begin.tms_stime)/TICK_PER_SEC,\
			(double)(end-begin)/TICK_PER_SEC);
		printf("%10d\n",loop_counter);
		//printf("-----------------------------------------------");
		buf_size*=2;
	}
	printf("-----------------------------------------------\n");

	close(dest_file);
	return 0;
}
