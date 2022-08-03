#ifndef SERVER_H
#define SERVER_H

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
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "queue.h"
#include<arpa/inet.h>
#define BUFSIZE 1024

volatile sig_atomic_t FlagInterupt = 0;

void sigterm_handler(int signo){
	if(signo == SIGINT){
		FlagInterupt = 1;
	}
}

struct Server_Thread_Struct{
	int index;
	char request[1024];
	
};

typedef struct Servants{
	int port_sr;
	char city1[2048];
	char city2[2048];
	int pid;
	int occur_fd;
	
}servants;

typedef struct requst{
    char Tcount[100];
    char type[100];
    char date1[100];
    char date2[100];
    char city[100];

}Request;


int TOTAL_REQUEST=0;//keep total number of request 
int size_servant=0;//keep number of servant 
servants* servants_info;//keep servants infos 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

int Connect_Servant(int which_port);
int check_relating_city(char* item,char* cities);
void connect_and_send(int which_port,char *request,int fd,int check,char* wit_city,int pid_servant);
int connect_and_send_ALL(int which_port,char *request);
void display_message2(char* request);
void display_message1(int index_servant,int port,char* city1,char* city2);
void display_message6(char* city1,int pid,int port);
void display_message10(int pid);
void display_message11();
void display_message3(char* msg);
int exist_city_in_servant(char* whole_city,char* req_city);
void get_connet(int Client_Socket);
void exit_and_free_SIGINT(int Client_Socket,int fd_socket);
void *Server_Thread_job(void *arg);


#endif