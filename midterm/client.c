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
#include <time.h>
#define FIFO_PERM (S_IRUSR | S_IWUSR | S_IWGRP)
#define BUFSIZE 256
int clientFD,serverfd;
void handler(int signum){

	if(signum == SIGINT)
	{

		write(STDOUT_FILENO, "CTRL-C ,free all of P's resources and children ",25);
		
		close(clientFD);
		close(serverfd);

		kill(0, SIGINT);
	}
}


/*static void removefifo(char * NAMEOFCLIENT){

	unlink(NAMEOFCLIENT);
}
*/
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
	//int* DATA_Response;;/*Message response */
	int DATA_Response;
	int size_matrix;
	int inv_flag;
};


void Display_client_output(pid_t pid_client,char* pathtodatafile,int matrix_format,double totaltime,int if_inv);


int main (int argc, char *argv[]) {



	char fifo_client_name[10];
	sprintf(fifo_client_name,"%d",getpid());

	struct sigaction act;
	memset (&act,0 , sizeof(act));
	act.sa_flags = 0;
	act.sa_handler = &handler;
	struct tm* ptm = NULL;
	char timeBUFFER[120];
	struct request req;
	struct response res;

	for(int i=0;i<strlen(fifo_client_name);i++){

		req.fifo_client[i]=fifo_client_name[i];
	}

	
	//pid_t childpid;
	int opt;
	char pathToServerFifo[1024];
	char pathToDataFile[1024];
	int clientFD,serverfd;
	time_t t0,t1;
	/*******************Get opt for argument**************************/
	
	if(argc==1)
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	   

	if(argc != 5){
	    fprintf(stderr,"Command line argument is failed\n");
	    return -1;
	}
	//./client -s pathToServerFifo -o pathToDataFile
	while((opt = getopt(argc, argv, "s:o:")) != -1){
	    switch(opt){
	        case 's':
	            strcpy(pathToServerFifo, optarg);
	            break;
	        case 'o':
	            strcpy(pathToDataFile, optarg);
	            break;
	     }
	}

	

	
	/*******************Open data and Read Matric**************************/
	int temp=0;
	size_t bytes_read;

	int fd_data = open(pathToDataFile, O_RDONLY);

	if(fd_data == -1) {
	perror ("open parent");
	return -1;
	}
	//mkfifo(pathToServerFifo, S_IRUSR | S_IWUSR | S_IWGRP);
	
	char* Tempbuffer;
	char* Buffer_text;
	Buffer_text = (char *) malloc(sizeof(char)*1);
	Tempbuffer = (char *) malloc(sizeof(char)*1);

    do{ 
       temp=strlen(Buffer_text)+4;
       bytes_read = read(fd_data, Tempbuffer, 1);
       Buffer_text = (char*)realloc(Buffer_text,temp);
         
       strcat(Buffer_text,Tempbuffer);
    }while(bytes_read!=0);
    /*******************Parsing " ; , \n **************************/
  
  // printf("%s\n",Buffer_text );

   char temp2[strlen(Buffer_text)];
	int count = 0;
	for(int i=0;i<strlen(Buffer_text);i++){
		if(Buffer_text[i] != '"'){
			temp2[count] = Buffer_text[i];
			count++;
		}
	}
	count = 0;
	for(int i=0;i<strlen(temp2);i++){
		if((int)temp2[i] == 10){
			temp2[i] = ',';
			count++;
		}
	}
   
	//printf("%s\n",temp2 );
 
	char *token;
   int size=0;
   /* get the first token */
   token = strtok(temp2, ",");
   
   /* walk through other tokens */
   while( token != NULL ) {
      req.DATA_Request[size] = atoi(token);
    	size++;
      token = strtok(NULL, ",");
   }
   req.pid=getpid();
	req.DATA_Request_Size=size;


	/*for(int i=0;i<req.DATA_Request_Size;i++){
		printf("%d-",req.DATA_Request[i]);

	}
 	*/
	
	
	
	/*******************open fifo for write**************************/ 
   t0 = time(NULL);
	ptm = localtime(&t0);
	strftime(timeBUFFER, 120, "%d/%m/%Y-%T", ptm);
	


	 /********************Fifo create *********************/


	if(mkfifo(fifo_client_name,FIFO_PERM) == -1){//Create Name Pipe

			fprintf(stderr, "mkfifo\n");
		
			exit(EXIT_FAILURE);
	}


	//open server fifo
  	
	
	/*construct request message , open client FIFO , and client request*/
	while(((clientFD = open(pathToServerFifo ,O_WRONLY )) == -1) && (errno == EINTR));
	if(clientFD==-1){
			fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",
				(long)getpid(),pathToServerFifo,strerror(errno));
			exit(EXIT_FAILURE);
	}
	
	/*******************write request to fifo **************************/
	

   t0 = time(NULL);
	if(write(clientFD, &req, sizeof(struct request)) != sizeof(struct request)){
       fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",
				(long)getpid(),pathToServerFifo,strerror(errno));

   }
	
	/*******************read respond from fifo **************************/

	/*get request message , open server FIFO , and server read*/
	
	
	/*construct request message , open client FIFO , and client request*/
	while(((serverfd = open(fifo_client_name ,O_RDONLY)) == -1) && (errno == EINTR));
	if(serverfd==-1){
			fprintf(stderr,"[%ld]:failed to open name pipe %s: %s\n",
				(long)getpid(),fifo_client_name,strerror(errno));
			
	}

	if(read(serverfd,&res,sizeof(struct response))!=sizeof(struct response)){
		fprintf(stderr,"Cant read with client");

		
	}


   t1 = time(NULL);	
	Display_client_output(getpid(),pathToDataFile,res.size_matrix,difftime(t1, t0),res.inv_flag);
	
	close(clientFD);
	free(Tempbuffer);
	free(Buffer_text);
	free(token);
	close(fd_data);
	unlink(fifo_client_name);
	
	return 1;

}

void Display_client_output(pid_t pid_client,char* pathtodatafile,int matrix_format,double totaltime,int if_inv){
	if(if_inv==1){
		fprintf(stdout,"Client PID#%d (%s) is submitting a %dx%d matrix\n"
		,pid_client,pathtodatafile,matrix_format,matrix_format);
		fprintf(stdout,"Client PID#%d: the matrix is invertible ,total time %f second,goodbye. \n"
		,pid_client,totaltime);
	}
	if(if_inv==0){
		fprintf(stdout,"Client PID#%d (%s) is submitting a %dx%d matrix\n"
		,pid_client,pathtodatafile,matrix_format,matrix_format);
		fprintf(stdout,"Client PID#%d: the matrix is  not invertible ,total time %f second,goodbye. \n"
		,pid_client,totaltime);
	}

}

