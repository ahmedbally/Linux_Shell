/* ----------------------------------------------------------------- */
/* PROGRAM  sish                                                     */
/* ----------------------------------------------------------------- */
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define true 1
#define false 0
#define bool int
//global variables for input output redirections
char input[100],output[100];
//flag for background process status
bool back=false;
/* ----------------------------------------------------------------- */
/* function to check for input output redirection and background process */
/* ----------------------------------------------------------------- */
void ioRedirection(char *line){
	int x=0,y=0;//used for the size of file name
	//store the string name of input&output files
	input[x]='\0';
	output[y]='\0';
    while (*line != '\0'){       
		if(*line == '<'){
			*line++=' ';
			while (*line == ' ' || *line == '\t' ||  *line == '\n') line++;
				
			while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n'){                		  
				input[x++]=*line;
				*line=' ';
				line++; 		
			}
			input[x++]='\0';
		}
        if(*line == '>'){
		    *line++=' ';
			while (*line == ' ' || *line == '\t' ||  *line == '\n') line++;
			while (*line != '\0' &&*line != ' ' && *line != '\t' && *line != '\n'){  
				output[y++]=*line;
				*line=' ';
				line++; 		
			}
			output[y++]='\0';
		}
		if(*line =='&') {
			back=true;
			*line=' ';
		}
		line++;
    }
}

/* ----------------------------------------------------------------- */
/* function to split command into token depend on pipe char "|"		 */
/* ----------------------------------------------------------------- */
void  parsePip(char *line, char **argv){

    while (*line != '\0'){
 		int t=0;    
        while (*line == ' ' || *line == '|' || *line == '\t' ||  *line == '\n') *line++ = '\0';
        *argv++ = line;
        while (*line != '\0' && (*line != '|'||t) && *line != '\t' && *line != '\n') {
            if(*line=='\"') t=!t;
			line++;             
	 	}
    }
    *argv = '\0';
}
/* ----------------------------------------------------------------- */
/* function to split a command into arguments						 */
/* ----------------------------------------------------------------- */
void  parse(char *line, char **argv){
	int t=0;
    while (*line != '\0') {       
		if (*line =='\"'){ 
			t=1;
			++line;
		}
        while (*line == ' ' || *line == '\t' || *line == '\n') *line++ = '\0';     
         *argv++ = line;          
		if(t){
			while (*line !='\"') {
               line++;
			}
			*line++='\0';
			t=0;
		}
		else
        	while(*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') line++;         
	 	while (*line == ' ' || *line == '\t' || *line == '\n') *line++ = '\0';    
	}
     *argv = '\0';                 
}
/* ----------------------------------------------------------------- */
/* function to excute the commands 									 */
/* ----------------------------------------------------------------- */     
void  execute(char ***cmd){
	int   p[2];
	int   fd_in = 0,fd_out=0;
  	pid_t pid;
	if(input[0]!=NULL) fd_in=open(input,O_RDONLY);
	if(output[0]!=NULL)fd_out=open(output, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	  

  	while (*cmd != NULL){
	    pipe(p);
	    if ((pid = fork()) == -1){
          exit(0);
        }
    	else if (pid == 0){
	        dup2(fd_in, 0); 
	        if (*(cmd + 1) != NULL) dup2(p[1], 1);
 	  		if (*(cmd + 1) == NULL && output[0]!=NULL)dup2(fd_out, 1);
            close(p[0]);
          	execvp((*cmd)[0], *cmd);
          	printf("*** error execute failed ***\n");
          	exit(0);
          
        }
      	else{
			int *status;
          	if(!back) waitpid(pid,status,0);
          	else{wait(NULL);}
          	close(p[1]);
          	fd_in = p[0]; 
          	cmd++;
        }
    }
}

void  main(void){
	char line[1024];         
	char *argv[64];          
	char *argm[64][64];  
	char **cmd[64];
	int i,j;

	while (1) {
		back=false;               
		printf("sish:>");     
		gets(line);    
		ioRedirection(line);
		parsePip(line, argv);       
		for(i=0;argv[i];i++){
			parse(argv[i],argm[i]);
			cmd[i]=argm[i];
		}	
	    if(*cmd != NULL)
	  	if ((strcmp(cmd[0][0], "exit") == 0)) exit(0);
	    execute(cmd);  
	  	for(i=0;cmd[i];i++) cmd[i]=NULL;
	}
}

                
