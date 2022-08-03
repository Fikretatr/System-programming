#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "become_daemon.h"
#include <sys/mman.h>
//#include "tlpi_hdr.h"
#define FIFO_PERM (S_IRUSR | S_IWUSR | S_IWGRP)
#define SHARED_MEMORY_FILENAME "Shared Memory"
int determinant(int *matrix, int order);
int becomeDaemon(int flags);
void infos_log(int fd,int cloumn_number,pid_t pid_child,pid_t pid_client,int busy_n,int poolsize);
int N = 0,logFD;
int determinant(int *matrix, int order);
struct sigaction old_action;
pid_t child_pids;
int *SharedMemAdress=NULL; 
int len; //Size of shared mem size


void handler(int signum){

	if (signum == SIGINT){
			write(logFD, "CTRL-C ,free all of P's resources and children \n", 25);
			if (munmap(SharedMemAdress, len) == -1)
	    	{
	           write(logFD,"Unmap failed",11);
	        }
	            
		    shm_unlink(SHARED_MEMORY_FILENAME);
		   	close(logFD);
			kill(0, SIGINT);
	   		kill(child_pids, SIGKILL);
	}
}
void getCofactor(int mat[N][N], int temp[N][N], int p, int q, int n)
{
    int i = 0, j = 0;
 
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            // Copying into temporary matrix only those element
            // which are not in given row and column
            if (row != p && col != q) {
                temp[i][j++] = mat[row][col];
 
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}
 
/* Recursive function for finding determinant of matrix.
n is current dimension of mat[][]. */
int determinantOfMatrix(int mat[N][N], int n)
{
    int D = 0; // Initialize result
 
    // Base case : if matrix contains single element
    if (n == 1)
        return mat[0][0];
 
    int temp[N][N]; // To store cofactors
 
    int sign = 1; // To store sign multiplier
 
    // Iterate for each element of first row
    for (int f = 0; f < n; f++) {
        // Getting Cofactor of mat[0][f]
        getCofactor(mat, temp, 0, f, n);
        D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);
 
        // terms are to be added with alternate sign
        sign = -sign;
    }
 
    return D;
}
struct request{//Request client -> server 

	pid_t pid; /*Pid of client */
	int seqLen; /*Length of desired sequence*/
	//char* DATA_Request;/*Message request*/
	int DATA_Request[1024];
	int DATA_Request_Size;
	int which_child_pid;
	char fifo_client[10];
	

};


struct response {/*response */
	pid_t pid; /*Pid of client */
	int seqLen; /*Length of desired sequence*/
	//int* DATA_Response;/*Message response */
	int DATA_Response;
	int size_matrix;
	int inv_flag;

	

};

struct ChildProcess{
	pid_t pid_child;
	int proper;
	int Fdescript;
};


typedef struct ChildProcess ChildProcess;

int main (int argc, char *argv[]) {


	/*sem_t* sem = sem_open("Di", (O_CREAT | O_RDWR | O_EXCL), 0666, 0);
	    if (sem == SEM_FAILED) {
		fprintf(stdout, "Error: Duble Instantiation\n");
		exit(1);

	    }*/
    

	int serverFD,cliendfd,dummyfd;
	struct response res;
	struct request req;
	//struct ChildProcess child;
	char pathToServerFifo[150];
	char pathToLogFile[150];
	int poolSize,poolSize2=0,t;
	char Message[1024];
	poolSize2=poolSize2+poolSize2;

	/****************Shared Mem*************************/
	
	int memFd=0;
	
	

	struct sigaction action;
	memset(&action, 0 ,sizeof(action));
	action.sa_handler = &handler;
	sigaction(SIGINT,&action, &old_action);

	//unlink(pathToServerFifo);

	
	becomeDaemon(1);

	/*******************Get opt for argument**************************/
	int opt;
	/*if(argc==1)
		write(logFD,"\nNo Extra Command Line Argument Passed Other Than Program Name",45);
	    

	if(argc != 11){
		write(logFD,"Command line argument is failed\n",20);
	   
	    return -1;
	}
	*/
	while((opt = getopt(argc,argv,"s:o:p:r:t:")) != -1){
		switch(opt){
			case 's':
				strcpy(pathToServerFifo, optarg);
				break;
			case 'o':
				strcpy(pathToLogFile, optarg);
				break;
			case 'p':
				poolSize = atoi(optarg);
				
				break;
			case 'r':
				poolSize2 = atoi(optarg);
				
				break;
			case 't':
				t = atoi(optarg);
				break;
		}
	}
	/******************Log Open******************/

	logFD = open (pathToLogFile,O_CREAT|O_WRONLY,0666);
	if(logFD == -1){
      write(logFD,"log file acilmadi",10);
      return -1;
    }
    
    umask(0);
	 /********************Fifo create *********************/
	if(mkfifo(pathToServerFifo,0666) == -1) {//Create Name Pipe

		if(errno!=EEXIST) {
			write(logFD,"Pipe didnt create",25);
		}
	}

	

	

    sprintf(Message,"Server Y %s started, p=%d , t=%d\n",pathToLogFile,poolSize,t);

    if(write(logFD,Message ,strlen(Message))==-1){

		write(logFD,"log baslik yazamadi",17);
	} 

    
	
	/*************************Open pathto server fifo*****************************/
	/*get request message , open server FIFO , and server read*/
	serverFD = open(pathToServerFifo ,O_RDWR);
	if(serverFD==-1){
			//fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",

				//(long)getpid(),pathToServerFifo,strerror(errno));
			write(logFD,"server fifo acilmadi" ,10);
			return 1;
	}

	dummyfd = open(pathToServerFifo ,O_WRONLY);
	if(dummyfd==-1){
			//fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",

			//	(long)getpid(),pathToServerFifo,strerror(errno));
			write(logFD,"server fifo acilmadi" ,10);
			return 1;
	}



	


	/******************memfd Open******************/
    memFd = shm_open(SHARED_MEMORY_FILENAME,O_RDWR|O_CREAT,0);
	if(memFd==-1){

		write(logFD,"mem open failed" ,8);

	}

	len= (poolSize*2) * sizeof(int);


	shm_unlink(SHARED_MEMORY_FILENAME);


	

	/******************ftruncate******************/
	if(ftruncate(memFd,len)==-1)
		write(logFD,"ftruncate" ,8);
	
		
	/******************Create  SharedMemAdress******************/
	SharedMemAdress = mmap(NULL,len,PROT_READ | PROT_WRITE,MAP_SHARED,memFd,0);
	if(SharedMemAdress == MAP_FAILED)
	{
		write(logFD,"Can not be maped" ,8);
		exit(EXIT_FAILURE);
	}
	



	close(memFd);//nolonger need



	//all child available set
	for(int i=0;i<poolSize;i++){
		SharedMemAdress[i]=1;

	}
	

	
	
	
	int busy_count=0;
	char message[1024];
	int A[poolSize][2];
	char message_child[1024];
	int q=0;
	for(int j=0;j<poolSize;j++)
		pipe(A[j]);
	
	pid_t check_pid;

    for(int i=0;i<poolSize;i++){ // loop will run n times (n=5)
    	
    	
    	check_pid = fork();
    	
    	if(check_pid==0){
       // printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
    		
        	SharedMemAdress[i+poolSize]=getpid();
        	
        	
			for(;;){
			
				
				close(A[i][1]);

				read(A[i][0],&req,sizeof(struct request));

				for(q=0;q<poolSize;q++){//which fd came
					if(A[i][0]==A[q][0]){
							break;
					}
				}

				
				//available child start working
				for(int j=q;0==SharedMemAdress[j];j++){
					

					
			
				int Buffer_request[req.DATA_Request_Size][req.DATA_Request_Size];
             	int enhancer = 0;
             	for(int s=0;s<req.DATA_Request_Size;s++){
             		for(int m=0;m<req.DATA_Request_Size;m++){
             			Buffer_request[s][m] = req.DATA_Request[enhancer];
             			enhancer++;
             		}
             	}
             	N = sqrt(req.DATA_Request_Size);
             	int Determinant = determinantOfMatrix(Buffer_request, sqrt(req.DATA_Request_Size));

	                 	
					
					
					//first det check and after print infos 
					if(Determinant!=0){
						//write respond by child
						res.pid=getpid();
						res.inv_flag=1;
					 	res.DATA_Response=1;
					    res.size_matrix=(int)sqrt(req.DATA_Request_Size);
					 	sprintf(message_child,"Worker PID#%d responding to client PID#%d: the matrix IS invertible.\n",getpid(),req.pid);
					
					 	write(logFD,message_child ,strlen(message_child));
					 	
					 	sleep(t);

					 	/*get request message , open server FIFO , and server read*/
						while(((cliendfd = open(req.fifo_client ,O_RDWR)) == -1) && (errno == EINTR));
						if(cliendfd==-1){
								//fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",

									//(long)getpid(),pathToServerFifo,strerror(errno));
								write(logFD,"server fifo acilmadi" ,8);
								return 1;
						}


					 	if(write(cliendfd,&res ,sizeof(struct response))!=sizeof(struct response)){
								write(logFD,"not write to response" ,15);
								//fprintf(stderr,"server Cant write ");
						}
						SharedMemAdress[j]=1;
						break;
					}
					else{
						//write respond by child
						res.pid=getpid();
						res.inv_flag=0;
						res.DATA_Response=0;
						res.size_matrix=(int)sqrt(req.DATA_Request_Size);
						sprintf(message_child,"Worker PID#%d responding to client PID#%d: the matrix IS NOT invertible.\n",getpid(),req.pid);
						
						//send massage
						write(logFD,message_child ,strlen(message_child));	
						sleep(t);


						/*get request message , open server FIFO , and server read*/
						while(((cliendfd = open(req.fifo_client ,O_RDWR)) == -1) && (errno == EINTR));
						if(cliendfd==-1){
								//fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",

									//(long)getpid(),pathToServerFifo,strerror(errno));
								write(logFD,"server fifo not open" ,8);
								return 1;
						}




						if(write(cliendfd,&res ,sizeof(struct response))!=sizeof(struct response)){
							
							write(logFD,"client a response yazilmadi\n" ,15);
							
						}
						SharedMemAdress[j]=1;
						break;
					
					}

					
				}
				
				
			}

    	}

    }

	while(1){

		
	/*************************Read Request Content*****************************/
		//printf("parent s waiting request...\n");
		if(read(serverFD,&req,sizeof(struct request))!=sizeof(struct request)){
			
			write(logFD,"readlenmedi\n" ,10);
		//return 1;
		}
		else{
			for(int i=0;i<poolSize;i++){
			//close(A[i][0]);
			
			//printf("parent bos child buldu\n");


			if(SharedMemAdress[i]==1){

				
				SharedMemAdress[i]=0;
				//printf("%d. fd a yaziyor\n",i);
				write(A[i][1], &req, sizeof(req));
				busy_count=0; 
				for(int k=0;k<poolSize;k++){
					if(SharedMemAdress[k]==0){
						
						busy_count++;
					}
				}
				//infos_log(logFD,(int)sqrt(req.DATA_Request_Size),SharedMemAdress[poolSize+i],req.pid,busy_count,poolSize);
				sprintf(message,"Worker PID#%d is handling client PID#%d, matrix size %fx%f, pool busy %d/%d\n",SharedMemAdress[poolSize+i]
					,req.pid,sqrt(req.DATA_Request_Size),sqrt(req.DATA_Request_Size),busy_count,poolSize);
				if(write(logFD,message ,strlen(message))==-1){
					write(logFD,"Cannot massage to log\n" ,10);
					
				}	
				break;
			}

		}
		}
	
		
	}

	//sem_close(sem);
    //sem_unlink("Di");
    close(serverFD);
    close(logFD);
    unlink(pathToServerFifo);
    close(dummyfd);
    
	//shm_unlink() 
}

int becomeDaemon(int flags){ /* Returns 0 on success, -1 on error */

	int maxfd, fd;

	switch (fork()) { /* Become background process */
		
		case -1: return -1;
		case 0: break; /* Child falls through; adopted by init */
	 default: _exit(EXIT_SUCCESS); /* parent terminates and shell prompt is back*/
	}
	if (setsid() == -1) /* Become leader of new session, dissociate from tty */
		return -1; /* can still acquire a controlling terminal */
	struct sigaction action;
	memset(&action, 0 ,sizeof(action));
	action.sa_handler = &handler;
	sigaction(SIGINT,&action, &old_action);
	switch (fork()) { /* Ensure we are not session leader */
		case -1: return -1; /* thanks to 2nd fork, there is no way of acquiring a tty */
		case 0: break;
		default: _exit(EXIT_SUCCESS);
	}

	if (!(flags & BD_NO_UMASK0))
		umask(0); /* Clear file mode creation mask */
	if (!(flags & BD_NO_CHDIR))
		chdir("/"); /* Change to root directory */
	if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
		maxfd = sysconf(_SC_OPEN_MAX);
		if (maxfd == -1) /* Limit is indeterminate... */
		maxfd = BD_MAX_CLOSE; /* so take a guess */
		
		for (fd = 0; fd < maxfd; fd++)
			close(fd);
	}
	if (!(flags & BD_NO_REOPEN_STD_FDS)) {
	close(STDIN_FILENO); /* Reopen standard fd's to /dev/null */
	fd = open("/dev/null", O_RDWR);
	if (fd != STDIN_FILENO) /* 'fd' should be 0 */
	return -1;
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
	return -1;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
	return -1;
	}
	 return 0;
}

void infos_log(int fd,int cloumn_number,pid_t pid_child,pid_t pid_client,int busy_n,int poolsize){

	
	char message[1024];

	sprintf(message,"Worker PID#%d is handling client PID#%d, matrix size %dx%d, pool busy %d/%d\n",pid_child,pid_client,cloumn_number,cloumn_number,busy_n,poolsize);
	if(write(fd,message ,strlen(message))==-1){
		write(fd,"Loga mesage yazilmadi\n" ,2);
		
	}	
	close(fd);

}

int determinant(int *matrix, int order)
{	
	
	
	
	int *Minor = (int *)malloc(order * order * sizeof(int));		//Matrix to calculate Minor
   int i,j,runloop,row,col;
   int det=0;																	//Initialize determinant as Zero
   int sign=1;																	//Initialize sign as 1 initially as first i=element needs to be positive while calculating determinant.
	if(order == 1)
   {
   	free(Minor);
   	return(*(matrix));													//return the determinant value for Single Element.
   }
   else
	{
		for(runloop = 0 ; runloop < order ; runloop++)				//need to run the loop order number of times.
   	{
      	row = 0, col = 0;													//Initialize row and column for Minor Matrix as (0,0).
      	for(i = 0 ; i < order ; i++)									//Loop to go through rows.
      	{
   	   	for(j = 0 ; j < order ; j++)								//Loop to go through columns.
         	{
     	   		*((Minor+i*(order-1))+j)=0;							//Initialize corresponding Minor element as Zero.
            	if(i != 0 && j != runloop)								//Consider only the elements which are not in the same row and column of considered minor.
            	{
            		*((Minor+row*(order-1))+col) = *((matrix+i*order)+j);			//Assign value to Minor from Main Matrix.
            		col++;													//Go to next column.
            		if(col > order-2)										//If column exceeds the Minor Matrix size.
               	{
               		row++;												//Go to next row.
               		col=0;												//Go to first column of next row. 
               	}
					}
				}
			}
         det = det + (sign * ((*(matrix+runloop)) * (determinant(Minor,order-1))));	//Recursive function to call the same function again with Minor size.
         sign=-1*sign;
		}
	}
   free(Minor);//Free the allocated memory.
   	
   printf("\n\n%d\n\n",det );


    return det;

   
   															
}




/*char content[10];
		for(int i=0;i<9;i++){
			content[i]=(char)req.DATA_Request[i];

		}
		content[10]='\n';

		write(logFD,content ,strlen(content));
		*/
