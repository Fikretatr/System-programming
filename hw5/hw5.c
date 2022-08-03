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
#define PI 3.141592

volatile sig_atomic_t FlagInterupt = 0;
pthread_t *Threads_tid;
int n,m;
void sigterm_handler(int signo){
	if(signo == SIGINT){
		FlagInterupt = 1;
	}
}
struct DFT{
    float Realside;
    float Imgside;
};

struct DFT **RESULT = NULL;
time_t TM;
char time_info[30];
struct tm* Tmobj;

int matrix_line_size;//-->2^n
void freeALL();
void display_infos_initial(int n,int m);
void display_timestamp();
int arrived = 0;//senc barrier
int **MatrixA,**MatrixB,**MatrixC;
char *outputTEXT;
clock_t t0, t1,t2;
char **buffer_1;
char **buffer_2;


struct tm* ptm = NULL;
char timeBUFFER[120];
int fd_path1,fd_path2,fd_output;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

/*Threads will calculate C=AxB in a parallel fashion*/
void *Thread_job(void *ing){
	ptm = localtime(&t2);
	strftime(timeBUFFER, 120, "%d/%m/%Y-%T", ptm);

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(ing);
		exit(EXIT_FAILURE);
	}
	//pthread_mutex_lock(&mutex1);
	int fail;//for error check
	int thread_index = *((int *) ing);//get arguman as a integer value
	free(ing);
	int temp_product;

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(ing);
		exit(EXIT_FAILURE);
	}
	/*Product Calculate*/
	for (int i=(thread_index-1)*(matrix_line_size/m) ; i<(thread_index-1)*(matrix_line_size/m)+(matrix_line_size/m) ; i++){
        for (int j = 0; j<matrix_line_size; j++){
           temp_product=0;
           for (int k = 0; k<matrix_line_size; k++){
           temp_product+=(MatrixA[i][k]*MatrixB[k][j]);
            }
            MatrixC[i][j]=temp_product;
        }
   }

	char *str_message_thread;
	str_message_thread = (char *) malloc(200);
	//display_timestamp();
	//sleep(1);
	
   TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);
	t1=clock()-t0;//until the calculations were completed time
	sprintf(str_message_thread,"%s,Thread %d has reached the rendezvous point in %f seconds.\n",time_info,thread_index,((float)t1) / CLOCKS_PER_SEC);
	fail =write (1, str_message_thread,strlen(str_message_thread));
	if(fail == -1){
		perror("write syscall for timestamp");
		free(str_message_thread);  
		exit(EXIT_FAILURE);
	}
	
	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(ing);
		free(str_message_thread);  
		exit(EXIT_FAILURE);
	}

	//synchronization barrier with N threads
	pthread_mutex_lock(&mutex);
	++arrived;
	
	if(arrived < m) // if this thread is not last
		pthread_cond_wait(&cond,&mutex); // then wait for others
	else
		pthread_cond_broadcast(&cond); // i’m last, awaken the other N-1
	pthread_mutex_unlock(&mutex);

	char *str_message_thread2;
	str_message_thread2 = (char *) malloc(200);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);
	sprintf(str_message_thread2,"%s,Thread %d is advancing to the second part.\n",time_info,thread_index);
	fail =write (1, str_message_thread2,strlen(str_message_thread2));
	if(fail == -1){
		perror("write syscall for timestamp");
		free(str_message_thread2);  
		exit(EXIT_FAILURE);
	}


	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(str_message_thread);
		free(str_message_thread2);
		exit(EXIT_FAILURE);
	}
     
	/*2D Discrete Fourier Transform of C*/
	for(int i=0;i<matrix_line_size;i++){
		/*Paralel working, threads share matrix C*/
   	for(int j=(thread_index-1)*(matrix_line_size/m);j<(thread_index-1)*(matrix_line_size/m)+(matrix_line_size/m);j++){
      		float ak=0; 
      		float bk=0;

         	for(int ii=0;ii<matrix_line_size;ii++){
             	for(int jj=0;jj<matrix_line_size;jj++){

               float x=-2.0*PI*i*ii/(float)matrix_line_size;
               float y=-2.0*PI*j*jj/(float)matrix_line_size;
          		ak+=MatrixC[ii][jj]*cos(x+y);
               bk+=MatrixC[ii][jj]*1.0*sin(x+y);
             	}
           	}

    			RESULT[i][j].Realside=ak;
     			RESULT[i][j].Imgside=bk;
      }
   }
   //sleep(2);
   // SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(str_message_thread);
		free(str_message_thread2);
		exit(EXIT_FAILURE);
	}
     
   char *str_message_thread3;
	str_message_thread3 = (char *) malloc(200);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);
	t1=clock()-t0;
	sprintf(str_message_thread3,"%s,Thread %d has has finished the second part in %f seconds.\n",time_info,thread_index,((float)t1) / CLOCKS_PER_SEC);  
	fail =write (1, str_message_thread3,strlen(str_message_thread3));
	if(fail == -1){
		perror("write syscall for timestamp");
		free(str_message_thread3);  
		exit(EXIT_FAILURE);
	}

	free(str_message_thread);
	free(str_message_thread2);
	free(str_message_thread3);

	return NULL;
}


int main (int argc, char *argv[]) {

	char filePath1[1024];
	char filePath2[1024];
	char output[1024];
	int opt;
	int byte_read;
	
	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_handler;

	if(sigaction(SIGINT, &sigterm_action, NULL) != 0){
	    perror("sigaction");
	}
	/*Get arguments
	Example:./hw5 -i filePath1 -j filePath2 -o output -n 4 -m 2
	*/
	if(argc==1){
		fprintf(stderr,"No Extra Command Line Argument Passed Other Than Program Name\n");
	}
	if(argc != 11){
		fprintf(stderr,"Command line argument is failed\n");
		return -1;
	}
	while((opt = getopt(argc, argv, "i:j:o:n:m:")) != -1){
		switch(opt){
		    case 'i':
		        strcpy(filePath1, optarg);
		        break;
		    case 'j':
		        strcpy(filePath2, optarg);
		        break;    
		    case 'o':
		    	strcpy(output, optarg);
		      	break;
		    case 'n':
		    	n = atoi(optarg);
		     	break;
		    case 'm':
		    	m = atoi(optarg);
		     	break; 	    
		}
	}
	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		//free(str_message);
		exit(EXIT_FAILURE);
	}

	//Restrictions

	if(m%2!=0){
   	write(1,"m must be even\n",strlen("m must be even\n"));
		exit(EXIT_FAILURE);
   }
   
	if(n<=2){
		write(1,"n must be greater than 2\n",strlen("n must be greater than 2\n"));
		exit(EXIT_FAILURE);
	}

	if(pow(2,n)< m){
      m = pow(2,n);
   }
	/*
	if((int)pow(2,n) % m != 0){
		write(1,"it should be (2^n) % m == 0",strlen("it should be (2^n) % m == 0"));
		exit(EXIT_FAILURE);
	}
	*/

   /*Raund down m*/
	if((int)pow(2,n) % m != 0){
		while(1){
			m--;
			if((int)pow(2,n) % m == 0){
				break;
			}
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		//free(str_message);
		exit(EXIT_FAILURE);
	}
	//open input file path1 and path2
	fd_path1 = open(filePath1, O_RDONLY);
	if(fd_path1==-1){
		perror("filePath1 can not open");
		exit(EXIT_FAILURE);
	}

	fd_path2 = open(filePath2, O_RDONLY);
	if(fd_path1==-1){
		perror("filePath2 can not open");
		exit(EXIT_FAILURE);
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		close(fd_path2);close(fd_path1);
		exit(EXIT_FAILURE);
	}
	/* (2^n) row and cloumn size*/
	matrix_line_size=pow(2,n);

	/*Read content from files*/
	buffer_1=(char **)malloc(matrix_line_size*sizeof(char *));

	for (int i=0;i<matrix_line_size;i++){
	    buffer_1[i]=(char*)malloc(matrix_line_size*sizeof(char));
	}

	buffer_2=(char **)malloc(matrix_line_size*sizeof(char *));

	for (int i=0;i<matrix_line_size;i++){
	    buffer_2[i]=(char*)malloc(matrix_line_size*sizeof(char));
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		exit(EXIT_FAILURE);
	}

	/*Read path1*/
	for(int i=0;i<matrix_line_size;i++){
		
		if((byte_read = read(fd_path1,buffer_1[i],matrix_line_size))!=matrix_line_size){
			perror("unsufficient data on file1");
			
			for (int i = 0; i < matrix_line_size;i++){
			   free(buffer_1[i]);
			   free(buffer_2[i]);
			   
			}
			free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
			exit(EXIT_FAILURE);
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		exit(EXIT_FAILURE);
	}
	/*Read path2*/
	for(int i=0;i<matrix_line_size;i++){
		
		if((byte_read = read(fd_path2,buffer_2[i],matrix_line_size))!=matrix_line_size){
			
			for (int i = 0; i < matrix_line_size;i++){
			   free(buffer_1[i]);
			   free(buffer_2[i]);
			   
			}
			free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
			perror("unsufficient data on file2");
			exit(EXIT_FAILURE);
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		for (int i = 0; i < matrix_line_size;i++){
			   free(buffer_1[i]);
			   free(buffer_2[i]);
			   
		}
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		exit(EXIT_FAILURE);
	}

	/*Char to ascii fill the matrix a and b*/

	MatrixA=(int **)malloc(matrix_line_size*sizeof(int *));

	for (int i=0;i<matrix_line_size;i++){
	    MatrixA[i]=(int*)malloc(matrix_line_size*sizeof(int));
	}

	MatrixB=(int **)malloc(matrix_line_size*sizeof(int *));

	for (int i=0;i<matrix_line_size;i++){
	    MatrixB[i]=(int*)malloc(matrix_line_size*sizeof(int));
	}

	MatrixC=(int **)malloc(matrix_line_size*sizeof(int *));

	for (int i=0;i<matrix_line_size;i++){
	    MatrixC[i]=(int*)malloc(matrix_line_size*sizeof(int));
	}

	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			MatrixA[i][j]=(int)buffer_1[i][j];
		}
	}

	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			MatrixB[i][j]=(int)buffer_2[i][j];
		}
	}
	/*RESULT will contain calculation result*/
	RESULT=(struct DFT**)calloc(matrix_line_size,sizeof(struct DFT*));
   for (int i = 0; i < matrix_line_size; ++i){
   	RESULT[i] = (struct DFT* ) calloc(matrix_line_size,sizeof(struct DFT));
   }
   
  
   // SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		
		for (int i = 0; i < matrix_line_size;i++){
		   free(buffer_1[i]);
		   free(buffer_2[i]);
		   free(MatrixA[i]);
			free(MatrixB[i]);
			free(MatrixC[i]);
			free(RESULT[i]);
		}
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		free(MatrixA);free(MatrixB);free(MatrixC);free(RESULT);
		exit(EXIT_FAILURE);
	}

	//Clock Start
	t0=clock();
	display_infos_initial(n,m);
	//m number of threads were created by process.
	Threads_tid=(pthread_t*)malloc(m*sizeof(pthread_t));
	
	for(int i=0;i<m;i++){
		//args will be thread index , it passes the arguman
		int *arg= malloc(sizeof(*arg));
		if(arg == NULL){
		   perror("Couldn't allocate memory for thread arg.\n");
		   freeALL();
		  	exit(EXIT_FAILURE);
		}
		*arg = i+1;
		if(pthread_create(&Threads_tid[i],NULL,Thread_job,arg)){
			perror("Thread create failed");
			freeALL();
			exit(EXIT_FAILURE);
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		for (int i = 0; i < matrix_line_size;i++){
		   free(buffer_1[i]);
		   free(buffer_2[i]);
		   free(MatrixA[i]);
			free(MatrixB[i]);
			free(MatrixC[i]);
			free(RESULT[i]);
		}
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		free(MatrixA);free(MatrixB);free(MatrixC);free(RESULT);free(Threads_tid);
		exit(EXIT_FAILURE);
	}

	//Main process wait thread with join
	int err_check;
	for(int i=0;i<m;i++){
		err_check=pthread_join(Threads_tid[i], NULL);
		if(err_check!=0){
			perror("Thread join failed");
			freeALL();
			exit(EXIT_FAILURE);
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		for (int i = 0; i < matrix_line_size;i++){
		   free(buffer_1[i]);
		   free(buffer_2[i]);
		   free(MatrixA[i]);
			free(MatrixB[i]);
			free(MatrixC[i]);
			free(RESULT[i]);
		}
		free(buffer_1);free(buffer_2);close(fd_path2);close(fd_path1);
		free(MatrixA);free(MatrixB);free(MatrixC);free(RESULT);free(Threads_tid);
		exit(EXIT_FAILURE);
	}
	/*Display part a result
	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			printf("%d-",MatrixA[i][j]);
		}
		printf("\n");
	}
	*/

	/*Display part a result
	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			printf("%d-",MatrixB[i][j]);
		}
		printf("\n");
	}
	*/


	/*Display part a result
	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			printf("%d-",MatrixC[i][j]);
		}
		printf("\n");
	}
	*/
	
	/*Display part a result
	for(int i=0;i<matrix_line_size;i++){
		for(int j=0;j<matrix_line_size;j++){
			printf("%f + %f ,",RealSide[i][j],ImageSide[i][j]);
		}
		printf("\n");
	}
	*/

	/*Open output file*/
	fd_output = open(output, O_WRONLY| O_CREAT,0666);
	if(fd_output==-1){
		perror("output can not open");
		freeALL();
		exit(EXIT_FAILURE);
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		freeALL();
		exit(EXIT_FAILURE);
	}
	/*Write to outputfile from RESULT array*/
	for(int i=0;i<matrix_line_size;++i){
      int cnt = 0;//length of buffer
		for(int j = 0; j < matrix_line_size; ++j){
			outputTEXT = (char*)malloc(1024 * sizeof(char));
         sprintf(outputTEXT,"%0.3fi+(%0.3f)j, ",RESULT[i][j].Realside,RESULT[i][j].Imgside);
			cnt = strlen(outputTEXT);
			err_check=write(fd_output,outputTEXT,cnt);
			if(err_check==-1){
				perror("write");
				freeALL();
				exit(EXIT_FAILURE);
			}
			free(outputTEXT);
      }

    	err_check=write(fd_output,"\n",1);
		if(err_check==-1){
			perror("write");
			freeALL();
			exit(EXIT_FAILURE);
		}
	}

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		freeALL();
		exit(EXIT_FAILURE);
	}

	char *str_message_process;
	str_message_process = (char *) malloc(200);
	TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);
	t1=clock()-t0;
	sprintf(str_message_process,"%s,The process has written the output file. The total time spent is %f seconds.\n",time_info,((float)t1) / CLOCKS_PER_SEC);  
	int fail =write (1, str_message_process,strlen(str_message_process));
	if(fail == -1){
		perror("write output");
		free(str_message_process);  
		freeALL();
		exit(EXIT_FAILURE);
	}
	free(str_message_process);

	freeALL();
	exit(EXIT_SUCCESS);
	
}

void display_infos_initial(int n,int m){
	int size=pow(2,n);
	int fail;
   TM = time(NULL);
	Tmobj = localtime(&TM);
	strftime(time_info, 30,"%Y-%m-%d %H:%M:%S", Tmobj);

	char *str_message0;
	str_message0 = (char *) malloc(200);

	// SIGINT signal check 
	if(FlagInterupt){
		write(1,"Ctrl-C has arrived",strlen("Ctrl-C has arrived"));
		free(str_message0);  
		exit(EXIT_FAILURE);
	}

	sprintf(str_message0,"%s,Two matrices of size %dx%d have been read. The number of threads is %d\n"
		,time_info,size,size,m);
	fail =write (1, str_message0,strlen(str_message0));
	if(fail == -1){
		perror("write syscall");
		free(str_message0);  
		exit(EXIT_FAILURE);
	}
	
	free(str_message0); 

}

void freeALL(){
	//Free for allocated memory
	for (int i = 0; i < matrix_line_size;i++){
	   free(buffer_1[i]);
	   free(buffer_2[i]);
	   free(MatrixA[i]);
		free(MatrixB[i]);
		free(MatrixC[i]);
		free(RESULT[i]);
	}
	/*
	for (int i = 0; i < m; ++i){        
      pthread_cancel(Threads_tid[i]);
   }*/

	free(Threads_tid);
	free(buffer_1);
	free(buffer_2);
	free(RESULT);
	//Close file fd
	close(fd_path1);
	close(fd_path2);
	close(fd_output);
	
	free(MatrixA);
	free(MatrixB);
	free(MatrixC);
}
