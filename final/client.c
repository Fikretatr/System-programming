/*client.c file*/

#include "client.h"


int main (int argc, char *argv[]) {

	int req_fd;
	int bytesread;
	char buf[BLKSIZE];
	char requestFile[1024];
	int opt;
	pthread_t *Threads_tid;
	int err_check;
	int thread_counter=0;
	/*SIGINT Struct*/
	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}

	/*Get arguments
	Example:./client -r requestFile -q PORT -s IP
	*/
	if(argc==1){
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	}
	if(argc != 7){
		fprintf(stderr,"Command line argument is failed\n");
		return -1;
	}
	while((opt = getopt(argc, argv, "r:q:s:")) != -1){
		switch(opt){
		    case 'r':
		        strcpy(requestFile, optarg);
		        break;
		    case 'q':
		        PORT = atoi(optarg);
		        break;
		    case 's':
		    	strcpy(IP_string, optarg);
		        break;          
		}
	}

	/*SIGINT signal check */ 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		//free(str_message);
		exit(EXIT_FAILURE);
	}

 	/*open input requestfile*/
	req_fd = open(requestFile, O_RDONLY);
	if(req_fd==-1){
		perror("requestFile can not open");
		exit(EXIT_FAILURE);
	}

	/*Read requestfile*/
	for(;;){
		
		while(((bytesread = read(req_fd,buf,BLKSIZE))==-1)&&(errno==EINTR));
		if(bytesread<=0){
			break;
		}		
	}

	/*Count new line */
	for(int i=0;i<strlen(buf);i++){
		if(buf[i]=='\n'){
			thread_counter++;
		}
	}

	Total_Number_Of_Threads=thread_counter;
	struct Cliend_Thread_Struct clients_thread[thread_counter];
	// Returns first token
    char* token = strtok(buf, "\n");
 	int k=0;
    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL) {
    	strcpy(clients_thread[k].request,token);
        clients_thread[k].index=k;
        token = strtok(NULL, "\n");
        k++;
    }
	
	
	Threads_tid=(pthread_t*)malloc(thread_counter*sizeof(pthread_t));
	
	display_message1(thread_counter);
	for(int i=0;i<thread_counter;i++){
		
		if(pthread_create(&Threads_tid[i],NULL,Thread_job,&clients_thread[i])){
			perror("Thread create failed");
			free(Threads_tid);
			close(req_fd);
			exit(EXIT_FAILURE);
		}
		display_message2(i);
	}

	/*Join Threads*/
	for (int i=0; i<thread_counter; i++){	
		err_check=pthread_join(Threads_tid[i], NULL);
		if(err_check!=0){
			perror("Thread join failed");
			free(Threads_tid);
			close(req_fd);
			exit(EXIT_FAILURE);
		}
	}
	/*Write message*/
	err_check =write(1,"Client: All threads have terminated, goodbye.",strlen("Client: All threads have terminated, goodbye."));
	if(err_check == -1){
		perror("write all error");
		free(Threads_tid);
		close(req_fd);
 		exit(EXIT_FAILURE);
	}

	free(Threads_tid);
	close(req_fd);

	return 0;

}

/*Client: I have loaded 30 requests and I’m creating 30 threads.*/
void display_message1(int nof_thread){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"Client: I have loaded %d requests and I’m creating %d threads.\n",nof_thread,nof_thread);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}


/*Client-Thread-0: Thread-0 has been created.*/
void display_message2(int nof_thread){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"Client-Thread-%d: Thread-%d has been created.\n",nof_thread,nof_thread);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message2");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}

/*Client-Thread-0: Thread-0 has been created.*/
void display_message3(int nof_thread,char *thread_req){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"Client-Thread-%d: I am requesting “%s”\n",nof_thread,thread_req);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message3");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}

/*Client-Thread-%d: Terminating\n*/
void display_message4(int nof_thread){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"Client-Thread-%d: Terminating\n",nof_thread);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message4");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  
}

/*Client-Thread-5: The server’s response to “/transactionCount TARLA 01-11-2021 30-11-2021 SAMSUN” is 1*/

void display_message5(int nof_thread,char *answer,char* thread_request){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"Client-Thread-%d: The server’s response to “%s” is %s\n",nof_thread,thread_request,answer);

	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message5");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  
}


void *Thread_job(void *received_struct){
	int fd_client,fd_socket,size_r;
	struct Cliend_Thread_Struct *struct_ptr = (struct Cliend_Thread_Struct*) received_struct;
	
	char transition_count[1024];
	//synchronization barrier with N threads
	pthread_mutex_lock(&mutex);
	++arrived;
		
	if(arrived < Total_Number_Of_Threads) // if this thread is not last
		pthread_cond_wait(&cond,&mutex); // then wait for others
	else
		pthread_cond_broadcast(&cond); // i’m last, awaken the other N-1
	pthread_mutex_unlock(&mutex);

	/*Send request*/
	display_message3(struct_ptr->index,struct_ptr->request);

	//strcat(struct_ptr->request,'\n");
	if((fd_socket = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("Error creating the server socket!");
        exit(EXIT_FAILURE);
    }
    //memset(&addr, 0, sizeof(addr));	
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_string);
    addr.sin_port = htons(PORT);

	fd_client = connect(fd_socket,(struct sockaddr*)&addr,sizeof(addr));
    if(fd_client == -1){
		perror("fail connect");
		exit(EXIT_FAILURE);
	}
	
	send(fd_socket, struct_ptr->request, strlen(struct_ptr->request),0);

	memset(transition_count, 0, sizeof(transition_count));

    size_r=read(fd_socket,transition_count,1024);
    if(size_r==-1){
    	perror("read fail");
	    close(fd_socket);
	   
	    exit(EXIT_FAILURE);
	}

	display_message5(struct_ptr->index,transition_count,struct_ptr->request);
	
	close(fd_client);
	
	/*Thread terminate*/
	display_message4(struct_ptr->index);
	
	return NULL;

}

void time_stamp(){

    time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

}

