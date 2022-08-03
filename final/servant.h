#ifndef SERVANT_H
#define SERVANT_H

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
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>

#define ENOUGH 1024

volatile sig_atomic_t FlagInterupt = 0;

typedef struct requst{
    char Tcount[200];
    char type[200];
    char date1[200];
    char date2[200];
    char city[200];

}Request;

typedef struct date{
    char day[5];
    char month[5];
    char year[5];
}Date;

typedef struct dataset{
    char street[1024];
    char type[1024];
    char city[1024];
    Date date;
    int surface;
    int price;
    int transaction_id;
}Dataset;


typedef struct node{
    Dataset data;
    struct node* next;
}Node;


typedef struct linkedList{

Node* (*getnode)(Dataset data);
void (*insert)(Dataset);

}LinkedList;

Request Client_Question;
int Total_Request=0;
Node* head;
Node* tail;

int servant_pid;

int PORT;
char IP_string[1024];
int value_result;

int Get_Pid();
void sigterm_handler(int signo);
Node* getnode(Dataset data);
void insert(Dataset data);
int parse_request(char* question);
void display_linked_list();
void display_message_7(int port,int pid);
void time_stamp();
void *Servant_Thread_job(void *request);
void create_own_socket(int own_PORT,char* own_IP);
Date determine_date(char* str_date);
void display_message_5(char* city1,char* city2,int pid);
void deleteList(struct node ** head_ref);
void printf_date(Date date);
void exit_and_free_SIGINT(int fd_accept,int SOCKET_VANT,int fd_date,int pid);
#endif