/*THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR  OR CODE WRITTEN BY OTHER STUDENTS - Nandar Soe*/

#define _XOPEN_SOURCE
#include "defs.h"

st_memory *shared_memory; 
st_message *message;
int memID;
int msgID;

void terminate(int signo){

	printf("\nTermination Process Started\n");

	//SEND INTR signal to all running computes
	for (int i =0 ; i<20 ; i++){
		if(shared_memory->process[i].pid !=0 ){
			kill(shared_memory->process[i].pid, SIGINT);
		}
	}

	//sleeps 5 seconds
	sleep(5);

	//delloactes the shared memroy
	shmdt(shared_memory); 			//detaches the shared memory seg 
	shmctl(memID, IPC_RMID, NULL); 	//mark the sement to be destroyed
	msgctl(msgID, IPC_RMID, NULL); 	//removes the msg q
	free(message);					//frees msg q buffer

	printf("Terminated\n");

	//terminates
	exit(0);

}

void initializeSharedMemory(){
	//clear bitmap
	for (int i = 0 ; i < BITMAPSIZE; i++){
		shared_memory->bitmap[i] = 0;
	}
	//clear all processes
	for (int i = 0; i < 20; i++){
		shared_memory->process[i].pid = 0;
		shared_memory->process[i].num_notest = 0;
		shared_memory->process[i].num_perfect = 0;
		shared_memory->process[i].num_test = 0;
	}
	//clear perfect num array 
	for(int i = 0 ; i < 20; i++){
		shared_memory->perfectfound[i]=0;
	}

}

int findIndex(){
	for (int i = 0; i<20; i++){
		if(shared_memory->process[i].pid == 0){
			return i;
		}
	}
	return -1;
}

int main(){

	int currentIndex;

	//create a shared memory
	memID = shmget(MEMKEY, sizeof(st_memory), IPC_CREAT | IPC_EXCL | 0666); 
	if(memID == -1){ perror("manage shmget error"); exit(1); } //error
	shared_memory = shmat(memID, NULL, 0); //pointer to the created shared memory

	initializeSharedMemory();

	// Alternative way to clear shared memory
	// memset(shared_memory->bitmap, 0, sizeof(shared_memory->bitmap));
	// memset(shared_memory->perfectfound, 0, sizeof(shared_memory->perfectfound));
	// memset(shared_memory->process, 0, sizeof(shared_memory->process));

	//signal handling
	struct sigaction sigact;
	memset(&sigact, 0, sizeof(sigaction));

	sigact.sa_handler = terminate;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);

	//create message q
	msgID = msgget(MSGKEY, IPC_CREAT | 0666);		
	if(msgID == -1) { perror("manage msgget error"); exit(1); }
	message = malloc(sizeof(st_message));

	//gets messages from compute and report
	while(1){

		memset(message, '\0', sizeof(st_message));

		//recieve message 
		if((msgrcv(msgID, message, sizeof(message), 0, 0)) == -1) {perror("manage msgrcv error"); exit(1);}
		//printf("recieved message!! %d of type %ld", message->msg_number, message->msg_type);

		long messagetype = message->msg_type;
		int perfectfoundindex = -1;

		switch(messagetype){
			case GET_PROCESSID : 
				//find an avaliable row index
				currentIndex = findIndex();
				
				//attach to a shared index in memory
				shared_memory->process[currentIndex].pid = message->msg_number;
				break;

			case FOUND_PERFECT_NUM : //found a perfect number from compute
				
				//find index in perfect found
				for(int i = 0 ; i < 20 ;i++){
					if(shared_memory->perfectfound[i] == 0){
						perfectfoundindex = i; 
					}
				}
				if(perfectfoundindex == -1){
					printf("perfect found array full");
				}
				else{
					//puts the perfect num in
					shared_memory->perfectfound[perfectfoundindex] = message->msg_number;
				}
				break;
			case KILL_ALL:
				terminate(0);
			break;
		}
	}

	terminate(0);
}
