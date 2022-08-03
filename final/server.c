/*server.c file*/

#include "server.h"
#include "queue.h"
int main (int argc, char *argv[]) {

	int PORT,numberOfThreads;
	int opt;
	int fd_socket;
	/*SIGINT Struct*/
	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}

	/*Get arguments
	Example:./server -p PORT -t numberOfThreads
	*/
	if(argc==1){
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	}
	if(argc != 5){
		fprintf(stderr,"Command line argument is failed\n");
		return -1;
	}
	while((opt = getopt(argc, argv, "p:t:")) != -1){
		switch(opt){
		    case 'p':
		       	PORT = atoi(optarg);
		        break;
		    case 't':
		        numberOfThreads = atoi(optarg);
		        break;    
		}
	}

	/*Restriction*/
	if(PORT < 0)
	{
		perror("port must be positive\n");
		exit(EXIT_FAILURE);
	}
	if(numberOfThreads < 5)
	{
		perror("number of threads greater than 5\n");
		exit(EXIT_FAILURE);
	}

	/*Create Clients Threads*/
	pthread_t *Threads_tid;
	//struct Server_Thread_Struct servers_thread[numberOfThreads];
	Threads_tid=(pthread_t*)malloc(numberOfThreads*sizeof(pthread_t));
	for(int i=0;i<numberOfThreads;i++){
		//servers_thread[i].index=i;
		if(pthread_create(&Threads_tid[i],NULL,Server_Thread_job,NULL)){
			perror("Thread create failed");
			free(Threads_tid);
			exit(EXIT_FAILURE);
		}

	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		//free(str_message);
		exit(EXIT_FAILURE);
	}

	//printf("%d , %d\n",PORT ,numberOfThreads);

	int Client_Socket;
	fd_socket = socket(AF_INET, SOCK_STREAM,0);
	if(fd_socket == -1){
		perror("Fail Socket");
		exit(EXIT_FAILURE);
	}
	//setsockopt(fd_socket,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
	struct sockaddr_in addr;
	int len_of_addr = sizeof(addr);
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    setsockopt(fd_socket,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
    if(bind(fd_socket, (struct sockaddr*)(&addr), sizeof(addr)) == -1){
		close(fd_socket);
		perror("fail bind");
		exit(EXIT_FAILURE);
	}

	/*200 = BACK LOG*/
	if(listen(fd_socket, 1024) < 0){
		close(fd_socket);
		perror("fail listen");
		exit(EXIT_FAILURE);
	}
   
    /*Part of main thread
    The  endless mainthread will terminate when it receives a SIGINT*/
    while(1){

    	

    	Client_Socket = accept(fd_socket, (struct sockaddr*)&addr,(socklen_t *)&len_of_addr);
    	if(Client_Socket==-1){
    		if(FlagInterupt){
    			free(Threads_tid);
				exit_and_free_SIGINT(Client_Socket,fd_socket);
				
    		}
			perror("accept fail");
		    close(fd_socket);
		    exit(EXIT_FAILURE);
    	}

    	// SIGINT signal check 
		if(FlagInterupt){
			free(Threads_tid);
			exit_and_free_SIGINT(Client_Socket,fd_socket);
		}
	 	
	 	int *fd_req = malloc(sizeof(int));
	 	*fd_req = Client_Socket;
		
	 	pthread_mutex_lock(&mutex);

	 	enqueue(fd_req);
	 	
	 	pthread_cond_signal(&cond);
	 	pthread_mutex_unlock(&mutex);

	 	
    }

	close(Client_Socket);
	close(fd_socket);
	//shutdown(fd_socket, SHUT_RDWR);
    
	return 0;

}

void exit_and_free_SIGINT(int Client_Socket,int fd_socket){

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	//memset(str_message6, 0, sizeof(str_message6));
	sprintf(str_message,"SIGINT has been received.I handled a total of %d requests. Goodbye.\n",TOTAL_REQUEST);


	fail=write(1,str_message,strlen(str_message));
	if(fail==-1){
		perror("write fail");
		close(Client_Socket);
		close(fd_socket);
		free(str_message);
		exit(EXIT_FAILURE);

	}

	close(Client_Socket);
	close(fd_socket);
	free(str_message);
	exit(EXIT_FAILURE);

}

void *Server_Thread_job(void *arg){
	int size_s;
	char Store[1024];
	int fd_client;
	char Temp_Buf[BUFSIZE];
	char port_servant[BUFSIZE];
	char pid_servant[BUFSIZE];
	Request Client_Question;


	while(FlagInterupt==0){

		int *fd_req ;

		pthread_mutex_lock(&mutex);

		while((fd_req=dequeue())==NULL){
			
			pthread_cond_wait(&cond,&mutex);//Monitor

		}
		
		pthread_mutex_unlock(&mutex);

	 	if(fd_req!=NULL){
	 		
	 		memset(Store, 0, sizeof(Store));

	 		size_s=read(*fd_req,Store,1024);
	 		fd_client=*fd_req;
	 		if(size_s==-1){
	 			perror("read fail 1");
	 			
	 			exit(EXIT_FAILURE);

	 		}
	 		
	 		
	 		pthread_mutex_lock(&mutex1);
	 		strcpy(Temp_Buf,Store);
	 		/*Check client or servant*/
	 		if(Temp_Buf[0]=='!'){
	 			
	 			size_servant++;
	 			servants_info = (servants *) realloc(servants_info, size_servant * sizeof(servants));
				
				char* token_buf = strtok(Temp_Buf, " ");
				token_buf = strtok(NULL, " ");
				strcpy(port_servant,token_buf);
				servants_info[size_servant-1].port_sr=atoi(port_servant);
				token_buf = strtok(NULL, " ");
				strcpy(pid_servant,token_buf);
				servants_info[size_servant-1].pid=atoi(pid_servant);
				strcpy(servants_info[size_servant-1].city1,strtok(NULL, " "));

				display_message6(servants_info[size_servant-1].city1,servants_info[size_servant-1].pid,servants_info[size_servant-1].port_sr);
				
	 		}
	 		
	 		else{
	 			
	 			TOTAL_REQUEST++;
	 			//bzero(Temp_Buf,sizeof(Temp_Buf));
	 			strcpy(Temp_Buf,Store);
 				int index=0;

				//strcat(question," ");
				char *ptr = strtok(Temp_Buf," ");
			    while(ptr != NULL){
			    index++;
			    if(index==1){
					strcpy(Client_Question.Tcount,ptr);
			        
			    }
			    else if(index==2){
			        strcpy(Client_Question.type,ptr);

			    }
			    else if(index==3){
			        strcpy(Client_Question.date1,ptr);

			    }
			    else if(index==4){
			        strcpy(Client_Question.date2,ptr);

			    }
			    else if(index==5){
			        strcpy(Client_Question.city,ptr);

			    }
			  
			    ptr = strtok(NULL," ");

				}
				
				/*All servant take this request*/
				if(index==4){

					if(size_servant==0){
						perror("servant not working");
						
						exit(EXIT_FAILURE);
					}
					
					//display_message10(servants_info[size_servant-1].pid);
					char General_ANS[1024];
					int total_trans=0;

					display_message11();
					for(int i=0;i<size_servant;i++){
					
						total_trans=total_trans+connect_and_send_ALL(servants_info[i].port_sr,Store);
						
					}
					sprintf(General_ANS,"%d",total_trans);

					display_message3(General_ANS);

					//Response received: 1, forwarded to clien
					send(fd_client, General_ANS, strlen(General_ANS),0);

				}
		
				if(index==5){

					if(size_servant==0){
						perror("servant not working");
						exit(EXIT_FAILURE);
					}

					int Is_there_city;

					for(int i=0;i<size_servant;i++){
						Is_there_city=exist_city_in_servant(servants_info[i].city1,Client_Question.city);
						if(Is_there_city){

							connect_and_send(servants_info[i].port_sr,Store,fd_client,Is_there_city,Client_Question.city,servants_info[i].pid);
						}

					}
				}
			}
			pthread_mutex_unlock(&mutex1);
	 		

	 	}//if request non empty
	 	close(*fd_req);
	 	free(fd_req);
	}

	return 0;
}



/*Servant 6672 present at port 16001 handling cities ADANA-ANKARA*/
void display_message1(int index_servant,int port,char* city1,char* city2){

	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);


	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"%s,Servant %d present at port %d handling cities %s-%s\n",time_info,index_servant,port,city1,city2);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}
/*check the city if there exits in servant*/
int exist_city_in_servant(char* whole_city,char* req_city){
	char buffer[1024];
	bzero(buffer,1024);
	strcpy(buffer,whole_city);

	char* token_buf3 = strtok(buffer,"-");
	
	int flag_exit=0;
	while (token_buf3 != NULL)
	{	
		if(strcmp(req_city,token_buf3)==0){

			flag_exit=1;

		}
		
		token_buf3 = strtok (NULL, "-");
	}

	return flag_exit;

}



/*Request arrived “transactionCount TARLA 01-01-2000 10-01-2021 ISTANBUL”*/
void display_message2(char* request){

	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"%s,Request arrived “%s”\n",time_info,request);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}

/*Response received: 1, forwarded to clien*/
void display_message3(char* msg){


	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"%s,Response received: %s, forwarded to client\n",time_info,msg);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}

/*Response received: 1, forwarded to clien*/
void display_message10(int pid){

	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"%s,Contacting servant %d\n",time_info,pid);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}

/*Response received: 1, forwarded to clien*/
void display_message11(){

	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message6;
	str_message6 = (char *) malloc(200);
	int fail;
	//memset(str_message6, 0, sizeof(str_message6));
	sprintf(str_message6,"%s,Contacting ALL servants\n",time_info);
	fail =write (1, str_message6,strlen(str_message6));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message6);  
		exit(EXIT_FAILURE);
	}
	free(str_message6);  

}


/*Response received: 1, forwarded to clien*/
void display_message6(char* city1,int pid,int port){

	time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char buffer[1024];
	char city_11[1024];
	char city_22[1024];

	strcpy(buffer,city1);
	char* token_buf2 = strtok(buffer,"-");
	
	
	int index=0,temp=0;

	while (token_buf2 != NULL)
	{
		index++;
		token_buf2 = strtok (NULL, "-");
	}

	strcpy(buffer,city1);
	token_buf2 = strtok(buffer,"-");
	temp=0;

	while (token_buf2 != NULL)
	{
		if(temp==0){

			strcpy(city_11,token_buf2);
		}
		if((index-1)==temp){

			strcpy(city_22,token_buf2);
		}
		temp++;
		token_buf2 = strtok (NULL, "-");
	}	

	
	char *str_message;
	str_message = (char *) malloc(200);
	int fail;
	sprintf(str_message,"%s,Servant %d present at port %d handling cities %s-%s\n",time_info,pid,port,city_11,city_22);
	fail =write (1, str_message,strlen(str_message));
	if(fail == -1){
		perror("write syscall for display_message1");
		free(str_message);  
		exit(EXIT_FAILURE);
	}
	free(str_message);  

}
/*get time infos*/
void time_stamp(){

    time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

    printf("%s\n", time_info);
}

/**Connect related cities and send request form them and send answer to the client*/
void connect_and_send(int which_port,char *request,int fd,int check,char* wit_city,int pid_servant){

	int FD_servant;
	//int FD_Rec_fd;
	char Answer[1024];
	char answer_without_city[1024];
	int err;
	int temp;

    FD_servant = socket(AF_INET,SOCK_STREAM,0);
    if(FD_servant==-1){
        perror("Error creating the server socket!");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr1;
    int length = sizeof(addr1);
    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr1.sin_port = htons(which_port);

   	if(check==1){

   		//contagting servant message
   		display_message10(pid_servant);

		err = connect(FD_servant,(struct sockaddr*)&addr1,length);
	    if(err == -1){
	        perror("fail connect");
	        exit(EXIT_FAILURE);
	    }
	    
		int check;

	  	check=send(FD_servant,request ,strlen(request),0);
	    if(check==-1){
	    	perror("send connect");
	    	exit(EXIT_FAILURE);

	    }

	    memset(Answer, 0, sizeof(Answer));

		temp=read(FD_servant,Answer,sizeof(Answer));
		if(temp==-1){
			perror("read failed");
			exit(EXIT_FAILURE);

		}

		//Response received: 1, forwarded to clien
		display_message3(Answer);

		send(fd, Answer, strlen(Answer),0);
		bzero(Answer,1024);
		close(err);
   	}
   	if(check==0){
		sprintf(answer_without_city,"no servant responsible for %s",wit_city);
   		send(fd, answer_without_city, strlen(answer_without_city),0);
   	}
 
}
/*Connect servant and send message to them*/
int connect_and_send_ALL(int which_port,char *request){

	int FD_servant;
	int err;
   	int temp;
   	char Answer[1024];
   	
   	
   	//ALL server coantact mesage
	FD_servant = socket(AF_INET,SOCK_STREAM,0);
    if(FD_servant==-1){
        perror("Error creating the server socket!");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr1;
    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr = INADDR_ANY;
    addr1.sin_port = htons(which_port);

    err = connect(FD_servant,(struct sockaddr*)&addr1,sizeof(addr1));
    if(err == -1){
        perror("fail connect");
        exit(EXIT_FAILURE);
    }
	
	send(FD_servant,request ,strlen(request),0);
	memset(Answer, 0, sizeof(Answer));
	temp=read(FD_servant,Answer,sizeof(Answer));
	if(temp==-1){
	perror("read failed");
	exit(EXIT_FAILURE);

	}
	
	close(err);
	return atoi(Answer);	
     
}
/*Is there related city in servant control*/
int check_relating_city(char* item,char* cities){

	char *ptr = strtok(cities, "-");

	while(ptr != NULL)
	{
		if(strcmp(item,ptr)==0){
			return 1;
		}

		ptr = strtok(NULL,  "-");
	}

	return 0;
}