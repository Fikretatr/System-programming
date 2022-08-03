/*servant.c file*/

#include "servant.h"

int main (int argc, char *argv[]) {

	servant_pid=Get_Pid();
	char denotes[1024],directoryPath[1024];
	int opt;
	
	/*SIGINT Struct*/
	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}

	/*Get arguments
	Example:./servant -d directoryPath -c 10-19 -r IP -p PORT
	*/
	if(argc==1){
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	}
	if(argc != 9){
		fprintf(stderr,"Command line argument is failed\n");
		return -1;
	}
	while((opt = getopt(argc, argv, "d:c:r:p:")) != -1){
		switch(opt){
		    case 'd':
		        strcpy(directoryPath, optarg);
		        break;
		    case 'c':
		        strcpy(denotes, optarg);
		        break;
		    case 'r':
		    	strcpy(IP_string, optarg);
		        break;
		    case 'p':
		    	PORT = atoi(optarg);
		        break;               
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		//free(str_message);
		exit(EXIT_FAILURE);
	}

	
	/*Define data set*/
	Dataset data;
	
	
	DIR *d;
	DIR *d2;
    struct dirent *dir;
   	/*open directory that get as a argument*/
    d = opendir(directoryPath);

	char Il[500][100];
    int k=0;
    char full_path[1000];
   
 	
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
        	k++;
            full_path[0]='\0';
            strcat(full_path,directoryPath);
            strcat(full_path,"/");
            strcat(full_path,dir->d_name);
            //printf("%s\n",full_path);

            if(!(strcmp(dir->d_name, "."))||!(strcmp(dir->d_name, ".."))){
            	k--;
            }
            else{
            	strcpy(Il[k],dir->d_name);
            	
            }
        }
       	closedir(d);	
    }
    else{
    	perror("dirPATH fail");
    }

    int donetes_A;
	int donetes_B;
	char donetes_A_str[5];
	char donetes_B_str[5];

	char* token_donates = strtok(denotes, "-");
	strcpy(donetes_A_str,token_donates);
	token_donates = strtok(NULL, "-");;
	strcpy(donetes_B_str,token_donates);
	donetes_A=atoi(donetes_A_str);
	donetes_B=atoi(donetes_B_str);
   
 
   	/*Sorting City file name*/
    char s[100];//temp array

	for(int i =0;i<82;i++){
      for(int j=i+1;j<82;j++){
         if(strcmp(Il[i],Il[j])>0){
            strcpy(s,Il[i]);
            strcpy(Il[i],Il[j]);
            strcpy(Il[j],s);
         	}
        }
    }
  
	/*Message display etc.Servant 6672: loaded dataset, cities ADANA-ANKARA*/
	display_message_5(Il[donetes_A],Il[donetes_B],6672);

	
	char full_path_open[1000];
	char full_path2[1000];
	int fd_date;
	char temp_str[20];

	/*servant adds the data into the data structure related city range*/
    for(int i=donetes_A;i<=donetes_B;i++){
    	
		
		full_path2[0]='\0';
		strcat(full_path2,directoryPath);
		strcat(full_path2,"/");
    	strcat(full_path2,Il[i]);
    	d2 = opendir(full_path2);
   		

   		/*As program go through this loop ,fill the linked list node when visit throughout dir path*/
   		while((dir = readdir(d2)) != NULL){
   			if(!(strcmp(dir->d_name, "."))||!(strcmp(dir->d_name, ".."))){
            	//do nothing
            }
            else{
            	full_path_open[0]='\0';
            	strcpy(temp_str,dir->d_name);

            	strcat(full_path_open,full_path2);
            	strcat(full_path_open,"/");
            	strcat(full_path_open,dir->d_name);	
            	/*Fill the linkes list node*/
            	strcpy(data.city,Il[i]);
            	data.date=determine_date(temp_str);
            	/*Open date files*/
				fd_date = open(full_path_open, O_RDONLY);
				if(fd_date==-1){
					perror("File can not open");
					exit(EXIT_FAILURE);
				}
				
				char b;
				int bytes_read=1;
				char word[50];
				int parse_index=0;
				int off_set=0;//for word
				
				/*In this part read the file content and fill linked list*/
				while(bytes_read!=0){

				 	bytes_read = read(fd_date, &b, 1);
			       	if(bytes_read==-1){
						perror("can not open");
						exit(EXIT_FAILURE);
					}
			       	/*No more need add operation*/
					if(0==bytes_read){
						break;
					}

					if(b!='\n'){
						
						if(b!=' '){
							
							word[off_set]=b;
							off_set++;
						}
						else if(parse_index==0&&b==' '){
							/*get transition from file*/
							parse_index++;
							data.transaction_id=atoi(word);
							memset(word, 0, sizeof(word));
							word[0]='\0';
							off_set=0;

						}
						else if(parse_index==1&&b==' '){
							/*get type from file*/
							parse_index++;
							data.type[0]='\0';
							strcpy(data.type,word);
							memset(word, 0, sizeof(word));
							word[0]='\0';
							off_set=0;
						}
						else if(parse_index==2&&b==' '){
							/*get street from file*/
							parse_index++;
							strcpy(data.street,word);
							memset(word, 0, sizeof(word));
							word[0]='\0';
							off_set=0;
						}
						else if(parse_index==3&&b==' '){
							/*get surface from file*/
							parse_index++;
							data.surface=atoi(word);
							memset(word, 0, sizeof(word));
							word[0]='\0';
							off_set=0;
						}
						
						
					}
					else{
						/*get price from file*/
						parse_index++;
						data.price=atoi(word);
						memset(word, 0, sizeof(word));
						word[0]='\0';
						off_set=0;

						/*clean all array and insert data to the linked list*/
						memset(word, 0, sizeof(word));
						parse_index=0;
						word[0]='\0';
						off_set=0;
						insert(data);
					}
				}
			}
   		}
   		closedir(d2);
   	}

   	

	/*Create message about servant socket information and pid
	This message include socket unique port and pid*/
  
   	int Unique_port=servant_pid+3000;
	char message55[100];
	sprintf(message55,"! %d %d ",Unique_port,servant_pid);
	/*insert to the message relating to cities */
	for(int i=donetes_A;i<=donetes_B;i++){
		strcat(message55,Il[i]);
		strcat(message55,"-");
	}

	/*******************Create servant own Socket*************************/
	int SOCKET_VANT;
	SOCKET_VANT = socket(AF_INET, SOCK_STREAM,0);
	if(SOCKET_VANT == -1){
		perror("Fail Socket");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in addr2;
	int len_of_addr2 = sizeof(addr2);
	addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = INADDR_ANY;
    addr2.sin_port = htons(Unique_port);
    setsockopt(SOCKET_VANT,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));
    if(bind(SOCKET_VANT, (struct sockaddr*)(&addr2), sizeof(addr2)) == -1){
		close(SOCKET_VANT);
		perror("fail bind");
		exit(EXIT_FAILURE);
	}

	/*200 = BACK LOG*/
	if(listen(SOCKET_VANT, 1024) < 0){
		close(SOCKET_VANT);
		perror("fail listen");
		exit(EXIT_FAILURE);
	}
	
	/*********************************************************************/

	/*******************Open server socket********************************/
	int FD_SERVER;
	int fd_connect_server;
    FD_SERVER = socket(AF_INET,SOCK_STREAM,0);

    if(FD_SERVER==-1){
        perror("Error creating the server socket!");
        exit(EXIT_FAILURE);
    }
       
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_string);
    addr.sin_port = htons(PORT);

    /*connect server socket*/
    fd_connect_server = connect(FD_SERVER,(struct sockaddr*)&addr,sizeof(addr));
    if(fd_connect_server == -1){
        perror("fail connect");
        exit(EXIT_FAILURE);
    }
    /*send the information of servant own pid and unique port number*/
	send(FD_SERVER,message55 ,strlen(message55),0);
	/*no longer need servers socket because message sent it*/
	close(fd_connect_server);
	close(FD_SERVER);
	/**********************************************************************/
	/*Message Servant 6672: listening at port 16001*/
	display_message_7(Unique_port,servant_pid);

	pthread_t Threads_tid;
	int* Thread_Answer;
    int fd_accept;
	int err_check;
	/*Listen to the request come from server 
	and send request to servant thread*/
	char Buf[500];

	/*MAIN THREAD get connection come from server and sent it to own thread*/
	while(1){

		int err;
		

		/*accept connection come from server*/
		fd_accept = accept(SOCKET_VANT, (struct sockaddr*)&addr2,(socklen_t *)&len_of_addr2 );
		if(fd_accept==-1){
			// SIGINT signal check 
			if(FlagInterupt){
				exit_and_free_SIGINT(fd_accept,SOCKET_VANT,fd_date,Get_Pid());
			}

			perror("accept failed");
			exit(EXIT_FAILURE);

		}

		// SIGINT signal check 
		if(FlagInterupt){
			exit_and_free_SIGINT(fd_accept,SOCKET_VANT,fd_date,Get_Pid());
		}

		Total_Request++;

		memset(Buf, 0, sizeof(Buf));
		/*read the message come from server*/
		err=read(fd_accept,Buf,1024);
		if(err==-1){
			perror("read failed");
			exit(EXIT_FAILURE);

		}

		// SIGINT signal check 
		if(FlagInterupt){
			exit_and_free_SIGINT(fd_accept,SOCKET_VANT,fd_date,Get_Pid());
		}
		/*create server thread and send it request come from server*/
		if(pthread_create(&Threads_tid,NULL,Servant_Thread_job,(void*)Buf)){
			perror("Thread create failed");
			
			exit(EXIT_FAILURE);
		}

		// SIGINT signal check 
		if(FlagInterupt){
			exit_and_free_SIGINT(fd_accept,SOCKET_VANT,fd_date,Get_Pid());
		}
		/*wait for the thread return result*/
		err_check=pthread_join(Threads_tid, (void **)&Thread_Answer );
		if(err_check!=0){
			perror("Thread join failed");
			
			exit(EXIT_FAILURE);
		}

		
		/*Send the answer to server part*/
		char store_answer[1024];

		memset(store_answer, 0, sizeof(store_answer));

		sprintf(store_answer,"%d", *Thread_Answer);

		send(fd_accept, store_answer, strlen(store_answer),0);

		memset(store_answer, 0, sizeof(store_answer));
		
		memset(Buf, 0, sizeof(Buf));

	}

	close(fd_accept);
	close(SOCKET_VANT);
	close(fd_date);
	deleteList(&head);
	return 0;

}

void exit_and_free_SIGINT(int fd_accept,int SOCKET_VANT,int fd_date,int pid){

	char *str_message;
	str_message = (char *) malloc(300);
	int fail;
	//memset(str_message6, 0, sizeof(str_message6));
	sprintf(str_message,"Servant %d: termination message received, handled %d requests in total.\n",pid,Total_Request);


	fail=write(1,str_message,strlen(str_message));
	if(fail==-1){
		perror("write fail");
		close(fd_accept);
		close(SOCKET_VANT);
		close(fd_date);
		deleteList(&head);
		free(str_message);
		exit(EXIT_FAILURE);
	}

	close(fd_accept);
	close(SOCKET_VANT);
	close(fd_date);
	deleteList(&head);
	free(str_message);
	exit(EXIT_FAILURE);



}

/*Thread function provide to scan linked list that 
contain data set information and return transition information*/
void *Servant_Thread_job(void *request){

    int NOF_Transaction=0;
    Date  date1_quest;
    Date  date2_quest;
    Request Server_Question;
    char *val_p = (char *) request;
    char temp_req[strlen(val_p)];

    Node* last;
    last=head;
    int set=0;
    
    /*If city exist*/
    if(parse_request(val_p)==0){

        strcpy(temp_req,val_p);
        char *str_token = strtok(temp_req," ");
        
        while(str_token != NULL)
        {
            set++;
            if(set==1){
                strcpy(Server_Question.Tcount,str_token);
            }
            else if(set==2){
                strcpy(Server_Question.type,str_token);
            }
            else if(set==3){
                strcpy(Server_Question.date1,str_token);
            }
            else if(set==4){
                strcpy(Server_Question.date2,str_token);
            }
            else if(set==5){
                strcpy(Server_Question.city,str_token);
            }
          
            str_token = strtok(NULL," ");
        }

        /*Scan Linked List*/
        while(last!=NULL){
            /*Compare City , Type and Date*/
            if(0==strcmp(last->data.city,Server_Question.city) && 0==strcmp(last->data.type,Server_Question.type)){
                
                date1_quest=determine_date(Server_Question.date1);
                date2_quest=determine_date(Server_Question.date2);
                /*Compare Year*/
                if(strcmp(last->data.date.year,date2_quest.year)<0 && strcmp(last->data.date.year,date1_quest.year)>0){

                    NOF_Transaction++;

                }
                else if( strcmp(last->data.date.year,date2_quest.year)==0 || strcmp(last->data.date.year,date1_quest.year)==0){

                    if(strcmp(last->data.date.month,date2_quest.month)<0 &&  strcmp(last->data.date.month,date1_quest.month)>0){

                        NOF_Transaction++;
                    }
                    else if(strcmp(last->data.date.month,date2_quest.month)==0 || strcmp(last->data.date.month,date1_quest.month)==0){

                        if(strcmp(last->data.date.day,date2_quest.day)<0 && strcmp(last->data.date.day,date1_quest.day)>0){

                            NOF_Transaction++;
                        }
                        else if(strcmp(last->data.date.day,date2_quest.day)==0 && strcmp(last->data.date.day,date1_quest.day)==0){
                            NOF_Transaction++;
                        }
                    }
                }
            }
            
            last=last->next;
        }

	}/*WITHOUT CITY SEARCHING ALL NODES ONLY CHECK TYPE*/
    else{

        strcpy(temp_req,val_p);
        char *str_token = strtok(temp_req," ");
        /*Parsing Request*/
        while(str_token != NULL){
            set++;
            if(set==1){
                strcpy(Server_Question.Tcount,str_token);
            }
            else if(set==2){
                strcpy(Server_Question.type,str_token);
            }
            else if(set==3){
                strcpy(Server_Question.date1,str_token);
            }
            else if(set==4){
                strcpy(Server_Question.date2,str_token);
            }
            str_token = strtok(NULL," ");
        }

        
        /*Scan Linked List without city*/
        while(last!=NULL){

            if(0==strcmp(last->data.type,Server_Question.type)){
                
                date1_quest=determine_date(Server_Question.date1);
                date2_quest=determine_date(Server_Question.date2);

                if(strcmp(last->data.date.year,date2_quest.year)<0 && strcmp(last->data.date.year,date1_quest.year)>0){

                    NOF_Transaction++;

                }
                else if(strcmp(last->data.date.year,date2_quest.year)==0 || strcmp(last->data.date.year,date1_quest.year)==0){

                    if(strcmp(last->data.date.month,date2_quest.month)<0 &&  strcmp(last->data.date.month,date1_quest.month)>0){

                        NOF_Transaction++;
                    }
                    else if(strcmp(last->data.date.month,date2_quest.month)==0 || strcmp(last->data.date.month,date1_quest.month)==0){

                        if(strcmp(last->data.date.day,date2_quest.day)<0 && strcmp(last->data.date.day,date1_quest.day)>0){

                            NOF_Transaction++;
                        }
                        else if(strcmp(last->data.date.day,date2_quest.day)==0 && strcmp(last->data.date.day,date1_quest.day)==0){
                            NOF_Transaction++;
                        }
                    }
                }
            }

            last=last->next;
        }
    }

    /*Return Result*/
   
    value_result = NOF_Transaction;
    NOF_Transaction = 0;
  	
    return (void*)&value_result;
}

/*Message : Servant 6672: listening at port 16001”*/
void display_message_7(int port,int pid){

    time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

    char *str_message6;
    str_message6 = (char *) malloc(200);
    int fail;
    sprintf(str_message6,"%s,Servant %d: listening at port %d\n",time_info,pid,port);
    fail =write (1, str_message6,strlen(str_message6));
    if(fail == -1){
        perror("write syscall for display_message1");
        free(str_message6);  
        exit(EXIT_FAILURE);
    }
    free(str_message6);  

}

/*Get Curent Node from linked list*/
Node* getnode(Dataset data){
    struct node* newnode = malloc(sizeof(struct node));

    strcpy(newnode->data.city,data.city);
    strcpy(newnode->data.type,data.type);
    strcpy(newnode->data.street,data.street);

    newnode->data.surface=data.surface;
    newnode->data.price=data.price;
    newnode->data.transaction_id=data.transaction_id;
    newnode->data.date=data.date;
    

    newnode->next = NULL;
    return newnode;
};

/*Insert item to linked list*/
void insert(Dataset data){
    struct node* newnode = getnode(data);
    struct node* last = head;
    newnode->next = NULL;
    if (head == NULL)
    {
       head = newnode;
       return;
    }
    while (last->next != NULL)
        last = last->next;

    last->next = newnode;
}

/*Delete linked list and free*/
void deleteList(Node ** head_ref){
    Node* current = *head_ref;
    Node* next;

    while(current != NULL){

  	
        next = current->next;
        //free(&(current->data.street));
        free(current);
        current = next;
    }
    *head_ref = NULL;
}

/*CHAR TO DATE TYPE converter*/
Date determine_date(char* str_date){
    Date date;
    char get_date_info[10];
    char temp_str[strlen(str_date)];
    strcpy(temp_str,str_date);

    char* token = strtok(temp_str, "-");
    int k=0;
    
	while (token != NULL) {
        get_date_info[0]='\0';
        k++;
        strcpy(get_date_info,token);
        if(k==1){
            strcpy(date.day,get_date_info);
          
        }
        else if(k==2){
            strcpy(date.month,get_date_info);
            
        }
        else{
            strcpy(date.year,get_date_info);
           
        }
       
       	token = strtok(NULL, "-");
        
    }

    return date;
}

void printf_date(Date date){

    printf("%s/%s/%s\n",date.day,date.month,date.year);
}

/*char* question[1024]="transactionCount TARLA 01-01-2073 30-12-2074 ADANA"*/
/*if there is no city return 1 otherwise 0*/
int parse_request(char* question){

    int index=0;
    char temp_req[2048];
	strcpy(temp_req,question);
    char *ptr = strtok(temp_req," ");
    
    while(ptr != NULL)
    {
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

    if(index==4){
        return 1;
    }
	
	return 0;
    
}
/*Display linked list element*/
void display_linked_list(){

    tail=head;
    int i=0;

    //printf("%s\n",tail->data.city );
    while(tail!=NULL){

        if(0==strcmp(tail->data.city,"IZMIR")){
            printf("Node%d:%s,%s/%s/%s,%d,%s,%s,%d,%d\n",i,tail->data.city,tail->data.date.day
                ,tail->data.date.month,tail->data.date.year,tail->data.transaction_id,tail->data.type
                ,tail->data.street,tail->data.surface,tail->data.price );
            
            i++;
        }
        tail=tail->next;
    }
    
}
/*function provide to get time info*/
void time_stamp(){

    time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

    printf("%s\n", time_info);
}

/*Request arrived “transactionCount TARLA 01-01-2000 10-01-2021 ISTANBUL”*/
void display_message_5(char* city1,char* city2,int pid){

    time_t TM;
    char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
    Tmobj = localtime(&TM);
    strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

    char *str_message;
    str_message = (char *) malloc(200);
    int fail;
    sprintf(str_message,"%s,Servant %d: loaded dataset, cities %s-%s\n",time_info,pid,city1,city2);
    fail =write (1, str_message,strlen(str_message));
    if(fail == -1){
        perror("write syscall for display_message1");
        free(str_message);  
        exit(EXIT_FAILURE);
    }
    free(str_message);  

}
void sigterm_handler(int signo){
    if(signo == SIGINT){
        FlagInterupt = 1;
    }
}
/*get pid using proc*/
int Get_Pid(){

	int desc;
    size_t bytesread;
    
    desc = open("/proc/self/status", O_RDONLY);
    if(desc == -1){
    	perror("fail read proc");
		exit(EXIT_FAILURE);
    }
    char character;
    char buf[1024];
    int index=0;
    int flag=1;
    
	/*Read requestfile*/
	for(;;){
		
		while(((bytesread = read(desc,&character,1))==-1)&&(errno==EINTR));
		if(bytesread<=0){
			break;
		}
		if(character!='\n'&&character!=' '&&character!='\t'){
			buf[index]=character;
			index++;
		}
		else{
			if(flag==0){
				break;
			}
			if(strcmp(buf,"Pid:")==0){

				flag=0;
			}
			index=0;
			memset(buf, 0, sizeof(buf));
		}

	}

	int pid_int = atoi(buf);

	return pid_int;
	
}