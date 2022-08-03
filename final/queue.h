#ifndef QUEUE_H
#define QUEUE_H

#include<stdlib.h>

struct node
{
    struct node  *next;
    int *client_socket;
};

typedef struct node node_t;

void enqueue(int * client_socket);
int * dequeue();
void empty_queue();

node_t* head=NULL;
node_t* tail=NULL;

void enqueue(int * client_socket){

    node_t * new_node= malloc(sizeof(node_t));
    new_node->client_socket=client_socket;
    new_node->next=NULL;
    if (NULL==tail){
        head=new_node;
    }
    else{
        tail->next=new_node;
    }

    tail=new_node;
}


int * dequeue(){

    if (NULL==head){
        return NULL;
    }
    else{
        int * result=head->client_socket;
        node_t * temp=head;
        head=head->next;
        if (NULL==head){
            tail=NULL;
        }
        free(temp);
        return result;
    }
    return NULL;
}

void empty_queue(){
    while(NULL!=dequeue());
}
#endif