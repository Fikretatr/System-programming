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
#define MINUMUM 99999999999999 

static int exit_flag = 0;
int number_of_process=1;
pid_t* child_pid_arr;
/*
If P receives SIGINT,
it will forward the signal to all of its children, free all of its resources,
close open files, and remove the output file, clean-up 
after its children and terminate.
*/
static void handler (int signum)
{
   fprintf(stdout,"CTRL-C ,free all of P's resources and children %d",signum);
     for(int i=0;i<number_of_process;i++)
        kill((child_pid_arr[i]),SIGINT);
     for(int i=0;i<number_of_process;i++)
        wait(NULL);
   exit_flag = 1;
    
}

void FindFrobeniusNorm_Calc_Closest(char *matrix){

   //Char matrix is first converted to double temp and then to double matrix
   char *Temp_char_double;
   int i=0,Reincrement=0,size_double_matrix=0;
   char *ptr;
   double Temp_double;
   //The calculated covariances are also included in this matrix.
   double *Matrices;
   Matrices = (double *) malloc(sizeof(double)*1);
   Temp_char_double = (char *) malloc(sizeof(char)*30);
  
   for(int j=0;j<strlen(matrix);j++){
         //Getting the number up to the blank
         if(matrix[j]!=' '){
         Temp_char_double[i]=matrix[j];
         i++;
         }
         else{
             Reincrement=Reincrement+8;
             size_double_matrix++;
             Temp_double = strtod(Temp_char_double, &ptr);
             Matrices = (double*)realloc(Matrices,1*Reincrement);
             Matrices[size_double_matrix-1]=Temp_double;
             i=0;
      }
   }
  

   int Size_Norms=(size_double_matrix)/9;
   
   double Norms[Size_Norms];
   int sqsum=0;
   int index_norm=0;
   //Norms are calculated and added to the norms array.
   for(int i=1;i<=size_double_matrix;i++){
      sqsum+=(Matrices[i-1]*Matrices[i-1]);
      
      if((i%9)==0){
         

         Norms[index_norm]=sqrt(sqsum);
         index_norm++;
         sqsum=0;
      }

   }
   //Calculating the distance between the two closest matrices
   int index1,index2;
   double closest_value=MINUMUM;
   for(int i=0;i<Size_Norms;++i){
      for(int j=0;j<Size_Norms;++j){
            if(abs(Norms[i]-Norms[j])<= closest_value){
                  if(i != j){
                     closest_value=abs(Norms[i]-Norms[j]);
                        index1=i;
                        index2=j;

                  }
            }
      }
   }


   //Printing the two closest matrices   
   fprintf(stdout,"The closest 2 matrices are \n[(%lf,%lf,%lf),(%lf,%lf,%lf),(%lf,%lf,%lf)] \nand [(%lf,%lf,%lf),(%lf,%lf,%lf),(%lf,%lf,%lf)]"
      ,Matrices[(index1*9)],Matrices[(index1*9)+1],Matrices[(index1*9)+2]
      ,Matrices[(index1*9)+3],Matrices[(index1*9)+4],Matrices[(index1*9)+5]
      ,Matrices[(index1*9)+6],Matrices[(index1*9)+7],Matrices[(index1*9)+8]
      ,Matrices[(index2*9)],Matrices[(index2*9)+1],Matrices[(index2*9)+2]
      ,Matrices[(index2*9)+3],Matrices[(index2*9)+4],Matrices[(index2*9)+5]
      ,Matrices[(index2*9)+6],Matrices[(index2*9)+7],Matrices[(index2*9)+8]);
   fprintf(stdout,"and their distance is %.4f\n",closest_value);    
   free(Matrices);
   free(Temp_char_double);

}

int main (int argc, char *argv[])
{
    struct sigaction act;

    memset (&act, '\0', sizeof(act));
    act.sa_handler = &handler;
    
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror ("sigaction");
        return 1;
    }

    if(exit_flag==1){

      exit(0);

    }

   int err;
   char inputFilePath[1024];
   char outputFilePath[1024];
   char buffer[30];
   if(argc==1)
      perror("\nNo Extra Command Line Argument Passed Other Than Program Name");

   if(argc != 5){
      fprintf(stderr,"Command line argument is failed\n");
      return -1;
   }

   while((err = getopt(argc, argv, "i:o:")) != -1){
      switch(err){
         case 'i':
            strcpy(inputFilePath, optarg);
            break;
         case 'o':
            strcpy(outputFilePath, optarg);
            break;
      }
   }
	//Outfile is created here
	int fd_write = open (outputFilePath,O_CREAT,0666);
   
   if(fd_write == -1){
      perror ("open");
      return -1;
   }   
   if(exit_flag==1){
      close(fd_write);
      exit(0);

    }

   close(fd_write);
   //Environment variables set
   char *args[]={outputFilePath,"1","2",NULL};
   char *env[]={"1",NULL};
   
   fprintf(stderr,"Process P reading %s\n",inputFilePath);
   
   size_t bytes_read;
   
   int n=1;
   /*inputfile is open here and read contents of input file
   The read characters are added to the buffer and sent to the
   child process with execv with environmental 
   variables for use in each child process. 
   */
   int fd = open(inputFilePath, O_RDONLY);
   //number of child process
   child_pid_arr = (pid_t*) malloc(1 * sizeof(pid_t));

    if(exit_flag==1){
      //close(fd_write);
      close(fd);
      free(child_pid_arr);
      exit(0);

    }

       do{ 
       	bytes_read = read(fd, buffer, sizeof(buffer));
        
         if(bytes_read==30){
        	
        	env[0]=buffer;

         //Child processes create here.
        
         child_pid_arr = (pid_t*)realloc(child_pid_arr,number_of_process * sizeof(pid_t));

         child_pid_arr[number_of_process-1] = fork();

    	   	if(child_pid_arr[number_of_process-1] !=0){//Parent process
               	
               fprintf(stdout,"Created R_%d with ",number_of_process);
               	for(int s=0;s<10;s++){
               		if(s==9){
               			fprintf(stdout,"(%d,%d,%d)",buffer[s],buffer[s+1],buffer[s+2]);
               		}
               		else{
               					fprintf(stdout,"(%d,%d,%d),",buffer[s],buffer[s+1],buffer[s+2]);
               		}
                  }
                     fprintf(stdout,"\n");
           				number_of_process++;
            }
           			else{//Child process
   				
           				if(execve("./Child", args,env)<0){
           					perror("execv error");
           				}
            		}
            		n++;  		
			}	

         }while(bytes_read == sizeof(buffer));
          
         fprintf(stderr,"Reached EOF, collecting outputs from  %s\n",outputFilePath);

         if(exit_flag==1){
            close(fd);
            free(child_pid_arr);
            exit(0);
            

         }

         /*The process P must not start collecting the outputs 
         before all the children processes have
         finished their calculations therefore using wait
         */
         for(int i=0;i<number_of_process;i++){
           wait(NULL);
         }
         //Close reader file
         close(fd);

         char* Buffer_matrix;
         char* Tempbuffer;
         Buffer_matrix = (char *) malloc(sizeof(char)*1);
         Tempbuffer = (char *) malloc(sizeof(char)*1);


         int fd_parent = open(outputFilePath, O_RDONLY);

         if(fd_parent == -1) {
            perror ("open parent");
         return -1;
         }


         if(exit_flag==1){
            close(fd_parent);
            free(Buffer_matrix);
            free(Tempbuffer);        
            exit(0);

         }

         int temp=0;
            //All char coordinates in the file are read character by character by Parent
            
            do{ 
               temp=strlen(Buffer_matrix)+8;
               bytes_read = read(fd_parent, Tempbuffer, 1);
               Buffer_matrix = (char*)realloc(Buffer_matrix,temp);
                 
               strcat(Buffer_matrix,Tempbuffer);
            }while(bytes_read!=0);
            /*
            Then P will
            calculate the two matrices that are closest to 
            each other with respect to the Frobenius norm, and
            print them on stdout and terminate.
            */
            FindFrobeniusNorm_Calc_Closest(Buffer_matrix);

            close(fd_parent);
            free(Buffer_matrix);
            free(Tempbuffer);
            free(child_pid_arr);
    	    
         return 0;

}    