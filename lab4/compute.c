/*THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR  OR CODE WRITTEN BY OTHER STUDENTS - Nandar Soe*/

#include "defs.h"

#define	SETBIT( x, i )	( x |= (1 << i) )		//sets ith bit in x 
#define TESTBIT( x, i )	(( x & (1 << i)) != 0 ) //test if ith bit in x is set

st_memory *shared_memory; 
st_message *message;
pid_t currentpid;
int memID;
int msgID;
int rowindex;

/*functions*/
int isPerfect(int n);
int whichInt(int N);
int whichBit(int N);

void terminate(int sig){

	if(rowindex == -1){ exit(1); }

	shared_memory->process[rowindex].pid = 0;
	shared_memory->process[rowindex].num_test = 0;
	shared_memory->process[rowindex].num_notest = 0;
	shared_memory->process[rowindex].num_perfect = 0;

	exit(0);
}

int main(int argc, char** argv){

	int N, testnum; //the numbers to be tested
	rowindex = -1;

	//Get the N to compute
	if(argc < 2){ N = 3; }
	else{ N = atoi(argv[1]);}

	//Check upper and lower limit
	if(N>BITMAPSIZE*32){ printf("Compute too large.\n"); return 1; } 		
	if( N < 3 ){ N = 3; }
	// printf("arg: %s\n", argv[1]);
	// printf("N  : %d\n", N);

	//Signal Handling
	struct sigaction sigact;
	memset(&sigact, 0, sizeof(sigact));

	sigact.sa_handler = terminate;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);

	//connecting shared memory segment with MEMKEY
	memID = shmget(MEMKEY, sizeof(st_memory), 0);
	if(memID == -1){ perror("compute error shmget"); return 1; }		//error
	shared_memory = shmat(memID, NULL, 0); 								//pointer to shared memory segment allocated

	//connecting to msg q with MSGKEY
	msgID = msgget(MSGKEY, IPC_CREAT | 0666);
	if(msgID == -1){ perror("compute error msgget"); return 1; }		//error
	message = malloc(sizeof(st_message));								//pointer to the message

	currentpid = getpid();

	//Send the pid to manage
	message->msg_type = GET_PROCESSID;
	message->msg_number = currentpid;
	if(msgsnd(msgID, message, sizeof(message), 0) != 0 ) {perror("compute pid send error"); return 1;}

//	printf("looking for: %d", currentpid);

	//wait for manage to allocate index to this compute
	sleep(4);

	//Find the index to work on
	for(int i = 0 ; i<20 ; i++){

		if(shared_memory->process[i].pid == currentpid){
			rowindex = i;
			break;
		}
	}
	for(int i = 0 ; i<20 ; i++){
		if(shared_memory->process[i].pid == currentpid){
			rowindex = i;
			break;
		}
	}

	if(rowindex == -1){
		printf("Processes are full");
		terminate(SIGQUIT);
		return 0;
	}

	int compute = 1;
	int loop = 0;
	for(testnum = N; compute != 0; testnum++){

		//looping around
		if(testnum == N && loop == 1){
			compute = 0;
			char *command = "./report";
			char *arguments[] = { "./report", "-k", NULL, NULL };
			execvp(command, arguments);
		}
		else if(testnum == BITMAPSIZE*32){
			loop = 1;
			testnum = 2; //skip 0 and 1
		}

		//find the bit to operate on
		int x = whichInt(testnum);
		int i = whichBit(testnum);

		//not tested 
		if(TESTBIT(shared_memory->bitmap[x],i) == 0){
			SETBIT(shared_memory->bitmap[x],i); 		//mark as tested
			shared_memory->process[rowindex].num_test++; 	//increase tested count
			if(isPerfect(testnum) == 1){ 				//perfect num
				shared_memory->process[rowindex].num_perfect++;
				message->msg_type = FOUND_PERFECT_NUM;
				message->msg_number = testnum;
				if(msgsnd(msgID, message, sizeof(message), 0) != 0 ) {perror("compute perfect num send error"); return 1;}
			}
		}
		else{ //TESTBIT is 1, already computed
			shared_memory->process[rowindex].num_notest++; //increase not tested count
		}

	}

	return 0;
}

int isPerfect(int n){

	//exclude 0 and 1 from calculation
	if(n == 0 || n == 1){
		return 0;
	}

	int sum = 1;

	//find all divisors and add them
	for(int i=2; i*i<=n; i++){
		if(n%i == 0)
			sum = sum + i + n/i;
	}

	if (sum == n && n !=1 )
		return 1; //1 = perfect number

	return 0; //0 = not perfect
}

int whichInt(int N) { return N/32; }

int whichBit(int N) { return N%32; }
