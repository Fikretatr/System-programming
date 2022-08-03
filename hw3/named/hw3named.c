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

#define SHARED_MEMORY_FILENAME "MySharedMemory2"

void initally_print(int index_child,pid_t child_pid,char* ingredient1,char* ingredient2,char* SM ){

	fprintf(stdout, "chef%d (pid %d) is waiting for %s and %s ->Contents of the character array: %c%c \n",index_child,child_pid,ingredient1,ingredient2,SM[0],SM[1]);

}

void After_obtaining_the_ingredients_print(int index_child,pid_t child_pid,char ingredient,char* SM ){

	fprintf(stdout, "chef%d (pid %d) has taken the %c ->Contents of the character array: %c%c \n",index_child,child_pid,ingredient,SM[0],SM[1]);

}

void Dessert_Display(int index_child,pid_t child_pid,char* SM ){

	fprintf(stdout, "chef%d (pid %d) is preparing the dessert ->Contents of the character array: %c%c \n",index_child,child_pid,SM[0],SM[1]);
	fprintf(stdout, "chef%d (pid %d) has delivered the dessert ->Contents of the character array: %c%c \n",index_child,child_pid,SM[0],SM[1]);
}


void The_wholesaler_display(pid_t wholesaler,char* ingredient1,char* ingredient2,char* SM ){


	fprintf(stdout, "the wholesaler (pid %d) delivers %s and %s ->Contents of the character array: %c%c \n",wholesaler,ingredient1,ingredient2,SM[0],SM[1]);
	fprintf(stdout, "the wholesaler (pid %d) is waiting for the dessert ->Contents of the character array: %c%c \n",wholesaler,SM[0],SM[1]);



}

void At_exit_print(int index_child,pid_t child_pid,char* SM){

fprintf(stdout,"chef%d (pid %d) is exiting ->Contents of the character array: %c%c \n",index_child,child_pid,SM[0],SM[1]);

}

void whosaler_exit_display(pid_t wholesaler,int total_desert,char* SM){



fprintf(stdout, "the wholesaler (pid %d) is done (total desserts: %d) ->Contents of the character array: %c%c \n",wholesaler,total_desert,SM[0],SM[1]);

}

void whosaler_left_display(pid_t wholesaler,char* SM){


fprintf(stdout, "the wholesaler (pid %d) has obtained the dessert and left ->Contents of the character array: %c%c \n",wholesaler,SM[0],SM[1]);


}



/*
void handler(int signum){

	if (signum == SIGINT){
			fprintf(stdout, "CTRL-C ,free all of P's resources and children \n");
			
				sem_close(MF2);
	sem_unlink("chef0");

	sem_close(MS2);
	sem_unlink("chef1");

	sem_close(MW2);
	sem_unlink("chef2");

	sem_close(SW2);
	sem_unlink("chef3");

	sem_close(SF2);
	sem_unlink("chef4");

	sem_close(FW2);
	sem_unlink("chef5");

	sem_close(milkSEM);
	sem_unlink("milk");

	sem_close(flourSEM);
	sem_unlink("flour");

	sem_close(walnutSEM);
	sem_unlink("walnut");

	sem_close(sugarSEM);
	sem_unlink("sugar");

	sem_close(WholesalerSEM);
	sem_unlink("Wholesaler1");

	sem_close(helperMutex);
	sem_unlink("helperMutex");

	sem_close(MflagSEM);
	sem_unlink("MflagSEM");

	sem_close(FflagSEM);
	sem_unlink("FflagSEM");

	sem_close(WflagSEM);
	sem_unlink("WflagSEM");

	sem_close(SflagSEM);
	sem_unlink("SflagSEM");

	

    
	close(fd_inputfile);
	close(fd_shared_mem);//nolonger need
	free(Tempbuffer);
	free(Buffer_text);
	            
		   
			kill(0, SIGINT);
	   		kill(child_pids, SIGKILL);
	}
}
*/

struct Chef {
	char leak[2];
	pid_t pid;
	char ingredients1[100];
	char ingredients2[100];
	int number_of_desert;
	char Taken_item;
}chefs[10];




int main (int argc, char *argv[]) {
	
	chefs[0].leak[0]='W';
	chefs[0].leak[1]='S';
	strcpy(chefs[0].ingredients1,"walnuts");
	strcpy(chefs[0].ingredients2,"sugar");
	chefs[0].number_of_desert=0;


	chefs[1].leak[0]='F';
	chefs[1].leak[1]='W';
	strcpy(chefs[1].ingredients1,"flour");
	strcpy(chefs[1].ingredients2,"walnuts");
	chefs[1].number_of_desert=0;

	chefs[2].leak[0]='S';
	chefs[2].leak[1]='F';
	strcpy(chefs[2].ingredients1,"sugar");
	strcpy(chefs[2].ingredients2,"flour");
	chefs[2].number_of_desert=0;

	chefs[3].leak[0]='M';
	chefs[3].leak[1]='F';
	strcpy(chefs[3].ingredients1,"milk");
	strcpy(chefs[3].ingredients2,"flour");
	chefs[3].number_of_desert=0;

	chefs[4].leak[0]='M';
	chefs[4].leak[1]='W';
	strcpy(chefs[4].ingredients1,"milk");
	strcpy(chefs[4].ingredients2,"walnuts");
	chefs[4].number_of_desert=0;

	chefs[5].leak[0]='S';
	chefs[5].leak[1]='M';
	strcpy(chefs[5].ingredients1,"sugar");
	strcpy(chefs[5].ingredients2,"milk");
	chefs[5].number_of_desert=0;

	//char Chef_lack[6][2][10]={{"walnuts","sugar"},{"flour","walnuts"},{"sugar","flour"},{"milk","flour"},{"milk","walnuts"},{"sugar","milk"}};

	//printf("%s %s %s %s\n", Chef_lack[0][0], Chef_lack[0][1], Chef_lack[1][0], Chef_lack[1][1]);

	int opt;
	char inputFilePath[1024];
	char name[1024];
	int fd_inputfile;
	int fd_shared_mem;;
	int bytes_read;
	size_t len; /*Size of shared memory object*/
	char *myadress;
	int first_come=0;
	
	//struct stat sb;
	
	/*
	struct sigaction action;
	memset(&action, 0 ,sizeof(action));
	action.sa_handler = &handler;
	sigaction(SIGINT,&action, &old_action);
	*/

	if(argc==1)
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	   

	if(argc != 5){
	    fprintf(stderr,"Command line argument is failed\n");
	    return -1;
	}
	
	while((opt = getopt(argc, argv, "i:n:")) != -1){
	    switch(opt){
	        case 'i':
	            strcpy(inputFilePath, optarg);
	            break;
	        case 'n':
	            strcpy(name, optarg);
	            break;
	     }
	}


	//open shared mem
	fd_shared_mem=shm_open(SHARED_MEMORY_FILENAME,O_RDWR|O_CREAT,0);
	if(fd_shared_mem==-1){
		fprintf(stderr,"shm_open failed\n");
		exit(EXIT_FAILURE);
	}
	//open input file path
	fd_inputfile = open(inputFilePath, O_RDONLY);
	if(fd_inputfile==-1){
		fprintf(stderr,"failed open input file!\n");
		exit(EXIT_FAILURE);
	}

	shm_unlink(SHARED_MEMORY_FILENAME);


    
  /*Resize object to hold string*/
  len=3*sizeof(char);
  if(ftruncate(fd_shared_mem,len)==-1){
  	fprintf(stderr,"failed ftruncate!\n");
  	exit(EXIT_FAILURE);
  }

  myadress = mmap(NULL,len,PROT_READ | PROT_WRITE,MAP_SHARED,fd_shared_mem,0);
	if(myadress == MAP_FAILED)
	{
		fprintf(stderr,"can not mmap\n");
		exit(EXIT_FAILURE);
	}
	/*fd no longer needed*/
	if(close(fd_shared_mem)==-1){
		fprintf(stderr,"close fd sm\n");
		exit(EXIT_FAILURE);
	}

	

	//Create SEMAPHORE

	//FOR CHEF AND WHOSALER SEM
	sem_t *MF2,*MS2,*MW2,*SW2,*SF2,*FW2,*WholesalerSEM;
	MF2 = sem_open("chef0", O_CREAT | O_EXCL, 0644, 0);
	MS2 = sem_open("chef1", O_CREAT | O_EXCL, 0644, 0);
	MW2 = sem_open("chef2", O_CREAT | O_EXCL, 0644, 0);
	SW2 = sem_open("chef3", O_CREAT | O_EXCL, 0644, 0);
	SF2 = sem_open("chef4", O_CREAT | O_EXCL, 0644, 0);
	FW2 = sem_open("chef5", O_CREAT | O_EXCL, 0644, 0);
	WholesalerSEM = sem_open("Wholesaler1", O_CREAT | O_EXCL, 0644, 1);


	//INGREDIENT SEM
	sem_t *milkSEM,*flourSEM,*walnutSEM,*sugarSEM;
	milkSEM = sem_open("milk", O_CREAT | O_EXCL, 0644, 0);
	flourSEM = sem_open("flour", O_CREAT | O_EXCL, 0644, 0);
	walnutSEM = sem_open("walnut", O_CREAT | O_EXCL, 0644, 0);
	sugarSEM = sem_open("sugar", O_CREAT | O_EXCL, 0644, 0);

	//Helper mutex

	sem_t *helperMutex;
	helperMutex = sem_open("helperMutex", O_CREAT | O_EXCL, 0644, 1);

	//SEM FLAG

	sem_t *MflagSEM,*FflagSEM,*WflagSEM,*SflagSEM;
	MflagSEM = sem_open("MflagSEM", O_CREAT | O_EXCL, 0644, 0);
	FflagSEM = sem_open("FflagSEM", O_CREAT | O_EXCL, 0644, 0);
	WflagSEM = sem_open("WflagSEM", O_CREAT | O_EXCL, 0644, 0);
	SflagSEM = sem_open("SflagSEM", O_CREAT | O_EXCL, 0644, 0);

	
	
	int sval ;
	int sval2 ;
	int sval3 ;


	/*Create Chef*/
	
	pid_t check_pid;

	for(int i=0;i<10;i++){
		check_pid = fork();
		//childs (chef and helper)
		if(check_pid==0){

			chefs[i].pid=getpid();
			if(i<6){
				initally_print(i,chefs[i].pid,chefs[i].ingredients1,chefs[i].ingredients2,myadress);
			}
			
			//Chef 1
			for(;i==0;){
					
					sem_wait(MF2);

					if(myadress[2]=='T'){
						chefs[i].Taken_item=myadress[0];
						myadress[0]=' ';
						After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);

						chefs[i].Taken_item=myadress[1];
						myadress[1]=' ';
						After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
						Dessert_Display(i,chefs[i].pid,myadress);
						chefs[i].number_of_desert++;
						sem_post(WholesalerSEM);
					}
					if(myadress[2]=='F'){
						At_exit_print(i,chefs[i].pid,myadress);
						return chefs[i].number_of_desert;
					}
			}

			//Chef 2
			for(;i==1;){
	
				 
					sem_wait(MS2);

					if(myadress[2]=='T'){
						chefs[i].Taken_item=myadress[0];
						myadress[0]=' ';
						After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
						chefs[i].Taken_item=myadress[1];
						myadress[1]=' ';
						After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
						Dessert_Display(i,chefs[i].pid,myadress);
						chefs[i].number_of_desert++;
						sem_post(WholesalerSEM);
				  }
				  if(myadress[2]=='F'){
				  		At_exit_print(i,chefs[i].pid,myadress);
						return chefs[i].number_of_desert;
				  }

			}
			//Chef 3
			for(;i==2;){
				
					sem_wait(MW2);
					if(myadress[2]=='T'){
							chefs[i].Taken_item=myadress[0];
							myadress[0]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							chefs[i].Taken_item=myadress[1];
							myadress[1]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							Dessert_Display(i,chefs[i].pid,myadress );
							chefs[i].number_of_desert++;
							sem_post(WholesalerSEM);
					}
					if(myadress[2]=='F'){

							At_exit_print(i,chefs[i].pid,myadress);
							return chefs[i].number_of_desert;

					}	
				
			}
			//Chef 4
			for(;i==3;){
				
				
					sem_wait(SW2);
					if(myadress[2]=='T'){
							chefs[i].Taken_item=myadress[0];
							myadress[0]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							chefs[i].Taken_item=myadress[1];
							myadress[1]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							Dessert_Display(i,chefs[i].pid,myadress);
							chefs[i].number_of_desert++;
							sem_post(WholesalerSEM);

					}
					if(myadress[2]=='F'){
							
							At_exit_print(i,chefs[i].pid,myadress);
							return chefs[i].number_of_desert;
						
					}
				
			}
			//Chef 5
			for(;i==4;){
				
			
					sem_wait(SF2);
					if(myadress[2]=='T'){
							chefs[i].Taken_item=myadress[0];
							myadress[0]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							chefs[i].Taken_item=myadress[1];
							myadress[1]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							Dessert_Display(i,chefs[i].pid,myadress);
							chefs[i].number_of_desert++;
							sem_post(WholesalerSEM);

					}
					if(myadress[2]=='F'){
							At_exit_print(i,chefs[i].pid,myadress);
							return chefs[i].number_of_desert;

					}
				
			}
			//Chef 6
			for(;i==5;){
			
			
					sem_wait(FW2);

					if(myadress[2]=='T'){
							chefs[i].Taken_item=myadress[0];	
							myadress[0]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							chefs[i].Taken_item=myadress[1];
							myadress[1]=' ';
							After_obtaining_the_ingredients_print(i,chefs[i].pid,chefs[i].Taken_item,myadress);
							Dessert_Display(i,chefs[i].pid,myadress);
							chefs[i].number_of_desert++;
							sem_post(WholesalerSEM);

					}
					if(myadress[2]=='F'){
							At_exit_print(i,chefs[i].pid,myadress);
							return chefs[i].number_of_desert;

					}
				
			}
			//Helper 1
			for(;i==6;){

				sem_wait(milkSEM);
				
				if(myadress[2]=='F'){
					return 0;
				}
				else{
					sem_wait(helperMutex);
					sem_getvalue(FflagSEM ,&sval);
					sem_getvalue(WflagSEM ,&sval2);
					sem_getvalue(SflagSEM ,&sval3);

					if(sval==1){
						sem_wait(FflagSEM);
						sem_post(SW2);
					}
					else if(sval2==1){

						sem_wait(WflagSEM);
						sem_post(SF2);
					}
					else if(sval3==1){

						sem_wait(SflagSEM);
						sem_post(FW2);
					}
					else{
						
						sem_post(MflagSEM);
						
					}
					sem_post(helperMutex);

				}
				

				
			}
			//Helper 2
			for(;i==7;){
				
				sem_wait(flourSEM);
				
				if(myadress[2]=='F'){
					return 0;
				}
				else{
					  sem_wait(helperMutex);
						sem_getvalue(MflagSEM ,&sval);
						sem_getvalue(WflagSEM ,&sval2);
						sem_getvalue(SflagSEM ,&sval3);

						if(sval==1){
							sem_wait(MflagSEM);
							sem_post(SW2);
						}
						else if(sval2==1){

							sem_wait(WflagSEM);
							sem_post(MS2);
						}
						else if(sval3==1){

							sem_wait(SflagSEM);
							sem_post(MW2);
						}
						else{
							
							sem_post(FflagSEM);
							
						}
						sem_post(helperMutex);
				}
				
				
			}
			//Helper 3
			for(;i==8;){

				sem_wait(walnutSEM);

				if(myadress[2]=='F'){
					return 0;
				}
				else{
					sem_wait(helperMutex);
					sem_getvalue(MflagSEM ,&sval);
					sem_getvalue(FflagSEM ,&sval2);
					sem_getvalue(SflagSEM ,&sval3);
					
					if(sval==1){
						sem_wait(MflagSEM);
						sem_post(SF2);
					}
					else if(sval2==1){

						sem_wait(FflagSEM);
						sem_post(MS2);
					}
					else if(sval3==1){

						sem_wait(SflagSEM);
						sem_post(MF2);
					}
					else{
						
						sem_post(WflagSEM);
						
					}
					sem_post(helperMutex);	


				}
				
				
				
				
			}
			//Helper 4
			for(;i==9;){
				
				sem_wait(sugarSEM);

				if(myadress[2]=='F'){
					return 0;
				}
				else{
					sem_wait(helperMutex);
				
					sem_getvalue(MflagSEM ,&sval);
					sem_getvalue(FflagSEM ,&sval2);
					sem_getvalue(WflagSEM ,&sval3);
					
					if(sval==1){
						sem_wait(MflagSEM);
						sem_post(FW2);
					}
					else if(sval2==1){

						sem_wait(FflagSEM);
						sem_post(MW2);
					}
					else if(sval3==1){

						sem_wait(WflagSEM);
						sem_post(MF2);
					}
					else{
						
						sem_post(SflagSEM);
						
					}
					sem_post(helperMutex);	

				}
			}
		}
	}
	//Wholesaler(Parent)
	while(1){
		
		
		char* Buffer_text;
		Buffer_text = (char *) malloc(sizeof(char)*3);

		/*Read file 2 by 2*/
	    do{ 

	       sem_wait(WholesalerSEM);	
	       if(first_come!=0){
	       		whosaler_left_display(getpid(),myadress);		 
	       }
	       first_come++;
	       	

	       bytes_read = read(fd_inputfile, Buffer_text, 3);
	       if(bytes_read==-1){
	       		fprintf(stderr,"input file cant read!\n");
	       		exit(EXIT_FAILURE);
	       }
	       if(bytes_read!=0 || (bytes_read==0 && Buffer_text[2]=='\0') ){

	       			
	       			Buffer_text[2]=' ';
	       					
	       				
	       			
				 	memcpy(myadress,Buffer_text,3);
				 	myadress[2]='T';
				 
				 	if((Buffer_text[0]=='S'&&Buffer_text[1]=='W')||(Buffer_text[0]=='W'&&Buffer_text[1]=='S')){

				 		The_wholesaler_display(getpid(),"Sugar","Walnuts",myadress );
				 		
				 		sem_post(sugarSEM);
						
						sem_post(walnutSEM);

				 	}
				 	else if((Buffer_text[0]=='F'&&Buffer_text[1]=='W')||(Buffer_text[0]=='W'&&Buffer_text[1]=='F')){
				 		The_wholesaler_display(getpid(),"Walnuts","Flour",myadress );
				 		
				 		sem_post(flourSEM);
		
						sem_post(walnutSEM);

				 	}
				 	else if((Buffer_text[0]=='S'&&Buffer_text[1]=='F')||(Buffer_text[0]=='F'&&Buffer_text[1]=='S')){
				 		The_wholesaler_display(getpid(),"Sugar","Flour",myadress );
				 		
				 		sem_post(sugarSEM);
		
						sem_post(flourSEM);

				 	}
				 	else if((Buffer_text[0]=='M'&&Buffer_text[1]=='F')||(Buffer_text[0]=='F'&&Buffer_text[1]=='M')){
				 		The_wholesaler_display(getpid(),"Milk","Flour",myadress );
				 		
				 		sem_post(milkSEM);
		
						sem_post(flourSEM);

				 	}
				 	else if((Buffer_text[0]=='M'&&Buffer_text[1]=='W')||(Buffer_text[0]=='W'&&Buffer_text[1]=='M')){
				 		The_wholesaler_display(getpid(),"Milk","Walnuts",myadress );
				 		
				 		sem_post(milkSEM);
		
						sem_post(walnutSEM);

				 	}
				 	else if((Buffer_text[0]=='S'&&Buffer_text[1]=='M')||(Buffer_text[0]=='M'&&Buffer_text[1]=='S')){
				 		The_wholesaler_display(getpid(),"Sugar","Milk",myadress );
				 		
				 		sem_post(sugarSEM);
		
						sem_post(milkSEM);

				 	}

       				
          	}
          	else{

	          	myadress[2]='F';
				sem_post(MF2);
				sem_post(MS2);
				sem_post(MW2);
				sem_post(SW2);
				sem_post(SF2);
				sem_post(FW2);

				sem_post(milkSEM);
				sem_post(flourSEM);
				sem_post(walnutSEM);
				sem_post(sugarSEM);

				int Total_Desert=0;
				int returnValue;
	        	for(int i=0 ; i<10 ; i++){
		            waitpid(chefs[i].pid,&returnValue,0);
		            Total_Desert += WEXITSTATUS(returnValue);
	        	}
				whosaler_exit_display(getpid(),Total_Desert,myadress);	
			}
  			
	      		
		 }while(bytes_read!=0);
	
	sem_close(MF2);
	sem_unlink("chef0");

	sem_close(MS2);
	sem_unlink("chef1");

	sem_close(MW2);
	sem_unlink("chef2");

	sem_close(SW2);
	sem_unlink("chef3");

	sem_close(SF2);
	sem_unlink("chef4");

	sem_close(FW2);
	sem_unlink("chef5");

	sem_close(milkSEM);
	sem_unlink("milk");

	sem_close(flourSEM);
	sem_unlink("flour");

	sem_close(walnutSEM);
	sem_unlink("walnut");

	sem_close(sugarSEM);
	sem_unlink("sugar");

	sem_close(WholesalerSEM);
	sem_unlink("Wholesaler1");

	sem_close(helperMutex);
	sem_unlink("helperMutex");

	sem_close(MflagSEM);
	sem_unlink("MflagSEM");

	sem_close(FflagSEM);
	sem_unlink("FflagSEM");

	sem_close(WflagSEM);
	sem_unlink("WflagSEM");

	sem_close(SflagSEM);
	sem_unlink("SflagSEM");



	free(Buffer_text);

   
    
	close(fd_inputfile);
	close(fd_shared_mem);//nolonger need
	

	exit(EXIT_SUCCESS);
		

		
		
	}


	return 0;


}