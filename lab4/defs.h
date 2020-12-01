/*THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR  OR CODE WRITTEN BY OTHER STUDENTS - Nandar Soe*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BITMAPSIZE 1048576 // 2^25/32 == 1,048,576 
#define MEMKEY 26677
#define MSGKEY 46677

#define GET_PROCESSID 1 
#define FOUND_PERFECT_NUM 2
#define KILL_ALL 3

//The structure of process
typedef struct{
	pid_t pid;			//pid
	int num_perfect;	//num of perfect found
	int num_test;		//num tested
	int num_notest;		//num not tested
} st_process;

//The structure of shared memroy
typedef struct{
	int bitmap[BITMAPSIZE]; //bit map large enough for 2^25 bits
	int perfectfound[20];	//array of int of perfect num found
	st_process process[20]; //array of processes len 20
} st_memory;

//The structure of messages
typedef struct{
	long msg_type;
	int msg_number;
} st_message;
