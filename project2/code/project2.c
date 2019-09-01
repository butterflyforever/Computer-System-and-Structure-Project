#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_LINE 80
#define BUFFER_SIZE 50
char buffer[BUFFER_SIZE];
char *history[10];
int n = 0;


void showhistory(){
  int cont = 1;
  int i = n - 1;
  if (i<0) i = i + 10; 
  printf("\n");
  while(1){
    if( cont>10 || history[i] == NULL) break;
    printf("%d\t%s\n",cont,history[i]);
    cont ++;
    i--;
    if (i<0) i = i + 10; 
  }
}


void handle_SIGINT(){
  write(STDOUT_FILENO, buffer, strlen(buffer));
  fflush(stdout);
  showhistory();

}

void setup(char inputBuffer[], char *args[], int *background){
  char c;
  const char *d = " "; 
  char *p;
  int len = 0, index = 0;
  
  //initialize
  for(int i = 0; i< MAX_LINE/2; i++)
	args[i] = NULL;
  
  //get input
  while( (c = getchar()) != EOF && len < MAX_LINE ){
	if ('\n' == c) break;       
	inputBuffer[len++] = c;
  }
  inputBuffer[len] = '\0';
  if(len == 0) return;
  //background
  if(inputBuffer[len - 1] == '&') {*background = 1; inputBuffer[len - 1] = '\0';}
  
  //r & rx
  if(inputBuffer[0] == 'r'){
	//r
        int tmp_n = n;
        if(tmp_n == 0) tmp_n = 9;
        else tmp_n--;
	if(inputBuffer[1] == '\0'){
           if(history[tmp_n] == '\0'){
		printf("No history!\n");
		return;
	     }
	   strcpy(inputBuffer,history[tmp_n]);
	}
	//rx
	if(inputBuffer[1] == ' ' && inputBuffer[2] != '\0' && inputBuffer[3] == '\0'){
	  int cont = 1;
	  int i = n - 1;
	  if (i<0) i = i + 10; 
	  while(1){
	    if( cont>10 || history[i] == NULL){
		printf("No proper history!\n");
		return;
	    }
            if(history[i][0] == inputBuffer[2]){ 
		strcpy(inputBuffer,history[i]);
		break;
	    }
	    cont ++;
	    i--;
	    if (i<0) i = i + 10; 
	  }
	}

  }

  //push in history
  int tmp_n = n;
  if(tmp_n == 0) tmp_n = 9;
  else tmp_n--;
  if(history[tmp_n] == NULL || strcmp(inputBuffer,history[tmp_n])){
	  history[n] = (char*)malloc(strlen(inputBuffer));
	  strcpy(history[n],inputBuffer);
	  n = (n + 1)%10;
  }
  //divide by space
  p = strtok(inputBuffer,d);
  args[index] = p;
  while ((p = strtok(NULL,d))){
      index++;
      args[index] = p;
  }
  
}

int main()
{
   struct sigaction handler;
   handler.sa_handler = handle_SIGINT;
   sigaction(SIGINT, &handler, NULL);

   char inputBuffer[MAX_LINE];
   int background;
   char *args[MAX_LINE/2 + 1];
   for(int i = 0; i<10; i++) history[i] = NULL;

   while(1){
	background = 0;

	fflush(stdin); // clean inputBuffer	

    	printf("COMMAND->");

	fflush(stdout); // clean outputBuffer	
    	setup(inputBuffer, args, &background);
	
        if(args[0] != NULL){
	   pid_t pid;
           pid = fork();
           if(pid<0){
		printf("Process Error!\n");
                exit(1);
	   }
	   else if(pid == 0){
		execvp(args[0],args);
		background = 0;
                printf("error\n");
                exit(0);
	   }
           else{
		if(background == 0) waitpid(pid,NULL,0);// No wait means run background
	   }
	}
   }
   //capture the ctrl+C
   signal(SIGINT,handle_SIGINT);
   return 0;
}
