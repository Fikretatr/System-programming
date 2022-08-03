#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>


#define buffersize 128


char* STR1;
char* STR2;
char Temp_Buffer[1024];
char Temp_Buffer2[1024];
char buffer[1024];
int flag_kose=0;//for case D
char temp_Buf[1024];
char line[128] = {0};
struct flock lock;
int check_free=0;



int  caseE(char* Str,char* first_word,int alert);
void Replace_All_SENS(char *TexT,char *variable,char *target_word);
void parsingARGV_1_caseA(char* argv1);
char* Search_str_OffSet(char* X, char* Y);
int  Check_equal(char *X, char *Y);
ssize_t Reader_LBL (char *Buffer, size_t Size, char *Path, off_t *st);
void Replace_All_INSE(char *TexT,char *variable,char *target_word);
void which_statement(char* argv1);
void state_a_b_d_e(char* argv1);
int last_element(char* Str,char* first_word,int alert);


int main(int argc,char* argv[])
{
	memset(&lock,0,sizeof(lock));
    off_t offset = 0;
    ssize_t len = 0;
    //size_t i = 0;
    int set;

	ssize_t File_Reader;
	
	/*The path at which to create the new file. */
	char *inputFilePath = argv[2];
	/*The permission for new file*/
	//mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;


    int counter;
    printf("Program Name Is: %s",argv[0]);
    if(argc==1)
        perror("\nNo Extra Command Line Argument Passed Other Than Program Name");
    if(argc>=2)
    {
        printf("\nNumber Of Arguments Passed: %d",argc);
        printf("\n----Following Are The Command Line Arguments Passed----");
        for(counter=0;counter<argc;counter++)
        printf("\nargv[%d]: %s",counter,argv[counter]);
    }
   	

    /*Create new file*/
    int fd = open (inputFilePath, O_RDONLY,0666);
    if(fd == -1) {
    	perror ("open");
    	return -1;
    }


    File_Reader = read(fd, buffer , sizeof(buffer));


    int a;
    for(a=0;a<File_Reader;a++){
    	printf("%c",buffer[a]);
    }
    close(fd);


    /*Create new file*/
    fd = open (inputFilePath, O_WRONLY,0666);
    if(fd == -1) {
    	perror ("open");
    	return -1;
    }
    strcpy(Temp_Buffer,buffer);
    strcpy( Temp_Buffer2,buffer);

    parsingARGV_1_caseA(argv[1]);
 
    int alert;
    char tempcHAR1;
    char tempcHAR2;
    char* stringA;
    char* stringB;
    
     stringA = (char *) malloc(sizeof(char)*strlen(STR1));
     stringB = (char *) malloc(sizeof(char)*strlen(STR1));
     
     if(argv[1][strlen(argv[1])-1]=='i'){

     	alert=1;
     }
     else{
     	alert=0;

     }
 
     //FIRST ELEMENT
    if(argv[1][1]=='^'){
    
    	 STR1 = strtok(STR1, "^");
    	
    	 if(STR1[0]=='['){
    	 
    	 		tempcHAR1=STR1[1];
    	 		tempcHAR2=STR1[2];
    	 		stringA[0]=tempcHAR1;
    	 		stringB[0]=tempcHAR2;
    	 		int camo=1;
    	 	for(int x=4;x<strlen(STR1);x++){

    	 		stringA[camo]=STR1[x];
    	 		camo++;
    	 	}
    	 	camo=1;
    	 	for(int x=4;x<strlen(STR1);x++){

    	 		stringB[camo]=STR1[x];
    	 		camo++;
    	 	}
    	 	

    	 		
   		 while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(caseE(line,stringA,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}
		}

    		
   			while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(caseE(line,stringB,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}
      
    		}

    	 }
    	 else
    	 {

    	
   		 while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(caseE(line,STR1,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}
		}
    }
    		 lock.l_type =F_WRLCK;
    		 fcntl(fd,F_SETLKW,&lock);
    		 ssize_t length =strlen(temp_Buf);
			 set=write(fd, temp_Buf, length);
			 if(set == -1) {
    		 perror ("write");
    		 return -1;
    		 }
			 lock.l_type=F_UNLCK;
			 fcntl(fd,F_SETLKW,&lock);
			 close(fd);

   		 }//LAST ELEMENT CHECK

   		 else if(STR1[strlen(STR1)-1]=='$'){
    		
    
    	 STR1 = strtok(STR1, "$");
    	
    	 if(STR1[0]=='['){
    	 
    	 		tempcHAR1=STR1[1];
    	 		tempcHAR2=STR1[2];
    	 		stringA[0]=tempcHAR1;
    	 		stringB[0]=tempcHAR2;
    	 		int camo=1;
    	 	for(int x=4;x<strlen(STR1);x++){

    	 		stringA[camo]=STR1[x];
    	 		camo++;
    	 	}
    	 	camo=1;
    	 	for(int x=4;x<strlen(STR1);x++){

    	 		stringB[camo]=STR1[x];
    	 		camo++;
    	 	}
    	 	

    	 		
   		    while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(last_element(line,stringA,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}

      		}

   		    while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(last_element(line,stringB,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}

     	}

    }
    	 else{

    	
   		 while ((len = Reader_LBL (line, buffersize, inputFilePath, &offset)) != -1){

    		if(last_element(line,STR1,alert)){
    			state_a_b_d_e(argv[1]);
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	
    			
    		}
    		else{
    			
    			strcat(temp_Buf,line);	
    			strcat(temp_Buf,"\n");	

    		}
    	}
    }

   	    
   	    lock.l_type =F_WRLCK;
   	    fcntl(fd,F_SETLKW,&lock);
    	ssize_t length =strlen(temp_Buf);
	    set=write(fd, temp_Buf, length);
	    if(set == -1) {
    		 perror ("write");
    		 return -1;
        }
	    lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLKW,&lock);
	   	close(fd);

    }
    else{
			which_statement(argv[1]);
			ssize_t length =strlen(buffer);
			lock.l_type =F_WRLCK;
			fcntl(fd,F_SETLKW,&lock);
    		
	   	    set=write(fd, buffer, length);
	   	    if(set == -1) {
    		 perror ("write");
    		 return -1;
            }
	   	    lock.l_type=F_UNLCK;
			 fcntl(fd,F_SETLKW,&lock);
	   		close(fd);
    }

  	

    	   
  
    
    free(stringA);
    free(stringB);
    free(STR2);


		    return 0;
		  
   		
  
}

void Replace_All_SENS(char *TexT,char *variable,char *target_word)
{
	char *Buffer_Str;
    int OffSet = 0;
    int lENGTH;
    char *tempOLD;
	tempOLD = (char *) malloc(sizeof(char)*strlen(variable));
	strcpy(tempOLD, variable);
	char *temp;
	temp = (char *) malloc(sizeof(char)*strlen(TexT));
    lENGTH = strlen(variable);

    while ((Buffer_Str = strstr(TexT, variable)) != NULL)//eslesme
    {
		strcpy(temp, TexT);
		OffSet = Buffer_Str - TexT;
		TexT[OffSet] = '\0';
		strcat(TexT, target_word);
		strcat(TexT, temp + OffSet + lENGTH);
    }
    free(tempOLD);
    free(temp);
   
}


void Replace_All_INSE(char *TexT,char *variable,char *target_word)
{
	
	char *tempOLD;
	tempOLD = (char *) malloc(sizeof(char)*strlen(variable));
	strcpy(tempOLD, variable);
	char *temp;
	temp = (char *) malloc(sizeof(char)*strlen(TexT));

    char *Buffer_Str;
    int OffSet = 0;
    int lENGTH;
 
	lENGTH = strlen(variable);

    while ((Buffer_Str = Search_str_OffSet(TexT, variable)) != NULL)
    {
		strcpy(temp, TexT);
		OffSet = Buffer_Str - TexT;
		TexT[OffSet] = '\0';
		strcat(TexT, target_word);
		strcat(TexT, temp + OffSet + lENGTH);
    }
    free(tempOLD);
    free(temp);
}

int Check_equal(char *STR11,char *STR22)
{
    while (*STR11 && *STR22)
    {
        char a=*STR11;
        char b=*STR22;

         a=tolower(a);
         b=tolower(b);

        if (b != a) {
            return 0;
        }
 
        STR11++;
        STR22++;
    }
 
    return (*STR22 == '\0');
}
 
char* Search_str_OffSet(char* text, char* OldStr)
{
    while (*text != '\0')
    {
        char x=text[0];
        char y=OldStr[0];

         x=tolower(x);
         y=tolower(y);

        if ((x == y) && Check_equal(text, OldStr)) {
            return text;
        }
        text++;
    }
     return NULL;
}


int last_element(char* Str,char* first_word,int alert){
	int flag=0;

	int bck_count = strlen(first_word)-1;
	//alert is there i or no
	if(alert==1){
		for(int a=strlen(Str)-1;Str[a]!=' ';a--){
	
			
			if(toupper(first_word[bck_count])==toupper(Str[a])){
				flag=0;

			}
			else{
				flag=1;

			}

				bck_count--;
		}


	}
	else{

		for(int a=strlen(Str)-1;Str[a]!=' ';a--){
	
			

			if(first_word[bck_count]==Str[a]){
				flag=0;

			}
			else{
				flag=1;

			}
			bck_count--;

		}


	}




	if(flag==1)
		return 0;
	else
		return 1;

}

int caseE(char* Str,char* first_word,int alert){
	int flag=0;

	//alert is there i or no
	if(alert==1){
		for(int a=0;Str[a]!=' ';a++){
	
			
			if(toupper(first_word[a])==toupper(Str[a])){
				flag=0;

			}
			else{
				flag=1;

			}
		}


	}
	else{

		for(int a=0;Str[a]!=' ';a++){
	
			
			if(first_word[a]==Str[a]){
				flag=0;

			}
			else{
				flag=1;

			}
		}


	}

	if(flag==1)
		return 0;
	else
		return 1;

}

void parsingARGV_1_caseA(char* argv1){


	
	int i;
	int counter=0;
	int counter_size_str1=0;
	int counter_size_str2=0;
	int counter_size_cont=0;

	for(i=0;i<strlen(argv1);i++){
		if(argv1[i]=='/'){
			
			counter_size_cont++;
		}
		if(argv1[i]!='/'&&counter_size_cont==1){
			
			counter_size_str1++;
		}
		if(argv1[i]!='/'&&counter_size_cont==2){
			
			counter_size_str2++;
		}
		

	}

	STR1 = (char *) malloc(sizeof(char)*counter_size_str1);
	STR2 = (char *) malloc(sizeof(char)*counter_size_str1);
	check_free=1;
	int a=0;
	int b=0;
	for(i=0;i<strlen(argv1);i++){
		if(argv1[i]=='/'){
			
			counter++;
		}
		if(argv1[i]!='/'&&counter==1){
			
			STR1[a]=argv1[i];
			a++;
		}
		if(argv1[i]!='/'&&counter==2){
			
			STR2[b]=argv1[i];
			b++;
		}

	}

	/*
	int c=0;
	printf("\n");
	printf("\n");
	printf("\n");
	for(c=0;c<strlen(STR1);c++){
		printf("%c",STR1[c]);
	}
	for(c=0;c<strlen(STR2);c++){
		printf("%c",STR2[c]);
	}
	printf("\n");
	printf("\n");
	*/

}

ssize_t Reader_LBL (char *Buffer, size_t Size, char *Path, off_t *st)
{
	ssize_t check_read = 0;
    ssize_t counter = 0;
    char *pointer;
    int fd = open (Path, O_RDONLY);
    if (fd == -1) {
        perror ("open");
        return -1;
    }
	if ((check_read = lseek (fd, *st, SEEK_SET)) != -1)
        check_read = read (fd, Buffer, Size);
    close (fd);

    if (check_read == -1) { 
        perror ("read");
        return check_read;
    }
	if (check_read == 0) return -1;

    pointer = Buffer;  
    while (counter < check_read && *pointer != '\n') pointer++, counter++;
    *pointer = 0;

    if (counter == check_read) {  
        *st += check_read;

        
        return check_read < (ssize_t)Size ? check_read : 0;
    }

    *st += counter + 1;

    return counter;
}

void which_statement(char* argv1){
	
	char* temp_STR1_1;
	char* temp_STR1_2;
	
	temp_STR1_1 = (char *) malloc(sizeof(char)*strlen(STR1)-3);
	temp_STR1_2 = (char *) malloc(sizeof(char)*strlen(STR1)-3);
	
	/*CASE D*/
	int cc=0;
	
	char temp_Char2;
	
	int k=0;
	int memo=0;
	
	
	for(cc=0;cc<strlen(STR1);cc++){
		if(STR1[cc]=='['){
			flag_kose=1;

		}
	}


 
	int i=0;
	int slash_caunt=0;
	for(i=0;i<strlen(argv1);i++){
		if(argv1[i]=='/'){
			++slash_caunt;
		}

		if(slash_caunt==3){/*INSENSITIVE*/
		
			if(argv1[i+1]=='i'){
			
				
				if(flag_kose==1){


				for(cc=0;cc<strlen(STR1);cc++){

				if(STR1[cc]=='['){					//first and secod cha get
					
					//temp_Char1=STR1[cc+1];
					temp_Char2=STR1[cc+2];

					temp_STR1_1[k]=STR1[cc+1];
				
					memo=k;
					k++;

					cc=cc+3;

				}
				else{
					temp_STR1_1[k]=STR1[cc];

					
					k++;
				}

			    }
			   

				strcpy(temp_STR1_2,temp_STR1_1);
				
				temp_STR1_2[memo]=temp_Char2;

				
    			 Replace_All_INSE(buffer, temp_STR1_1, STR2);

    			 Replace_All_INSE(buffer, temp_STR1_2, STR2);

				}
				else{

				
    			Replace_All_INSE(buffer, STR1, STR2);
    			
    			 
				}

			}
			else{/*SENSITIVE*/

				if(flag_kose==1){
					for(cc=0;cc<strlen(STR1);cc++){

				if(STR1[cc]=='['){
					
					//temp_Char1=STR1[cc+1];
					temp_Char2=STR1[cc+2];

					temp_STR1_1[k]=STR1[cc+1];
				
					memo=k;
					k++;

					cc=cc+3;

				}
				else{
					temp_STR1_1[k]=STR1[cc];

					
					k++;
				}

			}
				strcpy(temp_STR1_2,temp_STR1_1);
				
				temp_STR1_2[memo]=temp_Char2;

				Replace_All_SENS(buffer, temp_STR1_1, STR2);

				Replace_All_SENS(buffer, temp_STR1_2, STR2);

				}
				else{
					

					

					
				Replace_All_SENS(buffer, STR1, STR2);
				}
	
			}

		}

	}

}

void state_a_b_d_e(char* argv1){
	

	char* temp_STR1_1;
	char* temp_STR1_2;
	
	temp_STR1_1 = (char *) malloc(sizeof(char)*strlen(STR1)-3);
	temp_STR1_2 = (char *) malloc(sizeof(char)*strlen(STR1)-3);
	
	/*CASE D*/
	int cc=0;
	
	char temp_Char2;
	
	int k=0;
	int memo=0;
	

	for(cc=0;cc<strlen(STR1);cc++){
		if(STR1[cc]=='['){
			flag_kose=1;

		}
	}

	int i=0;
	int slash_caunt=0;
	for(i=0;i<strlen(argv1);i++){
		if(argv1[i]=='/'){
			++slash_caunt;
		}

		if(slash_caunt==3){/*INSENSITIVE*/
		
			if(argv1[i+1]=='i'){
			
				
				if(flag_kose==1){
					

				for(cc=0;cc<strlen(STR1);cc++){

				if(STR1[cc]=='['){					//first and secod cha get
					
					//temp_Char1=STR1[cc+1];
					temp_Char2=STR1[cc+2];

					temp_STR1_1[k]=STR1[cc+1];
				
					memo=k;
					k++;

					cc=cc+3;

				}
				else{
					temp_STR1_1[k]=STR1[cc];

					
					k++;
				}

			    }
			   
				strcpy(temp_STR1_2,temp_STR1_1);
				
				temp_STR1_2[memo]=temp_Char2;


       			 Replace_All_INSE(line, temp_STR1_1, STR2);
    			 Replace_All_INSE(line, temp_STR1_2, STR2);

				}
				else{

    			Replace_All_INSE(line, STR1, STR2);
    			
				}

			}
			else{/*SENSITIVE*/

				if(flag_kose==1){
					for(cc=0;cc<strlen(STR1);cc++){

				if(STR1[cc]=='['){
					
					//temp_Char1=STR1[cc+1];
					temp_Char2=STR1[cc+2];

					temp_STR1_1[k]=STR1[cc+1];
				
					memo=k;
					k++;

					cc=cc+3;

				}
				else{
					temp_STR1_1[k]=STR1[cc];

					
					k++;
				}

			}
				strcpy(temp_STR1_2,temp_STR1_1);
				
				temp_STR1_2[memo]=temp_Char2;

				Replace_All_SENS(line, temp_STR1_1, STR2);

				Replace_All_SENS(line, temp_STR1_2, STR2);

				}
				else{
					

					Replace_All_SENS(line, STR1, STR2);
				}
	
			}

		}

	}

	free(temp_STR1_1);
	free(temp_STR1_2);

}


