

#include <stdio.h>          // printf()                 
#include <stdlib.h>         // exit(), malloc(), free() 
#include <sys/types.h>      // key_t, sem_t, pid_t      
#include <sys/shm.h>        // shmat(), IPC_RMID        
#include <errno.h>          // errno, ECHILD            
#include <semaphore.h>      // sem_open(), sem_destroy(), sem_wait() 
#include <fcntl.h>          // O_CREAT, O_EXEC          
#include <unistd.h>			// fork, sleep
#include <sys/wait.h>		// waitpid
#include <sys/stat.h>		// S_IRUSR, S_IWUSR
#include <math.h>

#define DOHYO_SAIZU 10000	//radius of dohyo (ring) -- much smaller for sumo_with_semaphore than sumo_no_semaphore
#define NORTH 0
#define NORTH_EAST 1
#define EAST 2
#define SOUTH_EAST 3
#define SOUTH 4
#define SOUTH_WEST 5
#define WEST 6
#define NORTH_WEST 7
//description of sumo ring structure to be placed in shared memory (terms: http://en.wikipedia.org/wiki/Glossary_of_sumo_terms)
typedef struct dohyo {	 
			int uchi_EW;					//'inside' -- fighter position (distance from dohyo center at '0')
			int uchi_NS;
			int hanamichi_east;			//number of positive throws by rikishi_east() 
			int hanamichi_west;				//number of negative throws by rikishi_west()
			int hanamichi_north;
			int hanamichi_south;
			int gunbai;					//war fan: initially 0; held at 1 by gyoji to start match;  '2' signals finish.  
			sem_t sem;					//semaphore for sumo_with_semaphore example
} dohyo_t;
int ring_pos(int a, int b){
	double A=(double)a;
	double B=(double)b;
	double dist= pow((pow(A,2)+ pow(B,2)),0.5);
	int pos = (int) dist;
	return pos;
}
void print_final_pos(dohyo_t *pdohyo, int off_balance_EW, int off_balance_NS){
	printf("final result: **************\n");
	int EW=0, NS=0, final_pos=-1;
	 EW=(pdohyo->uchi_EW>0)? 1 : -1;
	 NS=(pdohyo->uchi_NS>0)? 1 : -1;
	 if(EW==0 && NS ==1) {final_pos=NORTH; printf("N wins\n");}
	 else if(EW==1 && NS == 1){final_pos = NORTH_EAST; printf("NE wins\n");} 
     else if (EW==1 && NS==-1){final_pos=SOUTH_EAST; printf("SE wins\n");}
     else if (EW == 1 && NS ==0){final_pos= EAST; printf("E wins\n");}
	 else if (EW == 0 && NS ==-1){final_pos= SOUTH; printf("S wins\n");}
	 else if (EW == -1 && NS ==-1){final_pos= SOUTH_WEST; printf("SW wins\n");}
	 else if (EW == -1 && NS ==0){final_pos= WEST;printf("W wins\n"); }
	 else if (EW == -1 && NS ==1){final_pos= NORTH_WEST; printf("NW wins\n");}	 
 	 
		
	 
	 
	fflush(stdout);
}
//rikishi_east -- sumo wrestler from the east
void rikishi_east(dohyo_t *pdohyo) 		
{ 
	pdohyo->hanamichi_east += 1;		//stomping foot to signal 'ready'
	while(pdohyo->gunbai == 0); 		//wait for gyoji() to start match

	while(pdohyo->gunbai < 2) { 		//fight until gyoji() signals match is over
		sem_wait(&(pdohyo->sem));		//might have to wait
		pdohyo->uchi_EW += 1;				//push against west
		sem_post(&(pdohyo->sem));		//free lock 
		pdohyo->hanamichi_east += 1;	//record push
	}
	return;	 							//exit ring when match is over
}

//rikishi_west -- sumo wrestler from the west
void rikishi_west(dohyo_t *pdohyo) 		
{ 	
	pdohyo->hanamichi_west += 1;		//stomping foot to signal 'ready'
	while(pdohyo->gunbai == 0); 		//wait for gyoji() to start match

	while(pdohyo->gunbai < 2) { 		//fight until gyoji() signals match is over
		sem_wait(&(pdohyo->sem));		//might have to wait
		pdohyo->uchi_EW -= 1;				//push against east
		sem_post(&(pdohyo->sem));		//free lock 
		pdohyo->hanamichi_west += 1;	//record push
	}
	return;	 							//exit ring when match is over
}
//rikishi north sumo wrestler
void rikishi_north(dohyo_t *pdohyo) 		
{ 
	pdohyo->hanamichi_north += 1;		//stomping foot to signal 'ready'
	while(pdohyo->gunbai == 0); 		//wait for gyoji() to start match

	while(pdohyo->gunbai < 2) { 		//fight until gyoji() signals match is over
		sem_wait(&(pdohyo->sem));		//might have to wait
		pdohyo->uchi_NS += 1;				//push against west
		sem_post(&(pdohyo->sem));		//free lock 
		pdohyo->hanamichi_north += 1;	//record push
	}
	return;	 							//exit ring when match is over
}
// rikishi south

void rikishi_south(dohyo_t *pdohyo) 		
{ 
	pdohyo->hanamichi_south += 1;		//stomping foot to signal 'ready'
	while(pdohyo->gunbai == 0); 		//wait for gyoji() to start match

	while(pdohyo->gunbai < 2) { 		//fight until gyoji() signals match is over
		sem_wait(&(pdohyo->sem));		//might have to wait
		pdohyo->uchi_NS += 1;				//push against west
		sem_post(&(pdohyo->sem));		//free lock 
		pdohyo->hanamichi_south += 1;	//record push
	}
	return;	 							//exit ring when match is over
}
//gyoji -- sumo judge
void gyoji(dohyo_t *pdohyo) 			
{ 
//	unsigned int k=0; 					//counter for status updates
	int pushes = 0;          	//net of actual pushes by east and by west
	int off_balance_EW=0;	//difference between actual pushes and scored pushes (critical region violations)
	int off_balance_NS=0;
	int prev_east,prev_west;	//so only new status is reported
	int pushes_EW=0;
	int pushes_NS=0;
	int prev_north;
	int prev_south;
	int final_pos=-1;
	while( (pdohyo->hanamichi_east==0) || (pdohyo->hanamichi_west==0) || (pdohyo->hanamichi_north==0) || (pdohyo->hanamichi_south==0) ) ; //wait for both east and west to stomp foot

	printf ("\n gyoji: match is starting\n"); fflush(stdout);

	pdohyo->gunbai=1;												//signal start of match to rikishi

	while( ring_pos(pdohyo->uchi_EW, pdohyo->uchi_NS)  < DOHYO_SAIZU ) { 					//both rikishi remain in dohyo -- fight continues
		if((prev_east != pdohyo->hanamichi_east) || (prev_west != pdohyo->hanamichi_west) || (prev_north !=pdohyo->hanamichi_north) || (prev_south !=pdohyo->hanamichi_south) ) { //new activity by east or west
			pushes_EW = pdohyo->hanamichi_east - pdohyo->hanamichi_west ;	//calculate net of actual pushes by east and by west
			off_balance_EW = pdohyo->uchi_EW - pushes_EW;						//difference between actual pushes and scored pushes
			prev_east= pdohyo->hanamichi_east;							//update lagged value
			prev_west= pdohyo->hanamichi_west;				// ""
			pushes_NS=pdohyo->hanamichi_north - pdohyo->hanamichi_south;
			off_balance_NS=pdohyo->uchi_NS - pushes_NS;
			prev_north=pdohyo->hanamichi_north;
			prev_south= pdohyo->hanamichi_south;
			printf ("gyoji East Vs West: uchi_EW=%012d east=%012d west=%012d off_balance_EW=%012d\n",
					pdohyo->uchi_EW,pdohyo->hanamichi_east,pdohyo->hanamichi_west, off_balance_EW); //display fight status
		    printf ("gyoji North Vs South: uchi_NS=%012d north=%012d south=%012d off_balance_NS=%012d\n",
					pdohyo->uchi_NS,pdohyo->hanamichi_north,pdohyo->hanamichi_south, off_balance_NS);
			
		}
//		sleep(1);
	}
	//here if either rikishi is pushed out of dohyo
	
	

	pdohyo->gunbai=2;
print_final_pos(pdohyo, off_balance_EW, off_balance_NS);												//signal end of match to rikishi
}

int main (int argc, char **argv){
    int i;							//      loop variables          
 //   key_t shmkey;					//      shared memory key  -- eliminated because we made shared segment private    
    int shmid;						//      shared memory id        
    pid_t pid;						//      fork pid                
    dohyo_t *pdohyo;				//      pointer to dohyo in shared memory
 
    //create the dohyo in shared memory 
//old way -- had to be changed because (I think) other users captured it
//    shmkey = ftok ("/dev/null", 5);	// valid directory name and a number 
//    printf ("shmkey for pdohyo = %d\n", shmkey);
//    shmid = shmget (shmkey, sizeof (dohyo_t), 0644 | IPC_CREAT);
//new way -- private segment for me only
	shmid = shmget (IPC_PRIVATE, sizeof (dohyo_t),S_IRUSR|S_IWUSR);	//create a private segment with user r/w access only
    if (shmid < 0){					// shared memory error check 
        perror ("shmget\n");
        exit (1);
    }

	//shared memory attach : get a pointer to the new dohyo in shared memory
    pdohyo = (dohyo_t *) shmat (shmid, NULL, 0);    
	if (pdohyo < 0){				// shared memory error check 
        perror ("shmat\n");
        exit (1);
    }

	//initialize the dohyo
	pdohyo->uchi_EW=pdohyo->uchi_NS= 0;
	
	pdohyo->hanamichi_east= pdohyo->hanamichi_west= 0;
	pdohyo->gunbai=0;
	
	//initialize the dohyo semaphore to '1'
	if(sem_init( &(pdohyo->sem),99,1) != 0 ){ perror ("semaphore ");exit(0);}//'99' or any non-zero: use shm for semaphore

    // create gyogi and  two rikishi processes 
    for (i = 0; i < 4; i++){
		pid = fork ();
		if (pid < 0)              // check for error      
		    printf ("Fork error.\n");
		else if (pid == 0)
		    break;               // child process goes on below with current value of 'i'
    }

	// decode which process this is and dispatch it
	if (pid != 0){	// this is the giyoji (parent) process
		gyoji(pdohyo);	//will not return until match is over		 

		//gioji continues here on return from gyoji(pdohyo) and waits for rikishi to exit ring
		while ( (pid = waitpid (-1, NULL, 0)) != 0 ){
			if (errno == ECHILD)
				break;
		}

		printf ("\n gyoji: All rikishi have exited.\n"); fflush(stdout);

		// shared memory detach 
		if( shmdt ((char *)pdohyo) <0 ) {			//added (char *) cast to quiet nuisance warning
		       perror ("gioji: shmdt\n");
        exit (1);
		}

		//shared memory deallocate
		if( shmctl (shmid, IPC_RMID, NULL) < 0 ) {
		       perror ("gioji: shmctl\n");
        exit (1);
		}
		

		exit (0);
	}

	else{ // pid == 0 : these are rikishi processes (to say 'child' is insult)

		if(i==0) { 	//first rikishi --
			printf("\n rikishi_east is entering the ring\n"); fflush(stdout);
			rikishi_east(pdohyo); //will not return until gyoji signals match is over (gunbai == 2) 
		}
		else if (i==1){ //second rikishi -- 
			printf("\n rikishi_west is entering the ring\n"); fflush(stdout);
			rikishi_west(pdohyo); //will not return until gyoji signals match is over (gunbai == 2)
		}
		else if (i==2){ //second rikishi -- 
			printf("\n rikishi_north is entering the ring\n"); fflush(stdout);
			rikishi_north(pdohyo); //will not return until gyoji signals match is over (gunbai == 2)
		}
		else if (i==3){ //second rikishi -- 
			printf("\n rikishi_south is entering the ring\n"); fflush(stdout);
			rikishi_south(pdohyo); //will not return until gyoji signals match is over (gunbai == 2)
		}

		//each rikishi continues here when either rikishi leaves the ring

		// shared memory detach 
		if( shmdt ((char *)pdohyo) <0 ) {			//added (char *) cast to quiet nuisance warning
		       perror ("rikishi: shmdt\n");
        exit (1);
		}

		//shared memory deallocate done by server

		exit (0);
	}
	return 0;	//makes C happy, but will never be executed
}
