/*THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR  OR CODE WRITTEN BY OTHER STUDENTS - Nandar Soe*/

#include "defs.h"

st_memory *shared_memory; 
st_message *message;
int memID;
int msgID;


int main(int argc, char** argv){

	//connect to a shared memory
	memID = shmget(MEMKEY, sizeof(st_memory), 0); 
	if(memID == -1){ perror("manage shmget error"); exit(1); } //error
	shared_memory = shmat(memID, NULL, 0); //pointer to the created shared memory

    //connect tp to msg q with MSGKEY
	msgID = msgget(MSGKEY, IPC_CREAT | 0666);
	if(msgID == -1){ perror("compute error msgget"); return 1; }		//error
	message = malloc(sizeof(st_message));	   

    printf("\nReport Summary\n");
    //printf("========================================================\n");
    int totalperfectnum = 0;
    int totalnumbertested = 0; 
    int individualperfectsum = 0;
    int individualnumbertested = 0;

    printf("pid\t#perfect\t#skipped\t\t#tested\n\n");

    for(int i = 0; i < 20 ; i++){

        //if the process is running
        if(shared_memory->process[i].pid != 0){

            individualnumbertested = shared_memory->process[i].num_test;
            individualperfectsum = shared_memory->process[i].num_perfect;

            printf("%d\t%d\t\t%d\t\t%d\n\n", 
            shared_memory->process[i].pid, 
            individualperfectsum,
            shared_memory->process[i].num_notest,
            individualnumbertested);

            totalnumbertested += individualnumbertested;
            totalperfectnum += individualperfectsum;

        }

    }

    printf("The Perfect Numbers: \n");
    for (int i=0; i<20; i++){
        if(shared_memory->perfectfound[i] != 0){
            printf("%d ", shared_memory->perfectfound[i]);
        }
    }
    printf("\n\n");

    printf("Total Number Tested: %d\n", totalnumbertested);
    printf("Total Perfect Found: %d\n\n", totalperfectnum);

    if(argc >= 2){
        if((strcmp(argv[1], "-k")) == 0){
            message->msg_type = KILL_ALL;
            if(msgsnd(msgID, message, sizeof(message), 0) != 0 ) {perror("report KILL ALL err"); return 1;}
        }
    }

}
