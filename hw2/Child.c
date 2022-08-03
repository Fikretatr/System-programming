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
#include <stdint.h>

extern char** environ;
struct flock lock;
int fd_write;


static int exit_flag = 0;

static void handler (int signum)
{
   printf("Childdd process %d",signum);
   exit_flag = 1;
    
}

int main(int argc, char *argv[]){

	struct sigaction action;

    memset (&action, '\0', sizeof(action));
    action.sa_handler = &handler;
    
    if (sigaction(SIGINT, &action, NULL) < 0) {
        perror ("sigaction");
        return 1;
    }

    if(exit_flag==1){

      exit(0);

    }
	
	char str[100];
	int set=1;
	int i=0;
	int ten_cordinates[10][3];
	//Getting environmental variables
	for(int k=0;k<10;k++){
		for(int y=0;y<3;y++){
			ten_cordinates[k][y]=environ[0][i];
			i++;
		}
	}

	//Variance is a measure of the variability or spread in a set of data. 
	//Mathematically, it is the average squared deviation from the mean score. 
	double ort[3];
	double covariance_matrix[3][3];
	//The ascii values of the unsigned chars I took as 30 ar were calculated and averaged.
	
	int x=0;
	while(x<3){
		for(int y=0;y<10;y++){
			ort[x] += ten_cordinates[y][x];
		}
		x++;
	}

	//Then, divide each term in the deviation sums of squares and 
	//cross product matrix by n to create the variance-covariance matrix. 	
	ort[0] /= 10; ort[1] /= 10; ort[2] /= 10;
	if(exit_flag==1){

      exit(0);

    }

	for(i=0;i<3;i++){
        for(int m=0;m<3;m++){
            covariance_matrix[i][m]=0;
                for (int k=0;k<10;k++){
					covariance_matrix[i][m]+=(ort[i]-ten_cordinates[k][i])*(ort[m]-ten_cordinates[k][m]);
                }
                	covariance_matrix[i][m]/=10;
        }
    }

    //Opening file to be overwritten
    fd_write = open (argv[0],O_WRONLY,0666);
    if(fd_write == -1) {
    perror ("open");
    return -1;
    } 

    if(exit_flag==1){
      close(fd_write);
      exit(0);

    }
    memset(&lock,0,sizeof(lock));  
    lock.l_type =F_WRLCK;
      
    //fcntl() performs one of the operations described below 
    //on the open file descriptor fd. The operation is determined by cmd.
	fcntl(fd_write,F_SETLKW,&lock);
    		
    if(exit_flag==1){
      close(fd_write);
      exit(0);

    }
	//writing covariance matrices to file
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			 lseek(fd_write,0,SEEK_END);
			 snprintf(str,100,"%.4f",covariance_matrix[i][j]);
			 set=write(fd_write, str, strlen(str));
			 lseek(fd_write,0,SEEK_END);
			 set=write(fd_write," ", 1);
			 if(set == -1) {
    		 perror ("write failed");
    		 return -1;
    		 }	
		}
		set=write(fd_write, "\n", 1);
	}
	if(exit_flag==1){
	  close(fd_write);
      exit(0);

    }

	lock.l_type=F_UNLCK;
	fcntl(fd_write,F_SETLKW,&lock);
	close(fd_write);
	
	return 0;
}


