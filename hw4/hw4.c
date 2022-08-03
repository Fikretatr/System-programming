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
#include <sys/mman.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>

#define number_of_sem 2

int sem_get_id;
int input_C;
int input_N;
volatile sig_atomic_t FlagInterupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT)
		FlagInterupt = 1;
}

void display_before_consuming(int id_consumer,int iteration_number,int number_of_1,int number_of_2);
void display_after_consuming(int id_consumer,int iteration_number,int number_of_1,int number_of_2);
void display_after_delivery(char read_char,int number_of_1,int number_of_2);
void display_before_delivery(char read_char,int number_of_1,int number_of_2);
void display_invalid_input_file();
void sup_left();
void consumer_left(int id_consumer);


void *consumer_jobs(void *arg){

	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}
	

	struct sembuf sem_buf[2];
	int Cons_id = *((int *) arg);
	
	int sem1_val=0,sem2_val=0;
	//For '1'
	sem_buf[0].sem_num=0;
	sem_buf[0].sem_op=-1;
	sem_buf[0].sem_flg=0;
	//For '2'
	sem_buf[1].sem_num=1;
	sem_buf[1].sem_op=-1;
	sem_buf[1].sem_flg=0;

	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		semctl(sem_get_id, 1, IPC_RMID);
		exit(EXIT_FAILURE);
	}

	for(int j=0;j<input_N;j++){
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
		sem1_val = semctl(sem_get_id, 0,GETVAL);
		sem2_val = semctl(sem_get_id, 1,GETVAL);
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
		display_before_consuming(Cons_id,j,sem1_val,sem2_val);
		
		if(semop(sem_get_id,sem_buf,2) == -1){
			perror("Semop Error");
			exit(EXIT_FAILURE);
		}
		
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
		sem1_val = semctl(sem_get_id, 0,GETVAL);
		sem2_val = semctl(sem_get_id, 1,GETVAL);
		display_after_consuming(Cons_id,j,sem1_val,sem2_val);
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
	}

	consumer_left(Cons_id);
	return NULL;

}

void *supplier(void *arg){

	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}
	
	char buf;
	int fd;
	char* inputFilePath;
	ssize_t nbytes;
	int sem1_val=0,sem2_val=0;
	struct sembuf sembuf;
	sembuf.sem_num=0;
	sembuf.sem_op=1;
	sembuf.sem_flg=0;

	inputFilePath=(char *)arg;

	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		semctl(sem_get_id, 1, IPC_RMID);
		exit(EXIT_FAILURE);
	}

	//open input file path
	fd = open(inputFilePath, O_RDONLY);
	if(fd==-1){
		fprintf(stderr,"failed open input file!\n");
		exit(EXIT_FAILURE);
	}

	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		close(fd);
		semctl(sem_get_id, 1, IPC_RMID);
		exit(EXIT_FAILURE);
	}
	for(;;){
		//EOF
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			close(fd);
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
		if((nbytes = read(fd,&buf,1))<=0){
			break;
		}
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			close(fd);
			semctl(sem_get_id, 1, IPC_RMID);
			exit(EXIT_FAILURE);
		}
		else{
			if(buf=='1'){
				sembuf.sem_num=0;
				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				sem1_val = semctl(sem_get_id, 0,GETVAL);
				sem2_val = semctl(sem_get_id, 1,GETVAL);
				display_before_delivery(buf,sem1_val,sem2_val);
				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}

				if(semop(sem_get_id,&sembuf,1) == -1){
					perror("Semop Error");
					exit(EXIT_FAILURE);
				}

				
				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				sem1_val = semctl(sem_get_id, 0,GETVAL);
				sem2_val = semctl(sem_get_id, 1,GETVAL);
				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				display_after_delivery(buf,sem1_val,sem2_val);
				
			}
			else if(buf=='2'){
				sembuf.sem_num=1;

				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}

				sem1_val = semctl(sem_get_id, 0,GETVAL);
				sem2_val = semctl(sem_get_id, 1,GETVAL);
				display_before_delivery(buf,sem1_val,sem2_val);

				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}

				if(semop(sem_get_id,&sembuf,1) == -1){
					perror("Semop Error");
					exit(EXIT_FAILURE);
				}

				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				
				sem1_val = semctl(sem_get_id, 0,GETVAL);
				sem2_val = semctl(sem_get_id, 1,GETVAL);

				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				display_after_delivery(buf,sem1_val,sem2_val);
				
			}
			else if(buf != '\n' && buf != '\0'){
				if(FlagInterupt){
					write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
					close(fd);
					semctl(sem_get_id, 1, IPC_RMID);
					exit(EXIT_FAILURE);
				}
				display_invalid_input_file();
				exit(EXIT_FAILURE);

			}
		}
	}
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		close(fd);
		semctl(sem_get_id, 1, IPC_RMID);
		exit(EXIT_FAILURE);
	}
	sup_left();
	close(fd);

	return (void *) 1;

}

int main (int argc, char *argv[]) {

setbuf(stdout, NULL);
union semun {
int val; /*individual semaphore value*/
struct semid_ds * buf ; /*semaphore data structure*/
unsigned short* array; /*multiple semaphore values*/
struct seminfo* __buf; /* linux specific */
};

struct sigaction sigterm_action;
memset(&sigterm_action, 0, sizeof(sigterm_action));
sigterm_action.sa_handler = &sigterm_handler;

if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
    perror("sigaction");
}

int opt;
char inputFilePath[1024];
char *str_message;
str_message = (char *) malloc(200);

if(argc==1){
	fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
}
if(argc != 7){
	fprintf(stderr,"Command line argument is failed\n");
	return -1;
}
while((opt = getopt(argc, argv, "C:N:F:")) != -1){
	switch(opt){
	    case 'F':
	        strcpy(inputFilePath, optarg);
	        break;
	    case 'C':
	    	input_C = atoi(optarg);
	      	break;
	    case 'N':
	    	input_N = atoi(optarg);
	     	break;    

	}
}


if(input_C<4){
	strcpy(str_message,"C must be greater than 4\n");
	write (STDOUT_FILENO, str_message,strlen(str_message));
	free(str_message); 
	exit(EXIT_FAILURE);
}

if(input_N<1){
	strcpy(str_message,"N must be greater than 1\n");
	write (STDOUT_FILENO, str_message,strlen(str_message));
	free(str_message);
	exit(EXIT_FAILURE); 
}

int fd_temp;
int byte_read;
char buf;
//open input file path
fd_temp = open(inputFilePath, O_RDONLY);
if(fd_temp==-1){
	write(1,"failed open input file!\n",strlen(str_message));
	exit(EXIT_FAILURE);
}
int counter=0;
for(;;){
		//EOF
		if(FlagInterupt){
			write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
			close(fd_temp);
			free(str_message);
			exit(EXIT_FAILURE);
		}
		if((byte_read = read(fd_temp,&buf,1))<=0){
			break;
		}
		if(buf=='\r'||buf=='\n'||buf=='\0'){
			
		}
		else if(buf=='1'||buf=='2'){
			counter++;
		}
		else{
			close(fd_temp);
			free(str_message);
			display_invalid_input_file();
			exit(EXIT_FAILURE);
		}
}

if((counter)!=2*(input_C*input_N)){
	close(fd_temp);
	free(str_message);
	display_invalid_input_file();
	exit(EXIT_FAILURE); 
}		

close(fd_temp);

if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	free(str_message);
	exit(EXIT_FAILURE);
}

//SystemV implementation

if((sem_get_id=semget(IPC_PRIVATE, number_of_sem, IPC_CREAT|IPC_EXCL|0600))==-1){
	strcpy(str_message,"sem get failed\n");
	write (STDOUT_FILENO, str_message,strlen(str_message));
	free(str_message);
	exit(EXIT_FAILURE); 
}

if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	semctl(sem_get_id, 1, IPC_RMID);
	free(str_message);
	exit(EXIT_FAILURE);
}
//Thread Creat
pthread_t tid_supplier;
pthread_t Consumer[input_C];
int C_index[input_C];
int error;
pthread_attr_t attr;

error = pthread_attr_init(&attr);
if(error != 0){
	perror("pthread_attr_init");
	exit(EXIT_FAILURE);
}
error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(error != 0){
		perror("pthread_attr_setdetachstate");
		exit(EXIT_FAILURE);
}

error=pthread_create(&tid_supplier, &attr, supplier,inputFilePath);
if(error!=0){
	strcpy(str_message,"Thread create failed supplier\n");
	write (STDOUT_FILENO, str_message,strlen(str_message));
	free(str_message);
	exit(EXIT_FAILURE);
}

//pthread_detach(tid_supplier);


if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	semctl(sem_get_id, 1, IPC_RMID);
	
	free(str_message);
	exit(EXIT_FAILURE);
}

for(int i=0;i<input_C;i++){
	C_index[i]=i;
	error=pthread_create(&Consumer[i], NULL,consumer_jobs,(void *)&C_index[i]);
	if(error!=0){
		strcpy(str_message,"Pthread create consumer\n");
		write (STDOUT_FILENO, str_message,strlen(str_message));
		free(str_message);
		exit(EXIT_FAILURE);
	}
}
if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	semctl(sem_get_id, 1, IPC_RMID);
	
	free(str_message);
	exit(EXIT_FAILURE);
}
//Thread Join 
for(int i=0;i<input_C;i++){
	error=pthread_join(Consumer[i], NULL);
	if(error!=0){
		strcpy(str_message,"Join failed\n");
		write (STDOUT_FILENO, str_message,strlen(str_message));
		free(str_message);
		exit(EXIT_FAILURE);
	}
}
if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	semctl(sem_get_id, 1, IPC_RMID);
	
	free(str_message);
	exit(EXIT_FAILURE);
}


if(FlagInterupt){
	write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
	semctl(sem_get_id, 1, IPC_RMID);
	
	free(str_message);
	exit(EXIT_FAILURE);
}
error = pthread_attr_destroy(&attr);
if(error != 0){
	perror("pthread_attr_destroy");
	exit(EXIT_FAILURE);
}

//Destroy
if(semctl(sem_get_id, 1, IPC_RMID)==-1){
	strcpy(str_message,"Semctl destroy failed\n");
	write (STDOUT_FILENO, str_message,strlen(str_message));
	free(str_message);
	exit(EXIT_FAILURE); 
}
free(str_message);
}

void display_before_consuming(int id_consumer,int iteration_number,int number_of_1,int number_of_2){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message0;
	str_message0 = (char *) malloc(200);
	
	sprintf(str_message0,"%s,Consumer-%d at iteration %d (waiting).Current amounts: %d x ‘1’, %d x ‘2’.\n",time_info,id_consumer,iteration_number,number_of_1,number_of_2);
	fail =write (STDOUT_FILENO, str_message0,strlen(str_message0));
	if(fail == -1){
		perror("write syscall");
		free(str_message0);  
		exit(EXIT_FAILURE);
	} 
	
	free(str_message0); 

}

void display_after_consuming(int id_consumer,int iteration_number,int number_of_1,int number_of_2){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);


	char *str_message1;
	str_message1 = (char *) malloc(200);
	
	sprintf(str_message1,"%s,Consumer-%d at iteration %d (consumed).Post-consumption amounts:: %d x ‘1’, %d x ‘2’.\n",time_info,id_consumer,iteration_number,number_of_1,number_of_2);
	fail =write(STDOUT_FILENO, str_message1,strlen(str_message1));
	if(fail == -1){
		perror("write syscall");
		free(str_message1);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message1);  

}

void display_before_delivery(char read_char,int number_of_1,int number_of_2){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);


	char *str_message2;
	str_message2 = (char *) malloc(200);
	
	sprintf(str_message2,"%s,Supplier: read from input a ‘%c’. Current amounts: %d x ‘1’, %d x ‘2’\n",time_info,read_char,number_of_1,number_of_2);
	fail =write (STDOUT_FILENO, str_message2,strlen(str_message2));
	if(fail == -1){
		perror("write syscall");
		free(str_message2);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message2); 

}

void display_after_delivery(char read_char,int number_of_1,int number_of_2){

	setbuf(stdout, NULL);	
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);
	
	
	char *str_message3;
	str_message3 = (char *) malloc(200);
	
	sprintf(str_message3,"%s,Supplier: delivered a ‘%c’. Post-delivery amounts: %d x ‘1’, %d x ‘2’.\n",time_info,read_char,number_of_1,number_of_2);
	fail =write (STDOUT_FILENO, str_message3,strlen(str_message3));
	if(fail == -1){
		perror("write syscall");
		free(str_message3);  
		exit(EXIT_FAILURE);
	} 
	
	free(str_message3); 

}

void display_invalid_input_file(){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);


	char *str_message4;
	str_message4 = (char *) malloc(200);
	
	sprintf(str_message4,"%s,Invalid input file.\n",time_info);
	fail =write (STDOUT_FILENO, str_message4,strlen(str_message4));
	if(fail == -1){
		perror("write syscall");
		free(str_message4);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message4); 
}

void sup_left(){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message5;
	str_message5 = (char *) malloc(200);
	
	sprintf(str_message5,"%s,The Supplier has left..\n",time_info);
	fail =write (STDOUT_FILENO, str_message5,strlen(str_message5));
	if(fail == -1){
		perror("write syscall");
		free(str_message5);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message5); 
}

void consumer_left(int id_consumer){
	
	setbuf(stdout, NULL);
	int fail;
	time_t TM;
	char time_info[30];
    struct tm* Tmobj;
    TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message6;
	str_message6 = (char *) malloc(200);
	
	sprintf(str_message6,"%s,Consumer-%d has left.\n",time_info,id_consumer);
	fail =write (STDOUT_FILENO, str_message6,strlen(str_message6));
	if(fail == -1){
		perror("write syscall");
		free(str_message6);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message6); 
}

