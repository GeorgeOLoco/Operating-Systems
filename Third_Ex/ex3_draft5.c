#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

int pick_maximum(int a, int b){
	if(a>b) return a;
	return b;
}

void PrintMessage(int message_code){
	if(message_code==0)
		printf("Usage: ask3 <nChildren> [--random][--round-robin]\n");
	else if(message_code==1)
		printf("Type a number to send a job to a child\n");
}
	
	
		
int main(int argc, char* argv[]){
	int n;
	bool mode = 0;
	int child_counter = 0;
	//Elegxoi gia sfalmata
	if ((argc!=3) && (argc!=2)){ //checking for the correct number of arguments
		PrintMessage(0);
		exit(EXIT_FAILURE);
    	}
    	for (int i = 0; i < strlen(argv[1]); i++){ //checking if n is an integer
		if (isdigit(argv[1][i]) == false){
      			PrintMessage(0);
			exit(EXIT_FAILURE);
		}
	}
	n=atoi(argv[1]);
	if(n<1){ //checking if n is positive
		PrintMessage(0);
		exit(EXIT_FAILURE);
	}
	if (argc==3){ //elegxos orthotitas 3ou orismatos & kathorismos mode
		if(strcmp(argv[2],"--round-robin") == false) { mode = 0; }
		else if(strcmp(argv[2],"--random") == false) { mode = 1; }
		else {
			PrintMessage(0);
			exit(EXIT_FAILURE);
		}
	}
	//Telos elegxwn

	
	
	int* childrenpids;
	childrenpids = (int*)malloc(n*sizeof(int));
	int** pd;
	int pd2[2];
	pd = (int**)malloc(n*sizeof(int*));
	for(int i=0; i<n; i++){
		pd[i] = (int*)malloc(2*sizeof(int));
	}
	for(int i=0;i<n;i++){
		if (pipe(pd[i]) != 0) {
        		perror("pipe");
   		 }
   	}
   	if (pipe(pd2) != 0) {
        	perror("pipe");
        }
	
	for (int i = 0; i < n; i++){
        	pid_t child=fork();
        	if (child==-1){
        		perror("fork");
        		exit(1);
        	}
        	else if (child==0) { //CHILD process
        		int val=0;
        		int child_index = i;
         
            		while(true) {
				read(pd[child_index][0], &val, sizeof(int));
				printf("[Child %d] [%d] Child received %d!\n", child_index, getpid(), val);
				val++;
				sleep(5);
				write(pd2[1], &val, sizeof(int));
				write(pd2[1], &child_index, sizeof(int));
				printf("[Child %d] [%d] ", child_index, getpid());
				printf("Child Finished Hard Work, ");
				printf("writing back %d\n", val);
			}
			exit(0);
		}
		
		else{
			childrenpids[i] = child;
		}
	}
	
	while (1) {
        	fd_set inset;
        	int maxfd;

        	FD_ZERO(&inset);                // we must initialize before each call to select
        	FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        	FD_SET(pd2[0], &inset); 	// select will check for input from pipe
       		
       		
      		//select only considers file descriptors that are smaller than maxfd
            	maxfd = pick_maximum(STDIN_FILENO,pd2[0]);
        	maxfd ++;

        	//wait until any of the input file descriptors are ready to receive
        	
        	//printf("Perimenw 1\n\n");
        	int ready_fds = select(maxfd, &inset, NULL, NULL, NULL); //this line is blocking
        	//printf("Perimenw 2\n\n");
        	
        	/*The select() function will block the program until one or more file descriptors 
        	in the inset set become ready for reading, or until a timeout occurs (if one is
        	specified). After select() returns, the inset set will contain the file descriptors
        	that are ready for reading.
        	
        	The return value of select() is the number of file descriptors that are ready, or -1 
        	if an error occurred. The return value is stored in the ready_fds variable, which can 
        	then be used to determine how many file descriptors are ready for reading.*/
        	
        	if (ready_fds <= 0) {
            		perror("select");
           		 continue;  // just try again
        	}

        	// user has typed something, we can read from stdin without blocking
       		if (FD_ISSET(STDIN_FILENO, &inset)) {
       		/*After calling select(), the
              	FD_ISSET() macro can be used to test if a file descriptor
              	is still present in a set.  FD_ISSET() returns nonzero if
              	the file descriptor fd is present in set, and zero if it
              	is not.*/
            		char buffer[101];
            		int n_read = read(STDIN_FILENO, buffer, 100); 
            		if (n_read == -1){
                    		perror("read");
                    		exit(-1);
               		} 

            		// New-line is also read from the stream, discard it.
            		if (n_read > 0 && buffer[n_read-1] == '\n') {
                	buffer[n_read-1] = '\0';
            		}
			
			//if user input is 'exit' terminated all children
            		if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                		for(int i=0;i<n;i++){
                			printf("Waiting for %d children to terminate\n", n-i);
                			kill(childrenpids[i],SIGTERM);
                		}
                		printf("All children terminated\n");
                		free(childrenpids);
				for(int i=0; i<n; i++){
					free(pd[i]);
				}
				free(pd);
				return 0;
            		}
            		
            		//checking if buffer is an integer
            		bool integer_flag = true;
            		for (int i = 0; i < strlen(buffer); i++){ 
				if (isdigit(buffer[i]) == false){
      					integer_flag = false;
      					break;
				}
			}
			//if buffer is not an integer print the appropriate message
			if((integer_flag == false)||(strcmp(buffer,"\0") == 0)){
				PrintMessage(1);
			}
			
			//if buffer is an integer send its numeric value to a child
			else if(integer_flag == true){
				int num = atoi(buffer);
				int receiver;
				if(mode==0){ //mode 0 corresponds to round robin
					receiver = child_counter%n;
					printf("[Parent] Assigned number %d ", num);
					printf("to child: %d\n", receiver);
					write(pd[receiver][1], &num, sizeof(int));
					child_counter++;
				}
				else if (mode == 1){ //mode 1 corresponds to random
					receiver = rand()%n;
					printf("Assigned number %d ", num);
					printf("to child: %d\n", receiver);
					write(pd[receiver][1], &num, sizeof(int));
				}

			}
        	}
		
       		// someone has written bytes to the pipe, we can read without blocking
       		if (FD_ISSET(pd2[0], &inset)) {
            		int val;
            		int index;
            		read(pd2[0], &val, sizeof(int));   
            		read(pd2[0], &index, sizeof(int));  
            		printf("[PARENT] Received result from child %d", index);
            		printf(" --> %d\n", val);
        	}
		
	}
}
	
	
