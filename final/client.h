#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<arpa/inet.h>

#define BLKSIZE 2048

volatile sig_atomic_t FlagInterupt = 0;

void display_message1(int nof_thread);
void display_message2(int nof_thread);
void display_message3(int nof_thread,char *thread_req);
void display_message4(int nof_thread);
void display_message5(int nof_thread,char *answer,char* thread_request);
void thread_resp_connect();
void *Thread_job(void *received_struct);
void time_stamp();
int PORT;

char IP_string[1024];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

int arrived = 0;
int Total_Number_Of_Threads=0;

struct sockaddr_in addr;

void sigterm_handler(int signo){
	if(signo == SIGINT){
		FlagInterupt = 1;
	}
}

struct Cliend_Thread_Struct{
	int index;
	char request[1024];
	char respond[1024];
};





